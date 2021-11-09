import tensorflow as tf
import pathlib
import os
import time
from IPython.display import display, clear_output # for outputting
import matplotlib.pyplot as plt
import argparse
import numpy as np
import glob
from pix2pix_builder import GeneratorTemplate, DiscriminatorTemplate
# from piper import Piper
from lossManager import LossManager
from lossWriter import Messager
from logger import LogManager
import shutil
from math import floor


"""
DATASET --------------------------------------------------------------------------------------
"""

# loads image file and splits into two tensors
def load(image):
    # - one with a real building facade image
    # - one with an architecture label image
    # print(image.shape)
    w = tf.shape(image)[1]
    w = w // 2

    w = image.shape[2] // 2
    # print(w)
    input_image = image[:, :, w:, :]
    real_image = image[:, :, :w, :]

    # print(input_image.shape)


    # Convert both images to float32 tensors
    input_image = tf.cast(input_image, tf.float32)
    real_image = tf.cast(real_image, tf.float32)

    return input_image, real_image

def resize(input_image, real_image, height, width):
    input_image = tf.image.resize(input_image, [height, width],
                                method=tf.image.ResizeMethod.NEAREST_NEIGHBOR)
    real_image = tf.image.resize(real_image, [height, width],
                               method=tf.image.ResizeMethod.NEAREST_NEIGHBOR)

    return input_image, real_image

def random_crop(input_image, real_image):
    stacked_image = tf.stack([input_image, real_image], axis=0)
    cropped_image = tf.image.random_crop(
      stacked_image, size=[2, IMG_HEIGHT, IMG_WIDTH, 3])

    return cropped_image[0], cropped_image[1]

# Normalizing the images to [-1, 1]
def normalize(input_image, real_image):
    input_image = (input_image / 127.5) - 1
    real_image = (real_image / 127.5) - 1

    return input_image, real_image


@tf.function()
def random_jitter(input_image, real_image):
    # Resizing to 286x286
    input_image, real_image = resize(input_image, real_image, 286, 286)

    # Random cropping back to 256x256
    input_image, real_image = random_crop(input_image, real_image)

    if tf.random.uniform(()) > 0.5:
        # Random mirroring
        input_image = tf.image.flip_left_right(input_image)
        real_image = tf.image.flip_left_right(real_image)

    return input_image, real_image
"""
plt.figure(figsize=(6, 6))
for i in range(4):
  rj_inp, rj_re = random_jitter(inp, re)
  plt.subplot(2, 2, i + 1)
  plt.imshow(rj_inp / 255.0)
  plt.axis('off')
plt.show()
"""

def load_image_train(image):
    input_image, real_image = load(image)
    # input_image, real_image = random_jitter(input_image, real_image) # TODO ADD BACK IN
    input_image, real_image = normalize(input_image, real_image)

    return input_image, real_image

"""""
---------------------------------------------------------------------------------
"""""

def generator_loss(disc_generated_output, gen_output, target, LAMBDA):
    loss_object = tf.keras.losses.BinaryCrossentropy(from_logits=True)
    gan_loss = loss_object(tf.ones_like(disc_generated_output), disc_generated_output)

    # Mean absolute error
    l1_loss = tf.reduce_mean(tf.abs(target - gen_output))

    total_gen_loss = gan_loss + (LAMBDA * l1_loss)

    return total_gen_loss, gan_loss, l1_loss

def discriminator_loss(disc_real_output, disc_generated_output):
    loss_object = tf.keras.losses.BinaryCrossentropy(from_logits=True)
    real_loss = loss_object(tf.ones_like(disc_real_output), disc_real_output)

    generated_loss = loss_object(tf.zeros_like(disc_generated_output), disc_generated_output)

    total_disc_loss = real_loss + generated_loss

    return total_disc_loss

"""
TRAINING ---------------------------------------------------------------------------
"""
@tf.function
def train_step(input_image, target, generator_optimizer, discriminator_optimizer, gen_loss_, disc_loss_, LAMBDA, generator, discriminator):

    with tf.GradientTape() as gen_tape, tf.GradientTape() as disc_tape:
        gen_output = generator(input_image, training=False)

        disc_real_output = discriminator([input_image, target], training=False)
        disc_generated_output = discriminator([input_image, gen_output], training=False)

        gen_total_loss, gen_gan_loss, gen_l1_loss = gen_loss_(disc_generated_output, gen_output, target, LAMBDA)
        disc_loss = disc_loss_(disc_real_output, disc_generated_output)

        generator_gradients = gen_tape.gradient(gen_total_loss,
                                              generator.trainable_variables)
        discriminator_gradients = disc_tape.gradient(disc_loss,
                                                   discriminator.trainable_variables)

        generator_optimizer.apply_gradients(zip(generator_gradients,
                                              generator.trainable_variables))
        discriminator_optimizer.apply_gradients(zip(discriminator_gradients,
                                                  discriminator.trainable_variables))
    return gen_total_loss, disc_loss
    # with summary_writer.as_default():
    #     tf.summary.scalar('gen_total_loss', gen_total_loss, step=step//1000)
    #     tf.summary.scalar('gen_gan_loss', gen_gan_loss, step=step//1000)
    #     tf.summary.scalar('gen_l1_loss', gen_l1_loss, step=step//1000)
    #     tf.summary.scalar('disc_loss', disc_loss, step=step//1000)

def train(train_ds, max_epochs, learning_rate, beta, gen_loss_, disc_loss_, LAMBDA,
 ROOT_IMG_SAVE, ROOT_CHECKPOINT_SAVE, steps_per_epoch, gen, disc, log_msg,
  RANDOM_HORIZONTAL, RANDOM_VERTICAL, RANDOM_CROP, RANDOM_BRIGHTNESS, RANDOM_CONTRAST):

    save_dir = ROOT_CHECKPOINT_SAVE+"ckpt"
    epochs = 0

    gen_msg = Messager(ROOT_CHECKPOINT_SAVE+"gen.txt")
    disc_msg = Messager(ROOT_CHECKPOINT_SAVE+"disc.txt")

    save = glob.glob(save_dir+"/*")
    if not os.path.exists(save_dir):
        print("TRAINING FROM SCRATCH")
        log_msg.send("Training from scratch")
        os.makedirs(save_dir)
    elif(len(save) != 0):
        if(len(save) != 2):
            nums = [s.split("/")[-1].split("_")[0][1:] for s in save]

            for i, n in enumerate(nums):
                if(nums.count(n) != 2):
                    save.pop(i)

        print("RESTORING FROM BACKUP")
        log_msg.send("Restoring from saved")
        print(save)
        save_ = save[0].split("/")[-1].split("_")[0][1:]
        epochs = int(save_)

        if("discriminator" in save[0]):
            disc = tf.keras.models.load_model(save[0])
            gen = tf.keras.models.load_model(save[1])
        else:
            disc = tf.keras.models.load_model(save[1])
            gen = tf.keras.models.load_model(save[0])

    lm = LossManager(ROOT_CHECKPOINT_SAVE+"losses/")

    # gen_pipe = Piper("/tmp/gen")
    # disc_pipe = Piper("/tmp/disc")

    example_input, example_target = next(iter(train_ds.take(1)))
    example_input2, example_target2 = next(iter(train_ds.take(1)))
    example_input3, example_target3 = next(iter(train_ds.take(1)))
    start = time.time()



    g_opt = tf.keras.optimizers.Adam(learning_rate, beta_1=beta)
    d_opt = tf.keras.optimizers.Adam(learning_rate, beta_1=beta)

    log_msg.send("Beginning training...")
    for epoch in range(max_epochs):

        log_msg.send("Epoch " + str(epochs))
        clear_output(wait=True)
        generate_images(gen, example_input, example_target,example_input2, example_target2,example_input3, example_target3, ROOT_IMG_SAVE, 1)
        plt.savefig(ROOT_IMG_SAVE + str(epochs) +".png")


        gen.save(save_dir+"/-" +str(epochs)+"_generator")
        disc.save(save_dir+"/-" +str(epochs)+"_discriminator")

        shutil.rmtree(save_dir+"/-" +str(epochs-1)+"_generator", ignore_errors=True)
        shutil.rmtree(save_dir+"/-" +str(epochs-1)+"_discriminator", ignore_errors=True)
        epochs += 1

        if epoch != 0:
            print(f'Time taken for 1 epoch: {time.time()-start:.2f} sec\n')

            start = time.time()


            if not os.path.exists(ROOT_IMG_SAVE[:-1]):
                os.makedirs(ROOT_IMG_SAVE[:-1])
                fig = plt.savefig(ROOT_IMG_SAVE+str(epochs)+".png")
                plt.clf()
                plt.close(fig)
                # print(epochs)
                # print("EPOCHS!!")
                print(f"Epoch: {epoch}")

        lossG = []
        lossD = []

        for (input_image, target) in train_ds:
            input_image = apply_augmentations(input_image, RANDOM_HORIZONTAL, RANDOM_VERTICAL, RANDOM_CROP, RANDOM_BRIGHTNESS, RANDOM_CONTRAST)
            g_loss, d_loss = train_step(input_image, target, g_opt, d_opt, gen_loss_, disc_loss_, LAMBDA, gen, disc)
            g_loss = g_loss.numpy()
            d_loss = d_loss.numpy()
            # val = np.format_float_positional(val, precision=precision, unique=False, fractional=False, trim='k')
            # print(str(g_lossg_loss)[:5])
            # print("-------------------")
            # gen_pipe.send_message(str(g_loss)[:5] + "\n")
            # disc_pipe.send_message(str(d_loss)[:5] + "\n")

            gen_msg.send_message(str(g_loss)[:5] + "\n")
            disc_msg.send_message(str(d_loss)[:5] + "\n")


            lossG.append(g_loss)
            lossD.append(d_loss)
            # Training step
            # if (step+1) % 10 == 0:
            #     print('.', end='', flush=True)


            # # Save (checkpoint) the model every epoch
            # if int(step.numpy()) % steps_per_epoch == 0:

        lm.addGeneratorLosses(np.array(lossG))
        lm.addDiscriminatorLosses(np.array(lossD))
        lm.generate_graph(ROOT_CHECKPOINT_SAVE)
        lm.generate_graphD(ROOT_CHECKPOINT_SAVE)
        lm.generate_graphG(ROOT_CHECKPOINT_SAVE)



    plt.close()

# relies on square images atm.. TODO: add img width and height
def apply_augmentations(image_batch, RANDOM_HORIZONTAL, RANDOM_VERTICAL, RANDOM_CROP, RANDOM_BRIGHTNESS, RANDOM_CONTRAST):
    if(RANDOM_HORIZONTAL):
        image_batch = tf.image.random_flip_left_right(image_batch)
    if(RANDOM_VERTICAL):
        image_batch = tf.image.random_flip_up_down(image_batch)
    if(RANDOM_CROP > 0):
        new_size = (image_batch.shape[0], floor(image_batch.shape[1]*(1-RANDOM_CROP)), floor(image_batch.shape[2]*(1-RANDOM_CROP)), image_batch.shape[3])
        old_width = image_batch.shape[1]
        old_height = image_batch.shape[2]
        image_batch = tf.image.random_crop(image_batch, new_size)
        image_batch = tf.image.resize(image_batch, (old_width, old_height))
    if(RANDOM_BRIGHTNESS > 0):
        image_batch = tf.image.random_brightness(image_batch, RANDOM_BRIGHTNESS)
    if(RANDOM_CONTRAST > 0):
        image_batch = tf.image.random_contrast(image_batch, 0, RANDOM_CONTRAST)

    return image_batch


def generate_images(model, i1, t1, i2, t2, i3, t3, ROOT_IMG_SAVE, num=1):
    p1 = model(i1, training=False)
    p2 = model(i2, training=False)
    p3 = model(i3, training=False)
    # plt.figure(figsize=(4, 4))

    display_list = [i1[0], t1[0], p1[0], i2[0], t2[0], p2[0], i3[0], t3[0], p3[0]]
    title = ['Input Image', 'Ground Truth', 'Predicted Image']
    plt.cla()
    fig, axs = plt.subplots(3, 3, figsize=(4, 4))
    axs = axs.flatten()
    plt.axis('off')
    for img, ax in zip(display_list, axs):
        ax.imshow(img *0.5 + 0.5)
        ax.axis('off')
    return fig
    # for i in range(3):
    #     plt.subplot(1, 3, i+1)
    #     plt.title(title[i], fontsize=10)
    #     # Getting the pixel values in the [0, 1] range to plot.
    #     plt.imshow(display_list[i] * 0.5 + 0.5)



def get_dataset(dir, batch_size, image_width, image_height, image_channels):
    colour_mode = "rgb"
    if(image_channels == 1):
        colour_mode = "grayscale"

    print("colour mode " + colour_mode)
    train_images = tf.keras.utils.image_dataset_from_directory(dir, label_mode = None, batch_size=batch_size,
    image_size=(image_height,image_width*2), color_mode = colour_mode)


    # train_images = train_images.astype('float32')
    train_dataset = train_images.map(load_image_train, num_parallel_calls=tf.data.experimental.AUTOTUNE)
    # TODO image augmentations
    return train_dataset

if __name__ == '__main__':

    NUM_IMGS = 400
    parser = argparse.ArgumentParser()
    parser.add_argument("--max_epochs", help="epochs to train for", type=int, default=10000, required=False)
    parser.add_argument("--batch_size", help="batch size", type=int, default=1, required=False)
    parser.add_argument("--learning_rateX", help="X: X^-Y learning rate for discriminator and generator", type=int, default=2, required=False)
    parser.add_argument("--learning_rateY", help="Y: X^-Y learning rate for discriminator and generator", type=int, default=-6, required=False)
    parser.add_argument("--image_width", help="image width for input and output", type=int, default=256, required=False)
    parser.add_argument("--image_height", help="image height for input and output", type=int, default=256, required=False)
    parser.add_argument("--input_channel", help="number of channels for input image", type=int, default=3, required=False)
    parser.add_argument("--output_channel", help="number of channels for output image", type=int, default=3, required=False)
    parser.add_argument("--num_layers", help="[advanced] number of layers in generator decoder and encoder", type=int, default=7, required=False)
    parser.add_argument("--kernel_size", help="[advanced] kernel size for convolutional layers", type=int, default=4, required=False)
    parser.add_argument("--beta", help="[advanced] (*10-2) momentum for ADAM optimiser", type=float, default=0.5, required=False)
    parser.add_argument("--lambda_", help="[advanced] variable to improve structural loss", type=int, default=100, required=False)
    parser.add_argument("--img_save_dir", help="Directory to save images to", default="data/default_save/", required=False)
    parser.add_argument("--checkpoint_save_dir", help="Directory to save checkpoints to", default="data/default_save/", required=False)
    parser.add_argument("--dataset_dir", help="Location of dataset", required=True)
    parser.add_argument("--disc_noise", help="Noise applied to discrininator", type=float, default = 0, required=False)
    parser.add_argument("--random_horizontal", help="Random horizontal flip?", type =int, default = 0, required=False)
    parser.add_argument("--random_vertical", help="Random vertical flip?",  type =int, default = 0, required=False)
    parser.add_argument("--random_crop", help="Random crop amount (%)",  type =int, default = 0, required=False)
    parser.add_argument("--random_brightness", help="Random brightness amount",  type =float, default = 0, required=False)
    parser.add_argument("--random_contrast", help="Random contrast amount",  type =float, default = 0, required=False)

    args = parser.parse_args()
    DATASET_DIR = args.dataset_dir
    ROOT_IMG_SAVE = args.img_save_dir
    ROOT_CHECKPOINT_SAVE = args.checkpoint_save_dir
    # print(args.echo)

    MAX_EPOCHS = args.max_epochs
    BATCH_SIZE = args.batch_size
    LEARNING_RATE = args.learning_rateX * pow(10, args.learning_rateY)
    IMG_WIDTH = args.image_width
    IMG_HEIGHT = args.image_height
    INPUT_CHANNEL = args.input_channel
    OUTPUT_CHANNEL = args.output_channel # currently useless

    # advanced:
    NUM_LAYERS = args.num_layers
    KERNEL_SIZE = args.kernel_size
    BETA = args.beta /100
    LAMBDA = args.lambda_

    DISC_NOISE =args.disc_noise
    RANDOM_HORIZONTAL = args.random_horizontal == 1
    RANDOM_VERTICAL = args.random_vertical == 1
    RANDOM_CROP = args.random_crop / 100
    RANDOM_BRIGHTNESS = args.random_brightness
    RANDOM_CONTRAST = args.random_contrast

    GEN_LOSS = generator_loss
    DISC_LOSS = discriminator_loss

    steps_per_epoch = NUM_IMGS // BATCH_SIZE

    log_msg = LogManager(ROOT_CHECKPOINT_SAVE+"log.txt")
    log_msg.send("Initialising...")

    gen = GeneratorTemplate(IMG_WIDTH, IMG_HEIGHT, INPUT_CHANNEL, OUTPUT_CHANNEL, KERNEL_SIZE, NUM_LAYERS)
    disc = DiscriminatorTemplate(IMG_WIDTH, IMG_HEIGHT, INPUT_CHANNEL, OUTPUT_CHANNEL, KERNEL_SIZE, NUM_LAYERS, DISC_NOISE)
    generator = gen.build()
    discriminator = disc.build()

    train_dataset = get_dataset(DATASET_DIR, BATCH_SIZE, IMG_WIDTH, IMG_HEIGHT, INPUT_CHANNEL)


    if(len(train_dataset) == 0):
        log_msg.send("[ERROR] No images found in dataset at " + DATASET_DIR)
        exit(0)
    else:
        log_msg.send("Found " + str(len(train_dataset)) + " images...")
    train(train_dataset, MAX_EPOCHS, LEARNING_RATE, BETA, GEN_LOSS, DISC_LOSS,
    LAMBDA, ROOT_IMG_SAVE, ROOT_CHECKPOINT_SAVE, steps_per_epoch, generator, discriminator,
     log_msg, RANDOM_HORIZONTAL, RANDOM_VERTICAL, RANDOM_CROP, RANDOM_BRIGHTNESS, RANDOM_CONTRAST)

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
from piper import Piper

import shutil


"""
DATASET --------------------------------------------------------------------------------------
"""

# loads image file and splits into two tensors
def load(image):
    # - one with a real building facade image
    # - one with an architecture label image
    print(image.shape)
    w = tf.shape(image)[1]
    w = w // 2

    w = image.shape[2] // 2
    print(w)
    input_image = image[:, :, w:, :]
    real_image = image[:, :, :w, :]

    print(input_image.shape)


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
        gen_output = generator(input_image, training=True)

        disc_real_output = discriminator([input_image, target], training=True)
        disc_generated_output = discriminator([input_image, gen_output], training=True)

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

def train(train_ds, max_epochs, learning_rate, beta, gen_loss_, disc_loss_, LAMBDA, ROOT_IMG_SAVE, ROOT_CHECKPOINT_SAVE, steps_per_epoch, gen, disc):

    # checkpoint.restore(manager.latest_checkpoint)
    # epochs = 0
    # if manager.latest_checkpoint:
    #     print(manager.latest_checkpoint.split("/")[-1][1:])
    #
    #     epochs =int(manager.latest_checkpoint.split("/")[-1][1:])
    #
    #     print("Restored from {}".format(manager.latest_checkpoint))
    # else:
    #     print("Initializing from scratch.")

    save_dir = ROOT_CHECKPOINT_SAVE+"ckpt"
    epochs = 0
    # if not os.path.exists(save_dir):
    #     print("TRAINING FROM SCRATCH")
    #     os.makedirs(save_dir)
    # else:
    #     save = glob.glob(save_dir+"/*.h5")
    #     if(len(save) == 2):
    #         print("RESTORING FROM BACKUP")
    #         save_ = save[0].split("/")[-1].split(".")[0][1:]
    #         epochs = int(save_)
    #
    #
    #         if("discriminator.h5" in save[0]):
    #             disc = tf.keras.models.load_model(save[0])
    #             gen = tf.keras.models.load_model(save[1])
    #         else:
    #             disc = tf.keras.models.load_model(save[1])
    #             gen = tf.keras.models.load_model(save[0])

    if not os.path.exists(save_dir):
        print("TRAINING FROM SCRATCH")
        os.makedirs(save_dir)
    else:
        save = glob.glob(save_dir+"/*")
        print(save)
        if(len(save) != 2):
            nums = [s.split("/")[-1].split("_")[0][1:] for s in save]

            for i, n in enumerate(nums):
                if(nums.count(n) != 2):
                    save.pop(i)

        print("RESTORING FROM BACKUP")
        print(save)
        save_ = save[0].split("/")[-1].split("_")[0][1:]
        epochs = int(save_)

        if("discriminator" in save[0]):
            disc = tf.keras.models.load_model(save[0])
            gen = tf.keras.models.load_model(save[1])
        else:
            disc = tf.keras.models.load_model(save[1])
            gen = tf.keras.models.load_model(save[0])

    gen_pipe = Piper("/tmp/gen")
    disc_pipe = Piper("/tmp/disc")

    example_input, example_target = next(iter(train_ds.take(1)))
    example_input2, example_target2 = next(iter(train_ds.take(1)))
    example_input3, example_target3 = next(iter(train_ds.take(1)))
    start = time.time()



    g_opt = tf.keras.optimizers.Adam(learning_rate, beta_1=beta)
    d_opt = tf.keras.optimizers.Adam(learning_rate, beta_1=beta)

    for epoch in range(max_epochs):

        clear_output(wait=True)
        generate_images(gen, example_input, example_target,example_input2, example_target2,example_input3, example_target3, ROOT_IMG_SAVE, 1)
        plt.savefig(ROOT_IMG_SAVE + str(epochs) +".png")

        files = glob.glob(save_dir+"/*")
        gen.save(save_dir+"/-" +str(epochs)+"_generator")
        disc.save(save_dir+"/-" +str(epochs)+"_discriminator")

        epochs += 1
        for f in files:
            shutil.rmtree(f, ignore_errors=True)

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

        for (input_image, target) in train_ds:


            g_loss, d_loss = train_step(input_image, target, g_opt, d_opt, gen_loss_, disc_loss_, LAMBDA, gen, disc)
            g_loss = g_loss.numpy()
            d_loss = d_loss.numpy()
            # val = np.format_float_positional(val, precision=precision, unique=False, fractional=False, trim='k')
            # print(str(g_lossg_loss)[:5])
            # print("-------------------")
            gen_pipe.send_message(str(g_loss)[:5] + "\n")
            disc_pipe.send_message(str(d_loss)[:5] + "\n")
            # Training step
            # if (step+1) % 10 == 0:
            #     print('.', end='', flush=True)


            # # Save (checkpoint) the model every epoch
            # if int(step.numpy()) % steps_per_epoch == 0:

def generate_images(model, i1, t1, i2, t2, i3, t3, ROOT_IMG_SAVE, num=1):
    p1 = model(i1, training=True)
    p2 = model(i2, training=True)
    p3 = model(i3, training=True)
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

    GEN_LOSS = generator_loss
    DISC_LOSS = discriminator_loss



    # dataset_name = "facades"
    #
    # _URL = f'http://efrosgans.eecs.berkeley.edu/pix2pix/datasets/{dataset_name}.tar.gz'
    #
    # path_to_zip = tf.keras.utils.get_file(
    #     fname=f"{dataset_name}.tar.gz",
    #     origin=_URL,
    #     extract=True)
    #
    # path_to_zip  = pathlib.Path(path_to_zip)
    #
    # PATH = path_to_zip.parent/dataset_name
    # train_dataset = tf.data.Dataset.list_files(str(PATH / 'train/*.jpg'))
    # train_dataset = train_dataset.map(load_image_train,
    #                                   num_parallel_calls=tf.data.experimental.AUTOTUNE)
    # train_dataset = train_dataset.shuffle(NUM_IMGS)
    # train_dataset = train_dataset.batch(BATCH_SIZE)
    #
    # try:
    #   test_dataset = tf.data.Dataset.list_files(str(PATH / 'test/*.jpg'))
    # except tf.errors.InvalidArgumentError:
    #   test_dataset = tf.data.Dataset.list_files(str(PATH / 'val/*.jpg'))
    # test_dataset = test_dataset.map(load_image_test)
    # test_dataset = test_dataset.batch(BATCH_SIZE)




    steps_per_epoch = NUM_IMGS // BATCH_SIZE

    """
    END TODO
    """
    gen = GeneratorTemplate(IMG_WIDTH, IMG_HEIGHT, INPUT_CHANNEL, OUTPUT_CHANNEL, KERNEL_SIZE, NUM_LAYERS)
    disc = DiscriminatorTemplate(IMG_WIDTH, IMG_HEIGHT, INPUT_CHANNEL, OUTPUT_CHANNEL, KERNEL_SIZE, NUM_LAYERS)
    generator = gen.build()
    discriminator = disc.build()

    train_dataset = get_dataset(DATASET_DIR, BATCH_SIZE, IMG_WIDTH, IMG_HEIGHT, INPUT_CHANNEL)

    train(train_dataset, MAX_EPOCHS, LEARNING_RATE, BETA, GEN_LOSS, DISC_LOSS, LAMBDA, ROOT_IMG_SAVE, ROOT_CHECKPOINT_SAVE, steps_per_epoch, generator, discriminator)

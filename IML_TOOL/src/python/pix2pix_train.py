import tensorflow as tf
import pathlib
import os
import time
from IPython.display import display, clear_output # for outputting
import matplotlib.pyplot as plt
import argparse
import numpy as np
from pix2pix_builder import GeneratorTemplate, DiscriminatorTemplate
from piper import Piper

"""
DATASET --------------------------------------------------------------------------------------
"""

# loads image file and splits into two tensors
def load(image_file):
    # Read and decode an image file to a uint8 tensor
    image = tf.io.read_file(image_file)
    image = tf.image.decode_jpeg(image)

    # Split each image tensor into two tensors:
    # - one with a real building facade image
    # - one with an architecture label image
    w = tf.shape(image)[1]
    w = w // 2
    input_image = image[:, w:, :]
    real_image = image[:, :w, :]

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

def load_image_train(image_file):
    input_image, real_image = load(image_file)
    input_image, real_image = random_jitter(input_image, real_image)
    input_image, real_image = normalize(input_image, real_image)

    return input_image, real_image

def load_image_test(image_file):
    input_image, real_image = load(image_file)
    input_image, real_image = resize(input_image, real_image,
                                   IMG_HEIGHT, IMG_WIDTH)
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
def train_step(input_image, target, step, generator_optimizer, discriminator_optimizer, gen_loss_, disc_loss_, LAMBDA, generator, discriminator):

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

def fit(train_ds, test_ds, steps, g_opt, d_opt, gen_loss_, disc_loss_, LAMBDA, ROOT_IMG_SAVE, checkpoint, gen, disc):

    gen_pipe = Piper("/tmp/gen")
    disc_pipe = Piper("/tmp/disc")

    example_input, example_target = next(iter(test_ds.take(1)))
    start = time.time()

    for step, (input_image, target) in train_ds.repeat().take(steps).enumerate():
        if (step) % 1000 == 0:
              clear_output(wait=True)

              if step != 0:
                  print(f'Time taken for 1000 steps: {time.time()-start:.2f} sec\n')

              start = time.time()

              generate_images(gen, example_input, example_target, step, ROOT_IMG_SAVE)
              print(f"Step: {step//1000}k")

        g_loss, d_loss = train_step(input_image, target, step, g_opt, d_opt, gen_loss_, disc_loss_, LAMBDA, gen, disc)
        val = tf.keras.backend.get_value(g_loss)
        print(val)
        precision = 8
        if(val > 1):
            precision = 9
        # val = np.format_float_positional(val, precision=precision, unique=False, fractional=False, trim='k')
        print(str(val)[:5])
        print("-------------------")
        gen_pipe.send_message(str(val)[:5] + "\n")
        # Training step
        if (step+1) % 10 == 0:
            print('.', end='', flush=True)


        # Save (checkpoint) the model every 5k steps
        if (step + 1) % 5000 == 0:
            checkpoint.save(file_prefix=checkpoint_prefix)




def generate_images(model, test_input, tar, step, ROOT_IMG_SAVE):
    prediction = model(test_input, training=True)
    plt.figure(figsize=(15, 15))

    display_list = [test_input[0], tar[0], prediction[0]]
    title = ['Input Image', 'Ground Truth', 'Predicted Image']
    plt.cla()
    for i in range(3):
        plt.subplot(1, 3, i+1)
        plt.title(title[i])
        # Getting the pixel values in the [0, 1] range to plot.
        plt.imshow(display_list[i] * 0.5 + 0.5)
        plt.axis('off')
    plt.savefig(ROOT_IMG_SAVE+str(step)+".png")

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--max_epochs", help="epochs to train for", type=int, default=10000, required=False)
    parser.add_argument("--batch_size", help="batch size", type=int, default=1, required=False)
    parser.add_argument("--learning_rate", help="learning rate for discriminator and generator", type=float, default=2e-4, required=False)
    parser.add_argument("--image_width", help="image width for input and output", type=int, default=256, required=False)
    parser.add_argument("--image_height", help="image height for input and output", type=int, default=256, required=False)
    parser.add_argument("--input_channel", help="number of channels for input image", type=int, default=3, required=False)
    parser.add_argument("--output_channel", help="number of channels for output image", type=int, default=3, required=False)
    parser.add_argument("--num_layers", help="[advanced] number of layers in generator decoder and encoder", type=int, default=7, required=False)
    parser.add_argument("--kernel_size", help="[advanced] kernel size for convolutional layers", type=int, default=4, required=False)
    parser.add_argument("--beta", help="[advanced] momentum for ADAM optimiser", type=float, default=0.5, required=False)
    parser.add_argument("--lambda_", help="[advanced] variable to improve structural loss", type=int, default=100, required=False)
    parser.add_argument("--img_save_dir", help="Directory to save images to", default="../../bin/data/default_save/", required=False)
    parser.add_argument("--checkpoint_save_dir", help="Directory to save checkpoints to", default="../../bin/data/default_save/", required=False)




    #
    args = parser.parse_args()
    ROOT_IMG_SAVE = args.img_save_dir
    ROOT_CHECKPOINT_SAVE = args.checkpoint_save_dir
    # print(args.echo)

    NUM_IMGS = 400
    MAX_EPOCHS = args.max_epochs
    BATCH_SIZE = args.batch_size
    LEARNING_RATE = args.learning_rate
    IMG_WIDTH = args.image_width
    IMG_HEIGHT = args.image_height
    INPUT_CHANNEL = args.input_channel
    OUTPUT_CHANNEL = args.output_channel

    # advanced:
    NUM_LAYERS = args.num_layers
    KERNEL_SIZE = args.kernel_size
    BETA = args.beta
    LAMBDA = args.lambda_

    GEN_LOSS = generator_loss
    DISC_LOSS = discriminator_loss
    generator_optimizer = tf.keras.optimizers.Adam(LEARNING_RATE, beta_1=BETA)
    discriminator_optimizer = tf.keras.optimizers.Adam(LEARNING_RATE, beta_1=BETA)


    """
    TODO: implement dataset selection
    """
    dataset_name = "facades"

    _URL = f'http://efrosgans.eecs.berkeley.edu/pix2pix/datasets/{dataset_name}.tar.gz'

    path_to_zip = tf.keras.utils.get_file(
        fname=f"{dataset_name}.tar.gz",
        origin=_URL,
        extract=True)

    path_to_zip  = pathlib.Path(path_to_zip)

    PATH = path_to_zip.parent/dataset_name
    train_dataset = tf.data.Dataset.list_files(str(PATH / 'train/*.jpg'))
    train_dataset = train_dataset.map(load_image_train,
                                      num_parallel_calls=tf.data.experimental.AUTOTUNE)
    train_dataset = train_dataset.shuffle(NUM_IMGS)
    train_dataset = train_dataset.batch(BATCH_SIZE)

    try:
      test_dataset = tf.data.Dataset.list_files(str(PATH / 'test/*.jpg'))
    except tf.errors.InvalidArgumentError:
      test_dataset = tf.data.Dataset.list_files(str(PATH / 'val/*.jpg'))
    test_dataset = test_dataset.map(load_image_test)
    test_dataset = test_dataset.batch(BATCH_SIZE)




    """
    END TODO
    """
    gen = GeneratorTemplate(IMG_WIDTH, IMG_HEIGHT, INPUT_CHANNEL, OUTPUT_CHANNEL, KERNEL_SIZE, NUM_LAYERS)
    disc = DiscriminatorTemplate(IMG_WIDTH, IMG_HEIGHT, INPUT_CHANNEL, OUTPUT_CHANNEL, KERNEL_SIZE, NUM_LAYERS)
    generator = gen.build()
    discriminator = disc.build()

    checkpoint_dir = ROOT_CHECKPOINT_SAVE
    checkpoint_prefix = os.path.join(checkpoint_dir, "ckpt")
    checkpoint = tf.train.Checkpoint(generator_optimizer=generator_optimizer,
    discriminator_optimizer=discriminator_optimizer,
    generator=generator,
    discriminator=discriminator)

    fit(train_dataset, test_dataset, MAX_EPOCHS, generator_optimizer, discriminator_optimizer, GEN_LOSS, DISC_LOSS, LAMBDA, ROOT_IMG_SAVE, checkpoint, generator, discriminator)

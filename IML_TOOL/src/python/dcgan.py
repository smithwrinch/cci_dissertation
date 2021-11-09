import numpy as np
import tensorflow as tf
# import tensorview as tv
import glob
import imageio
import matplotlib.pyplot as plt
import numpy as np
import os
import PIL
from tensorflow.keras import layers
import time
import argparse
from math import floor
from IPython import display

from piper import Piper
from lossWriter import Messager
from lossManager import LossManager
from logger import LogManager
from dcgan_builder import GAN
from math import pow
import os
import shutil
# BATCH_SIZE = 256

# EPOCHS = 50
noise_dim = 128
num_examples_to_generate = 16

def load_data(BUFFER_SIZE = 60000, batch_size=100):
    (train_images, train_labels), (_, _) = tf.keras.datasets.mnist.load_data()
    train_images = train_images.reshape(train_images.shape[0], 28, 28, 1).astype('float32')
    train_images = (train_images - 127.5) / 127.5  # Normalize the images to [-1, 1]

    # Batch and shuffle the data
    train_dataset = tf.data.Dataset.from_tensor_slices(train_images).shuffle(BUFFER_SIZE).batch(batch_size)

    return train_dataset



cross_entropy = tf.keras.losses.BinaryCrossentropy(from_logits=True)

def discriminator_loss(real_output, fake_output):
    real_loss = cross_entropy(tf.ones_like(real_output), real_output)
    fake_loss = cross_entropy(tf.zeros_like(fake_output), fake_output)
    total_loss = real_loss + fake_loss
    return total_loss
def generator_loss(fake_output):
    return cross_entropy(tf.ones_like(fake_output), fake_output)

# Notice the use of `tf.function`
# This annotation causes the function to be "compiled".
@tf.function
def train_step(images, generator, discriminator, generator_optimizer, discriminator_optimizer, latent_dim, batch_size):
    noise = tf.random.normal([batch_size, latent_dim])
    with tf.GradientTape() as gen_tape, tf.GradientTape() as disc_tape:
      generated_images = generator(noise, training=True)

      real_output = discriminator(images, training=True)
      fake_output = discriminator(generated_images, training=True)

      gen_loss = generator_loss(fake_output)
      disc_loss = discriminator_loss(real_output, fake_output)
    gradients_of_generator = gen_tape.gradient(gen_loss, generator.trainable_variables)
    gradients_of_discriminator = disc_tape.gradient(disc_loss, discriminator.trainable_variables)

    generator_optimizer.apply_gradients(zip(gradients_of_generator, generator.trainable_variables))
    discriminator_optimizer.apply_gradients(zip(gradients_of_discriminator, discriminator.trainable_variables))

    return gen_loss, disc_loss

def train(dataset, epochs, seed, img_size, img_channel, latent_dim, kernel_size,
batch_size, learning_rate, root_checkpoint_save, root_img_save, log_msg,
RANDOM_HORIZONTAL, RANDOM_VERTICAL, RANDOM_CROP, RANDOM_BRIGHTNESS, RANDOM_CONTRAST,
DISC_NOISE):

    generator_optimizer = tf.keras.optimizers.Adam(learning_rate)
    discriminator_optimizer = tf.keras.optimizers.Adam(learning_rate)

    gan = GAN(img_size, img_channel, kernel_size, latent_dim, DISC_NOISE)
    generator = gan.generator
    discriminator = gan.discriminator

    save_dir = root_checkpoint_save+"ckpt"
    save = glob.glob(save_dir+"/*")
    epochs_passed = 0
    if not os.path.exists(save_dir):
        print("TRAINING FROM SCRATCH")
        log_msg.send("Training from scratch")
        os.makedirs(save_dir)
    elif(len(save) !=0):
        print(save)
        if(len(save) != 2):
            nums = [s.split("/")[-1].split("_")[0][1:] for s in save]

            for i, n in enumerate(nums):
                if(nums.count(n) != 2):
                    save.pop(i)

        print("RESTORING FROM BACKUP")
        log_msg.send("Restoring from saved")
        print(save)
        save_ = save[0].split("/")[-1].split("_")[0][1:]
        epochs_passed = int(save_)

        if("discriminator" in save[0]):
            discriminator = tf.keras.models.load_model(save[0])
            generator = tf.keras.models.load_model(save[1])
        else:
            discriminator = tf.keras.models.load_model(save[1])
            generator = tf.keras.models.load_model(save[0])

    # noise = tf.random.normal([1, 100])
    # generated_image = generator(noise, training=False)

    # discriminator = make_discriminator_model(28, 1, noise_dim)
    # decision = discriminator(generated_image)

    lm = LossManager(ROOT_CHECKPOINT_SAVE+"losses/")
    gen_pipe = Piper("/tmp/gen")
    disc_pipe = Piper("/tmp/disc")

    gen_msg = Messager(root_checkpoint_save+"gen.txt")
    disc_msg = Messager(root_checkpoint_save+"disc.txt")



    # generate_and_save_images(generator, seed, root_img_save+str(epochs_passed+1))

    if not os.path.exists(ROOT_IMG_SAVE[:-1]):
        os.makedirs(ROOT_IMG_SAVE[:-1])

    lossG = []
    lossD = []

    log_msg.send("Beginning training...")
    for epoch in range(epochs):

        log_msg.send("Epoch " + str(epoch+epochs_passed))
        generate_and_save_images(generator, seed, root_img_save+str(epoch+epochs_passed), img_channel)
        files = glob.glob(save_dir+"/*")
        generator.save(save_dir+"/-" +str(epoch+epochs_passed)+"_generator")
        discriminator.save(save_dir+"/-" +str(epoch+epochs_passed)+"_discriminator")

        shutil.rmtree(save_dir+"/-" +str(epoch+epochs_passed-1)+"_generator", ignore_errors=True)
        shutil.rmtree(save_dir+"/-" +str(epoch+epochs_passed-1)+"_discriminator", ignore_errors=True)
        start = time.time()

        for n, image_batch in enumerate(dataset):
            image_batch = apply_augmentations(image_batch, RANDOM_HORIZONTAL, RANDOM_VERTICAL, RANDOM_CROP, RANDOM_BRIGHTNESS, RANDOM_CONTRAST)

            # print(n)
            g_loss, d_loss = train_step(image_batch, generator, discriminator, generator_optimizer, discriminator_optimizer, latent_dim, batch_size)
            g_loss = g_loss.numpy()
            d_loss = d_loss.numpy()
            gen_msg.send_message(str(g_loss)[:5] + "\n")
            disc_msg.send_message(str(d_loss)[:5] + "\n")
            lossG.append(g_loss)
            lossD.append(d_loss)


        lm.addGeneratorLosses(np.array(lossG))
        lm.addDiscriminatorLosses(np.array(lossD))
        lm.generate_graph(ROOT_CHECKPOINT_SAVE)
        lm.generate_graphD(ROOT_CHECKPOINT_SAVE)
        lm.generate_graphG(ROOT_CHECKPOINT_SAVE)

        display.clear_output(wait=True)
        # generate_and_save_images(generator, seed)


        print ('Time for epoch {} is {} sec'.format(epoch + 1 + epochs_passed, time.time()-start))
    # Generate after the final epoch
    display.clear_output(wait=True)
    # generate_and_save_images(generator, epochs, seed)


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


def generate_and_save_images(model, test_input,filepath='../bin/data/images_/current', img_channel=1):
  # Notice `training` is set to False.
  # This is so all layers run in inference mode (batchnorm).
  predictions = model(test_input, training=False)

  fig = plt.figure(figsize=(4, 4))

  for i in range(predictions.shape[0]):
      plt.subplot(4, 4, i+1)
      if(img_channel == 1):
          plt.imshow(predictions[i, :, :, 0] * 127.5 + 127.5, cmap='gray')
      else:
          plt.imshow(predictions[i, :, :, :]* 0.5 + 0.5)
      plt.axis('off')

  plt.savefig(filepath+'.png')
  plt.close(fig)
  # plt.show()

def process(image):
    image = tf.cast((image-127.5)/127.5,tf.float32)
    return image


def get_dataset(dir, batch_size, image_width, image_height, image_channels):
    colour_mode = "rgb"
    if(image_channels == 1):
        colour_mode = "grayscale"

    print("colour mode " + colour_mode)
    train_images = tf.keras.utils.image_dataset_from_directory(dir, label_mode = None, batch_size=batch_size,
    image_size=(image_width, image_height), color_mode = colour_mode)


    # train_images = train_images.astype('float32')
    train_dataset  = train_images.map(process)
    # TODO image augmentations
    return train_dataset

from tensorflow.python.client import device_lib
if __name__ == '__main__':


    # You will reuse this seed overtime (so it's easier)
    # to visualize progress in the animated GIF)


    parser = argparse.ArgumentParser()
    parser.add_argument("--max_epochs", help="epochs to train for", type=int, default=50, required=False)
    parser.add_argument("--batch_size", help="batch size", type=int, default=256, required=False)
    parser.add_argument("--learning_rateX", help="X: Xe^Y learning rate for discriminator and generator", type=int, default=1, required=False)
    parser.add_argument("--learning_rateY", help="Y: Xe^Y learning rate for discriminator and generator", type=int, default=-4, required=False)
    parser.add_argument("--img_size", help="(size x size) pixels", type=int, default=28, required=False)
    parser.add_argument("--img_channel", help="number of channels for output image", type=int, default=1, required=False)
    parser.add_argument("--latent_dim", help="[advanced] latent vector length", type=int, default=128, required=False)
    parser.add_argument("--kernel_size", help="[advanced] kernel size for convolutional layers", type=int, default=4, required=False)
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
    ROOT_IMG_SAVE = args.img_save_dir
    ROOT_CHECKPOINT_SAVE = args.checkpoint_save_dir

    DATASET_DIR = args.dataset_dir
    # print(args.echo)

    MAX_EPOCHS = args.max_epochs
    BATCH_SIZE = args.batch_size
    LEARNING_RATE = args.learning_rateX * pow(10, args.learning_rateY)
    IMG_SIZE = args.img_size
    IMG_CHANNEL = args.img_channel

    # advanced:
    KERNEL_SIZE = args.kernel_size
    LATENT_DIM = args.latent_dim

    DISC_NOISE =args.disc_noise
    RANDOM_HORIZONTAL = args.random_horizontal == 1
    RANDOM_VERTICAL = args.random_vertical == 1
    RANDOM_CROP = args.random_crop / 100
    RANDOM_BRIGHTNESS = args.random_brightness
    RANDOM_CONTRAST = args.random_contrast

    log_msg = LogManager(ROOT_CHECKPOINT_SAVE+"log.txt")
    log_msg.send("Initialising...")

    seed = tf.random.normal([num_examples_to_generate, LATENT_DIM])

    # train_dataset = load_data(60000, BATCH_SIZE)
    train_dataset = get_dataset(DATASET_DIR, BATCH_SIZE, IMG_SIZE, IMG_SIZE, IMG_CHANNEL)

    if(len(train_dataset) == 0):
        log_msg.send("[ERROR] No images found in dataset at " + DATASET_DIR)
        exit(0)
    else:
        log_msg.send("Found " + str(len(train_dataset)) + " images...")

    train(train_dataset, MAX_EPOCHS, seed, IMG_SIZE, IMG_CHANNEL, LATENT_DIM,
    KERNEL_SIZE, BATCH_SIZE, LEARNING_RATE, ROOT_CHECKPOINT_SAVE, ROOT_IMG_SAVE,
    log_msg, RANDOM_HORIZONTAL, RANDOM_VERTICAL, RANDOM_CROP, RANDOM_BRIGHTNESS,
    RANDOM_CONTRAST, DISC_NOISE)

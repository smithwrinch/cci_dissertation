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

from IPython import display

from piper import Piper
from dcgan_builder import GAN
from math import pow
import os

# BATCH_SIZE = 256

# EPOCHS = 50
noise_dim = 128
num_examples_to_generate = 16

def load_data(BUFFER_SIZE = 60000):
    (train_images, train_labels), (_, _) = tf.keras.datasets.mnist.load_data()
    train_images = train_images.reshape(train_images.shape[0], 28, 28, 1).astype('float32')
    train_images = (train_images - 127.5) / 127.5  # Normalize the images to [-1, 1]

    # Batch and shuffle the data
    train_dataset = tf.data.Dataset.from_tensor_slices(train_images).shuffle(BUFFER_SIZE).batch(BATCH_SIZE)

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

def train(dataset, epochs, seed, img_size, img_channel, latent_dim, kernel_size, batch_size, learning_rate, root_checkpoint_save, root_img_save):

    generator_optimizer = tf.keras.optimizers.Adam(learning_rate)
    discriminator_optimizer = tf.keras.optimizers.Adam(learning_rate)

    gan = GAN(img_size, img_channel, kernel_size, latent_dim)
    generator = gan.generator
    discriminator = gan.discriminator

    save_dir = ROOT_CHECKPOINT_SAVE+"ckpt"
    epochs_passed = 0
    if not os.path.exists(save_dir):
        print("TRAINING FROM SCRATCH")
        os.makedirs(save_dir)
    else:
        save = glob.glob(save_dir+"/*.h5")
        if(len(save) == 2):
            print("RESTORING FROM BACKUP")
            save_ = save[0].split("/")[-1].split(".")[0][1:]
            epochs_passed = int(save_)


            if("discriminator.h5" in save[0]):
                discriminator = tf.keras.models.load_model(save[0])
                generator = tf.keras.models.load_model(save[1])
            else:
                disc = tf.keras.models.load_model(save[1])
                discriminator = tf.keras.models.load_model(save[0])



    # noise = tf.random.normal([1, 100])
    # generated_image = generator(noise, training=False)

    # discriminator = make_discriminator_model(28, 1, noise_dim)
    # decision = discriminator(generated_image)

    gen_pipe = Piper("/tmp/gen")
    disc_pipe = Piper("/tmp/disc")



    # generate_and_save_images(generator, seed, root_img_save+str(epochs_passed+1))

    if not os.path.exists(ROOT_IMG_SAVE[:-1]):
        os.makedirs(ROOT_IMG_SAVE[:-1])

    for epoch in range(epochs):
        generate_and_save_images(generator, seed, root_img_save+str(epoch+epochs_passed+1))
        files = glob.glob(save_dir+"/*")
        for f in files:
            os.remove(f)
        generator.save(save_dir+"/-" +str(epoch+epochs_passed+1)+".generator.h5")
        discriminator.save(save_dir+"/-" +str(epoch+epochs_passed+1)+".discriminator.h5")
        start = time.time()

        for n, image_batch in enumerate(dataset):
            # print(n)
            g_loss, d_loss = train_step(image_batch, generator, discriminator, generator_optimizer, discriminator_optimizer, latent_dim, batch_size)
            g_loss = g_loss.numpy()
            d_loss = d_loss.numpy()
            gen_pipe.send_message(str(g_loss)[:5] + "\n")
            disc_pipe.send_message(str(d_loss)[:5] + "\n")


        display.clear_output(wait=True)
        # generate_and_save_images(generator, seed)


        print ('Time for epoch {} is {} sec'.format(epoch + 1 + epochs_passed, time.time()-start))
    # Generate after the final epoch
    display.clear_output(wait=True)
    # generate_and_save_images(generator, epochs, seed)

def generate_and_save_images(model, test_input,filepath='../bin/data/images_/current'):
  # Notice `training` is set to False.
  # This is so all layers run in inference mode (batchnorm).
  predictions = model(test_input, training=False)

  fig = plt.figure(figsize=(4, 4))

  for i in range(predictions.shape[0]):
      plt.subplot(4, 4, i+1)
      plt.imshow(predictions[i, :, :, 0] * 127.5 + 127.5, cmap='gray')
      plt.axis('off')

  plt.savefig(filepath+'.png')
  # plt.show()

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


    args = parser.parse_args()
    ROOT_IMG_SAVE = args.img_save_dir
    ROOT_CHECKPOINT_SAVE = args.checkpoint_save_dir
    # print(args.echo)

    MAX_EPOCHS = args.max_epochs
    BATCH_SIZE = args.batch_size
    LEARNING_RATE = args.learning_rateX * pow(10, args.learning_rateY)
    IMG_SIZE = args.img_size
    IMG_CHANNEL = args.img_channel

    # advanced:
    KERNEL_SIZE = args.kernel_size
    LATENT_DIM = args.latent_dim

    seed = tf.random.normal([num_examples_to_generate, LATENT_DIM])

    train_dataset = load_data(60000)

    print("LEARNIGN RATE " + str(LEARNING_RATE))


    train(train_dataset, MAX_EPOCHS, seed, IMG_SIZE, IMG_CHANNEL, LATENT_DIM, KERNEL_SIZE, BATCH_SIZE, LEARNING_RATE, ROOT_CHECKPOINT_SAVE, ROOT_IMG_SAVE)

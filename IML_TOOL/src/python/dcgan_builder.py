import tensorflow as tf

from tensorflow.keras import layers
def make_generator_model(img_size, img_channel = 3, latent_vector = 128, kernel_size=4):

    model = tf.keras.Sequential()
    if(img_size == 28):
        model.add(tf.keras.Input(shape=(latent_vector,)))
        model.add(layers.Dense(7*7*256, use_bias=False ))
        model.add(layers.BatchNormalization())
        model.add(layers.LeakyReLU())

        model.add(layers.Reshape((7, 7, 256)))
        assert model.output_shape == (None, 7, 7, 256)  # Note: None is the batch size

        model.add(layers.Conv2DTranspose(128, kernel_size, strides=(1, 1), padding='same', use_bias=False))
        assert model.output_shape == (None, 7, 7, 128)
        model.add(layers.BatchNormalization())
        model.add(layers.LeakyReLU())

        model.add(layers.Conv2DTranspose(64, kernel_size, strides=(2, 2), padding='same', use_bias=False))
        assert model.output_shape == (None, 14, 14, 64)
        model.add(layers.BatchNormalization())
        model.add(layers.LeakyReLU())

        model.add(layers.Conv2DTranspose(img_channel, kernel_size, strides=(2, 2), padding='same', use_bias=False, activation='tanh'))
        print(model.output_shape)
        assert model.output_shape == (None, 28, 28, img_channel)

    elif(img_size == 64):

        model.add(tf.keras.Input(shape=(latent_vector,)))
        model.add(layers.Dense(8*8*128, use_bias=False))
        model.add(layers.BatchNormalization())
        model.add(layers.LeakyReLU())

        model.add(layers.Reshape((8, 8, 128)))


        model.add(layers.Conv2DTranspose(128, kernel_size, strides=2, padding='same', use_bias=False))
        model.add(layers.BatchNormalization())
        model.add(layers.LeakyReLU())

        model.add(layers.Conv2DTranspose(256, kernel_size, strides=2, padding='same', use_bias=False))
        model.add(layers.BatchNormalization())
        model.add(layers.LeakyReLU())

        model.add(layers.Conv2DTranspose(512, kernel_size, strides=2, padding='same', use_bias=False))
        model.add(layers.BatchNormalization())
        model.add(layers.LeakyReLU())

        model.add(layers.Conv2DTranspose(img_channel, kernel_size, padding='same', use_bias=False, activation='tanh'))
        assert model.output_shape == (None, 64, 64, img_channel)

    return model


    # if(img_size == 128):
    #     model.add(layers.Dense(7*7*256, use_bias=False, input_shape=(latent_vector,)))
    #     model.add(layers.BatchNormalization())
    #     model.add(layers.LeakyReLU())
    #
    #     model.add(layers.Reshape((7, 7, 256)))
    #
    #
    #
    #     model.add(layers.Conv2DTranspose(512, kernel_size, strides=(1, 1), padding='same', use_bias=False))
    #     model.add(layers.BatchNormalization())
    #     model.add(layers.LeakyReLU())
    #
    #
    #     model.add(layers.Conv2DTranspose(256, kernel_size, strides=(1, 1), padding='same', use_bias=False))
    #     model.add(layers.BatchNormalization())
    #     model.add(layers.LeakyReLU())
    #
    #     model.add(layers.Conv2DTranspose(128, kernel_size, strides=(1, 1), padding='same', use_bias=False))
    #     model.add(layers.BatchNormalization())
    #     model.add(layers.LeakyReLU())
    #
    #     model.add(layers.Conv2DTranspose(64, kernel_size, strides=(2, 2), padding='same', use_bias=False))
    #     model.add(layers.BatchNormalization())
    #     model.add(layers.LeakyReLU())
    #
    #     model.add(layers.Conv2DTranspose(1, kernel_size, strides=(2, 2), padding='same', use_bias=False, activation='tanh'))
    #     assert model.output_shape == (None, 64, 64, img_channel)
    #
    #     return model



def make_discriminator_model(img_size, img_channel=3, kernel_size=5):


    model = tf.keras.Sequential()
    if(img_size == 28):
        model.add(layers.Conv2D(64, kernel_size, strides=(2, 2), padding='same',
                                         input_shape=[28, 28, img_channel]))

        model.add(layers.LeakyReLU())
        model.add(layers.Dropout(0.3))

        model.add(layers.Conv2D(128, kernel_size, strides=(2, 2), padding='same'))
        model.add(layers.LeakyReLU())
        model.add(layers.Dropout(0.3))

        model.add(layers.Flatten())
        model.add(layers.Dense(1))

    elif(img_size == 64):
        model.add(layers.Conv2D(64, kernel_size, strides=(2, 2), padding='same',
                                         input_shape=[64, 64, img_channel]))

        model.add(layers.LeakyReLU())
        model.add(layers.Dropout(0.3))

        model.add(layers.Conv2D(128, kernel_size, strides=(2, 2), padding='same'))
        model.add(layers.LeakyReLU())
        model.add(layers.Dropout(0.3))

        model.add(layers.Conv2D(128, kernel_size, strides=(2, 2), padding='same'))
        model.add(layers.LeakyReLU())
        model.add(layers.Dropout(0.3))

        model.add(layers.Flatten())
        model.add(layers.Dense(1))
    return model


class GAN():
    def __init__(self, img_size, img_channel, kernel_size, latent_dim):
        self.generator = make_generator_model(img_size, img_channel, latent_dim, kernel_size)
        self.discriminator = make_discriminator_model(img_size, img_channel, kernel_size)



if __name__ == '__main__':
    pass

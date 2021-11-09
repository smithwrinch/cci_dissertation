import tensorflow as tf

"""
Encoder  ---------------------------------------------------------------------------------
"""
def downsample(filters, size, apply_batchnorm=True):
    initializer = tf.random_normal_initializer(0., 0.02)

    result = tf.keras.Sequential()
    result.add(
      tf.keras.layers.Conv2D(filters, size, strides=2, padding='same',
                             kernel_initializer=initializer, use_bias=False))

    if apply_batchnorm:
        result.add(tf.keras.layers.BatchNormalization())

        result.add(tf.keras.layers.LeakyReLU())

    return result


"""
Decoder  ---------------------------------------------------------------------------------
"""
def upsample(filters, size, apply_dropout=False):
    initializer = tf.random_normal_initializer(0., 0.02)

    result = tf.keras.Sequential()
    result.add(
    tf.keras.layers.Conv2DTranspose(filters, size, strides=2,
                                    padding='same',
                                    kernel_initializer=initializer,
                                    use_bias=False))

    result.add(tf.keras.layers.BatchNormalization())

    if apply_dropout:
      result.add(tf.keras.layers.Dropout(0.5))

    result.add(tf.keras.layers.ReLU())

    return result


def Generator(img_width, img_height, input_channel, output_channel, kernel_size, num_layers):
    inputs = tf.keras.layers.Input(shape=[img_width, img_height, input_channel])

    # 32x32 (untested)
    down_stack32 = [
        downsample(64, kernel_size, apply_batchnorm=False),  # (batch_size, 128, 128, 64)
        downsample(128, kernel_size),  # (batch_size, 64, 64, 128)
        downsample(256, kernel_size),  # (batch_size, 32, 32, 256)
        downsample(512, kernel_size),  # (batch_size, 8, 8, 512)
        downsample(512, kernel_size),  # (batch_size, 2, 2, 512)
    ]
    # 64x64 (untested)
    down_stack64 = [
        downsample(64, kernel_size, apply_batchnorm=False),  # (batch_size, 128, 128, 64)
        downsample(128, kernel_size),  # (batch_size, 64, 64, 128)
        downsample(256, kernel_size),  # (batch_size, 32, 32, 256)
        downsample(512, kernel_size),  # (batch_size, 8, 8, 512)
        downsample(512, kernel_size),  # (batch_size, 4, 4, 512)
        downsample(512, kernel_size),  # (batch_size, 2, 2, 512)
    ]
    # 128x128 (untested)
    down_stack128 = [
        downsample(64, kernel_size, apply_batchnorm=False),  # (batch_size, 128, 128, 64)
        downsample(128, kernel_size),  # (batch_size, 64, 64, 128)
        downsample(256, kernel_size),  # (batch_size, 32, 32, 256)
        downsample(512, kernel_size),  # (batch_size, 16, 16, 512)
        downsample(512, kernel_size),  # (batch_size, 8, 8, 512)
        downsample(512, kernel_size),  # (batch_size, 4, 4, 512)
        downsample(512, kernel_size),  # (batch_size, 2, 2, 512)
    ]

    # 256x256
    down_stack256 = [
        downsample(64, kernel_size, apply_batchnorm=False),  # (batch_size, 128, 128, 64)
        downsample(128, kernel_size),  # (batch_size, 64, 64, 128)
        downsample(256, kernel_size),  # (batch_size, 32, 32, 256)
        downsample(512, kernel_size),  # (batch_size, 16, 16, 512)
        downsample(512, kernel_size),  # (batch_size, 8, 8, 512)
        downsample(512, kernel_size),  # (batch_size, 4, 4, 512)
        downsample(512, kernel_size),  # (batch_size, 2, 2, 512)
        downsample(512, kernel_size),  # (batch_size, 1, 1, 512)
    ]

    # 512x512 (untested)
    down_stack512 = [
        downsample(64, kernel_size, apply_batchnorm=False),  # (batch_size, 128, 128, 64)
        downsample(128, kernel_size),  # (batch_size, 64, 64, 128)
        downsample(256, kernel_size),  # (batch_size, 32, 32, 256)
        downsample(256, kernel_size),  # (batch_size, 16, 16, 512)
        downsample(512, kernel_size),  # (batch_size, 8, 8, 512)
        downsample(512, kernel_size),  # (batch_size, 4, 4, 512)
        downsample(512, kernel_size),  # (batch_size, 2, 2, 512)
        downsample(512, kernel_size),  # (batch_size, 1, 1, 512)
        downsample(1024, kernel_size),  # (batch_size, 1, 1, 512)
    ]


    """
    architecture from:
     https://github.com/karolmajek/face2face-demo/blob/master/reduce_model.py
    """
    down_stack1024 = [
        downsample(64, kernel_size, apply_batchnorm=False),
        downsample(128, kernel_size),
        downsample(256, kernel_size),
        downsample(512, kernel_size),
        downsample(512, kernel_size),
        downsample(1024, kernel_size),
        downsample(1024, kernel_size),
        downsample(1024, kernel_size),
        downsample(1024, kernel_size),
        downsample(2048, kernel_size)
        # ngf * 2, # encoder_2: [batch, 512, 512, ngf] => [batch, 256, 256, ngf * 2]
        # # ngf * 2, # encoder_2: [batch, 128, 128, ngf] => [batch, 64, 64, ngf * 2]
        # # ngf * 4, # encoder_3: [batch, 64, 64, ngf * 2] => [batch, 32, 32, ngf * 4]
        # ngf * 4, # encoder_3: [batch, 256, 256, ngf * 2] => [batch, 128, 128, ngf * 4]
        # ngf * 8, # encoder_4: [batch, 128, 128, ngf * 4] => [batch, 64, 64, ngf * 8]
        # ngf * 8, # encoder_4: [batch, 64, 64, ngf * 4] => [batch, 32, 32, ngf * 8]
        # ngf * 16, # encoder_4: [batch, 32, 32, ngf * 4] => [batch, 16, 16, ngf * 8]
        # ngf * 16, # encoder_5: [batch, 16, 16, ngf * 8] => [batch, 8, 8, ngf * 8]
        # ngf * 16, # encoder_6: [batch, 8, 8, ngf * 8] => [batch, 4, 4, ngf * 8]
        # ngf * 16, # encoder_7: [batch, 4, 4, ngf * 8] => [batch, 2, 2, ngf * 8]
        # ngf * 32, # encoder_8: [batch, 2, 2, ngf * 8] => [batch, 1, 1, ngf * 8]
    ]

    # down_stack = [
    # downsample(input_size // 4, kernel_size, apply_batchnorm=False)
    # ]
    # current_feature_size = input_size //2
    # current_dim = input_size // 4
    # for i in range(3):
    #     down_stack.append(downsample(current_feature_size, kernel_size))
    #     current_feature_size = current_feature_size * 2
    #     current_dim = current_dim // 2
    #
    # while(current_dim > 0):
    #     current_dim = current_dim // 2
    #     down_stack.append(downsample(current_feature_size, kernel_size))
    #
    # print(len(down_stack))

    # assert 2**len(down_stack) == input_size , "Generator encoder initialised wrong!"
    # current_dim = 1

    # 32x32 (untested)
    up_stack32 = [
    upsample(512, 4, apply_dropout=True),  # (batch_size, 8, 8, 1024)
    upsample(256, 4),  # (batch_size, 32, 32, 512)
    upsample(128, 4),  # (batch_size, 64, 64, 256)
    upsample(64, 4)  # (batch_size, 128, 128, 128)
    ]
    # 64x64 (untested)
    up_stack64 = [
    upsample(512, 4, apply_dropout=True),  # (batch_size, 2, 2, 1024)
    upsample(512, 4, apply_dropout=True),  # (batch_size, 8, 8, 1024)
    upsample(256, 4),  # (batch_size, 32, 32, 512)
    upsample(128, 4),  # (batch_size, 64, 64, 256)
    upsample(64, 4)  # (batch_size, 128, 128, 128)
    ]
    # 128x128 (untested)
    up_stack128 = [
    upsample(512, 4, apply_dropout=True),  # (batch_size, 2, 2, 1024)
    upsample(512, 4, apply_dropout=True),  # (batch_size, 8, 8, 1024)
    upsample(512, 4, apply_dropout=True),  # (batch_size, 16, 16, 1024)
    upsample(256, 4),  # (batch_size, 32, 32, 512)
    upsample(128, 4),  # (batch_size, 64, 64, 256)
    upsample(64, 4)  # (batch_size, 128, 128, 128)
    ]

    # 256x256
    up_stack256 = [
    upsample(512, 4, apply_dropout=True),  # (batch_size, 2, 2, 1024)
    upsample(512, 4, apply_dropout=True),  # (batch_size, 4, 4, 1024)
    upsample(512, 4, apply_dropout=True),  # (batch_size, 8, 8, 1024)
    upsample(512, 4),  # (batch_size, 16, 16, 1024)
    upsample(256, 4),  # (batch_size, 32, 32, 512)
    upsample(128, 4),  # (batch_size, 64, 64, 256)
    upsample(64, 4)  # (batch_size, 128, 128, 128)
    ]

    # 512x512 (untested)
    up_stack512 = [
    upsample(512, 4, apply_dropout=True),  # (batch_size, 2, 2, 1024)
    upsample(512, 4, apply_dropout=True),  # (batch_size, 4, 4, 1024)
    upsample(512, 4, apply_dropout=True),  # (batch_size, 8, 8, 1024)
    upsample(512, 4, apply_dropout=True),  # (batch_size, 16, 16, 1024)
    upsample(256, 4),  # (batch_size, 32, 32, 512)
    upsample(256, 4),  # (batch_size, 32, 32, 512)
    upsample(128, 4),  # (batch_size, 64, 64, 256)
    upsample(64, 4)  # (batch_size, 128, 128, 128)
    ]

    """
    architecture from:
     https://github.com/karolmajek/face2face-demo/blob/master/reduce_model.py
    """
    up_stack1024 = [
    upsample(1024, kernel_size, apply_dropout=True),
    upsample(1024, kernel_size,apply_dropout=True),
    upsample(1024, kernel_size, apply_dropout=True),
    upsample(1024, kernel_size,apply_dropout=True),
    upsample(512, kernel_size,apply_dropout=True),
    upsample(512, kernel_size,apply_dropout=True),
    upsample(256, kernel_size,apply_dropout=False),
    upsample(128, kernel_size,apply_dropout=False),
    upsample(64, kernel_size,apply_dropout=False),

    ]
    # layer_specs = [
    #     (ngf * 16, 0.5),   # decoder_8: [batch, 1, 1, ngf * 8] => [batch, 2, 2, ngf * 8 * 2]
    #     (ngf * 16, 0.5),   # decoder_7: [batch, 2, 2, ngf * 8 * 2] => [batch, 4, 4, ngf * 8 * 2]
    #     (ngf * 16, 0.5),   # decoder_6: [batch, 4, 4, ngf * 8 * 2] => [batch, 8, 8, ngf * 8 * 2]
    #     (ngf * 16, 0.5),   # decoder_5: [batch, 8, 8, ngf * 8 * 2] => [batch, 16, 16, ngf * 8 * 2]
    #     (ngf * 8, 0.5),   # decoder_5: [batch, 16, 16, ngf * 8 * 2] => [batch, 32, 32, ngf * 8 * 2]
    #     (ngf * 8, 0.5),   # decoder_5: [batch, 32, 32, ngf * 8 * 2] => [batch, 64, 64, ngf * 8 * 2]
    #     # (ngf * 4, 0.0),   # decoder_4: [batch, 16, 16, ngf * 8 * 2] => [batch, 32, 32, ngf * 4 * 2]
    #     (ngf * 4, 0.0),   # decoder_4: [batch, 64, 64, ngf * 8 * 2] => [batch, 128, 128, ngf * 4 * 2]
    #     # (ngf * 2, 0.0),   # decoder_3: [batch, 32, 32, ngf * 4 * 2] => [batch, 64, 64, ngf * 2 * 2]
    #     (ngf * 2, 0.0),   # decoder_3: [batch, 128, 128, ngf * 4 * 2] => [batch, 256, 256, ngf * 2 * 2]
    #     (ngf, 0.0),       # decoder_2: [batch, 256, 256, ngf * 2 * 2] => [batch, 512, 512, ngf * 2]
    # ]
    # up_stack = []
    #
    # for i in range(3):
    #     up_stack.append(upsample(input_size * 2, kernel_size, apply_dropout=True))
    #     current_dim *= 2
    #
    # print(current_dim)
    # while(current_dim < input_size):
    #     up_stack.append((upsample(current_feature_size, kernel_size)))
    #     current_feature_size = current_feature_size // 2
    #
    #     current_dim *= 2


    initializer = tf.random_normal_initializer(0., 0.02)
    last = tf.keras.layers.Conv2DTranspose(output_channel, kernel_size,
                                         strides=2,
                                         padding='same',
                                         kernel_initializer=initializer,
                                         activation='tanh')  # (batch_size, 256, 256, 3)
    x = inputs

    if(num_layers == 4):
        down_stack = down_stack32
        up_stack = up_stack32
    elif(num_layers == 5):
        down_stack = down_stack64
        up_stack = up_stack64
    elif(num_layers == 6):
        down_stack = down_stack128
        up_stack = up_stack128
    elif(num_layers == 7):
        down_stack = down_stack256
        up_stack = up_stack256
    elif(num_layers == 8):
        down_stack = down_stack512
        up_stack = up_stack512
    elif(num_layers == 9):
        down_stack = down_stack1024
        up_stack = up_stack1024

    # Downsampling through the model
    skips = []
    for down in down_stack:
        x = down(x)
        skips.append(x)

    skips = reversed(skips[:-1])

    # Upsampling and establishing the skip connections
    for up, skip in zip(up_stack, skips):
        x = up(x)
        x = tf.keras.layers.Concatenate()([x, skip])

    x = last(x)

    return tf.keras.Model(inputs=inputs, outputs=x)


class GeneratorTemplate():
    def __init__(self, img_width, img_height, input_channel, output_channel, kernel_size, num_layers):
        self.img_width = img_width
        self.img_height = img_height
        self.input_channel = input_channel
        self.output_channel = output_channel
        self.num_layers = num_layers
        self.kernel_size = kernel_size

    def build(self):
        print("Building Generator")
        self.generator = Generator(self.img_width, self.img_height,
            self.input_channel, self.output_channel, self.kernel_size, self.num_layers)
        return self.generator
    def __str__(self):
        self.generator.summary()
        return ""


#
"""
Building DISCRIMINATOR ---------------------------------------------------------------------------------
"""

def Discriminator(img_width, img_height, input_channel, output_channel, kernel_size, num_layers):
    initializer = tf.random_normal_initializer(0., 0.02)

    inp = tf.keras.layers.Input(shape=[img_width, img_height, input_channel], name='input_image')
    tar = tf.keras.layers.Input(shape=[img_width, img_height, output_channel], name='target_image')

    x = tf.keras.layers.concatenate([inp, tar])  # (batch_size, 256, 256, channels*2)


    # down = downsample(64, kernel_size, False)(x)  # (batch_size, 128, 128, 64)
    # feature_length = 64
    # print(2**num_layers)
    # while(feature_length < 2**(num_layers+1)):
    #     feature_length *= 2
    #     print (feature_length)
    #     down = downsample(feature_length, kernel_size)(down)

    down1 = downsample(64, 4, False)(x)  # (batch_size, 128, 128, 64)
    down2 = downsample(128, 4)(down1)  # (batch_size, 64, 64, 128)
    down = downsample(256, 4)(down2)  # (batch_size, 32, 32, 256)

    zero_pad1 = tf.keras.layers.ZeroPadding2D()(down)  # (batch_size, 34, 34, 256)
    conv = tf.keras.layers.Conv2D(512, 4, strides=1,
                                kernel_initializer=initializer,
                                use_bias=False)(zero_pad1)  # (batch_size, 31, 31, 512)

    batchnorm1 = tf.keras.layers.BatchNormalization()(conv)

    leaky_relu = tf.keras.layers.LeakyReLU()(batchnorm1)

    zero_pad2 = tf.keras.layers.ZeroPadding2D()(leaky_relu)  # (batch_size, 33, 33, 512)

    last = tf.keras.layers.Conv2D(1, 4, strides=1,
                                kernel_initializer=initializer)(zero_pad2)  # (batch_size, 30, 30, 1)

    return tf.keras.Model(inputs=[inp, tar], outputs=last)

class DiscriminatorTemplate():
    def __init__(self, img_width, img_height, input_channel, output_channel, kernel_size, num_layers):
        self.img_width = img_width
        self.img_height = img_height
        self.input_channel = input_channel
        self.output_channel = output_channel
        self.num_layers = num_layers
        self.kernel_size = kernel_size

    def build(self):
        print("Building Discriminator")
        self.discriminator = Discriminator(self.img_width, self.img_height,
            self.input_channel, self.output_channel, self.kernel_size, self.num_layers)
        return self.discriminator
    def __str__(self):
        self.discriminator.summary()
        return ""
if __name__ == '__main__':
    pass
    # img_width = 256
    # img_height = 256
    # input_channel = 3
    # output_channel = 3
    # kernel_size = 4
    # num_layers = 8
    # # gen = GeneratorTemplate(img_width, img_height, input_channel, output_channel, kernel_size, num_layers)
    # # gen.build()
    # # print(gen)
    # disc = DiscriminatorTemplate(img_width, img_height, input_channel, output_channel, kernel_size, num_layers)
    # disc.build()

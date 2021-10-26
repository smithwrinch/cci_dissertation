from mpl_toolkits.axes_grid1 import ImageGrid
import tensorflow as tf

import matplotlib.pyplot as plt

def _bytes_feature(value):
    """Returns a bytes_list from a string / byte."""
    if isinstance(value, type(tf.constant(0))):
        value = value.numpy()  # BytesList won't unpack a string from an EagerTensor.
    return tf.train.Feature(bytes_list=tf.train.BytesList(value=[value]))

def _float_feature(value):
    """Returns a float_list from a float / double."""
    return tf.train.Feature(float_list=tf.train.FloatList(value=[value]))

def _int64_feature(value):
    """Returns an int64_list from a bool / enum / int / uint."""
    return tf.train.Feature(int64_list=tf.train.Int64List(value=[value]))

# Create the features dictionary.
def image_example(image, dimension):
    feature = {
        'dimension': _int64_feature(dimension),
        # 'label': _int64_feature(label),
        'image_raw': _bytes_feature(image.tobytes()),
    }
    return tf.train.Example(features=tf.train.Features(feature=feature))


def save_images(x_train, record_file="mnistTrain.tfrecords"):
    # record_file = 'mnistTrain.tfrecords'
    n_samples = x_train.shape[0]
    dimension = x_train.shape[1]
    with tf.io.TFRecordWriter(record_file) as writer:
       for i in range(n_samples):
          image = x_train[i]
          # label = y_train[i]
          tf_example = image_example(image, dimension)
          writer.write(tf_example.SerializeToString())

# Decoding function
def parse_record(record):
    name_to_features = {
        'dimension': tf.io.FixedLenFeature([], tf.int64),
        'image_raw': tf.io.FixedLenFeature([], tf.string),
    }
    return tf.io.parse_single_example(record, name_to_features)

def decode_record(record, DATA_TYPE):
    image = tf.io.decode_raw(
        record['image_raw'], out_type=DATA_TYPE, little_endian=True, fixed_length=None, name=None
    )
    dimension = record['dimension']
    image = tf.reshape(image, (dimension, dimension))
    return image

def load_images(record_file, DATA_TYPE):
    dataset = tf.data.TFRecordDataset(record_file, buffer_size=None)
    for record in dataset:
        parsed_record = parse_record(record)
        decoded_record = decode_record(parsed_record, DATA_TYPE)
        image = decoded_record
        # break



def load_data():
    (train_images, train_labels), (_, _) = tf.keras.datasets.mnist.load_data()
    train_images = train_images.reshape(train_images.shape[0], 28, 28, 1).astype('float32')
    train_images = (train_images - 127.5) / 127.5  # Normalize the images to [-1, 1]

    # Batch and shuffle the data
    # train_dataset = tf.data.Dataset.from_tensor_slices(train_images).shuffle(BUFFER_SIZE).batch(batch_size)

    return train_images, train_images.dtype



if __name__ == '__main__':
    # train_x, datatype= load_data()
    #
    # # save_images(train_x)
    #
    # dataset = tf.data.TFRecordDataset("mnistTrain.tfrecords", buffer_size=None)
    # # load_images("mnistTrain.tfrecords", datatype)
    #
    #
    # im_list = []
    # n_samples_to_show = 16
    # c = 0
    # for record in dataset:
    #     c+=1
    #     if c > n_samples_to_show:
    #         break
    #     parsed_record = parse_record(record)
    #     decoded_record = decode_record(parsed_record, DATA_TYPE=tf.float32)
    #     image = decoded_record
    #     im_list.append(image)
    # # Visualization
    # fig = plt.figure(figsize=(4., 4.))
    # # Ref: https://matplotlib.org/3.1.1/gallery/axes_grid1/simple_axesgrid.html
    # grid = ImageGrid(fig, 111,  # similar to subplot(111)
    # nrows_ncols=(4, 4),  # creates 2x2 grid of axes
    # axes_pad=0.1,  # pad between axes in inch.
    # )
    # # Show image grid
    # for ax, im in zip(grid, im_list):
    # # Iterating over the grid returns the Axes.
    #     ax.imshow(im, 'gray')
    # plt.show()



    IML_TOOL/bin/data/saved_models/*

import tensorflow as tf
import matplotlib.pyplot as plt


gen = tf.keras.models.load_model("../../bin/data/saved_models/frog/saved_networks/ckpt/-12_generator")

input = tf.zeros((1, 256, 256, 3))

output = gen(input, training=True)

plt.imshow(tf.squeeze(output) *0.5 + 0.5)
plt.show()

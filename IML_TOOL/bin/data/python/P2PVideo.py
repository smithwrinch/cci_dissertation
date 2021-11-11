import imageio
import argparse
import glob
import os
import cv2


import numpy as np

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--output_txt",  required=True)
    parser.add_argument("--model_dir", required=True)
    parser.add_argument("--export_dir", required=True)
    parser.add_argument("--video_dir", required=True)
    parser.add_argument("--name", required=True)
    parser.add_argument("--width", required=True)
    parser.add_argument("--height", required=True)

    args = parser.parse_args()
    dir = args.output_txt
    MODEL_DIR = args.model_dir
    VIDEO_DIR = args.video_dir
    NAME = args.name
    EXPORT_DIR = args.export_dir
    width =  int(args.width)
    height = int(args.height)

    out_width = 720
    out_height = 480

    with open(dir, "w") as f:
        f.write("Initialising tensorflow....")
    import tensorflow as tf


    print(dir)
    with open(dir, "w") as f:
        f.write("Loading model....")

    try:
        model = tf.keras.models.load_model(MODEL_DIR)
    except:
        with open(dir, "w") as f:
            f.write("[Error] Model could not be loaded")
        exit(0)

    vidcap = cv2.VideoCapture(VIDEO_DIR)
    success,image = vidcap.read()
    if(not success):
        with open(dir, "w") as f:
            f.write("[Error] Video file could not be loaded")
        exit(0)
    count = 0

    print(EXPORT_DIR + NAME + ".mov")
    fourcc = cv2.VideoWriter_fourcc('m', 'p', '4', 'v') # note the lower case
    video= cv2.VideoWriter(EXPORT_DIR + "/" + NAME + ".mov", fourcc, int(30), (int(width),int(height)), True)


    print(EXPORT_DIR + "/" + NAME + ".mov")
    print(NAME)
    while success:
        success,image = vidcap.read()
        print(count)
        with open(dir, "w") as f:
            f.write("frame%d.jpg" % count);

        if(not success):
            break
        image = image.astype('float32')
        image /= 127.5
        image -= 1
        image = cv2.resize(image, (width, height));
        image = np.expand_dims(image, axis=0)
        out = model(image)

        out = out.numpy()[0]
        out +=1
        out *= 127.5


        out = out.astype('uint8')

        # out = cv2.resize(out, (width, height));

        video.write(out)
        if(not success):
            with open(dir, "w") as f:
                f.write("[Error] Processing video file")
        count += 1

    video.release()
    vidcap.release()

    with open(dir, "w") as f:
        f.write("Finished!")

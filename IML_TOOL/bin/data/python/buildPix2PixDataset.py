import imageio
import argparse
import glob
import os
import cv2

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--input_txt", required=True)
    parser.add_argument("--output_txt",  required=True)
    parser.add_argument("--dir", required=True)
    parser.add_argument("--output_dir", required=True)

    args = parser.parse_args()
    input_txt = args.input_txt
    output_txt = args.output_txt
    dir = args.dir
    output_dir = args.output_dir

    if os.path.exists(dir):
        os.remove(dir)

    with open(dir, "w") as f:
        f.write("Beginning...")

    images = []
    input_filenames = glob.glob(input_txt)
    if(len(input_filenames) == 0):
        with open(dir, "w") as f:
            f.write("[Error] No input images found")
        exit(0)

    with open(dir, "w") as f:
        f.write(str(len(input_filenames)) + " input images found")
    output_filenames = glob.glob(output_txt)

    if(len(output_filenames) == 0):
        with open(dir, "w") as f:
            f.write("[Error] No output images found")
        exit(0)

    with open(dir, "w") as f:
        f.write(str(len(output_filenames)) + " output images found")


    if(len(output_filenames) != len(output_filenames):
        with open(dir, "w") as f:
            f.write("[Error] Input images not equal to output\n" +
            str(len(input_filenames)) + " output images found\n" +
            str(len(output_filenames)) + " input images found")
        exit(0)

    # big assumption they are ordered the same...

    fnames = zip(output_filenames, input_filenames)

    for n, fname in enumerate(fnames):
        with open(dir, "w") as f:
            f.write(str(n+1) + " of "+str(len(output_filenames)) + "images processed")

        im1 = cv2.imread(fname[0])
        im2 = cv2.imread(fname[1])

        out_img = np.concatenate((im, img), axis=1)
        cv2.imwrite(output_dir+str(n)+".png", out_img)
        print(output_dir+str(n)+".png")

    with open(dir, "w") as f:
        f.write("[Finished]")


    # out = OUTPUT_DIR
    # if(out != ".gif"):
    #     out+=".gif"
    # imageio.mimsave(out, images)

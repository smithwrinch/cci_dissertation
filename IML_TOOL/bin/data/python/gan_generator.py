import imageio
import argparse
import glob
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("--input_dir", help="Input image directory", required=True)
    parser.add_argument("--output_dir", help="Output directory", required=True)

    args = parser.parse_args()
    OUTPUT_DIR = args.output_dir
    INPUT_DIR = args.input_dir

    images = []
    filenames = glob.glob(INPUT_DIR+"/*")
    for filename in filenames:
        images.append(imageio.imread(filename))

    out = OUTPUT_DIR
    if(out != ".gif"):
        out+=".gif"
    imageio.mimsave(out, images)

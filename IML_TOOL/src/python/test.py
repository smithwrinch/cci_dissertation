# pyApp.py
# example usage for comunnicating with ofxFifo
# Lingdong@antimodular 2018

# python modules required to run this demo:
# - cv2   (pip install opencv-python)
# - numpy (pip install numpy)

# import cv2
import time
import pipes
import os

from piper import Piper

def main():

    gen_pipe = Piper("/tmp/lol")

    for i in range(10):
        print(i)
        gen_pipe.send_message(str(i))
        time.sleep(1)
    print("done")

if __name__ == "__main__":
    main()

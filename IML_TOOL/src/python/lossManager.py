import os
import numpy as np
import glob
import matplotlib.pyplot as plt

class LossManager():

    def __init__(self, rootDir):
        self.rootDir = rootDir
        self.lossesG = np.array([0])
        self.lossesD = np.array([0])
        self.lossesG = np.expand_dims(self.lossesG, axis=0)
        self.lossesD = np.expand_dims(self.lossesD, axis=0)
        if not os.path.exists(rootDir):
            os.mkdir(rootDir)
        else:
            if(len(glob.glob(rootDir)) == 2):
                self.lossesG = np.load(rootDir+"gen.npy")
                self.lossesD = np.load(rootDir+"disc.npy")


    def addGeneratorLosses(self, loss):
        # fifo=open(self.pipename,'w')
        loss = np.expand_dims(loss, axis=0)
        self.lossesG = np.concatenate((self.lossesG,loss), axis=1)
        np.save(self.rootDir+"gen.npy", self.lossesG)


    def addDiscriminatorLosses(self, loss):
        # fifo=open(self.pipename,'w')

        loss = np.expand_dims(loss, axis=0)
        self.lossesD = np.concatenate((self.lossesD,loss), axis=1)
        np.save(self.rootDir+"disc.npy", self.lossesD)


    def generate_graph(self, ROOT_CHECKPOINT_SAVE):
        plt.cla()
        plt.figure(figsize=(8,4))
        plt.plot(range(self.lossesG.shape[1]), np.squeeze(self.lossesG), 'r')
        plt.plot(range(self.lossesD.shape[1]), np.squeeze(self.lossesD), 'b')
        if(os.path.exists(self.rootDir+"graph.png")):
            os.remove(self.rootDir+"graph.png")
        plt.savefig(self.rootDir+"graph.png")
        plt.close()

    def generate_graphD(self, ROOT_CHECKPOINT_SAVE):
        plt.cla()
        plt.figure(figsize=(8,4))
        plt.plot(range(self.lossesD.shape[1]), np.squeeze(self.lossesD), 'b')
        if(os.path.exists(ROOT_CHECKPOINT_SAVE+"graphD.png")):
            os.remove(self.rootDir+"graphD.png")
        plt.title("Discriminator")
        plt.savefig(self.rootDir+"graphD.png")
        plt.close()

    def generate_graphG(self, ROOT_CHECKPOINT_SAVE):
        plt.cla()
        plt.figure(figsize=(8,4))
        plt.plot(range(self.lossesG.shape[1]), np.squeeze(self.lossesG), 'r')
        if(os.path.exists(self.rootDir+"graphG.png")):
            os.remove(self.rootDir+"graphG.png")
        plt.title("Generator")
        plt.savefig(self.rootDir+"graphG.png")
        plt.close()

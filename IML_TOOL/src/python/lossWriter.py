import os

class Messager():

    def __init__(self, fname="/tmp/lossG.txt"):
        self.fname = fname
        if os.path.exists(fname):

            os.remove(fname)


    def send_message(self, message):
        # fifo=open(self.pipename,'w')
        with open(self.fname, "w") as f:
            f.write(message)

import os

class Piper():

    def __init__(self, pipename="/tmp/fifopipe"):
        self.pipename = pipename
        path=pipename
        try:
            print("creating pipe at " + path)
            os.mkfifo(path)
        except FileExistsError:
            print("pipe exits at " + pipename)
            print("overwriting...")
            os.remove(path)
            os.mkfifo(path)


    def send_message(self, message):
        # fifo=open(self.pipename,'w')
        fd = os.open(self.pipename, os.O_RDWR) #non-blocking
        f = os.fdopen(fd, 'w') #also non-blocking
        f.write(message)
        f.close()

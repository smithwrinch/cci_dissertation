class LogManager():

    def __init__(self, fname):
        self.fname = fname

    def send(self, msg, line_length=40):
        f = open(self.fname, "w")
        out = None
        for i in range(len(msg)//line_length):
            msg.insert(str(line_length*(i+1))+"\n")
        f.write(msg)
        f.close()

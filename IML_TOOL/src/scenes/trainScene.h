# pragma once
#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"
#include "ofxGraph.h"
#include "ofxDirList.h"
#include "ofxThreadedImageLoader.h"

#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <filesystem>

/*
CODE FROM:
https://stackoverflow.com/questions/22802902/how-to-get-pid-of-process-executed-with-system-command-in-c

Could also use pipes from here to get std in and out

*/

static pid_t system2(const char * command, int * infp, int * outfp)
{
    int p_stdin[2];
    int p_stdout[2];
    pid_t pid;

    if (pipe(p_stdin) == -1)
        return -1;

    if (pipe(p_stdout) == -1) {
        close(p_stdin[0]);
        close(p_stdin[1]);
        return -1;
    }

    pid = fork();

    if (pid < 0) {
        close(p_stdin[0]);
        close(p_stdin[1]);
        close(p_stdout[0]);
        close(p_stdout[1]);
        return pid;
    } else if (pid == 0) {
        close(p_stdin[1]);
        dup2(p_stdin[0], 0);
        close(p_stdout[0]);
        dup2(p_stdout[1], 1);
        dup2(::open("/dev/null", O_RDONLY), 2);
        /// Close all other descriptors for the safety sake.
        for (int i = 3; i < 4096; ++i)
            ::close(i);

        setsid();
        execl("/bin/sh", "sh", "-c", command, NULL);
        _exit(1);
    }

    close(p_stdin[0]);
    close(p_stdout[1]);

    if (infp == NULL) {
        close(p_stdin[1]);
    } else {
        *infp = p_stdin[1];
    }

    if (outfp == NULL) {
        close(p_stdout[0]);
    } else {
        *outfp = p_stdout[0];
    }

    return pid;
}


class LossLoader: public ofThread{
		public:
			int fd;
			int MAX_BUF = 100;
			char *myfifo;
			ofxGraph * graph;
			void setup(char* fifo_name, ofxGraph* graph){
			    this->myfifo = fifo_name;
			    this->graph = graph;
			}

			void threadedFunction(){
				char buf[MAX_BUF];
				while(isThreadRunning()){
					fd = open(this->myfifo, O_RDONLY | O_NONBLOCK);
		      // read(fd, buf, MAX_BUF);
					while(read(fd, buf, MAX_BUF) == MAX_BUF) {
						// cout << strlen(buf) << endl;
					}
					// cout << buf << endl;
		      if(strlen(buf) != 0){
		        // cout<<"Received:"<< buf<<endl;
						size_t pos = 0;
						string token;
						string s = std::string(buf);
						string delimiter = "\n";
						while ((pos = s.find(delimiter)) != string::npos) {
						    token = s.substr(0, pos);
								// cout << token << endl;
								if(token.length() == 5){
									graph->add(stof(token));
									// cout << token << endl;
								}
						    s.erase(0, pos + delimiter.length());
						}
		      }
					sleep(10000); // amount of times graph is updated
				}

				// close(fd);

			}
};

class ImageLoader: public ofThread{
	public:
		ofImage * image;
    string image_dir;
		ofxThreadedImageLoader *loader;
		ofFile f;

    string img_dir;
		// string a = "saved_models/" + ModelManager::getInstance()->getModelName() + "/saved_networks";
		ofDirectory dir;
		void setup(ofImage * image, ofxThreadedImageLoader * loader){
			this->image = image;
			this->loader = loader;
			this->image_dir = "saved_models/"+ModelManager::getInstance()->getModelName()+"/images";
			dir.open("saved_models/" + ModelManager::getInstance()->getModelName() + "/saved_networks/ckpt");
		}
		void threadedFunction(){
			sleep(10000);

			while(isThreadRunning()){
					dir.listDir();
					//go through and print out all the paths
					for(int i = 0; i < dir.size(); i++){
					    f = dir.getFile(i);
							string fname = f.getFileName();
							string out_num = "";
							int j = 0;
							if(fname[j] == '-'){
									j++;
									while(isdigit(fname[j])){
										out_num = out_num + fname[j];
										j++;
									}

									lock();
									string filename = image_dir+"/"+out_num+".png";
									if(out_num == ""){
										filename = "images/-1.png";
									}
									else{
										ModelManager::getInstance()->setEpochsTrained(stoi(out_num));
										ModelManager::getInstance()->save();
									}
									loader->loadFromDisk(*image, filename);
									unlock();
									break;
							}
					}

		    	// int nImages = DIR->listDir(image_dir);
					//
					// cout << *current_image << "\n";
					// cout << nImages << "\n";
					//
					// fd = open(this->fifo, O_RDONLY | O_NONBLOCK);
					// read(fd, buf, MAX_BUF);
					//
					// // cout << buf << endl;
					// if(strlen(buf) != 0){
					// 	cout<<"EPOCHS Received:"<< buf<<endl;
					// 	size_t pos = 0;
					// 	string token = "-1";
					// 	string s = std::string(buf);
					// 	string delimiter = "\n";
					// 	while ((pos = s.find(delimiter)) != string::npos) {
					// 			token = s.substr(0, pos);
					// 			for (int i =0; i < token.length(); i++){
					// 				if(!isdigit(token[i])){
					// 					token = "-1";
					// 					break;
					// 				}
					// 			}
					// 			cout << token << "LOLOL" << endl;
					// 			s.erase(0, pos + delimiter.length());
					// 			// * current_image = stoi(token);
					// 	}
					//
					// 	cout << filename << endl;
					sleep(10000); // amount of times it checks for new images
					}
			}
};

class TrainingThread: public ofThread{
  public:
    string python_file;
    int img_width;
    int img_height;
    int input_channel;
    int output_channel;
    float learning_rate;
    int max_epochs;
    int batch_size;
    int num_layers;
    int kernel_size;
    float beta;
    int lambda;

    //lol massive
    void setup(string python_file,
    int img_width,
    int img_height,
    int input_channel,
    int output_channel,
    float learning_rate,
    int max_epochs,
    int batch_size,
    int num_layers,
    int kernel_size,
    float beta,
    int lambda){
      this->python_file = python_file;
      this->img_width = img_width;
      this->img_height = img_height;
      this->input_channel = input_channel;
      this->output_channel = output_channel;
      this->learning_rate = learning_rate;
      this->max_epochs = max_epochs;
      this->batch_size = batch_size;
      this->num_layers = num_layers;
      this->kernel_size = kernel_size;
      this->beta = beta;
      this->lambda = lambda;
    }

  	void threadedFunction(){
  		sleep(2000);
      string arguments = addArguments();
      string s("python ../src/python/pix2pix_train.py ");
			cout << s + arguments.c_str() << endl;
			pid = system2((s + arguments).c_str(), &input, &output);
  		cout << "STARTED:" << pid << endl;
  	}
		void stopThread(){
			kill(pid, SIGTERM);
			sleep(3000);
			kill(pid, SIGKILL);
			system("killall python"); // hacky workaround for now. just POSIX.
			ofThread::stopThread();
		}
  private:

		pid_t pid;
		int input, output;
    string addArguments(){
      string base_dir = "data/saved_models/" + ModelManager::getInstance()->getModelName() +"/";
      string out = "";
      out += "--image_width " + to_string(img_width);
      out += " --image_height " + to_string(img_height);
      out += " --input_channel " + to_string(input_channel);
      out += " --output_channel " + to_string(output_channel);
      out += " --learning_rate " + to_string(learning_rate);
      out += " --max_epochs " + to_string(max_epochs);
      out += " --batch_size " + to_string(batch_size);
      out += " --num_layers " + to_string(num_layers);
      out += " --kernel_size " + to_string(kernel_size);
      out += " --beta " + to_string(beta);
      out += " --lambda " + to_string(lambda);
      out += " --img_save_dir " + base_dir + "images/";
      out += " --checkpoint_save_dir " + base_dir + "saved_networks/";
      return out;
    }
};

class TrainingScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void refresh();

  private:
      LossLoader lossLoader;
      TrainingThread trainingThread;
			ImageLoader imageLoader; // used to check if new images

      bool training = false;

      ofxGraph graph;
      float current_loss = 0.0;

      int num_images;
			int state = 0; // 0 = not training, 1 = training, 2 = restarting

      ofxDirList DIR;
      int current_image;
      ofImage training_img;

      ofxDatGuiButton* startTrainingButton = new ofxDatGuiButton("START TRAINING");
      ofxDatGuiButton* resumeTrainingButton = new ofxDatGuiButton("RESUME TRAINING");
      ofxDatGuiButton* stopTrainingButton = new ofxDatGuiButton("STOP TRAINING");

      ofxDatGuiButton* restartTrainingButton = new ofxDatGuiButton("RESTART TRAINING");
      ofxDatGuiButton* confirmButton = new ofxDatGuiButton("CONFIRM RESTART");
      ofxDatGuiButton* unconfirmButton = new ofxDatGuiButton("BACK");

      ofxDatGuiButton* backButton = new ofxDatGuiButton("BACK<-");
      ofxDatGuiButton* playButton = new ofxDatGuiButton("CONTINUE");

			ofxThreadedImageLoader threadedImageLoader;
};

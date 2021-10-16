#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"
#include "ofxGraph.h"
#include "ofxDirList.h"

#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

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
								if(token.length() == 10){
									graph->add(stof(token));
									cout << token << endl;
								}
						    s.erase(0, pos + delimiter.length());
						}
		      }
					sleep(10000); // amount of times graph is updated
				}

				// close(fd);

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
  		system((s + arguments).c_str());
  	}
  private:

    string addArguments(){
      string out = "";
      out += "--image_width " + to_string(img_width);
      out += "--image_height " + to_string(img_height);
      out += "--input_channel " + to_string(input_channel);
      out += "--output_channel " + to_string(output_channel);
      out += "--learning_rate " + to_string(learning_rate);
      out += "--max_epochs " + to_string(max_epochs);
      out += "--batch_size " + to_string(batch_size);
      out += "--num_layers " + to_string(num_layers);
      out += "--kernel_size " + to_string(kernel_size);
      out += "--beta " + to_string(beta);
      out += "--lambda " + to_string(lambda);
      return out;
    }
};

class ImageLoader: public ofThread{
	public:
		int * current_image;
		ofImage * image;
		ofxDirList * DIR;
    string image_dir;
		void setup(string image_dir, int * current_image, ofxDirList * DIR, ofImage * image){
      this->image_dir = image_dir;
			this->current_image = current_image;
			this-> DIR = DIR;
			this->image = image;
		}
		void threadedFunction(){
			sleep(10000);
			while(isThreadRunning()){
		    	int nImages = DIR->listDir(image_dir);

					cout << *current_image << "\n";
					cout << nImages << "\n";

					if(*current_image +1< nImages){
						*current_image = nImages-1;
						lock();
						// image->clear();
						image->unbind();
						string filename = image_dir+"/"+to_string(*current_image-1)+".png";
						image->loadImage(filename);
						image->update();
						cout << filename << endl;
						unlock();
					}
					sleep(10000); // amount of times it checks for new images
			}
		}
};


class TrainingScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:
      LossLoader lossLoader;
      ImageLoader imageLoader; // used to check if new images
      TrainingThread trainingThread;

      bool training = false;

      ofxGraph graph;
      float current_loss = 0.0;

      int num_images;
      int max_epochs = 100; // also number of images....
      ofxDirList DIR;
      int current_image;
      ofImage training_img;

      ofxDatGuiButton* startTrainingButton = new ofxDatGuiButton("START TRAINING");
      ofxDatGuiButton* stopTrainingButton = new ofxDatGuiButton("STOP TRAINING");
};

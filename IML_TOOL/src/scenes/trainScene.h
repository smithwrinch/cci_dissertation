# pragma once
#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"
#include "ofxGraph.h"
#include "ofxDirList.h"
#include "ofxThreadedImageLoader.h"
#include "../helpWidget.h"


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

// gets loss and also out text
class LossLoader: public ofThread{
		public:
			// int fd_gen;
			// int fd_disc;
			// int MAX_BUF = 100;
			// char *genfifo = "/tmp/gen";
			// char * discfifo = "/tmp/disc";
			void setup(ofxGraph* graph, string* msg, string rootDir){
			    this->graph = graph;
          this->rootDir = rootDir;
          this->msg = msg;
          ofFile f1;
          ofFile f2;
          ofFile f3;

          f1.removeFile(rootDir+"gen.txt");
          f2.removeFile(rootDir+"disc.txt");
          f2.removeFile(rootDir+"log.txt");

			}
      // string line;

			void threadedFunction(){
				// char buf[MAX_BUF];
				vector <float> gen_values;
				vector <float> disc_values;
				vector <float> values;

				while(isThreadRunning()){
          ofFile fileGen;
          ofFile fileDisc;
          ofFile fileMsg;
          cout << rootDir+"gen.txt" << endl;
          if(fileGen.open(rootDir+"gen.txt") && fileDisc.open(rootDir+"disc.txt")){
            buffGen = fileGen.readToBuffer();
            buffDisc = fileDisc.readToBuffer();

            string gen = (buffGen.getText()).c_str();
            string disc = (buffDisc.getText()).c_str();

            if(gen.length() > 0 && disc.length() > 0){
              vector <float> values;
              values.push_back(stof(buffGen));
              values.push_back(stof(buffDisc));
              graph->add(values);
            }
          }
          if(fileMsg.open(rootDir+"log.txt")){
            buffMsg = fileMsg.readToBuffer();
            *msg = (buffMsg.getText()).c_str();
          }
          sleep(4000);
          fileGen.close();
          fileDisc.close();
          sleep(4000);
          // gen_values.push_back(ofRandom(1, 10));
          // disc_values.push_back(ofRandom(1, 10));
          // lock();
          // fd_gen = open(genfifo, O_RDONLY | O_NONBLOCK);
          // fd_disc = open(discfifo, O_RDONLY | O_NONBLOCK);
          //
		      // // read(fd_gen, buf, MAX_BUF);
          // // cout << buf << endl;
					// while(read(fd_gen, buf, MAX_BUF) == MAX_BUF) {
					// }
          // cout<<"Received:"<< buf<<endl;
		      // if(strlen(buf) != 0){
					// 	size_t pos = 0;
					// 	string token;
					// 	string s = std::string(buf);
					// 	string delimiter = "\n";
					// 	while ((pos = s.find(delimiter)) != string::npos) {
					// 	    token = s.substr(0, pos);
					// 			if(token.length() == 5){
					// 				gen_values.push_back(stof(token));
					// 				cout << token << endl;
					// 			}
          //
					// 	    s.erase(0, pos + delimiter.length());
					// 	}
		      // }
		      // // read(fd_disc, buf, MAX_BUF);
					// while(read(fd_disc, buf, MAX_BUF) == MAX_BUF) {
					// }
					// // cout << buf << endl;
		      // if(strlen(buf) != 0){
		      //   cout<<"Received:"<< buf<<endl;
					// 	size_t pos = 0;
					// 	string token;
					// 	string s = std::string(buf);
					// 	string delimiter = "\n";
					// 	while ((pos = s.find(delimiter)) != string::npos) {
					// 	    token = s.substr(0, pos);
					// 			// cout << token << endl;
					// 			if(token.length() == 5){
					// 				// graph->add(stof(token), 1);
					// 				// disc_values.push_back(stof(token));
					// 				// value.push_back(stof(token))
					// 				// cout << token << endl;
					// 				disc_values.push_back(stof(token));
					// 			}
          //
					// 	    s.erase(0, pos + delimiter.length());
					// 	}
		      // }
          //
					// int max_ = disc_values.size();
					// if(gen_values < disc_values){
					// 	max_ = gen_values.size();
					// }
					// for (int i =0; i < max_; i++){
					// 	vector<float> temp;
					// 	temp.push_back(gen_values[i]);
					// 	temp.push_back(disc_values[i]);
					// 	graph->add(temp);
					// 	temp.clear();
					// }
					// gen_values.clear();
					// disc_values.clear();
          //
          // unlock();
					// sleep(100); // amount of times graph is updated
          // lock();
          // close(fd_disc);
          // close(fd_gen);
          // unlock();

				}


			}
    private:
      ofBuffer buffGen;
      ofBuffer buffDisc;
      ofBuffer buffMsg;
      string rootDir;
			ofxGraph * graph;
      string *msg;
};

// essentially gets epoch info
class EpochManager: public ofThread{
	public:
		ofImage * image;
    ofImage * graphImage;
    string image_dir;
		ofxThreadedImageLoader* loader;
		ofFile f;
    int epochs;
    string img_dir;
    ofxDatGuiLabel* epochLabel;
    ofxDatGuiSlider* maxEpochsSlider;

		// string a = "saved_models/" + ModelManager::getInstance()->getModelName() + "/saved_networks";
		ofDirectory dir;
		void setup(ofImage * image, ofImage * graphImage, ofxThreadedImageLoader * loader, ofxDatGuiLabel * label, ofxDatGuiSlider * maxEpochsSlider){
			this->image = image;
      this->graphImage = graphImage;
			this->loader = loader;
      this->epochLabel = label;
      this->maxEpochsSlider = maxEpochsSlider;
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
                    ModelManager::getInstance()->setStatus(3);
										ModelManager::getInstance()->save();
                    epochLabel->setLabel(ofToString(ModelManager::getInstance()->getEpochsTrained()) + "/" + ofToString(ModelManager::getInstance()->getMaxEpochs()));
                    maxEpochsSlider->setMin(stoi(out_num));
									}
									loader->loadFromDisk(*image, filename);
									loader->loadFromDisk(*graphImage, "saved_models/" + ModelManager::getInstance()->getModelName()+"/saved_networks/losses/graph.png");
									unlock();
									break;
							}
              f.close();
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
    // int output_channel;
    int learning_rateX;
    int learning_rateY;
    int max_epochs;
    int batch_size;
    int num_layers;
    int kernel_size;
    float beta;
    int lambda;
		int latent_vector;
    string dataset_dir;
    float disc_noise;
    bool random_horizontal;
    bool random_vertical;
    int random_crop;
    float random_brightness;
    float random_contrast;

    //lol massive
    void setup(string python_file,
    int img_width,
    int img_height,
    int input_channel,
    // int output_channel,
    int learning_rateX,
    int learning_rateY,
    int max_epochs,
    int batch_size,
    int num_layers,
    int kernel_size,
    int beta,
    int lambda,
		int latent_vector,
    string dataset_dir,
    float disc_noise,
    bool random_horizontal,
    bool random_vertical,
    int random_crop,
    float random_brightness,
    float random_contrast){
      this->python_file = python_file;
      this->img_width = img_width;
      this->img_height = img_height;
      this->input_channel = input_channel;
      // this->output_channel = output_channel;
      this->learning_rateX = learning_rateX;
      this->learning_rateY = learning_rateY;
      this->max_epochs = max_epochs;
      this->batch_size = batch_size;
      this->num_layers = num_layers;
      this->kernel_size = kernel_size;
      this->beta = beta;
      this->lambda = lambda;
			this->latent_vector = latent_vector;
      this->dataset_dir = dataset_dir;
      this->disc_noise = disc_noise;
      this->random_horizontal = random_horizontal;
      this->random_vertical = random_vertical;
      this->random_crop = random_crop;
      this->random_brightness = random_brightness;
      this->random_contrast = random_contrast;
    }

  	void threadedFunction(){
  		sleep(2000);
      string arguments = addArguments();
			cout << python_file + arguments.c_str() << endl;
			pid = system((python_file + arguments).c_str());
			// pid = system2((python_file + arguments).c_str(), &input, &output);
  		cout << "STARTED:" << pid << endl;
  	}
		void stopThread(){
      cout << "STOPPING TRAINING THREAD:" << endl;
			kill(pid, SIGTERM);
			sleep(3000);
			kill(pid, SIGKILL);
			system("killall python"); // hacky workaround for now. just POSIX.
			system("killall python3");
			ofThread::stopThread();
		}
  private:

		pid_t pid;
		int input, output;
    string addArguments(){
      string base_dir = "data/saved_models/" + ModelManager::getInstance()->getModelName() +"/";
      string out = "";
			out += " --learning_rateX " + to_string(learning_rateX);
			out += " --learning_rateY " + to_string(learning_rateY);
			out += " --max_epochs " + to_string(max_epochs);
			out += " --batch_size " + to_string(batch_size);
			out += " --img_save_dir " + base_dir + "images/";
			out += " --checkpoint_save_dir " + base_dir + "saved_networks/";
			out += " --dataset_dir " + dataset_dir;
      out += " --kernel_size " + to_string(kernel_size);

      out += " --disc_noise " + to_string(disc_noise);
      out += " --random_horizontal " + random_horizontal ? "1" : "0";
      out += " --random_vertical " + random_vertical ? "1" : "0";
      out += " --random_crop " + to_string(random_crop);
      out += " --random_brightness " + to_string(random_brightness);
      out += " --random_contrast " + to_string(random_contrast);
			if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
				out += " --image_width " + to_string(img_width);
				out += " --image_height " + to_string(img_height);
				out += " --input_channel " + to_string(input_channel);
				out += " --output_channel " + to_string(input_channel);
				out += " --num_layers " + to_string(num_layers);
				out += " --beta " + to_string(beta);
				out += " --lambda_ " + to_string(lambda);
			}
			else if (ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
        out += " --img_channel " + to_string(input_channel);// + to_string(input_channel);
				out += " --latent_dim " + to_string(latent_vector);
				out += " --img_size " + to_string(img_width);
			}

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

      void saveModelAtCurrentEpoch();
      void refreshTraining();

      LossLoader lossLoader;
      TrainingThread trainingThread;
			EpochManager epochManager; // used to check if new images

      bool training = false;

      ofxGraph graph;
      float current_loss = 0.0;
      string msg;

      int num_images;
			int state = 0; // 0 = not training, 1 = training, 2 = restarting
      int graphToShow = 0;

      ofxDirList DIR;
      int current_image;
      ofImage training_img;
      ofImage graph_img;

      ofxDatGuiLabel* epochLabel = new ofxDatGuiLabel("");

      ofxDatGuiButton* startTrainingButton = new ofxDatGuiButton("START TRAINING");
      ofxDatGuiButton* resumeTrainingButton = new ofxDatGuiButton("RESUME TRAINING");
      ofxDatGuiButton* stopTrainingButton = new ofxDatGuiButton("STOP TRAINING");
      ofxDatGuiButton* saveModelButton = new ofxDatGuiButton("SAVE MODEL AT CURRENT EPOCH");

      ofxDatGuiButton* toggleGraphButton = new ofxDatGuiButton("TOGGLE GRAPH");

      ofxDatGuiButton* restartTrainingButton = new ofxDatGuiButton("RESTART TRAINING");
      ofxDatGuiButton* confirmButton = new ofxDatGuiButton("CONFIRM RESTART");
      ofxDatGuiButton* unconfirmButton = new ofxDatGuiButton("BACK");

      ofxDatGuiButton* loadButton = new ofxDatGuiButton("LOAD PREVIOUSLY TRAINED");

      ofxDatGuiButton* backButton = new ofxDatGuiButton("BACK<-");
      ofxDatGuiButton* playButton = new ofxDatGuiButton("CONTINUE");

			ofxThreadedImageLoader threadedImageLoader;

      ofxDatGuiSlider* learningRateSlider;
      ofxDatGuiSlider* learningRateSlider2;
      ofxDatGuiSlider* batchSizeSlider;
      ofxDatGuiSlider* maxEpochsSlider;
      ofxDatGuiSlider* discriminatorNoiseSlider;
      ofxDatGuiToggle* randomHorizontalToggle;
      ofxDatGuiToggle* randomVerticalToggle;
      ofxDatGuiSlider* cropSlider;
      ofxDatGuiSlider* brightnessSlider;
      ofxDatGuiSlider* contrastSlider;

      vector<HelpWidget*> helpWidgets;

      HelpWidget batchSizeHelp;
      HelpWidget discriminatorNoiseHelp;
      HelpWidget randomHorizontalHelp;
      HelpWidget randomVerticalHelp;
      HelpWidget randomCropHelp;
      HelpWidget randomBrightessHelp;
      HelpWidget randomContrastHelp;
      HelpWidget learningRateHelp;

};

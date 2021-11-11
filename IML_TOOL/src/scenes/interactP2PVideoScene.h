#pragma once
#include "../baseScene.h"
#include "../sceneManager.h"
#include "../sceneTypes.h"
#include "inputPix2PixScene.h"
#include <time.h>
#include "ofxTensorFlow2.h"
#include "ofxVideoRecorder.h"
#include "syncMusicScene.h"



class PythonThreadVideo : public ofThread {
  public:
    string model_dir;
    string txt_dir;
    string export_dir;
    string video_dir;
    string name;
    int width;
    int height;

    int pid;
    int input;
    int output;

    void setup(
      string model_dir,
      string txt_dir,
      string export_dir,
      string video_dir,
      string name,
      int width,
      int height
    ){
      this->model_dir = model_dir;
      this->txt_dir = txt_dir;
      this->export_dir = export_dir;
      this->video_dir = video_dir;
      this->name = name;
      this->width = width;
      this->height = height;
    }

    void threadedFunction(){
      string pythonFile = "python data/python/P2PVideo.py ";
      // system((pythonFile + getArguments()).c_str());
      pid = system_((pythonFile + getArguments()).c_str(), &input, &output);
      cout << "STARTED PROCESS: " << ofToString(pid) << endl;
    }

    void stopThread(){
      cout << "STOPPING PYTHON THREAD:" << endl;
			kill(pid, SIGTERM);
			sleep(3000);
			kill(pid, SIGKILL);
			system("killall python"); // hacky workaround for now. just POSIX.
			system("killall python3");
			ofThread::stopThread();
		}

  private:
    string getArguments(){
      string out = "";
      out += " --output_txt '" + txt_dir+"'";
      out += " --model_dir '" + model_dir+"'";
      out += " --export_dir '" + export_dir+"'";
      out += " --video_dir '" + video_dir+"'";
      out += " --name '" + name+"'";
      out += " --width " + ofToString(width);
      out += " --height " + ofToString(height);
      cout << out << endl;
      return out;
    }
};

class StatusThread2 : public ofThread {
  public:
    void setup(string fname, string * status, bool * finished){
      this->fname = fname;
      this->status = status;
      this->finished = finished;
      *finished = false;

      ofFile file;
      if(file.open(fname)){
        file.remove();
        file.close();
      }
    }

    void threadedFunction(){
      while(isThreadRunning()){
        ofFile file;
        if(file.open(fname)){
          buff = file.readToBuffer();
          cout << buff.getText() << endl;
          *status = buff.getText();
          if(buff.getText().find("Error") != string::npos){
            sleep(3000);
            *finished = true;
          }
          else if(buff.getText().find("Finished") != string::npos){
            sleep(3000);
            *status = "Exporting frames (10s)....";
            sleep(10000);
            *status = "";
            *finished = true;
          }
        }
        sleep(1000);
        file.close();
        sleep(1000);
      }
    }
    void stopThread(){
      cout << "STOPPING THREAD:" << endl;
      ofThread::stopThread();
      // file.close();
    }
  private:
    ofBuffer buff;
    string * status;
    string fname;
    bool * finished;

  };

class InteractP2PVideoScene : public BaseScene {
    public:
      void setup();
      void update();
      void draw();
      void refresh();
      void onButtonEvent(ofxDatGuiButtonEvent e);

    private:

			void setupGui();
      void beginPythonThread();
      StatusThread2 statusThread;
      PythonThreadVideo pythonThread;

			SceneManager * sceneManager = SceneManager::getInstance();

  		string exportDir;
      string videoDir;
			bool finished = false;
      bool running = false;
      string msg;

			bool hasChosenExportFolder = false;

			vector <ofxDatGuiComponent *> gui;


			ofxDatGuiButton* setExportFolderButton;
      ofxDatGuiButton* setVideoDirButton;
      ofxDatGuiButton* buildButton;
			ofxDatGuiButton* backButton;
};

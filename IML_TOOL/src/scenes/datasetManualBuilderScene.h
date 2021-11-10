#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"


class StatusThread_ : public ofThread {
  public:
    void setup(string rootDir, string* msg, bool* finished){
      this->rootDir = rootDir;
      this->msg = msg;
      this->finished = finished;
      ofFile f;
      f.removeFile(rootDir);
    }

    void threadedFunction(){
      if(isThreadRunning()){

        ofFile f;
        if(f.open(rootDir)){
          buff = f.readToBuffer();
          *msg = (buff.getText()).c_str();

          if((*msg).find("Error") != std::string::npos ||
        (*msg).find("Finished") != std::string::npos ){
            sleep(3000);
            ofThread::stopThread();
          }

        }

        sleep(500);
      }
    }
  private:
    ofBuffer buff;
    string rootDir;
    string *msg;
    bool *finished;
};

class PythonBuildThread : public ofThread {
  public:
    void setup(string input_, string output_, string txtDir){
      this->input_ = input_;
      this->output_ = output_;
      this->txtDir = txtDir;
    }

    void threadedFunction(){
      string s = " --input_txt " + input_;
      s += " --output_txt " + output_;
      s += " --dir " + txtDir;
      s += " --output_dir " + ModelManager::getInstance()->getDatasetDir()+"/";
      system(("python python/buildPix2PixDataset.py " + s).c_str());
    }

  private:
    string input_;
    string output_;
    string txtDir;
};

class DatasetManualBuilderScene : public BaseScene {
  public:
    void refresh();
    void setup();
    void update();
    void draw();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:

    string msg;
    bool finished;
    bool running;

    vector <ofxDatGuiComponent*> gui;
    ofxDatGuiButton * buildButton;
    ofxDatGuiButton * manualButton;
    ofxDatGuiButton * backButton;

    ofxDatGuiLabel * label1;
    ofxDatGuiLabel * label2;

    ofxDatGuiTextInput * inputText;
    ofxDatGuiTextInput * outputText;
    PythonBuildThread buildThread;
    StatusThread_ statusThread;


};

#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"
#include "ofxOpenCv.h"
#include "ofxImageSequenceRecorder.h"

#include <functional>


//god this needs cleaning up
class ImageSaverThread : public ofThread{
  public:
  string rootDir;
  string* msg;
  ofxImageSequenceRecorder* recorder;

  ofImage* im1;
  ofImage* outImg;
  ofFbo* fbo;
  ofDirectory* d;

  int idx;


  void setup(string* msg, ofxImageSequenceRecorder* recorder,
  ofImage* im1, ofImage* outImg, ofFbo* fbo, ofDirectory* d){
      this->msg = msg;
      this->recorder = recorder;
      // this->instance = instance;
      this->im1 = im1;
      this->outImg = outImg;
      this->fbo = fbo;
      this->d = d;

      idx = 0;
      rootDir = ModelManager::getInstance()->getDatasetDir();
      recorder->setPrefix(rootDir + "/"); // this directory must already exist
      recorder->setFormat("jpg"); // png is really slow but high res, bmp is fast but big, jpg is just right
      recorder->startThread();
  }
  // string line;

  void threadedFunction(){
    while(isThreadRunning()){
      if(idx < d->getFiles().size()){

        im1->load(d->getPath(idx));
        sleep(100); // TODO have menu config to change this for slow pcs.
        fbo->readToPixels(outImg->getPixels());
        recorder->addFrame(*outImg);
        *msg = ofToString(idx) + " IMAGES COMPLETED OF " + ofToString(d->getFiles().size());
        idx++;
        cout << *msg << endl;
      }
      else{
        stopThread();
      }
    }

  }
  void stopThread(){
    *msg = "Finished";
    recorder->stopThread();
    ofThread::stopThread();
  }
};

class DatasetPix2PixBuilderScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void refresh();
    void setupGui();
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void keyPressed(int key);
    void updateNormalImage();
    void updateContourImage();
    void updateCannyImage();
    void setupImages();

  private:
    ofImage im1, outImg;
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage r,g,b,canny;

    ofxImageSequenceRecorder recorder;
    ImageSaverThread imageSaver;

    ofFbo contourFbo;
    ofFbo outFbo;

    string outMsg = "";
    bool isApplying = false;
    bool bChangedInputFlag;

    ofxCvGrayscaleImage imgInGray;
    ofxCvContourFinder contourFinder;
    ofDirectory d;
    const int width = 256;
    const int height = 256;
    const int imgX = 400;
    const int imgY = 100;

    const int drawX = 50;
    const int drawY = 150;
    const int drawWidth = 300;

    int mode = 0;
    int fileIdx = 0;
    int updateCount = 0; // to be sure image is loaded for save

    vector<ofxDatGuiComponent*> gui;

    ofxDatGuiButton* backButton;
    ofxDatGuiButton* applyButton;
    ofxDatGuiLabel* inputLabel;
    ofxDatGuiLabel* outputLabel;
    ofxDatGuiButton* toggleModeButton;

    ofxDatGuiToggle* randomHorizontalToggle;
    ofxDatGuiToggle* randomVerticalToggle;

    vector<ofxDatGuiComponent*> guiNormal;
    vector<ofxDatGuiComponent*> guiContours;
    vector<ofxDatGuiComponent*> guiCanny;
    //0 --normal mode
    ofxDatGuiToggle* grayscaleToggle;
    ofxDatGuiToggle* invertToggle;
    ofxDatGuiSlider* blurSlider;
    ofxDatGuiSlider* contrastSlider;
    ofxDatGuiSlider* brightnessSlider;

    //1 --contours mode
    ofxDatGuiButton* getBackgroundButton;
    ofxDatGuiSlider* contourThresholdSlider;
    ofxDatGuiSlider* minContourSlider;
    ofxDatGuiSlider* maxContourSlider;
    ofxDatGuiSlider* blobsToConsiderSlider;
    ofxDatGuiToggle* invertToggleContour;
    ofxDatGuiSlider* lineWidthSizeSlider;

    //2 --canny mode
    ofxDatGuiToggle* invertToggleCanny;
    ofxDatGuiSlider* minThresholdSlider;
    ofxDatGuiSlider* maxThresholdSlider;
    // ofxDatGuiSlider* maxThresholdSlider;
};

#pragma once

#include "../baseScene.h"
#include "../sceneManager.h"
#include "../sceneTypes.h"

#include "ofxTensorFlow2.h"
#include "ofxGui.h"
#include "ofxVideoRecorder.h"
#include "ofxOpenCv.h"
#include "../latentGraphWidget.h"

#include <random>

class MotionDetectionScene : public BaseScene {
    public:
      void setup();
      void update();
      void draw();
      void refresh();
  		void startThread();
  		void stopThread();
      void onButtonEvent(ofxDatGuiButtonEvent e);


    protected:
      ofxTF2::ThreadedModel model;
  		cppflow::tensor input;
      cppflow::tensor output;
  		int nnWidth = 512;  // smaller = faster processing but lower resolution
  		int nnHeight = 512; // smaller = faster processing but lower resolution
      int latentDim = 512;

  		// draw
  		ofFbo fbo;
  		ofImage imgIn;
  		ofxCvColorImage nextImgIn;
      ofxCvColorImage lastImgIn;
      ofxCvGrayscaleImage imgDiff;
  		ofxCvGrayscaleImage nextImgInGray;
      ofxCvGrayscaleImage lastImgInGray;
  		ofImage imgOut;
  		bool autoRun = false;   // auto run every frame?
      bool bLearnBackground = true;

      const int imWidth = 400;
      const int imHeight = 400;
      const int videoGrabberWidth = 640;
      const int videoGrabberHeight = 480;
      const int fboWidth = 512;
      const int fboHeight = 512;

      ofVideoGrabber vidGrabber;
      ofxVideoRecorder vidRecorder;

  		// time metrics
  		std::chrono::time_point<std::chrono::system_clock> start;
  		std::chrono::time_point<std::chrono::system_clock> end;

    private:
      template <typename T>
      bool drawImage(const T& img, std::string label, int x, int y, int w, int h);

      void addGui();
      void randomiseLatentVector();
      void setLatentVector();
      void setPixelsToTrack();
      void record();

      SceneManager * sceneManager = SceneManager::getInstance();
      vector<float> latentVector;
      vector<float> pixelsToTrack;

      ofxDatGuiButton* backButton;
      ofxDatGuiButton* randomiseButton;
      ofxDatGuiButton* recordButton;
      ofxDatGuiButton* setExportFolderButton;
      ofxDatGuiButton* exportPictureButton;

      vector <ofxDatGuiComponent*> gui;

      LatentGraphWidget latentGraphWidget;

      default_random_engine generator;
      normal_distribution<float> distributionX;
      normal_distribution<float> distributionY;

      bool hasChosenExportFolder = false;
      bool recording = false;
      string exportDir;



};

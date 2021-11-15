#pragma once
#include "../baseScene.h"
#include "../sceneManager.h"
#include "../sceneTypes.h"
#include "inputPix2PixScene.h"
#include <time.h>
#include "ofxTensorFlow2.h"
#include "ofxVideoRecorder.h"
#include "ofxOpenCv.h"


class InteractP2PInputScene : public BaseScene {
    public:
      void setup();
      void update();
      void draw();
      void refresh();
      void onButtonEvent(ofxDatGuiButtonEvent e);
      void startThread();
      void stopThread();

    private:
      template <typename T>
      bool drawImage(const T& img, std::string label, int x, int y, int width, int height);
			void setupGui();
			void record();
			void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);

      void refreshModel2();

			SceneManager * sceneManager = SceneManager::getInstance();

  		// model
  		ImageToImageModel model;

  		int nnWidth = 512;
  		int nnHeight = 512;

      int model2Width = 0;
      int model2Height = 0;

      ImageToImageModel model2;
  		cppflow::tensor input;
  		cppflow::tensor output;

  		cppflow::tensor input2;
  		cppflow::tensor output2;

			const int drawWidth = 400;
			const int drawHeight = 400;

      const int videoGrabberWidth = 640;
      const int videoGrabberHeight = 480;
  		// draw
  		ofFbo fbo;

  		ofImage webcamOut;
  		ofImage imgIn;
  		ofImage imgIn2;
  		ofImage imgOut;

      ofxCvColorImage imgInCV;
      ofxCvGrayscaleImage imgInGray;

  		bool autoRun = false;   // auto run every frame?
      ofVideoGrabber vidGrabber;
	    ofxVideoRecorder vidRecorder;

  		// time metrics
  		std::chrono::time_point<std::chrono::system_clock> start;
  		std::chrono::time_point<std::chrono::system_clock> end;

			string exportDir;
			bool recording = false;
			bool hasChosenExportFolder = false;
      bool hasChosenModel = false;
      string model2Dir;
      string msg;



			vector <ofxDatGuiComponent *> gui;
			vector <ofxDatGuiComponent *> modelSelectGui;

			ofxDatGuiButton* exportPictureButton;
			ofxDatGuiButton* exportPictureButton2;
			ofxDatGuiButton* setExportFolderButton;
      ofxDatGuiButton* recordButton;
      ofxDatGuiToggle* invertToggle;
			ofxDatGuiButton* backButton;

			ofxDatGuiButton* modelSelectButton;
      ofxDatGuiSlider* imgWidthSlider;
      ofxDatGuiSlider* imgHeightSlider;
      ofxDatGuiToggle* imgChannelToggle;
      ofxDatGuiButton* continueButton;
};

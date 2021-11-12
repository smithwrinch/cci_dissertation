#pragma once
#include "../baseScene.h"
#include "../sceneManager.h"
#include "../sceneTypes.h"
#include "inputPix2PixScene.h"
#include <time.h>
#include "ofxTensorFlow2.h"
#include "ofxVideoRecorder.h"
#include "../dialWidget.h"
#include "../latentGraphWidget.h"
#include "ofxOpenCv.h"


class InteractMLInputScene : public BaseScene {
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
      void randomiseLatentVector();
  		void setupDrawingTool(std::string modelDir);
			void setupGui();
			void record();
			void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);

      void refreshModel2();

			SceneManager * sceneManager = SceneManager::getInstance();

  		// model
  		ImageToImageModel model;

  		int nnWidth = 512;
  		int nnHeight = 512;


      ofxTF2::ThreadedModel model2;
  		cppflow::tensor input;
  		cppflow::tensor output;

  		cppflow::tensor input2;
  		cppflow::tensor output2;

      int latentDim;
      vector<float> latentVector;

			const int drawWidth = 400;
			const int drawHeight = 400;

  		// draw
  		ofFbo fbo;
  		ofImage imgIn;
  		ofImage imgIn2;
  		ofImage imgOut;

      ofxCvColorImage imgInCV;
      ofxCvGrayscaleImage imgInGray;

  		bool autoRun = false;   // auto run every frame?

			// for video grabbing
	    ofxVideoRecorder vidRecorder;
  		// drawing tool

  		// time metrics
  		std::chrono::time_point<std::chrono::system_clock> start;
  		std::chrono::time_point<std::chrono::system_clock> end;

			string exportDir;
			bool recording = false;
			bool hasChosenExportFolder = false;
      bool hasChosenModel = false;
      string model2Dir;
      string msg;


      DialWidget dialWidget;
      LatentGraphWidget latentGraphWidget;

			vector <ofxDatGuiComponent *> gui;
			vector <ofxDatGuiComponent *> modelSelectGui;

			ofxDatGuiButton* exportPictureButton;
			ofxDatGuiButton* exportPictureButton2;
			ofxDatGuiButton* setExportFolderButton;
      ofxDatGuiButton* recordButton;
      ofxDatGuiButton* randomiseButton;
      ofxDatGuiButton* toggleModeButton;
      ofxDatGuiToggle* invertToggle;
			ofxDatGuiButton* backButton;

			ofxDatGuiButton* modelSelectButton;
      ofxDatGuiSlider* latentDimSlider;
      ofxDatGuiSlider* imgWidthSlider;
      ofxDatGuiSlider* imgHeightSlider;
      ofxDatGuiToggle* imgChannelToggle;
      ofxDatGuiButton* continueButton;
};

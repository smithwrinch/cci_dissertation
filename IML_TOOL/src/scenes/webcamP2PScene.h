#pragma once
#include "../baseScene.h"
#include "../sceneManager.h"
#include "../sceneTypes.h"
#include "inputPix2PixScene.h"
#include <time.h>
#include "ofxTensorFlow2.h"
#include "ofxVideoRecorder.h"
#include "ofxOpenCv.h"
// #include "ofxCv.h"

class WebcamP2PScene : public BaseScene {
    public:
      void setup();
      void update();
      void draw();
      void refresh();
      void onButtonEvent(ofxDatGuiButtonEvent e);
      void keyPressed(int key);
      void mouseDragged(int x, int y, int button);
      void mousePressed( int x, int y, int button);
      void mouseReleased(int x, int y, int button);
      void dragEvent(ofDragInfo dragInfo);
      void startThread();
      void stopThread();

    private:

			void setupGui();
			void record();
			void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);
  		template <typename T>
  		bool drawImage(const T& img, std::string label, int x, int y, int width, int height);
      void updateNormalImage();
      void updateContourImage();
      void updateCannyImage();

      SceneManager * sceneManager = SceneManager::getInstance();


  		// model
  		ImageToImageModel model;
  		cppflow::tensor input;
  		cppflow::tensor output;
  		int nnWidth = 512;  // smaller = faster processing but lower resolution
  		int nnHeight = 512; // smaller = faster processing but lower resolution

			int drawX = 0;
			int drawY = 0;
			int drawWidth = 256;
			int drawHeight = 256;

      const int videoGrabberWidth = 640;
      const int videoGrabberHeight = 480;

  		// draw
  		ofFbo fbo;
      ofImage imgIn;
  		ofImage imgOut;
  		bool autoRun = false;   // auto run every frame?

      ofxCvColorImage imgInCV;
      ofxCvGrayscaleImage imgInGray, grayBg, grayDiff;
      ofxCvGrayscaleImage canny;
      ofxCvColorImage contourImg;
      ofxCvGrayscaleImage r,g,b;


      int imageMode = 0; //0 - normal, 1 - contours, 2 - canny edges
      bool inverted = false;

      bool bLearnBackground = true;
      ofxCvContourFinder contourFinder;
  		ofFbo contourFbo;

			// for video grabbing
      ofVideoGrabber vidGrabber;
      // video recording
	    ofxVideoRecorder    vidRecorder;
      // a third useless comment

  		// time metrics
  		std::chrono::time_point<std::chrono::system_clock> start;
  		std::chrono::time_point<std::chrono::system_clock> end;

			string exportDir;
			bool recording = false;
			bool hasChosenExportFolder = false;

			vector <ofxDatGuiComponent *> gui;
      vector <ofxDatGuiComponent *> guiNormal;
      vector <ofxDatGuiComponent *> guiContours;
      vector <ofxDatGuiComponent *> guiCanny;

			ofxDatGuiButton* exportPictureButton;
			ofxDatGuiButton* exportPictureButton2;
			ofxDatGuiButton* setExportFolderButton;
			ofxDatGuiButton* recordButton;
			ofxDatGuiButton* backButton;

      ofxDatGuiButton* toggleModeButton;

      //0 -- normal mode
      ofxDatGuiToggle* grayscaleToggle;
      ofxDatGuiToggle* invertToggle;
      ofxDatGuiSlider* blurSlider;
      ofxDatGuiSlider* contrastSlider;
      ofxDatGuiSlider* brightnessSlider;

      //1 -- contours mode
      ofxDatGuiButton* getBackgroundButton;
      ofxDatGuiSlider* contourThresholdSlider;
      ofxDatGuiSlider* minContourSlider;
      ofxDatGuiSlider* maxContourSlider;
      ofxDatGuiSlider* blobsToConsiderSlider;
      ofxDatGuiToggle* invertToggleContour;
      ofxDatGuiSlider* lineWidthSizeSlider;
      // ofxDatGuiToggle* findHolesToggle;
      // ofxDatGuiToggle* useApproximationToggle;

      //2 -- canny mode
      ofxDatGuiToggle* invertToggleCanny;
      ofxDatGuiSlider* minThresholdSlider;
      ofxDatGuiSlider* maxThresholdSlider;
      // ofxDatGuiSlider* maxThresholdSlider;
};

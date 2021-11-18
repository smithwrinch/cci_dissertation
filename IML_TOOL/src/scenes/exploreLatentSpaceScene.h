#pragma once

#include "../baseScene.h"
#include "../sceneManager.h"
#include "../sceneTypes.h"
#include "../dialWidget.h"
#include "../latentGraphWidget.h"

#include "ofxTensorFlow2.h"
#include "ofxGui.h"
#include <math.h>
#include "ofxVideoRecorder.h"

class ExploreLatentSpaceScene : public BaseScene {
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

      const int imWidth = 400;
      const int imHeight = 400;
  		// draw
  		ofFbo fbo;
  		ofFbo fboRadialWidget;
  		ofImage imgOut;
  		bool autoRun = false;   // auto run every frame?

  		// time metrics
  		std::chrono::time_point<std::chrono::system_clock> start;
  		std::chrono::time_point<std::chrono::system_clock> end;

    private:
      template <typename T>
      bool drawImage(const T& img, std::string label, int w, int h);

      void addGui();
      void randomiseLatentVector();
      void setLatentVector();
      void updateLatentVector();
      void resetSpeedVector();
      void record();

      SceneManager * sceneManager = SceneManager::getInstance();
      vector<float> latentVector;
      vector<float> speedVector;

      bool dirWidget = false; // direction widgets


      ofxVideoRecorder vidRecorder;
      // ofxPanel gui;
      // ofxIntSlider latentVectorSelect;
      // ofxFloatSlider latentVectorSet;
      // ofxButton randomiseButton;

      ofxDatGuiButton* backButton;
      ofxDatGuiSlider* latentVectorSelectSlider;
      ofxDatGuiSlider* latentVectorSlider;
      ofxDatGuiButton* randomiseButton;
      ofxDatGuiButton* toggleWidgetsButton;
      ofxDatGuiButton* setAllButton;
      ofxDatGuiButton* resetSpeedButton;
      ofxDatGuiSlider* speedSlider;
      ofxDatGuiButton* recordButton;
      ofxDatGuiButton* setExportFolderButton;
      ofxDatGuiButton* exportPictureButton;

      DialWidget dialWidget;
      LatentGraphWidget latentGraphWidget;

      DialWidget speedDialWidget;
      LatentGraphWidget speedLatentGraphWidget;

      vector <ofxDatGuiComponent*> gui;


      bool hasChosenExportFolder = false;
      bool recording = false;
      string exportDir;

};

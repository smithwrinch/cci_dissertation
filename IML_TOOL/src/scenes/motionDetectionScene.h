#pragma once

#include "../baseScene.h"
#include "../sceneManager.h"
#include "../sceneTypes.h"

#include "ofxTensorFlow2.h"
#include "ofxGui.h"

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
  		ofImage imgOut;
  		bool autoRun = false;   // auto run every frame?
      bool bSetBackground = true;

      const int imWidth = 400;
      const int imHeight = 400;

  		// time metrics
  		std::chrono::time_point<std::chrono::system_clock> start;
  		std::chrono::time_point<std::chrono::system_clock> end;

    private:
      template <typename T>
      bool drawImage(const T& img, std::string label, int w, int h);

      void addGui();
      void randomiseLatentVector();
      void setLatentVector();

      SceneManager * sceneManager = SceneManager::getInstance();
      vector<float> latentVector;


      ofxDatGuiButton* backButton;
      ofxDatGuiButton* randomiseButton;
      ofxDatGuiButton* setBackgroundButton;

      vector <ofxDatGuiComponent*> gui;

};

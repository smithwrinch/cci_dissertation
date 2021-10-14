#pragma once

#include "../baseScene.h"
#include "../sceneManager.h"
#include "../sceneTypes.h"

#include "ofxTensorFlow2.h"
#include "ofxGui.h"

class InputGANScene : public BaseScene {
    public:
      void setup();
      void update();
      void draw();

  		void startThread();
  		void stopThread();
      void onButtonEvent(ofxDatGuiButtonEvent e);
    protected:
      ofxTF2::ThreadedModel model;
  		cppflow::tensor input;
      cppflow::tensor output;
  		int nnWidth = 512;  // smaller = faster processing but lower resolution
  		int nnHeight = 512; // smaller = faster processing but lower resolution

  		// draw
  		ofFbo fbo;
  		ofImage imgOut;
  		bool autoRun = false;   // auto run every frame?

  		// time metrics
  		std::chrono::time_point<std::chrono::system_clock> start;
  		std::chrono::time_point<std::chrono::system_clock> end;

    private:
      template <typename T>
      bool drawImage(const T& img, std::string label);

      void randomiseLatentVector();
      void setLatentVectorAt(int i, float val);

      SceneManager * sceneManager = SceneManager::getInstance();
      ofxDatGuiButton* backButton;
      vector<float> latentVector;


      ofxPanel gui;
      ofxIntSlider latentVectorSelect;
      ofxFloatSlider latentVectorSet;
      ofxButton randomiseButton;
};

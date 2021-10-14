#pragma once
#include "../baseScene.h"
#include "../sceneManager.h"
#include "../sceneTypes.h"

#include "ofxTensorFlow2.h"


// custom ofxTF2::ThreadedModel with built-in pre- & post-processing
class ImageToImageModel : public ofxTF2::ThreadedModel {

	public:

		// override the runModel function of ThreadedModel
		// this way the thread will take this augmented function
		cppflow::tensor runModel(const cppflow::tensor & input) const override {

			// cast data type and expand to batch size of 1
			auto tempInput = cppflow::cast(input, TF_UINT8, TF_FLOAT);
			tempInput = cppflow::expand_dims(tempInput, 0);

			// apply preprocessing as in python to change range to -1 to 1
			tempInput = cppflow::div(tempInput, cppflow::tensor({127.5f}));
			tempInput = cppflow::sub(tempInput, cppflow::tensor({1.0f}));

			// call to super
			auto output = Model::runModel(tempInput);

			// postprocess to change range to -1 to 1
			output = cppflow::add(output, cppflow::tensor({1.0f}));
			output = cppflow::mul(output, cppflow::tensor({127.5f}));
			return output;
		}
};
class InputPix2PixScene : public BaseScene {
    public:
      void setup();
      void update();
      void draw();
      void onButtonEvent(ofxDatGuiButtonEvent e);
      void keyPressed(int key);
      void mouseDragged(int x, int y, int button);
      void mousePressed( int x, int y, int button);
      void mouseReleased(int x, int y, int button);
      void dragEvent(ofDragInfo dragInfo);
      void startThread();
      void stopThread();

    private:
      SceneManager * sceneManager = SceneManager::getInstance();

      ofxDatGuiButton* backButton;


  		void setupDrawingTool(std::string modelDir);

  		template <typename T>
  		bool drawImage(const T& img, std::string label);

  		// model
  		ImageToImageModel model;
  		cppflow::tensor input;
  		cppflow::tensor output;
  		int nnWidth = 512;  // smaller = faster processing but lower resolution
  		int nnHeight = 512; // smaller = faster processing but lower resolution

  		// draw
  		ofFbo fbo;
  		ofImage imgIn;
  		ofImage imgOut;
  		bool autoRun = false;   // auto run every frame?

  		// drawing tool
  		std::vector<ofColor> colors; // contains color palette
  		int paletteDrawSize = 50;
  		int drawColorIndex = 0;
  		ofColor drawColor;
  		int drawMode = 0;   // 0: draw vs 1: draw boxes
  		int drawRadius = 10;
  		glm::vec2 mousePressPos;

  		// time metrics
  		std::chrono::time_point<std::chrono::system_clock> start;
  		std::chrono::time_point<std::chrono::system_clock> end;
};

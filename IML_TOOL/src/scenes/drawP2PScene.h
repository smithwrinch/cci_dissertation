#pragma once
#include "../baseScene.h"
#include "../sceneManager.h"
#include "../sceneTypes.h"
#include "inputPix2PixScene.h"
#include <time.h>
#include "ofxTensorFlow2.h"
#include "ofxVideoRecorder.h"

class DrawP2PScene : public BaseScene {
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

			std::string currentDate(){
	    time_t now = time(NULL);
	    struct tm tstruct;
	    char buf[40];
	    tstruct = *localtime(&now);
	    //format: day DD-MM-YYYY
	    strftime(buf, sizeof(buf), "%A_%d_%m_%Y", &tstruct);
	    return buf;
		}
		std::string currentTime(){
		    time_t now = time(NULL);
		    struct tm tstruct;
		    char buf[40];
		    tstruct = *localtime(&now);
		    //format: HH:mm:ss
		    strftime(buf, sizeof(buf), "%X", &tstruct);
		    return buf;
		}

  		void setupDrawingTool(std::string modelDir);
			void setupGui();
			void record();
			void recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args);
  		template <typename T>
  		bool drawImage(const T& img, std::string label, int x, int y, int width, int height);

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

  		// draw
  		ofFbo fbo;
  		ofImage imgIn;
  		ofImage imgOut;
  		bool autoRun = false;   // auto run every frame?

			// for video grabbing
	    ofxVideoRecorder    vidRecorder;

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

			string exportDir;
			bool recording = false;
			bool hasChosenExportFolder = false;
			int controlMode = 0; // 0 - text 1 - pallette 2 - buttons

			vector <ofxDatGuiComponent *> gui;
			vector <ofxDatGuiComponent *> guiControls;

			ofxDatGuiButton* exportPictureButton;
			ofxDatGuiButton* exportPictureButton2;
			ofxDatGuiButton* setExportFolderButton;
			ofxDatGuiButton* recordButton;
			ofxDatGuiButton* toggleControlsButton;
			ofxDatGuiButton* addColourButton;
			ofxDatGuiSlider* brushRadius;
			ofxDatGuiColorPicker* colourPicker;
			ofxDatGuiButton* backButton;
};

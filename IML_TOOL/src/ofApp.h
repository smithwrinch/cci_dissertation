#pragma once

#include "ofxDatGui.h"

#include "ofMain.h"
#include "baseScene.h"
#include "sceneTypes.h"
#include "sceneManager.h"
#include "model.h"
#include "scenes/menuScene.h"
#include "scenes/playModelSelectorScene.h"
#include "scenes/inputPix2PixScene.h"
#include "scenes/inputGANScene.h"
#include "scenes/architectureBasicBuilderScene.h"
#include "scenes/architectureTypeSelectorScene.h"
#include "scenes/trainScene.h"
#include "scenes/datasetMainScene.h"
#include "scenes/datasetSelectorScene.h"
#include "scenes/datasetBuilderScene.h"
#include "scenes/datasetPix2PixBuilderScene.h"
#include "scenes/datasetManualBuilderScene.h"
#include "scenes/interactMenuScene.h"
#include "scenes/exploreLatentSpaceScene.h"
#include "scenes/syncMusicScene.h"
#include "scenes/exportGifScene.h"
#include "scenes/drawP2PScene.h"
#include "scenes/webcamP2PScene.h"
#include "scenes/motionDetectionScene.h"
#include "scenes/interactCustomScene.h"
#include "interactP2PVideoScene.h"
#include "interactMLInputScene.h"
#include "interactP2PInputScene.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);


		SceneManager * sceneManager = SceneManager::getInstance();
		ModelManager * modelManager = ModelManager::getInstance();


	private:
		MenuScene menuScene;
		TrainingScene trainingScene;
		PlayModelSelectorScene playModelSelectorScene;
		InputPix2PixScene inputPix2PixScene;
		InputGANScene inputGANScene;
		ArchitectureTypeSelectScene architectureSelectorScene;
		ArchitectureBasicBuilderScene architectureBuilderScene;
		DatasetSelectorScene datasetSelectorScene;
		DatasetMainScene datasetMainScene;
		DatasetBuilderScene datasetBuilderScene;
		DatasetPix2PixBuilderScene datasetPix2PixBuilderScene;
		DatasetManualBuilderScene datasetManualBuilderScene;
		InteractMenuScene interactMenuScene;
		ExploreLatentSpaceScene exploreLatentSpaceScene;
		SyncMusicScene syncMusicScene;
		ExportGifScene exportGifScene;
		DrawP2PScene drawP2PScene;
		WebcamP2PScene webcamP2PScene;
		MotionDetectionScene motionDetectionScene;
		InteractCustomScene interactCustomScene;
		InteractP2PVideoScene videoP2PScene;
		InteractMLInputScene interactMLScene;
		InteractP2PInputScene interactP2PInputScene;

		void positionButtons();
		void onButtonEvent(ofxDatGuiButtonEvent e);

		ofxDatGuiButton* menuButton;
		ofxDatGuiButton* architectureButton;
		ofxDatGuiButton* datasetButton;
		ofxDatGuiButton* trainButton;
		ofxDatGuiButton* evaluateButton;

		ofxDatGuiButton* architectureButton_;
		ofxDatGuiButton* datasetButton_;
		ofxDatGuiButton* trainButton_;
		ofxDatGuiButton* evaluateButton_;

		vector<ofxDatGuiButton*> topGui;
		vector<ofxDatGuiButton*> topGuiSelected;


		int idToHighlight;

		string printModelType(MODEL_TYPE::ID id){
			if(id == MODEL_TYPE::GAN){
				return "GAN";
			}
			else if(id == MODEL_TYPE::PIX2PIX){
				return "PIX2PIX";
			}
			else{
				return "";
			}
		}

};

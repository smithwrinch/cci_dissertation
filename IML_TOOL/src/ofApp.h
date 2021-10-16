#pragma once

#include "ofxDatGui.h"

#include "ofMain.h"
#include "baseScene.h"
#include "sceneTypes.h"
#include "sceneManager.h"
#include "model.h"
#include "scenes/trainScene.h"
#include "scenes/menuScene.h"
#include "scenes/playModelSelectorScene.h"
#include "scenes/inputPix2PixScene.h"
#include "scenes/inputGANScene.h"
#include "scenes/architectureBasicBuilderScene.h"
#include "scenes/architectureTypeSelectorScene.h"


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

		ofxDatGuiButton* button;

	private:
		MenuScene menuScene;
		TrainingScene trainingScene;
		PlayModelSelectorScene playModelSelectorScene;
		InputPix2PixScene inputPix2PixScene;
		InputGANScene inputGANScene;
		ArchitectureTypeSelectScene architectureSelectorScene;
		ArchitectureBasicBuilderScene architectureBuilderScene;
		void positionButtons();
		void onButtonEvent(ofxDatGuiButtonEvent e);
};

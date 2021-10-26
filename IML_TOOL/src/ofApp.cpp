#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  // sceneManager = SceneManager::getInstance();
  menuScene.setup();
  sceneManager->addScene(&menuScene);
  playModelSelectorScene.setup();
  sceneManager->addScene(&playModelSelectorScene);
  inputPix2PixScene.setup();
  sceneManager->addScene(&inputPix2PixScene);
  inputGANScene.setup();
  sceneManager->addScene(&inputGANScene);
  trainingScene.setup();
  sceneManager->addScene(&trainingScene);

  architectureSelectorScene.setup();
  sceneManager->addScene(&architectureSelectorScene);
  architectureBuilderScene.setup();
  sceneManager->addScene(&architectureBuilderScene);

  datasetMainScene.setup();
  sceneManager->addScene(&datasetMainScene);
  datasetSelectorScene.setup();
  sceneManager->addScene(&datasetSelectorScene);

  sceneManager->changeSceneTo(SCENE_TYPE::MENU);
}

//--------------------------------------------------------------
void ofApp::update(){
  sceneManager->getCurrentScene()->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    sceneManager->getCurrentScene()->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    // sceneManager.changeSceneTo(SCENE_TYPE::TRAIN);
    sceneManager->getCurrentScene()->keyPressed(key);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
  sceneManager->getCurrentScene()->mouseDragged(x,y,button);
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
  sceneManager->getCurrentScene()->mousePressed(x, y, button);

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
  sceneManager->getCurrentScene()->mouseReleased(x,y,button);
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
  sceneManager->getCurrentScene()->dragEvent(dragInfo);
}

// //--------------------------------------------------------------
// void ofApp::onButtonEvent(ofxDatGuiButtonEvent e){
//
//     sceneManager.getCurrentScene()->onButtonEvent(e);
// }

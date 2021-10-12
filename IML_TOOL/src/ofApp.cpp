#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  menuScene.setup();
  sceneManager.addScene(&menuScene);
  trainingScene.setup();
  sceneManager.addScene(&trainingScene);
  sceneManager.changeSceneTo(SCENE_TYPE::MENU);
}

//--------------------------------------------------------------
void ofApp::update(){
  sceneManager.getCurrentScene()->update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    sceneManager.getCurrentScene()->draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    // sceneManager.changeSceneTo(SCENE_TYPE::TRAIN);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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

}

// //--------------------------------------------------------------
// void ofApp::onButtonEvent(ofxDatGuiButtonEvent e){
//
//     sceneManager.getCurrentScene()->onButtonEvent(e);
// }

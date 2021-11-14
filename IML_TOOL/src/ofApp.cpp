#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  // sceneManager = SceneManager::getInstance();

  float width = 1024 / 5;
  menuButton = new ofxDatGuiButton("MENU");
  menuButton->setWidth(width, 0.5);
  menuButton->setPosition(0, 0);
  menuButton->onButtonEvent(this, &ofApp::onButtonEvent);

  architectureButton = new ofxDatGuiButton("ARCHITECTURE");
  architectureButton->setWidth(width, 0.5);
  architectureButton->setPosition(width, 0);
  architectureButton->onButtonEvent(this, &ofApp::onButtonEvent);

  datasetButton = new ofxDatGuiButton("DATASET");
  datasetButton->setWidth(width, 0.5);
  datasetButton->setPosition(width*2, 0);
  datasetButton->onButtonEvent(this, &ofApp::onButtonEvent);

  trainButton = new ofxDatGuiButton("TRAIN");
  trainButton->setWidth(width, 0.5);
  trainButton->setPosition(width*3, 0);
  trainButton->onButtonEvent(this, &ofApp::onButtonEvent);

  evaluateButton = new ofxDatGuiButton("INTERACT");
  evaluateButton->setWidth(width, 0.5);
  evaluateButton->setPosition(width*4, 0);
  evaluateButton->onButtonEvent(this, &ofApp::onButtonEvent);

  topGui.push_back(menuButton);
  topGui.push_back(architectureButton);
  topGui.push_back(datasetButton);
  topGui.push_back(trainButton);
  topGui.push_back(evaluateButton);

  for (int i; i < topGui.size(); i++){
    topGui[i]->setStripeColor(ofColor(255,255,255));
  }
////////////////////////////////////////////////////////////////


  architectureButton_ = new ofxDatGuiButton("-ARCHITECTURE-");
  architectureButton_->setWidth(width, 0.5);
  architectureButton_->setPosition(width, 0);

  datasetButton_ = new ofxDatGuiButton("-DATASET-");
  datasetButton_->setWidth(width, 0.5);
  datasetButton_->setPosition(width*2, 0);

  trainButton_ = new ofxDatGuiButton("-TRAIN-");
  trainButton_->setWidth(width, 0.5);
  trainButton_->setPosition(width*3, 0);

  evaluateButton_ = new ofxDatGuiButton("-INTERACT-");
  evaluateButton_->setWidth(width, 0.5);
  evaluateButton_->setPosition(width*4, 0);

  topGuiSelected.push_back(architectureButton_);
  topGuiSelected.push_back(datasetButton_);
  topGuiSelected.push_back(trainButton_);
  topGuiSelected.push_back(evaluateButton_);

  for (int i; i < topGuiSelected.size(); i++){
    topGuiSelected[i]->setStripeColor(ofColor(0,255,0));
  }

///////////////////////////////////////////////////////////////
  menuScene.setup();
  sceneManager->addScene(&menuScene);
  playModelSelectorScene.setup();
  sceneManager->addScene(&playModelSelectorScene);
  inputPix2PixScene.setup();
  sceneManager->addScene(&inputPix2PixScene);
  inputGANScene.setup();
  sceneManager->addScene(&inputGANScene);
  interactCustomScene.setup();
  sceneManager->addScene(&interactCustomScene);

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
  datasetBuilderScene.setup();
  sceneManager->addScene(&datasetBuilderScene);
  datasetPix2PixBuilderScene.setup();
  sceneManager->addScene(&datasetPix2PixBuilderScene);
  datasetManualBuilderScene.setup();
  sceneManager->addScene(&datasetManualBuilderScene);



  interactMenuScene.setup();
  sceneManager->addScene(&interactMenuScene);
  exploreLatentSpaceScene.setup();
  sceneManager->addScene(&exploreLatentSpaceScene);
  syncMusicScene.setup();
  sceneManager->addScene(&syncMusicScene);
  exportGifScene.setup();
  sceneManager->addScene(&exportGifScene);
  drawP2PScene.setup();
  sceneManager->addScene(&drawP2PScene);
  webcamP2PScene.setup();
  sceneManager->addScene(&webcamP2PScene);
  motionDetectionScene.setup();
  sceneManager->addScene(&motionDetectionScene);
  videoP2PScene.setup();
  sceneManager->addScene(&videoP2PScene);
  interactMLScene.setup();
  sceneManager->addScene(&interactMLScene);


  sceneManager->changeSceneTo(SCENE_TYPE::MENU);
}

//--------------------------------------------------------------
void ofApp::update(){
  SCENE_TYPE::ID id = sceneManager->getCurrentSceneID();
  sceneManager->getCurrentScene()->update();

  if(!(id == SCENE_TYPE::MENU || id == SCENE_TYPE::PLAY_MODEL_SELECT)){

      if(sceneManager->getShowNavBar()){
        topGui[0]->update();

        if(modelManager->getStatus() >= 1){
          topGui[1]->update();
          topGui[2]->update();
        }
        if(modelManager->getStatus() >= 2){
          topGui[3]->update();
        }
        if(modelManager->getStatus() >= 3){
          topGui[4]->update();
        }
      }
  }
}

//--------------------------------------------------------------
void ofApp::draw(){
    SCENE_TYPE::ID id = sceneManager->getCurrentSceneID();
    sceneManager->getCurrentScene()->draw();
    ofDrawBitmapString(modelManager->getModelName(), 0, menuButton->getHeight()+12);
    // ofDrawBitmapString(ofToString(modelManager->getStatus()), 0, menuButton->getHeight()+36);
    ofDrawBitmapString(printModelType(modelManager->getModelType()), 0, menuButton->getHeight()+24);
    // ofDrawBitmapString(ofToString(sceneManager->getCurrentSceneID()), 0, menuButton->getHeight()+48);

    if(!(id == SCENE_TYPE::MENU || id == SCENE_TYPE::PLAY_MODEL_SELECT)){
      for(int i = 0; i < topGui.size(); i++){

          if(sceneManager->getShowNavBar()){

            topGui[i]->draw();

            if(id <=SCENE_TYPE::DATASET_SELECTOR ){
              topGuiSelected[1]->draw();
            }
            else if(id <= SCENE_TYPE::ARCHITECTURE_SELECT){
                topGuiSelected[0]->draw();
            }

            else if(id == SCENE_TYPE::TRAIN){
                topGuiSelected[2]->draw();
            }

            else{
              topGuiSelected[3]->draw();
            }
          }
        }
      }
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
void ofApp::onButtonEvent(ofxDatGuiButtonEvent e){
    if(e.target == menuButton){
      modelManager->reset();
      sceneManager->changeSceneTo(SCENE_TYPE::MENU);
    }
    if(e.target == architectureButton){
      BaseScene * scene = sceneManager->getScene(SCENE_TYPE::ARCHITECTURE_MENU);
      scene->refresh();
      sceneManager->changeSceneTo(SCENE_TYPE::ARCHITECTURE_MENU);
    }
    if(e.target == datasetButton){
      BaseScene * scene = sceneManager->getScene(SCENE_TYPE::DATASET_MENU);
      scene->refresh();
      sceneManager->changeSceneTo(SCENE_TYPE::DATASET_MENU);
    }
    if(e.target == trainButton){
      BaseScene * scene = sceneManager->getScene(SCENE_TYPE::TRAIN);
      scene->refresh();;
      sceneManager->changeSceneTo(SCENE_TYPE::TRAIN);
    }
    if(e.target == evaluateButton){
      sceneManager->changeSceneTo(SCENE_TYPE::INTERACT_MENU);
    }
}

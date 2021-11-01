#include "architectureTypeSelectorScene.h"

void ArchitectureTypeSelectScene::refresh(){
  state = 0;
}

void ArchitectureTypeSelectScene::setup(){
  setID(SCENE_TYPE::ARCHITECTURE_MENU);
  state = 0;
  int width = 600;
  int centreX = ofGetWidth()/2 - width / 2;
  buildButton = new ofxDatGuiButton("BUILD ARCHITECTURE");
  buildButton->setPosition(ofGetWidth()/2 - buildButton->getWidth()/2, ofGetHeight()/2 - buildButton->getHeight());
  buildButton->onButtonEvent(this, &ArchitectureTypeSelectScene::onButtonEvent);

  selectButton = new ofxDatGuiButton("LOAD ARCHITECTURE");
  selectButton->setPosition(buildButton->getX(), buildButton->getY() + buildButton->getHeight() + 50);
  selectButton->onButtonEvent(this, &ArchitectureTypeSelectScene::onButtonEvent);


  rebuildArchitectureButton = new ofxDatGuiButton("REBUILD ARCHITECTURE");
  rebuildArchitectureButton->setPosition(centreX, ofGetHeight()/2);
  rebuildArchitectureButton->setStripeColor(ofColor(255,0,0));
  rebuildArchitectureButton->setWidth(width);
  rebuildArchitectureButton->onButtonEvent(this, &ArchitectureTypeSelectScene::onButtonEvent);

  confirmButton = new ofxDatGuiButton("CONFIRM");
  confirmButton->setPosition(ofGetWidth() / 2 - confirmButton->getWidth() / 2 + 300, ofGetHeight()/2);
  confirmButton->setStripeColor(ofColor(255,0,0));
  confirmButton->onButtonEvent(this, &ArchitectureTypeSelectScene::onButtonEvent);

  unconfirmButton = new ofxDatGuiButton("BACK<-");
  unconfirmButton->setPosition(ofGetWidth() / 2 - confirmButton->getWidth() / 2 - 300, ofGetHeight()/2);
  unconfirmButton->onButtonEvent(this, &ArchitectureTypeSelectScene::onButtonEvent);

}

void ArchitectureTypeSelectScene::update(){
  if(ModelManager::getInstance()->getStatus() < 1){
    selectButton->update();
    buildButton->update();
  }
  else{
    if(state != 1){
      rebuildArchitectureButton->update();
    }
    else{
      confirmButton->update();
      unconfirmButton->update();
    }
  }
}

void ArchitectureTypeSelectScene::draw(){
  if(ModelManager::getInstance()->getStatus() < 1){
    selectButton->draw();
    buildButton->draw();
  }
  else{
    if(state != 1){
      rebuildArchitectureButton->draw();
    }
    else{
      ofDrawBitmapString("[WARNING] This is a destructive action. It will delete all training progress.", ofGetWidth() / 2 - 300, ofGetHeight() / 2 - 100);

      confirmButton->draw();
      unconfirmButton->draw();
    }
  }
}

void ArchitectureTypeSelectScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == selectButton){

  }
  else if(e.target == buildButton){

    ArchitectureBasicBuilderScene * scene = (ArchitectureBasicBuilderScene *) SceneManager::getInstance()->getScene(SCENE_TYPE::ARCHITECTURE_BUILDER);
    scene->refresh();
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::ARCHITECTURE_BUILDER);
  }

  else if(e.target == rebuildArchitectureButton){
    state = 1;
  }
  else if(e.target == confirmButton){
    string basePath = "saved_models/"+ModelManager::getInstance()->getModelName();
    ofDirectory::removeDirectory(basePath+"/saved_networks/ckpt", true);
    ofDirectory::removeDirectory(basePath+"/images", true);
    ModelManager::getInstance()->setEpochsTrained(0);
    ModelManager::getInstance()->setStatus(0);
    ModelManager::getInstance()->save();
    state = 0;
  }
  else if(e.target == unconfirmButton){
    state = 0;
  }
}

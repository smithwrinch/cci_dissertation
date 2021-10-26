#include "datasetMainScene.h"

void DatasetMainScene::setup(){
  setID(SCENE_TYPE::DATASET_MENU);
  buildButton = new ofxDatGuiButton("BUILD DATASET");
  buildButton->setPosition(ofGetWidth()/2 - buildButton->getWidth()/2, ofGetHeight()/2 - buildButton->getHeight());
  buildButton->onButtonEvent(this, &DatasetMainScene::onButtonEvent);

  selectButton = new ofxDatGuiButton("LOAD DATASET");
  selectButton->setPosition(buildButton->getX(), buildButton->getY() + buildButton->getHeight() + 50);
  selectButton->onButtonEvent(this, &DatasetMainScene::onButtonEvent);
}

void DatasetMainScene::update(){
  selectButton->update();
  buildButton->update();
}

void DatasetMainScene::draw(){
  selectButton->draw();
  buildButton->draw();
}

void DatasetMainScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == selectButton){
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_SELECTOR);
  }
  if(e.target == buildButton){
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_BUILDER);
  }
}

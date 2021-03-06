#include "datasetMainScene.h"

void DatasetMainScene::setup(){
  setID(SCENE_TYPE::DATASET_MENU);
  buildButton = new ofxDatGuiButton("BUILD DATASET");
  buildButton->setPosition(ofGetWidth()/2 - buildButton->getWidth()/2, ofGetHeight()/2 - buildButton->getHeight());
  buildButton->onButtonEvent(this, &DatasetMainScene::onButtonEvent);

  selectButton = new ofxDatGuiButton("LOAD DATASET");
  selectButton->setPosition(buildButton->getX(), buildButton->getY() + buildButton->getHeight() + 50);
  selectButton->onButtonEvent(this, &DatasetMainScene::onButtonEvent);

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(ofGetWidth() / 2 - 1.5 * backButton->getWidth(), ofGetHeight() - 100);
  backButton->onButtonEvent(this, &DatasetMainScene::onButtonEvent);

  playButton = new ofxDatGuiButton("CONTINUE");
  playButton->setPosition(ofGetWidth() / 2 + playButton->getWidth()/2, ofGetHeight() - 100);
  playButton->onButtonEvent(this, &DatasetMainScene::onButtonEvent);

}

void DatasetMainScene::update(){
  selectButton->update();
  buildButton->update();
  backButton->update();
  if(ModelManager::getInstance()->getStatus() > 2){
      playButton->update();
  }
}

void DatasetMainScene::draw(){
  selectButton->draw();
  buildButton->draw();
  backButton->draw();
  if(ModelManager::getInstance()->getStatus() > 2){
      playButton->draw();
  }
}

void DatasetMainScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == selectButton){
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_SELECTOR);
  }
  else if(e.target == buildButton){
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_BUILDER);
  }
  else if(e.target == backButton){
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::ARCHITECTURE_MENU);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::ARCHITECTURE_MENU);
  }
  else if(e.target == playButton){
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::TRAIN);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::TRAIN);
  }
}

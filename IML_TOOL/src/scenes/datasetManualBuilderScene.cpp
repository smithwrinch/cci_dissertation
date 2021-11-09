#include "datasetManualBuilderScene.h"

// void DatasetManualBuilderScene::refresh(){
//
// }

void DatasetManualBuilderScene::setup(){
  setID(SCENE_TYPE::DATASET_BUILDER_PIX2PIX_MANUAL);
  // setupGui();
}

void DatasetManualBuilderScene::update(){
  selectButton->update();
  buildButton->update();
}

void DatasetManualBuilderScene::draw(){
  selectButton->draw();
  buildButton->draw();
}

void DatasetManualBuilderScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == selectButton){
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_SELECTOR);
  }
  if(e.target == buildButton){
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_BUILDER);
  }
}

// void DatasetManualBuilder::setupGui(){
//
// }

#include "datasetManualBuilderScene.h"

// void DatasetManualBuilderScene::refresh(){
//
// }

void DatasetManualBuilderScene::setup(){
  setID(SCENE_TYPE::DATASET_BUILDER_PIX2PIX_MANUAL);
  int width= 300;
  int centreX = ofGetWidth()/2 -width / 2;

  buildButton = new ofxDatGuiButton("Build");
  manualButton = new ofxDatGuiButton("SELECT MANUALLY");
  backButton = new ofxDatGuiButton("BACK<-");
  label1 = new ofxDatGuiLabel(" For example: \"*_in\" for \"122_in.png\" ");
  label2 = new ofxDatGuiLabel(" For example: \"out_*\" for \"out_82.png\" ");
  inputText = new ofxDatGuiTextInput("INPUT FORMAT", "");
  outputText = new ofxDatGuiTextInput("OUTPUT FORMAT", "");

  gui.push_back(buildButton);
  gui.push_back(manualButton);
  gui.push_back(backButton);
  gui.push_back(label1);
  gui.push_back(label2);
  gui.push_back(inputText);
  gui.push_back(outputText);

  buildButton->setPosition(centreX, 250);
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
  if(e.target == backButton){
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::);
  }
  if(e.target == buildButton){
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_BUILDER);
  }
}

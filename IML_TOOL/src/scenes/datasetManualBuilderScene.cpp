#include "datasetManualBuilderScene.h"

void DatasetManualBuilderScene::refresh(){
  finished = false;
  running = false;
  msg = "";
}

void DatasetManualBuilderScene::setup(){
  setID(SCENE_TYPE::DATASET_BUILDER_PIX2PIX_MANUAL);
  int width= 300;
  int centreX = ofGetWidth()/2 -width / 2;

  buildButton = new ofxDatGuiButton("Build");
  manualButton = new ofxDatGuiButton("SELECT MANUALLY");
  backButton = new ofxDatGuiButton("BACK<-");
  label1 = new ofxDatGuiLabel(" For example: \"*_in\" for \"122_in.png\" ");
  label2 = new ofxDatGuiLabel(" For example: \"out*\" for \"out82.png\" ");
  inputText = new ofxDatGuiTextInput("INPUT FORMAT", "");
  outputText = new ofxDatGuiTextInput("OUTPUT FORMAT", "");

  buildButton->onButtonEvent(this, &DatasetManualBuilderScene::onButtonEvent);
  manualButton->onButtonEvent(this, &DatasetManualBuilderScene::onButtonEvent);
  backButton->onButtonEvent(this, &DatasetManualBuilderScene::onButtonEvent);

  gui.push_back(buildButton);
  gui.push_back(manualButton);
  gui.push_back(backButton);
  gui.push_back(label1);
  gui.push_back(label2);
  gui.push_back(inputText);
  gui.push_back(outputText);

  buildButton->setPosition(centreX, 250);
  inputText->setPosition(centreX, 250);
  label1->setPosition(centreX, 275);
  outputText->setPosition(centreX, 325);
  label2->setPosition(centreX, 350);

  manualButton->setPosition(centreX, 550);
  backButton->setPosition(50, ofGetHeight() - 50);

  buildButton->setWidth(400);
  inputText->setWidth(400, 50);
  label1->setWidth(400);
  outputText->setWidth(400, 50);
  label2->setWidth(400);
  manualButton->setWidth(400);

  finished = false;
  running = false;
  msg = "";

}

void DatasetManualBuilderScene::update(){

  if(!running){
    for(int i =0; i < gui.size(); i++){
      gui[i]->update();
    }
  }
}

void DatasetManualBuilderScene::draw(){
  if(running){
    if(finished){
      running = false;
    }
    ofDrawBitmapString(msg, ofGetWidth()/2, 250);
  }
  else{
    for(int i =0; i < gui.size(); i++){
      gui[i]->draw();
    }
  }
}

void DatasetManualBuilderScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_BUILDER);
  }
  if(e.target == buildButton){
    string txtDir = "/tmp/status__.txt";
    buildThread.setup(inputText->getText(), outputText->getText(), txtDir);
    statusThread.setup(txtDir, &msg, &finished);
  }
}

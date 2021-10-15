#include "architectureTypeSelectScene.h"

void ArchitectureTypeSelectScene::setup(){
  buildButton = new ofxDatGuiButton("BUILD ARCHITECTURE");
  buildButton->setPosition(ofGetWidth()/2 - buildButton->getWidth()/2, ofGetHeight()/2 - buildButton->getHeight()/2);

  selectButton = new ofxDatGuiButton("BUILD ARCHITECTURE");
  selectButton->setPosition(buildButton->getX(), buildButton->getY() + buildButton->getHeight() + 50);

}

void ArchitectureTypeSelectScene::update(){
  selectButton->update();
  buildButton->update();
}

void ArchitectureTypeSelectScene::draw(){
  selectButton->draw();
  buildButton->draw();
}

void ArchitectureTypeSelect::onButtonEvent(ofxDatGuiButton e){
  if(e.target == selectButton){

  }
  if(e.target == buildButton){
    
  }
}

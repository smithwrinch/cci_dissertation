#include "architectureTypeSelectorScene.h"

void ArchitectureTypeSelectScene::setup(){
  setID(SCENE_TYPE::ARCHITECTURE_MENU);
  buildButton = new ofxDatGuiButton("BUILD ARCHITECTURE");
  buildButton->setPosition(ofGetWidth()/2 - buildButton->getWidth()/2, ofGetHeight()/2 - buildButton->getHeight());
  buildButton->onButtonEvent(this, &ArchitectureTypeSelectScene::onButtonEvent);

  selectButton = new ofxDatGuiButton("LOAD ARCHITECTURE");
  selectButton->setPosition(buildButton->getX(), buildButton->getY() + buildButton->getHeight() + 50);
  selectButton->onButtonEvent(this, &ArchitectureTypeSelectScene::onButtonEvent);
}

void ArchitectureTypeSelectScene::update(){
  selectButton->update();
  buildButton->update();
}

void ArchitectureTypeSelectScene::draw(){
  selectButton->draw();
  buildButton->draw();
}

void ArchitectureTypeSelectScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == selectButton){

  }
  if(e.target == buildButton){

    ArchitectureBasicBuilderScene * scene = (ArchitectureBasicBuilderScene *) SceneManager::getInstance()->getScene(SCENE_TYPE::ARCHITECTURE_BUILDER);
    scene->refresh();
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::ARCHITECTURE_BUILDER);
  }
}

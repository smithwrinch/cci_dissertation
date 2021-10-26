#include "datasetBuilderScene.h"

void DatasetBuilderScene::setup(){
  setID(SCENE_TYPE::DATASET_BUILDER);

  errorLabel = new ofxDatGuiLabel("");
  errorLabel->setWidth(400, 0.5);
  errorLabel->setPosition(ofGetWidth()/2 - errorLabel->getWidth()/2, ofGetHeight()/2 - errorLabel->getHeight());
  errorLabel->setLabelColor(ofColor(255, 0, 0));

  datasetNameInput = new ofxDatGuiTextInput("Dataset Name: ", "");
  datasetNameInput->setWidth(400, 0.5);
  datasetNameInput->setPosition(ofGetWidth()/2 - datasetNameInput->getWidth()/2, errorLabel->getY() + datasetNameInput->getHeight());

  loadFromFolderButton = new ofxDatGuiButton("Load From Folder");
  loadFromFolderButton->setWidth(400, 0.5);
  loadFromFolderButton->setPosition(ofGetWidth()/2 - loadFromFolderButton->getWidth()/2, ofGetHeight()/2 + loadFromFolderButton->getHeight());
  loadFromFolderButton->onButtonEvent(this, &DatasetBuilderScene::onButtonEvent);

  buildButton = new ofxDatGuiButton("BUILD");
  buildButton->setWidth(400, 0.5);
  buildButton->setPosition(ofGetWidth()/2 - buildButton->getWidth()/2, loadFromFolderButton->getY() + buildButton->getHeight()*2);
  buildButton->onButtonEvent(this, &DatasetBuilderScene::onButtonEvent);


}

void DatasetBuilderScene::update(){

  buildButton->update();
  errorLabel->update();
  datasetNameInput->update();
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){

  }
  else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
    loadFromFolderButton->update();
  }
}

void DatasetBuilderScene::draw(){
  loadFromFolderButton->draw();
  buildButton->draw();
  errorLabel->draw();
  datasetNameInput->draw();
}

void DatasetBuilderScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == loadFromFolderButton){
    ofFileDialogResult result = ofSystemLoadDialog("select input folder", true);
    if (result.bSuccess) {
      dir = result.getPath();
    }
  }
  if(e.target == buildButton){
    if(checkTextValid() && checkFnameNew() && checkDir()){
      ofDirectory dir_ = ofDirectory(dir);
      dir_.copyTo("saved_datasets/"+datasetNameInput->getText()+"/");
      ModelManager::getInstance()->setDatasetDir(datasetNameInput->getText());
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::TRAIN);
    }
  }
}

// copied and pasted code bad
bool DatasetBuilderScene::checkTextValid(){
  string name = datasetNameInput->getText();
  if(name.length() == 0){
    errorLabel->setLabel("PLEASE ENTER A NAME");
    return false;
  }
  for(int i = 0; i < name.length(); i++){
        if( !isalpha(name[i]) && !isdigit(name[i])){
            errorLabel->setLabel("ONLY CHARACTERS A-Z [0-9] PERMITTED");
            return false;
          }
      }
  return true;
}

bool DatasetBuilderScene::checkFnameNew(){
  ofDirectory dir = ofDirectory("saved_datasets/"+datasetNameInput->getText());
  if(dir.exists()){
    errorLabel->setLabel("FILE NAME ALREADY EXISTS!");
    return false;
  }
  return true;
}

bool DatasetBuilderScene::checkDir(){
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    return true;
  }
  else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
    if(dir== ""){
        errorLabel->setLabel("CHOOSE A FOLDER FIRST!");
        return false;
    }
    return true;
  }
  return false;

}

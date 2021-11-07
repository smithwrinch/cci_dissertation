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

  imagesAlreadyPaired = new ofxDatGuiToggle("Images already paired");
  imagesAlreadyPaired->setWidth(400, 0.5);
  imagesAlreadyPaired->setPosition(ofGetWidth()/2 - imagesAlreadyPaired->getWidth()/2, loadFromFolderButton->getY() + imagesAlreadyPaired->getHeight());

  buildButton = new ofxDatGuiButton("BUILD");
  buildButton->setWidth(400, 0.5);
  buildButton->setPosition(ofGetWidth()/2 - buildButton->getWidth()/2, loadFromFolderButton->getY() + buildButton->getHeight()*4);
  buildButton->onButtonEvent(this, &DatasetBuilderScene::onButtonEvent);


}

void DatasetBuilderScene::update(){

  buildButton->update();
  errorLabel->update();
  datasetNameInput->update();
  loadFromFolderButton->update();
  imagesAlreadyPaired->update();
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    imagesAlreadyPaired->update();
  }
  else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
  }
}

void DatasetBuilderScene::draw(){
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    imagesAlreadyPaired->draw();
  }
  else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
  }
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
      if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
        if(imagesAlreadyPaired->getChecked()){
            ofDirectory dir_ = ofDirectory(dir);
            dir_.copyTo("saved_datasets/"+datasetNameInput->getText()+"/");
            ModelManager::getInstance()->setDatasetDir("data/saved_datasets/"+datasetNameInput->getText());

            BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::TRAIN);
            scene->refresh();
            ModelManager::getInstance()->setStatus(2);
            SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::TRAIN);
        }
        else{
          ModelManager::getInstance()->setDatasetDir("data/saved_datasets/"+dir);
          BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::DATASET_BUILDER_PIX2PIX);
          scene->refresh();
          SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_BUILDER_PIX2PIX);
        }
      }
      else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
        ofDirectory dir_ = ofDirectory(dir);
        dir_.copyTo("saved_datasets/"+datasetNameInput->getText()+"/");
        ModelManager::getInstance()->setDatasetDir(datasetNameInput->getText());
        BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::TRAIN);
        scene->refresh();
        ModelManager::getInstance()->setStatus(2);
        SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::TRAIN);
      }
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

#include "datasetBuilderScene.h"

void DatasetBuilderScene::setup(){
  setID(SCENE_TYPE::DATASET_BUILDER);

  errorLabel = new ofxDatGuiLabel("");
  errorLabel->setWidth(400, 0.5);
  errorLabel->setPosition(ofGetWidth()/2 - errorLabel->getWidth()/2, ofGetHeight()/2 - errorLabel->getHeight()-100);
  errorLabel->setLabelColor(ofColor(255, 0, 0));

  datasetNameInput = new ofxDatGuiTextInput("Dataset Name: ", "");
  datasetNameInput->setWidth(400, 0.5);
  datasetNameInput->setPosition(ofGetWidth()/2 - datasetNameInput->getWidth()/2, errorLabel->getY() + datasetNameInput->getHeight());

  loadFromFolderButton = new ofxDatGuiButton("Load From Folder");
  loadFromFolderButton->setWidth(400, 0.5);
  loadFromFolderButton->setPosition(ofGetWidth()/2 - loadFromFolderButton->getWidth()/2, datasetNameInput->getY() + loadFromFolderButton->getHeight());
  loadFromFolderButton->onButtonEvent(this, &DatasetBuilderScene::onButtonEvent);

  imagesAlreadyPaired = new ofxDatGuiToggle("Images already paired");
  imagesAlreadyPaired->setWidth(400, 0.5);
  imagesAlreadyPaired->setPosition(ofGetWidth()/2 - imagesAlreadyPaired->getWidth()/2, loadFromFolderButton->getY() + imagesAlreadyPaired->getHeight());

  buildImageProcessingButton = new ofxDatGuiButton("BUILD WITH IMAGE PROCESSING");
  buildImageProcessingButton->setWidth(400, 0.5);
  buildImageProcessingButton->setPosition(ofGetWidth()/2 - buildImageProcessingButton->getWidth()/2, loadFromFolderButton->getY() + buildImageProcessingButton->getHeight()*4);
  buildImageProcessingButton->onButtonEvent(this, &DatasetBuilderScene::onButtonEvent);

  buildButton = new ofxDatGuiButton("BUILD");
  buildButton->setWidth(400, 0.5);
  buildButton->setPosition(ofGetWidth()/2 - buildButton->getWidth()/2, buildImageProcessingButton->getY() + buildImageProcessingButton->getHeight()*2);
  buildButton->onButtonEvent(this, &DatasetBuilderScene::onButtonEvent);

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(50, ofGetHeight() - 50);
  backButton->onButtonEvent(this, &DatasetBuilderScene::onButtonEvent);

  alreadyPairedHelp.setup(imagesAlreadyPaired->getX() + 400, imagesAlreadyPaired->getY(),
  "If the images are already stitched together as TARGET_OUTPUT+INPUT then select this box");

}

void DatasetBuilderScene::update(){

  buildButton->update();
  errorLabel->update();
  datasetNameInput->update();
  loadFromFolderButton->update();
  imagesAlreadyPaired->update();
  backButton->update();
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    imagesAlreadyPaired->update();

    if(!imagesAlreadyPaired->getChecked()){
      buildImageProcessingButton->update();
    }
  }
  else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
  }
}

void DatasetBuilderScene::draw(){
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    imagesAlreadyPaired->draw();
    if(!imagesAlreadyPaired->getChecked()){
      buildImageProcessingButton->draw();
      alreadyPairedHelp.draw();
    }
  }
  else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
  }
  loadFromFolderButton->draw();
  buildButton->draw();
  errorLabel->draw();
  datasetNameInput->draw();
  backButton->draw();
}

void DatasetBuilderScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == loadFromFolderButton){
    ofFileDialogResult result = ofSystemLoadDialog("select input folder", true);
    if (result.bSuccess) {
      dir = result.getPath();
    }
  }
  else if(e.target == buildButton){
    if(checkTextValid() && checkFnameNew() && checkDir()){
      if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
        if(imagesAlreadyPaired->getChecked()){
            ofDirectory dir_ = ofDirectory(dir);
            dir_.copyTo("saved_datasets/"+datasetNameInput->getText()+"/");
            ModelManager::getInstance()->setDatasetDir("data/saved_datasets/"+datasetNameInput->getText());
            if(ModelManager::getInstance()->getStatus() < 2){
              ModelManager::getInstance()->setStatus(2);
            }
            ModelManager::getInstance()->save();
            BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::TRAIN);
            scene->refresh();
            SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::TRAIN);
        }
        else{
          ModelManager::getInstance()->setDatasetDir("data/saved_datasets/"+datasetNameInput->getText());
          ModelManager::getInstance()->set("temp_dataset", dir);
          ModelManager::getInstance()->setStatus(1); // 1 because it hasn't been made yet
          ModelManager::getInstance()->save();
          BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::DATASET_BUILDER_PIX2PIX_MANUAL);
          scene->refresh();
          SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_BUILDER_PIX2PIX_MANUAL);
        }
      }
      else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
        ofDirectory dir_ = ofDirectory(dir);
        dir_.copyTo("saved_datasets/"+datasetNameInput->getText()+"/");
        ModelManager::getInstance()->setDatasetDir("data/saved_datasets/"+datasetNameInput->getText());
        if(ModelManager::getInstance()->getStatus() < 2){
          ModelManager::getInstance()->setStatus(2);
        }
        ModelManager::getInstance()->save();
        BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::TRAIN);
        scene->refresh();
        SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::TRAIN);
      }
    }
  }
  else if(e.target == buildImageProcessingButton){
    //TODO: remove
    // dir = "/home/hans/uni/diss/nicholas_cage/Nicolas_Cage/Dataset/train/class1";

    if(checkTextValid() && checkFnameNew() && checkDir()){
      if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
        if(!imagesAlreadyPaired->getChecked()){
          ModelManager::getInstance()->setDatasetDir("data/saved_datasets/"+datasetNameInput->getText());
          ModelManager::getInstance()->set("temp_dataset", dir);
          ModelManager::getInstance()->setStatus(1); // 1 because it hasn't been made yet
          ModelManager::getInstance()->save();
          BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::DATASET_BUILDER_PIX2PIX);
          scene->refresh();
          SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_BUILDER_PIX2PIX);
        }
      }
    }
  }
  else if(e.target == backButton){
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::DATASET_MENU);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_MENU);
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
  // if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
  //   return true;
  // }
  // else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
  if(dir== ""){
      errorLabel->setLabel("CHOOSE A FOLDER FIRST!");
      return false;
    }
  return true;
  //   return true;
  // }
  // return false;

}

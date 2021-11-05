#include "exportGifScene.h"

void ExportGifScene::refresh(){
  gifDir = "";
  errorLabel->setLabel("");
  imgsDir = "data/saved_models/"+ModelManager::getInstance()->getModelName()+"/images";
}

void ExportGifScene::setup(){
  setID(SCENE_TYPE::INTERACT_EXPORT_GIF);
  int width = 325;
  int startY = 300;
  int yBuffer = 50;
  int centreX = ofGetWidth()/2 - width / 2;

  errorLabel = new ofxDatGuiLabel("");
  errorLabel->setPosition(centreX, startY);
  errorLabel->setLabelColor(ofColor(255, 0, 0));

  outputFolderButton = new ofxDatGuiButton("SELECT OUTPUT FOLDER");
  outputFolderButton->setPosition(centreX, errorLabel->getY() + errorLabel->getHeight() + yBuffer);
  outputFolderButton->onButtonEvent(this, &ExportGifScene::onButtonEvent);

  buildButton = new ofxDatGuiButton("BUILD");
  buildButton->setPosition(centreX, outputFolderButton->getY() + outputFolderButton->getHeight() + yBuffer);
  buildButton->onButtonEvent(this, &ExportGifScene::onButtonEvent);

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(100, ofGetHeight()-50);
  backButton->onButtonEvent(this, &ExportGifScene::onButtonEvent);


  gui.push_back(errorLabel);
  gui.push_back(outputFolderButton);
  gui.push_back(buildButton);
  gui.push_back(backButton);

  outputFolderButton->setWidth(width);
  buildButton->setWidth(width);
  backButton->setWidth(width);
  errorLabel->setWidth(width);
}

void ExportGifScene::update(){
  for(int i = 0; i <gui.size(); i++){
    gui[i]->update();
  }

}

void ExportGifScene::draw(){
  for(int i = 0; i <gui.size(); i++){
    gui[i]->draw();
  }

}

void ExportGifScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACT_MENU);
  }
  else if(e.target == outputFolderButton){
    ofFileDialogResult result = ofSystemLoadDialog("select video output folder", true);
    if (result.bSuccess) {
      gifDir = result.getPath();
    }
  }
  else if(e.target == buildButton){
    if(!checkValid()){
      errorLabel->setLabel("Please enter an output folder");
    }
    else{
      cout << ("python data/python/gan_generator.py --input_dir " + imgsDir +" --output_dir " + gifDir + "/"+ModelManager::getInstance()->getModelName()).c_str() << endl;
      system(("python data/python/gan_generator.py --input_dir " + imgsDir +" --output_dir " + gifDir + "/"+ModelManager::getInstance()->getModelName()).c_str());
    }
  }

}

bool ExportGifScene::checkValid(){
  if(gifDir.length() == 0){
    return false;
  }
  return true;
}

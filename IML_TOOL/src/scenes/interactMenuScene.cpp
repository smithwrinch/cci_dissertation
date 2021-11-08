#include "interactMenuScene.h"

void InteractMenuScene::refresh(){

    SceneManager::getInstance()->setShowNavBar(true);

}

void InteractMenuScene::setup(){
  setID(SCENE_TYPE::INTERACT_MENU);
  int topY = 285;
  int bufferY = 40;

  exploreLatentSpaceButton = new ofxDatGuiButton("EXPLORE LATENT SPACE");
  exploreLatentSpaceButton->onButtonEvent(this, &InteractMenuScene::onButtonEvent);
  exploreLatentSpaceButton->setPosition(ofGetWidth()/2 - exploreLatentSpaceButton->getWidth()/2, topY);

  int centreX = ofGetWidth()/2 - exploreLatentSpaceButton->getWidth()/2;

  musicSyncButton = new ofxDatGuiButton("SYNC TO MUSIC");
  musicSyncButton->setPosition(centreX, bufferY + exploreLatentSpaceButton->getY());
  musicSyncButton->onButtonEvent(this, &InteractMenuScene::onButtonEvent);

  motionDetectionButton = new ofxDatGuiButton("MOTION DETECTION");
  motionDetectionButton->setPosition(centreX,  bufferY + musicSyncButton->getY());
  motionDetectionButton->onButtonEvent(this, &InteractMenuScene::onButtonEvent);



  ganButtons.push_back(exploreLatentSpaceButton);
  ganButtons.push_back(musicSyncButton);
  ganButtons.push_back(motionDetectionButton);
  ////////////////////////////////////////////////
  mlInputButton = new ofxDatGuiButton("ML INPUT");
  mlInputButton->onButtonEvent(this, &InteractMenuScene::onButtonEvent);
  mlInputButton->setPosition(centreX, topY - bufferY );

  drawButton = new ofxDatGuiButton("DRAW INPUT");
  drawButton->onButtonEvent(this, &InteractMenuScene::onButtonEvent);
  drawButton->setPosition(centreX, topY);

  webcamInputButton = new ofxDatGuiButton("WEBCAM INPUT");
  webcamInputButton->onButtonEvent(this, &InteractMenuScene::onButtonEvent);
  webcamInputButton->setPosition(centreX,  bufferY + drawButton->getY());

  videoInputButton = new ofxDatGuiButton("VIDEO INPUT");
  videoInputButton->onButtonEvent(this, &InteractMenuScene::onButtonEvent);
  videoInputButton->setPosition(centreX, bufferY + webcamInputButton->getY());

  p2pButtons.push_back(mlInputButton);
  p2pButtons.push_back(drawButton);
  p2pButtons.push_back(webcamInputButton);
  p2pButtons.push_back(videoInputButton);


  /////////////////////////////////////////
  exportTrainingGifButton = new ofxDatGuiButton("EXPORT TRAINING TO GIF");
  exportTrainingGifButton->setPosition(centreX, 450);
  exportTrainingGifButton->onButtonEvent(this, &InteractMenuScene::onButtonEvent);

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(90, 670);
  backButton->onButtonEvent(this, &InteractMenuScene::onButtonEvent);

}

void InteractMenuScene::update(){
  backButton->update();
  exportTrainingGifButton->update();
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    for(int i =0; i < p2pButtons.size(); i++){
      p2pButtons[i]->update();
    }
  }
  else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
    for(int i =0; i < ganButtons.size(); i++){
      ganButtons[i]->update();
    }
  }
}

void InteractMenuScene::draw(){
  backButton->draw();
  exportTrainingGifButton->draw();
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    for(int i =0; i < p2pButtons.size(); i++){
      p2pButtons[i]->draw();
    }
  }
  else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
    for(int i =0; i < ganButtons.size(); i++){
      ganButtons[i]->draw();
    }
  }
}

void InteractMenuScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::TRAIN);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::TRAIN);
  }
  else if(e.target == exploreLatentSpaceButton){
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::EXPLORE_LATENT);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::EXPLORE_LATENT);
  }
  else if (e.target == musicSyncButton){
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::INTERACT_SYNC_MUSIC);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACT_SYNC_MUSIC);

  }
  else if (e.target == exportTrainingGifButton){
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::INTERACT_EXPORT_GIF);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACT_EXPORT_GIF);
  }
  else if (e.target == drawButton){
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::INTERACT_DRAW_P2P);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACT_DRAW_P2P);
  }
  else if (e.target == webcamInputButton){
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::INTERACT_WEBCAM_P2P);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACT_WEBCAM_P2P);
  }
}

#include "playModelSelectorScene.h"

void PlayModelSelectorScene::setup() {
    setID(SCENE_TYPE::PLAY_MODEL_SELECT);
    edges2shoesImg.load("img/pix2pix.jpg");
    landscapeGANImg.load("img/gan.jpg");
    edges2shoesImg.resize(imSize, imSize);
    landscapeGANImg.resize(imSize, imSize);

    edges2shoesButton = new ofxDatGuiButton("EDGES2SHOES PIX2PIX");
    edges2shoesButton->setPosition(ofGetWidth() /2 - edges2shoesImg.getWidth() / 2 + 200,  ofGetHeight() / 2 + edges2shoesImg.getHeight() / 2);
    edges2shoesButton->setWidth(edges2shoesImg.getWidth(), 1);
    edges2shoesButton->onButtonEvent(this, &PlayModelSelectorScene::onButtonEvent);
    edges2shoesButton->setStripeColor(ofColor(2,132,30));

    landscapeGANButton = new ofxDatGuiButton("LANDSCAPES GAN");
    landscapeGANButton->setPosition(ofGetWidth() /2 - landscapeGANImg.getWidth() / 2 - 200,  ofGetHeight() / 2 + landscapeGANImg.getHeight() / 2);
    landscapeGANButton->setWidth(landscapeGANImg.getWidth(), 1);
    landscapeGANButton->onButtonEvent(this, &PlayModelSelectorScene::onButtonEvent);
    landscapeGANButton->setStripeColor(ofColor(253,106,2));

    backButton = new ofxDatGuiButton("BACK<-");
    backButton->setPosition(200, ofGetHeight() - 100);
    backButton->onButtonEvent(this, &PlayModelSelectorScene::onButtonEvent);
}

void PlayModelSelectorScene::update(){
  edges2shoesButton->update();
  landscapeGANButton->update();
  backButton->update();
  landscapeGANImg.update();
  edges2shoesImg.update();
}

void PlayModelSelectorScene::draw() {
  edges2shoesImg.draw(ofGetWidth() /2 - edges2shoesImg.getWidth() / 2 + 200, ofGetHeight() / 2 - edges2shoesImg.getHeight() / 2);
  landscapeGANImg.draw(ofGetWidth() /2 - landscapeGANImg.getWidth() / 2 - 200, ofGetHeight() / 2 - landscapeGANImg.getHeight() / 2);

  edges2shoesButton->draw();
  landscapeGANButton->draw();
  backButton->draw();

}

void PlayModelSelectorScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == edges2shoesButton){
    cout << "EDGES TO SHOES" << endl;
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACTPIX2PIX);
    InputPix2PixScene* i = (InputPix2PixScene*) SceneManager::getInstance()->getCurrentScene();
    i->startThread();
  }
  if(e.target == landscapeGANButton){
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACTGAN);
      InputGANScene* i = (InputGANScene*) SceneManager::getInstance()->getCurrentScene();
      i->startThread();
      cout << "GAN" << endl;
  }
  if(e.target == backButton){
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::MENU);
  }
}

void PlayModelSelectorScene::mousePressed(int x, int y, int button){
  cout << "MOUSE PRESSED" << endl;

}

#include "datasetPix2PixBuilderScene.h"


void DatasetPix2PixBuilderScene::refresh(){
  colorImg.clear();
  im1.clear();
  ModelManager * m = ModelManager::getInstance();
  // width_ = m->getImgWidth();
  // height_ = m->getImgHeight();
  string dir = m->getDatasetDir();
  d.close();
  d = ofDirectory(dir);
  d.listDir();
  im1.load(d.getPath(0));
  im1.setImageType(OF_IMAGE_COLOR);
  colorImg.allocate(im1.getWidth(), im1.getHeight());
  // im1.resize(width, height);
  // im1.update();


}

void DatasetPix2PixBuilderScene::setup(){
  setID(SCENE_TYPE::DATASET_BUILDER_PIX2PIX);
  // buildButton = new ofxDatGuiButton("BUILD DATASET");
  // buildButton->setPosition(ofGetWidth()/2 - buildButton->getWidth()/2, ofGetHeight()/2 - buildButton->getHeight());
  // buildButton->onButtonEvent(this, &DatasetMainScene::onButtonEvent);

}

void DatasetPix2PixBuilderScene::update(){
  colorImg.setFromPixels(im1.getPixels());
}


void DatasetPix2PixBuilderScene::draw(){
  im1.draw(ofGetWidth()/2 -width - 25, ofGetHeight()/2 - height / 2, width, height);
  colorImg.draw(ofGetWidth()/2 + 25, ofGetHeight()/2 - height / 2, width, height);
}

void DatasetPix2PixBuilderScene::onButtonEvent(ofxDatGuiButtonEvent e){
}

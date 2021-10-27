#include "datasetPix2PixBuilderScene.h"


void DatasetPix2PixBuilderScene::refresh(){
  ModelManager * m = ModelManager::getInstance();
  string dir = m->getDatasetDir();
  d = ofDirectory(dir);
  d.listDir();
  im1.load(d.getPath(0));
  im2.load(d.getPath(1));
  im3.load(d.getPath(2));
  im4.load(d.getPath(3));
}

void DatasetPix2PixBuilderScene::setup(){
  setID(SCENE_TYPE::DATASET_BUILDER_PIX2PIX);
  // buildButton = new ofxDatGuiButton("BUILD DATASET");
  // buildButton->setPosition(ofGetWidth()/2 - buildButton->getWidth()/2, ofGetHeight()/2 - buildButton->getHeight());
  // buildButton->onButtonEvent(this, &DatasetMainScene::onButtonEvent);


}

void DatasetPix2PixBuilderScene::update(){
}

void DatasetPix2PixBuilderScene::draw(){
  im1.draw(0, 0);
  im2.draw(100, 0);
  im3.draw(0, 100);
  im4.draw(100, 100);
}

void DatasetPix2PixBuilderScene::onButtonEvent(ofxDatGuiButtonEvent e){
}

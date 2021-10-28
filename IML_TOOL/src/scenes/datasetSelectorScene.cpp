#include "datasetSelectorScene.h"

void DatasetSelectorScene::setup(){
  setID(SCENE_TYPE::DATASET_SELECTOR);
  datasetScroll = new ofxDatGuiScrollView("Saved Datasets", 10);
  datasetScroll->setWidth(300, 0.5);
  datasetScroll->setPosition(ofGetWidth()/2 - datasetScroll->getWidth()/2, ofGetHeight()/2 - datasetScroll->getHeight()/2);

  datasetScroll->onScrollViewEvent(this, &DatasetSelectorScene::onScrollViewEvent);

  scrollContinueButton = new ofxDatGuiButton("CONTINUE");
  scrollContinueButton->setPosition(ofGetWidth()/2 - scrollContinueButton->getWidth()/2 + 200 , datasetScroll->getY() + datasetScroll->getHeight() + 50);
  scrollContinueButton->onButtonEvent(this, &DatasetSelectorScene::onButtonEvent);

  scrollDeleteButton = new ofxDatGuiButton("DELETE");
  scrollDeleteButton->setPosition(ofGetWidth()/2 - scrollContinueButton->getWidth()/2 - 200, datasetScroll->getY() + datasetScroll->getHeight() + 50);
  scrollDeleteButton->onButtonEvent(this, &DatasetSelectorScene::onButtonEvent);
  scrollDeleteButton->setStripeColor(ofColor(255,0,0));

  scrollDeleteButton->setVisible(false);
  scrollContinueButton->setVisible(false);
  populateScroll();
  // buildButton = new ofxDatGuiButton("BUILD DATASET");
  // buildButton->setPosition(ofGetWidth()/2 - buildButton->getWidth()/2, ofGetHeight()/2 - buildButton->getHeight());
  // buildButton->onButtonEvent(this, &DatasetSelectorScene::onButtonEvent);
  //
  // selectButton = new ofxDatGuiButton("LOAD DATASET");
  // selectButton->setPosition(buildButton->getX(), buildButton->getY() + buildButton->getHeight() + 50);
  // selectButton->onButtonEvent(this, &DatasetSelectorScene::onButtonEvent);
}

void DatasetSelectorScene::update(){
  scrollContinueButton->update();
  scrollDeleteButton->update();
  datasetScroll->update();
}

void DatasetSelectorScene::draw(){
  scrollContinueButton->draw();
  scrollDeleteButton->draw();
  datasetScroll->draw();
}

void DatasetSelectorScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == scrollContinueButton){
    ModelManager::getInstance()->setDatasetDir("data/saved_datasets/"+currentScroll);
    ModelManager::getInstance()->setStatus(2);
    ModelManager::getInstance()->save();

    TrainingScene * scene = (TrainingScene *) SceneManager::getInstance()->getScene(SCENE_TYPE::TRAIN);
    scene->refresh();

    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::TRAIN);
  }
  if(e.target == scrollDeleteButton){
    // SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_BUILDER);
    filesystem::remove_all("../bin/data/saved_datasets/"+currentScroll);
  }
}

void DatasetSelectorScene::onScrollViewEvent(ofxDatGuiScrollViewEvent e)
{
    ofxDatGuiScrollView* parent = e.parent; // a pointer to the scrollview that dispatched the event //
    ofxDatGuiButton* button = e.target; // a pointer to the button that was selected //

    if(currentScroll == button->getLabel()){
        currentScroll = "";
        scrollDeleteButton->setVisible(false);
        scrollContinueButton->setVisible(false);
        refreshScroll();
    }
    else{


      scrollDeleteButton->setVisible(true);
      scrollContinueButton->setVisible(true);
      refreshScroll();
      button->setBorder(ofColor(255, 255, 255), 1);
      button->setStripeColor(ofColor(255,255,255));
      currentScroll = button->getLabel();

  }
}

void DatasetSelectorScene::populateScroll(){
  ofDirectory dir = ofDirectory("saved_datasets");
  int size = dir.listDir();
  dir.sortByDate();
  datasetScroll->clear();
  for (int i =size-1; i >= 0; i--){

    if(dir.getFile(i).isDirectory()==1){

        string name = dir.getFile(i).getBaseName();
        datasetScroll->add(name);
        datasetScroll->update();


    }
  }
}

void DatasetSelectorScene::refreshScroll(){
  currentScroll = "";
  for (int i =0; i < datasetScroll->getNumItems(); i++){
    ofxDatGuiScrollViewItem * button = datasetScroll->getItemAtIndex(i);
    button->setBorder(ofColor(200, 200, 200), 0);
  }
}

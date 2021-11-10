#include "datasetPix2PixBuilderScene.h"


void DatasetPix2PixBuilderScene::refresh(){
  im1.clear();
  ModelManager * m = ModelManager::getInstance();
  // width_ = m->getImgWidth();
  // height_ = m->getImgHeight();
  string dir = m->get("temp_dataset");
  d.close();
  d = ofDirectory(dir);
  d.listDir();
  im1.load(d.getPath(0));
  im1.setImageType(OF_IMAGE_COLOR);

  setupImages();
  setupGui();

  outMsg = "";
  mode = 0;
  fileIdx = 0;
  isApplying = false;
  bChangedInputFlag = false;

}

void DatasetPix2PixBuilderScene::setup(){
  setID(SCENE_TYPE::DATASET_BUILDER_PIX2PIX);
  // buildButton = new ofxDatGuiButton("BUILD DATASET");
  // buildButton->setPosition(ofGetWidth()/2 - buildButton->getWidth()/2, ofGetHeight()/2 - buildButton->getHeight());
  // buildButton->onButtonEvent(this, &DatasetMainScene::onButtonEvent);

}

void DatasetPix2PixBuilderScene::update(){
  updateCount++;
  updateCount%=1024;
  colorImg.setFromPixels(im1.getPixels());
  imgInGray = colorImg;

  for(int i = 0; i < gui.size(); i++){
    gui[i]->update();
  }
  switch(mode){
    case 0:
    for(int i = 0; i < guiNormal.size(); i++){
      guiNormal[i]->update();
    }
    updateNormalImage();
    break;
    case 1:
    for(int i = 0; i < guiContours.size(); i++){
      guiContours[i]->update();
    }
    updateContourImage();
    break;
    case 2:
    for(int i = 0; i < guiCanny.size(); i++){
      guiCanny[i]->update();
    }
    updateCannyImage();
    break;
  }
  // if(isApplying){
  //   setupImages();
  // }
  //
  // if(!imageSaver.isThreadRunning() && isApplying){
  //   isApplying == false;
  // }

    // isApplying = false;
    // recorder.stopThread();

}




void DatasetPix2PixBuilderScene::draw(){
  if(!isApplying){

    ofDrawBitmapString("PRESS [SPACE] TO CYCLE DATASET", imgX-150, imgY+height+75);
    for(int i = 0; i < gui.size(); i++){
      gui[i]->draw();
    }

    switch(mode){
      case 0:
      for(int i = 0; i < guiNormal.size(); i++){
        guiNormal[i]->draw();
      }
      break;
      case 1:
      for(int i = 0; i < guiContours.size(); i++){
        guiContours[i]->draw();
      }
      contourFbo.begin();
      if(invertToggleContour->getChecked()){
        ofClear(255);
        ofSetColor(0);
      }
      else{
        ofClear(0);
        ofSetColor(255);
      }
      ofSetLineWidth(lineWidthSizeSlider->getValue());
      contourFinder.draw(0, 0);
      contourFbo.end();
      // ofPushMatrix();
      // contourFbo.draw(imgX, imgY, width, height);
      // ofPopMatrix();
      break;
      case 2:
      for(int i = 0; i < guiCanny.size(); i++){
        guiCanny[i]->draw();
      }
      break;
    }
    im1.draw(700, 100, width, height);
    colorImg.draw(imgX, imgY, width, height);
  }
  else{
        ofDrawBitmapString(outMsg, imgX-150, imgY+height+75);
        outFbo.begin();
        im1.draw(0,0);
        colorImg.draw(im1.getWidth(),0);
        outFbo.end();
  }

}

void DatasetPix2PixBuilderScene::keyPressed(int key){
  if(key == ' ' && !isApplying){
    fileIdx++;
    fileIdx %= d.getFiles().size();
    im1.clear();
    im1.load(d.getPath(fileIdx));
    setupImages();
  }
}

void DatasetPix2PixBuilderScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == toggleModeButton){
    mode++;
    if(mode>=3){
      mode = 0;
    }
  }
  else if(e.target == applyButton){
    fileIdx = 0;
    isApplying = true;
    string rootDir = ModelManager::getInstance()->getDatasetDir();
    recorder.setPrefix(rootDir + "/"); // this directory must already exist
    recorder.setFormat("png"); // png is really slow but high res, bmp is fast but big, jpg is just right
    recorder.startThread();
    for(int idx =0; idx<d.getFiles().size(); idx++){

      im1.load(d.getPath(idx));
      setupImages();
      update();
      draw();
      outMsg = ofToString(idx) + " IMAGES COMPLETED OF " + ofToString(d.getFiles().size());
      ofSleepMillis(50);
      update();
      draw();
      cout << idx << endl;
      outFbo.readToPixels(outImg.getPixels());
      recorder.addFrame(outImg);
    }
    isApplying = false;
    recorder.stopThread();
  }
}


void DatasetPix2PixBuilderScene::updateNormalImage(){
  if(invertToggle->getChecked()){
    colorImg.invert();
  }


  colorImg.convertToGrayscalePlanarImages(r, g, b);

  r.brightnessContrast(brightnessSlider->getValue(), contrastSlider->getValue());
  g.brightnessContrast(brightnessSlider->getValue(), contrastSlider->getValue());
  b.brightnessContrast(brightnessSlider->getValue(), contrastSlider->getValue());


  colorImg.setFromGrayscalePlanarImages(r, g, b);

  if(int(blurSlider->getValue()) % 2 == 0){
    blurSlider->setValue(blurSlider->getValue()+1);
  }
  colorImg.blurGaussian(blurSlider->getValue());

  if(grayscaleToggle->getChecked()){
    imgInGray = colorImg;
    colorImg = imgInGray;
  }
  colorImg.flagImageChanged();

}

void DatasetPix2PixBuilderScene::updateContourImage(){
  imgInGray.threshold(contourThresholdSlider->getValue());
  contourFinder.findContours(imgInGray, minContourSlider->getValue(),
   maxContourSlider->getValue(), blobsToConsiderSlider->getValue(), true, true);

  contourFbo.readToPixels(colorImg.getPixels());
}

void DatasetPix2PixBuilderScene::updateCannyImage(){
  cvCanny(imgInGray.getCvImage(), canny.getCvImage(), minThresholdSlider->getValue(),  maxThresholdSlider->getValue(), 3);
  if(invertToggleCanny->getChecked()){
    canny.invert();
  }
  canny.flagImageChanged();

  r.setFromPixels(canny.getPixels());
  g.setFromPixels(canny.getPixels());
  b.setFromPixels(canny.getPixels());

  colorImg.setFromGrayscalePlanarImages(r, g, b);
}




//-------------------------------------------------------------------------------

void DatasetPix2PixBuilderScene::setupImages(){

  im1.setImageType(OF_IMAGE_COLOR);
  colorImg.clear();
  r.clear();
  b.clear();
  g.clear();
  canny.clear();
  outFbo.clear();
  contourFbo.clear();
  colorImg.allocate(im1.getWidth(), im1.getHeight());
  r.allocate(im1.getWidth(), im1.getHeight());
  g.allocate(im1.getWidth(), im1.getHeight());
  b.allocate(im1.getWidth(), im1.getHeight());
  imgInGray.allocate(im1.getWidth(), im1.getHeight());
  contourFbo.allocate(im1.getWidth(), im1.getHeight());
  outFbo.allocate(im1.getWidth()*2, im1.getHeight());
  canny.allocate(im1.getWidth(), im1.getHeight());
}

void DatasetPix2PixBuilderScene::setupGui(){

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(50, ofGetHeight()-50);

  applyButton = new ofxDatGuiButton("APPLY TO ALL IMAGES");
  applyButton->setWidth(556);
  applyButton->setPosition(400, 300+height);
  applyButton->onButtonEvent(this, &DatasetPix2PixBuilderScene::onButtonEvent);

  inputLabel = new ofxDatGuiLabel("INPUT");
  inputLabel->setPosition(400, 100 + height);
  inputLabel->setWidth(width);

  outputLabel = new ofxDatGuiLabel("OUTPUT");
  outputLabel->setPosition(700, 100 + height);
  outputLabel->setWidth(width);

  toggleModeButton = new ofxDatGuiButton("TOGGLE MODE");
  toggleModeButton->setPosition(drawX, ofGetHeight()/2);
  toggleModeButton->onButtonEvent(this, &DatasetPix2PixBuilderScene::onButtonEvent);

  gui.push_back(backButton);
  gui.push_back(applyButton);
  gui.push_back(inputLabel);
  gui.push_back(outputLabel);
  gui.push_back(toggleModeButton);

  //normal mode
  grayscaleToggle = new ofxDatGuiToggle("GRAYSCALE");
  invertToggle = new ofxDatGuiToggle("INVERT");
  blurSlider = new ofxDatGuiSlider("BLUR AMOUNT", 0, 300, 0);
  contrastSlider = new ofxDatGuiSlider("CONTRAST", -1, 1, 0);
  brightnessSlider = new ofxDatGuiSlider("BRIGHTNESS", -1, 1, 0);

  blurSlider->setPrecision(0);

  grayscaleToggle->setPosition(drawX, drawY);
  invertToggle->setPosition(drawX, grayscaleToggle->getY() + grayscaleToggle->getHeight());
  blurSlider->setPosition(drawX, invertToggle->getY() + invertToggle->getHeight());
  contrastSlider->setPosition(drawX, blurSlider->getY() + blurSlider->getHeight());
  brightnessSlider->setPosition(drawX, contrastSlider->getY()+ contrastSlider->getHeight());

  for(int i; i < guiNormal.size(); i++){
    delete guiNormal[i];
  }
  guiNormal.clear();
  guiNormal.push_back(grayscaleToggle);
  guiNormal.push_back(invertToggle);
  guiNormal.push_back(blurSlider);
  guiNormal.push_back(contrastSlider);
  guiNormal.push_back(brightnessSlider);

  //contours mode
  invertToggleContour = new ofxDatGuiToggle("INVERT");
  contourThresholdSlider = new ofxDatGuiSlider("THRESHOLD", 1, 255, 30);
  minContourSlider = new ofxDatGuiSlider("MIN SIZE", 1, 10, 3);
  maxContourSlider = new ofxDatGuiSlider("MAX SIZE", 11, im1.getWidth(), im1.getHeight());
  blobsToConsiderSlider = new ofxDatGuiSlider("BLOBS TO CONSIDER", 1, 100, 4);
  lineWidthSizeSlider = new ofxDatGuiSlider("LINE WIDTH", 1, 8, 1);

  contourThresholdSlider->setPrecision(0);
  minContourSlider->setPrecision(0);
  maxContourSlider->setPrecision(0);
  blobsToConsiderSlider->setPrecision(0);
  lineWidthSizeSlider->setPrecision(0);

  invertToggleContour->setPosition(drawX, drawY);
  contourThresholdSlider->setPosition(drawX, invertToggleContour->getY() + invertToggleContour->getHeight());
  minContourSlider->setPosition(drawX, contourThresholdSlider->getY() + contourThresholdSlider->getHeight());
  maxContourSlider->setPosition(drawX, minContourSlider->getY() + minContourSlider->getHeight());
  blobsToConsiderSlider->setPosition(drawX, maxContourSlider->getY() + maxContourSlider->getHeight());
  lineWidthSizeSlider->setPosition(drawX, blobsToConsiderSlider->getY() + blobsToConsiderSlider->getHeight());

  for(int i; i < guiContours.size(); i++){
    delete guiContours[i];
  }

  guiContours.clear();
  guiContours.push_back(invertToggleContour);
  guiContours.push_back(contourThresholdSlider);
  guiContours.push_back(minContourSlider);
  guiContours.push_back(maxContourSlider);
  guiContours.push_back(blobsToConsiderSlider);
  guiContours.push_back(lineWidthSizeSlider);

  //canny mode
  invertToggleCanny = new ofxDatGuiToggle("INVERT");
  minThresholdSlider = new ofxDatGuiSlider("MIN THRESHOLD", 1, 255, 100);
  maxThresholdSlider = new ofxDatGuiSlider("MAX THRESHOLD", 1, 255, 255);

  minThresholdSlider->setPrecision(0);
  maxThresholdSlider->setPrecision(0);


  invertToggleCanny->setPosition(drawX, drawY);
  minThresholdSlider->setPosition(drawX, invertToggleCanny->getY() + invertToggleCanny->getHeight());
  maxThresholdSlider->setPosition(drawX, minThresholdSlider->getY() + minThresholdSlider->getHeight());

  for(int i; i < guiCanny.size(); i++){
    delete guiCanny[i];
  }
  guiCanny.clear();
  guiCanny.push_back(invertToggleCanny);
  guiCanny.push_back(minThresholdSlider);
  guiCanny.push_back(maxThresholdSlider);


  grayscaleToggle->setWidth(drawWidth);
  invertToggle->setWidth(drawWidth);
  blurSlider->setWidth(drawWidth, 0.5);
  contrastSlider->setWidth(drawWidth, 0.5);
  brightnessSlider->setWidth(drawWidth, 0.5);

  //contours mode
  contourThresholdSlider->setWidth(drawWidth, 0.5);
  minContourSlider->setWidth(drawWidth, 0.5);
  maxContourSlider->setWidth(drawWidth, 0.5);
  blobsToConsiderSlider->setWidth(drawWidth, 0.5);
  invertToggleContour->setWidth(drawWidth);
  lineWidthSizeSlider->setWidth(drawWidth, 0.5);
  // ofxDatGuiToggle* findHolesToggle;
  // ofxDatGuiToggle* useApproximationToggle;

  //canny mode
  invertToggleCanny->setWidth(drawWidth);
  minThresholdSlider->setWidth(drawWidth, 0.5);
  maxThresholdSlider->setWidth(drawWidth, 0.5);
}

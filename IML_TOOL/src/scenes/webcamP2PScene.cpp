#include "webcamP2PScene.h"

void WebcamP2PScene::refresh(){
  // neural network setup, bail out on error
  // the default model is edges2shoes and excepts [None, None, None, 3]
  SceneManager::getInstance()->setShowNavBar(false);

  ModelManager * modelManager = ModelManager::getInstance();
  string modelDir = "saved_models/"+modelManager->getModelName()+
  "/saved_networks/ckpt/-"+ofToString(modelManager->getEpochsTrained())+"_generator";

  model.clear();
  if(!model.load(modelDir)) {

    string newModelDir = "saved_models/"+modelManager->getModelName()+
      "/saved_networks/ckpt/-"+ofToString(modelManager->getEpochsTrained()+1)+"_generator";
      // May have not updated getEpochsTrained
      if(!model.load(newModelDir)) {
        cout << "couldn't load model" << endl;
        std::exit(EXIT_FAILURE);
      }
      else{
        modelManager->setEpochsTrained(modelManager->getEpochsTrained()+1);
        modelManager->save();
      }
  }
  nnWidth = modelManager->getImgWidth();
  nnHeight = modelManager->getImgHeight();


  ofImage imgOut;

  drawWidth = 400;
  drawHeight = 400;

  drawY = 25;
  drawX = (ofGetWidth() / 2) - drawWidth;

  // allocate fbo and images with correct dimensions, no alpha channel
  ofLogVerbose() << "Allocating fbo and images ("
                 << nnWidth << ", " << nnHeight << ")";
  fbo.allocate(nnWidth, nnHeight, GL_RGB);
  contourFbo.allocate(nnWidth, nnHeight, GL_RGB);

  if(modelManager->getInputChannel() == 1){
    imgIn.allocate(nnWidth, nnHeight, OF_IMAGE_GRAYSCALE);
    imgOut.allocate(nnWidth, nnHeight, OF_IMAGE_GRAYSCALE);
  }
  else{
    imgIn.allocate(nnWidth, nnHeight, OF_IMAGE_COLOR);
    imgOut.allocate(nnWidth, nnHeight, OF_IMAGE_COLOR);

  }
  imgInCV.allocate(nnWidth, nnHeight);
  imgInGray.allocate(nnWidth, nnHeight);
  grayBg.allocate(nnWidth, nnHeight);
  grayDiff.allocate(nnWidth, nnHeight);
  canny.allocate(nnWidth, nnHeight);
  contourImg.allocate(nnWidth, nnHeight);
  r.allocate(nnWidth, nnHeight);
  g.allocate(nnWidth, nnHeight);
  b.allocate(nnWidth, nnHeight);

  setupGui();

  vidRecorder.setVideoCodec("mpeg4");
  vidRecorder.setVideoBitrate("800k");

  ofAddListener(vidRecorder.outputFileCompleteEvent, this, &WebcamP2PScene::recordingComplete);

  vidGrabber.setDeviceID(0);
  vidGrabber.setVerbose(true);
  vidGrabber.setDesiredFrameRate(60);
  vidGrabber.initGrabber(videoGrabberWidth,videoGrabberHeight);
  // shorten idle time to have model check for input more frequently,
  // this may increase responsiveness on faster machines but will use more cpu
  model.setIdleTime(10);
  autoRun = true;
  recording = false;
  hasChosenExportFolder = false;
  exportDir = "";
}

//--------------------------------------------------------------
void WebcamP2PScene::setup() {
  setID(SCENE_TYPE::INTERACT_WEBCAM_P2P);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	// ofBackground(54, 54, 54);
	ofSetLogLevel("ofxTensorFlow2", OF_LOG_VERBOSE);

	// use only a portion of the GPU memory & grow as needed
	if(!ofxTF2::setGPUMaxMemory(ofxTF2::GPU_PERCENT_70, true)) {
		ofLogError() << "failed to set GPU Memory options!";
	}



}

void WebcamP2PScene::startThread(){
  autoRun = true;
}

void WebcamP2PScene::stopThread(){
    autoRun = false;
}

void WebcamP2PScene::update() {

	// start & stop the model
	if(!autoRun && model.isThreadRunning()) {
		model.stopThread();
	}
	else if(autoRun && !model.isThreadRunning()) {
		model.startThread();
	}
  vidGrabber.update();
  if(vidGrabber.isFrameNew()){
    fbo.readToPixels(imgInCV.getPixels());


    if(imageMode == 0){
      updateNormalImage();
    }

    imgInGray = imgInCV; // convert our color image to a grayscale image

    if(imageMode == 1){
      updateContourImage();
    }

    else if(imageMode == 2){
      updateCannyImage();
    }
    imgInCV.flagImageChanged();
    imgInGray.flagImageChanged();

  }
	// write fbo to ofImage

	// async update on model input
	if(model.readyForInput()) {

    imgIn.setFromPixels(imgInCV.getPixels());
    if(imageMode == 0){
        if(grayscaleToggle->getChecked()){
            imgIn.setFromPixels(imgInGray.getPixels());
            if(ModelManager::getInstance()->getInputChannel() == 3){
              imgIn.setImageType(OF_IMAGE_COLOR);
            }
        }
    }
    else if( imageMode == 2){
        imgIn.setFromPixels(canny.getPixels());
        if(ModelManager::getInstance()->getInputChannel() == 3){
          imgIn.setImageType(OF_IMAGE_COLOR);
        }
    }

    else if(imageMode == 1){
      imgIn.setFromPixels(contourImg.getPixels());
    }

		// read tensor from ofImage
		input = ofxTF2::imageToTensor(imgIn);

		// feed input into model
		model.update(input);

		// end measurment
		end = std::chrono::system_clock::now();
		std::chrono::duration<double> diff = end - start;
		ofLog() << "Run took: " << diff.count() << " s or ~"
		        << (int)(1.0/diff.count()) << " fps";
	}

	// async read from model output
	if(model.isOutputNew()) {

		// pull output from model
		output = model.getOutput();

		// write tensor to ofImage
		ofxTF2::tensorToImage(output, imgOut);
		imgOut.update();

    if(recording){
      bool success = vidRecorder.addFrame(imgOut.getPixels());
      if (!success) {
         ofLogWarning("Frame was not added!");
      }
    }

		// start new measurement
		start = std::chrono::system_clock::now();
	}

  for(int i = 0; i < gui.size(); i++){
    gui[i]->update();
  }

  if(hasChosenExportFolder){
    recordButton->update();
    exportPictureButton->update();
    exportPictureButton2->update();
  }
  if(!recording){
    backButton->update();
  }

  if(imageMode == 0){
    for(int i = 0; i < guiNormal.size(); i++){
      guiNormal[i]->update();
    }
  }
  else if(imageMode == 1){
      for(int i = 0; i < guiContours.size(); i++){
        guiContours[i]->update();
      }
  }
  else if(imageMode == 2){
      for(int i = 0; i < guiCanny.size(); i++){
        guiCanny[i]->update();
      }
  }
}

//--------------------------------------------------------------
void WebcamP2PScene::draw() {


  fbo.begin();
  ofScale(float(nnWidth) / float(videoGrabberWidth), float(nnHeight) / float(videoGrabberHeight));
  vidGrabber.draw(0, 0);
  fbo.end();
	ofPushMatrix();
		if(!drawImage(imgIn, "(draw in here \nor drag in image)",drawX, drawY, drawWidth, drawHeight)) {
			cout << "fbo not allocated !!" << std::endl;
		}
		if(!drawImage(imgOut, "imgOut", drawX+drawWidth, drawY, drawWidth, drawHeight)) {
			cout << "imgOut not allocated !!" << std::endl;
		}
	ofPopMatrix();


  if(imageMode == 1){
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
    ofPushMatrix();
    if(!drawImage(contourFbo, "(draw in here \nor drag in image)",drawX, drawY, drawWidth, drawHeight)) {
			cout << "contour fbo not allocated !!" << std::endl;
    }
    ofPopMatrix();
  }
  if(imageMode == 2){
    canny.draw(drawX, drawY, drawWidth, drawHeight);
  }

  ofFill();
  for(int i = 0; i < gui.size(); i++){
    gui[i]->draw();
  }
  if(hasChosenExportFolder){
    recordButton->draw();
    exportPictureButton->draw();
    exportPictureButton2->draw();
  }

  if(!recording){
    backButton->draw();
  }

  if(imageMode == 0){
    for(int i = 0; i < guiNormal.size(); i++){
      guiNormal[i]->draw();
    }
  }
  else if(imageMode == 1){
      for(int i = 0; i < guiContours.size(); i++){
        guiContours[i]->draw();
      }
  }
  else if(imageMode == 2){
      for(int i = 0; i < guiCanny.size(); i++){
        guiCanny[i]->draw();
      }
  }
}

//--------------------------------------------------------------
void WebcamP2PScene::keyPressed(int key) {
  if(key == 'b' || key == 'B'){
    bLearnBackground = true;
  }
}

//--------------------------------------------------------------
void WebcamP2PScene::mouseDragged( int x, int y, int button) {

}

//--------------------------------------------------------------
void WebcamP2PScene::mousePressed( int x, int y, int button) {
}

//--------------------------------------------------------------
void WebcamP2PScene::mouseReleased(int x, int y, int button) {

}


//--------------------------------------------------------------
void WebcamP2PScene::dragEvent(ofDragInfo dragInfo) {

}


void WebcamP2PScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
    stopThread();
    update();
    SceneManager::getInstance()->setShowNavBar(true);
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACT_MENU);
  }
  else if (e.target == setExportFolderButton){
    ofFileDialogResult result = ofSystemLoadDialog("select output dir", true);
    if (result.bSuccess) {
      exportDir = result.getPath();
      hasChosenExportFolder = true;
    }
  }
  else if(e.target == exportPictureButton){
    ofSaveImage(imgOut,exportDir+"/"+ofGetTimestampString()+".png");
  }
  else if(e.target == exportPictureButton2){
    ofSaveImage(imgIn,exportDir+"/"+ofGetTimestampString()+"_doodle.png");
  }

  else if(e.target == recordButton){
    record();
  }
  else if(e.target == toggleModeButton){
    imageMode++;
    if(imageMode >= 3){
      imageMode = 0;
    }
  }
  else if (e.target == getBackgroundButton){
      bLearnBackground = true;
  }
}

// PRIVATE


//--------------------------------------------------------------
// draw image or fbo etc with border and label
// typename T must have draw(x,y), isAllocated(), getWidth(), getHeight()
template <typename T>
bool WebcamP2PScene::drawImage(const T& img, string label, int x, int y, int width, int height) {
	// if(img.isAllocated()) {
		ofSetColor(255);
		ofFill();

		// draw image
		img.draw(x, y, width, height);

		// draw border
		ofNoFill();
		ofSetColor(200);
		ofSetLineWidth(1);
		ofDrawRectangle(x, y, width, height);

		// draw label
		ofDrawBitmapString(label, x, y- 15);

		// next position
		// ofTranslate(img.getWidth(), 0);

		return true;
	// }
	// return false;
}

void WebcamP2PScene::setupGui(){

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(100, ofGetHeight()-50);
  backButton->onButtonEvent(this, &WebcamP2PScene::onButtonEvent);

  exportPictureButton = new ofxDatGuiButton("EXPORT PICTURE");
  exportPictureButton->setPosition(drawX+drawWidth,drawY+drawHeight);
  exportPictureButton->setWidth(drawHeight);
  exportPictureButton->onButtonEvent(this, &WebcamP2PScene::onButtonEvent);


  exportPictureButton2 = new ofxDatGuiButton("EXPORT PICTURE");
  exportPictureButton2->setPosition(drawX,drawY+drawHeight);
  exportPictureButton2->setWidth(drawHeight);
  exportPictureButton2->onButtonEvent(this, &WebcamP2PScene::onButtonEvent);

  int controlsWidth = 400;
  int controlsX = 100;

  int buttonsX = ofGetWidth() - controlsWidth;

  recordButton = new ofxDatGuiButton("START RECORDING");
  recordButton->setPosition(buttonsX, drawY+drawHeight+exportPictureButton->getHeight()*2);
  recordButton->onButtonEvent(this, &WebcamP2PScene::onButtonEvent);
  recordButton->setStripeColor(ofColor(0,255,0));

  setExportFolderButton = new ofxDatGuiButton("SET EXPORT FOLDER");
  setExportFolderButton->setPosition(buttonsX, recordButton->getY()+recordButton->getHeight());
  setExportFolderButton->onButtonEvent(this, &WebcamP2PScene::onButtonEvent);

  toggleModeButton = new ofxDatGuiButton("TOGGLE MODE");
  toggleModeButton->setPosition(buttonsX, setExportFolderButton->getY()+setExportFolderButton->getHeight());
  toggleModeButton->onButtonEvent(this, &WebcamP2PScene::onButtonEvent);


  gui.clear();
  gui.push_back(setExportFolderButton);
  gui.push_back(toggleModeButton);

  //normal mode
  grayscaleToggle = new ofxDatGuiToggle("GRAYSCALE");
  invertToggle = new ofxDatGuiToggle("INVERT");
  blurSlider = new ofxDatGuiSlider("BLUR AMOUNT", 0, nnWidth, 0);
  contrastSlider = new ofxDatGuiSlider("CONTRAST", -1, 1, 0);
  brightnessSlider = new ofxDatGuiSlider("BRIGHTNESS", -1, 1, 0);

  blurSlider->setPrecision(0);

  grayscaleToggle->setPosition(drawX, drawY + drawHeight + exportPictureButton->getHeight());
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
  getBackgroundButton = new ofxDatGuiButton("SET BACKGROUND (B)");
  invertToggleContour = new ofxDatGuiToggle("INVERT");
  contourThresholdSlider = new ofxDatGuiSlider("THRESHOLD", 1, 255, 30);
  minContourSlider = new ofxDatGuiSlider("MIN SIZE", 1, 10, 3);
  maxContourSlider = new ofxDatGuiSlider("MAX SIZE", 11, (videoGrabberWidth*videoGrabberHeight)/2, (videoGrabberWidth*videoGrabberHeight)/4);
  blobsToConsiderSlider = new ofxDatGuiSlider("BLOBS TO CONSIDER", 1, 20, 4);
  lineWidthSizeSlider = new ofxDatGuiSlider("LINE WIDTH", 1, 8, 1);

  getBackgroundButton->onButtonEvent(this, &WebcamP2PScene::onButtonEvent);
  contourThresholdSlider->setPrecision(0);
  minContourSlider->setPrecision(0);
  maxContourSlider->setPrecision(0);
  blobsToConsiderSlider->setPrecision(0);
  lineWidthSizeSlider->setPrecision(0);

  getBackgroundButton->setPosition(drawX, drawY + drawHeight + exportPictureButton->getHeight());
  invertToggleContour->setPosition(drawX, getBackgroundButton->getY() + getBackgroundButton->getHeight());
  contourThresholdSlider->setPosition(drawX, invertToggleContour->getY() + invertToggleContour->getHeight());
  minContourSlider->setPosition(drawX, contourThresholdSlider->getY() + contourThresholdSlider->getHeight());
  maxContourSlider->setPosition(drawX, minContourSlider->getY() + minContourSlider->getHeight());
  blobsToConsiderSlider->setPosition(drawX, maxContourSlider->getY()+ maxContourSlider->getHeight());
  lineWidthSizeSlider->setPosition(drawX, blobsToConsiderSlider->getY()+ blobsToConsiderSlider->getHeight());

  for(int i; i < guiContours.size(); i++){
    delete guiContours[i];
  }
  guiContours.clear();
  guiContours.push_back(getBackgroundButton);
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


  invertToggleCanny->setPosition(drawX, drawY + drawHeight + exportPictureButton->getHeight());
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
  getBackgroundButton->setWidth(drawWidth);
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

void WebcamP2PScene::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
  cout << "The recoded video file is now complete." << endl;
}

void WebcamP2PScene::record(){
  if(recording){
      recording = false;
      vidRecorder.close();
      recordButton->setLabel("START RECORDING");
      recordButton->setStripeColor(ofColor(0,255,0));
  }
  else if(hasChosenExportFolder){
    recording = true;
    vidRecorder.setup(exportDir+"/"+ofGetTimestampString()+".mov", imgOut.getHeight(), imgOut.getWidth(), 30);
    vidRecorder.start();
    recordButton->setLabel("STOP RECORDING");
    recordButton->setStripeColor(ofColor(255,0,0));
  }
}

void WebcamP2PScene::updateNormalImage(){
  if(invertToggle->getChecked()){
    imgInCV.invert();
  }
  if(int(blurSlider->getValue()) % 2 == 0){
    blurSlider->setValue(blurSlider->getValue()+1);
  }
  imgInCV.blurGaussian(blurSlider->getValue());

  imgInCV.convertToGrayscalePlanarImages(r, g, b);

  r.brightnessContrast(brightnessSlider->getValue(), contrastSlider->getValue());
  g.brightnessContrast(brightnessSlider->getValue(), contrastSlider->getValue());
  b.brightnessContrast(brightnessSlider->getValue(), contrastSlider->getValue());


  imgInCV.setFromGrayscalePlanarImages(r, g, b);
  imgInCV.flagImageChanged();

}

void WebcamP2PScene::updateContourImage(){
  if (bLearnBackground == true) {
      grayBg = imgInGray; // update the background image
      bLearnBackground = false;
      cout << "BG" << endl;
  }
  grayDiff.absDiff(grayBg, imgInGray);
  grayDiff.threshold(contourThresholdSlider->getValue());
  contourFinder.findContours(grayDiff, minContourSlider->getValue(),
  maxContourSlider->getValue(), blobsToConsiderSlider->getValue(), true, true);

  contourFbo.readToPixels(contourImg.getPixels());
}

void WebcamP2PScene::updateCannyImage(){
  cvCanny(imgInGray.getCvImage(), canny.getCvImage(), minThresholdSlider->getValue(),  maxThresholdSlider->getValue(), 3);
  if(invertToggleCanny->getChecked()){
    canny.invert();
  }
  canny.flagImageChanged();
}

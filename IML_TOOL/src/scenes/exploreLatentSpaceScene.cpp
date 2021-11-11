#include "exploreLatentSpaceScene.h"


void ExploreLatentSpaceScene::refresh(){
  // neural network setup, bail out on error
  // the default model is edges2shoes and accepts [None, None, None, 3]
  ModelManager * modelManager = ModelManager::getInstance();

  SceneManager::getInstance()->setShowNavBar(false);

  if(modelManager->getModelType() != MODEL_TYPE::GAN){
      cout << "Should not have gone here..." << endl;
      std::exit(EXIT_FAILURE);
  }

  nnWidth = modelManager->getImgWidth();
  nnHeight = modelManager->getImgHeight();
  latentDim = modelManager->getLatentVector();

  resetSpeedVector();
  randomiseLatentVector();
  latentVectorSelectSlider->setMax(latentDim);

  string modelDir = "saved_models/"+modelManager->getModelName()+
  "/saved_networks/ckpt/-"+ofToString(modelManager->getEpochsTrained())+"_generator";
  if(modelManager->getStatus() == -10){
    // custom loaded
    modelDir = modelManager->getModelName();
  }
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


  vidRecorder.setVideoCodec("mpeg4");
  vidRecorder.setVideoBitrate("800k");

  // allocate fbo and images with correct dimensions, no alpha channel
  ofLogVerbose() << "Allocating fbo and images ("
                 << nnWidth << ", " << nnHeight << ")";
  // fbo.allocate(nnWidth, nnHeight, GL_RGB);
  // imgIn.allocate(nnWidth, nnHeight, OF_IMAGE_COLOR);
  imgOut.clear();
  if(ModelManager::getInstance()->getInputChannel() == 1){
      imgOut.allocate(nnWidth, nnHeight, OF_IMAGE_GRAYSCALE);
  }
  else{
    imgOut.allocate(nnWidth, nnHeight, OF_IMAGE_COLOR);
  }


  // shorten idle time to have model check for input more frequently,
  // this may increase responsiveness on faster machines but will use more cpu
  model.setIdleTime(10);

  // start the model background thread
  model.startThread();

  hasChosenExportFolder = false;
  recording = false;
  autoRun = true;
  dirWidget = false;
  dialWidget.setLatentVector(&latentVector);
  latentGraphWidget.setLatentVector(&latentVector);
  speedDialWidget.setLatentVector(&speedVector);
  speedLatentGraphWidget.setLatentVector(&speedVector);
}

void ExploreLatentSpaceScene::setup(){
  setID(SCENE_TYPE::EXPLORE_LATENT);
  randomiseLatentVector();
  resetSpeedVector();

  ofSetFrameRate(60);
	ofSetVerticalSync(true);
	// ofBackground(54, 54, 54);
	ofSetLogLevel("ofxTensorFlow2", OF_LOG_VERBOSE);

	// use only a portion of the GPU memory & grow as needed
	if(!ofxTF2::setGPUMaxMemory(ofxTF2::GPU_PERCENT_70, true)) {
		ofLogError() << "failed to set GPU Memory options!";
	}
  addGui();

}

void ExploreLatentSpaceScene::startThread(){
  autoRun = true;
}

void ExploreLatentSpaceScene::stopThread(){
    autoRun = false;
}

void ExploreLatentSpaceScene::update(){
  for(int i = 0; i < gui.size(); i++){
    gui[i]->update();
  }
  if(dirWidget){
      speedLatentGraphWidget.update();
      speedDialWidget.update();
      speedSlider->update();
      resetSpeedButton->update();
  }
  else{
    latentGraphWidget.update();
    dialWidget.update();
  }
  if(hasChosenExportFolder){
    recordButton->update();
    exportPictureButton->update();
  }

  // start & stop the model
  if(!autoRun && model.isThreadRunning()) {
    model.stopThread();
  }
  else if(autoRun && !model.isThreadRunning()) {
    model.startThread();
  }

  setLatentVector();
  updateLatentVector();

  // write fbo to ofImage
  // fbo.readToPixels(imgIn.getPixels());

  // async update on model input
  if(model.readyForInput()) {


    input = ofxTF2::vectorToTensor<float>(latentVector, {1, latentDim});
    // auto input_ = cppflow::fill({1, 512}, 0.5f);
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
    output = (output *127.5f ) + 127.5f;
    // output = cppflow::cast(input, TF_UINT8, TF_FLOAT);
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

}
void ExploreLatentSpaceScene::draw(){
    for(int i = 0; i < gui.size(); i++){
      gui[i]->draw();
    }
    if(dirWidget){
        speedLatentGraphWidget.draw();
        speedDialWidget.draw();
        speedSlider->draw();
        resetSpeedButton->draw();
    }
    else{
      latentGraphWidget.draw();
      dialWidget.draw();
    }

    if(hasChosenExportFolder){
      recordButton->draw();
      exportPictureButton->draw();
    }
    std::stringstream str;
  	ofPushMatrix();
  		// just to check fbo is reading correctly
  		//if(!drawImage(imgIn, "imgIn")) {
  		//	str << "imgIn not allocated !!" << srd::endl;
  		//}
  		if(!drawImage(imgOut, "imgOut", imWidth, imHeight)) {
  			cout << "imgOut not allocated !!" << std::endl;
  		}
  	ofPopMatrix();
    // gui.draw();

}

void ExploreLatentSpaceScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
    stopThread();
    update();
    SceneManager::getInstance()->setShowNavBar(true);
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACT_MENU);
  }
  else if(e.target == randomiseButton){
    randomiseLatentVector();
  }
  else if(e.target == toggleWidgetsButton){
    dirWidget = !dirWidget;
  }
  else if (e.target == resetSpeedButton){
    resetSpeedVector();
  }
  else if (e.target == setAllButton){
    for(int i =0; i < latentVector.size(); i++){
      latentVector[i] = 0;
    }
    latentVectorSlider->setValue(0);
  }
  else if(e.target == setExportFolderButton){
    ofFileDialogResult result = ofSystemLoadDialog("select output dir", true);
    if (result.bSuccess) {
      exportDir = result.getPath();
      hasChosenExportFolder = true;
    }
  }
  else if(e.target == recordButton){
    record();
  }
  else if(e.target == exportPictureButton){
    ofSaveImage(imgOut,exportDir+"/"+ofGetTimestampString()+".png");
  }
}

void ExploreLatentSpaceScene::randomiseLatentVector(){

  latentVector.clear();
  for (int i =0; i < latentDim; i++){
      float b = ofRandom(-1.f, 1.f);
      latentVector.push_back(b);
  }

}

void ExploreLatentSpaceScene::updateLatentVector(){

  for (int i =0; i < latentDim; i++){
      latentVector[i] += (speedVector[i] * (speedSlider->getValue())/100.f);
      if(latentVector[i] > 1){
        latentVector[i] = 1;
      }
      else if (latentVector[i] < -1){
        latentVector[i] = -1;
      }
  }

}


void ExploreLatentSpaceScene::resetSpeedVector(){

  speedVector.clear();
  for (int i =0; i < latentDim; i++){
      speedVector.push_back(0);
  }

}


void ExploreLatentSpaceScene::setLatentVector(){
  latentVector[latentVectorSelectSlider->getValue()] = latentVectorSlider->getValue();
}


// private -------------------------------------------------------------
template <typename T>
bool ExploreLatentSpaceScene::drawImage(const T& img, string label, int width, int height) {
	if(img.isAllocated()) {
		ofSetColor(255);
		ofFill();

		// draw image
		img.draw(ofGetWidth()/2, ofGetHeight()/2 - height/2, width, height);

		// draw border
		ofNoFill();
		ofSetColor(200);
		ofSetLineWidth(1);
		ofDrawRectangle(ofGetWidth()/2, ofGetHeight()/2 - height/2, width, height);

		// draw label
		ofDrawBitmapString(label, 10, height + 15);

		// next position
		ofTranslate(img.getWidth(), 0);

		return true;
	}
	return false;
}

void ExploreLatentSpaceScene::record(){
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

void ExploreLatentSpaceScene::addGui(){
  int width = 350;
  int buffer = 25;
  latentVectorSelectSlider = new ofxDatGuiSlider("Latent vector idx", 0, latentDim);
  latentVectorSelectSlider->setPosition(0, 50);
  latentVectorSelectSlider->setPrecision(0);
  latentVectorSelectSlider->setWidth(width, 0.5);

  latentVectorSlider = new ofxDatGuiSlider("Latent vector value", -1, 1);
  latentVectorSlider->setPosition(0, buffer+latentVectorSelectSlider->getY());
  latentVectorSlider->setWidth(width, 0.5);


  randomiseButton = new ofxDatGuiButton("RANDOMISE");
  randomiseButton->setPosition(0, buffer+latentVectorSlider->getY());
  randomiseButton->onButtonEvent(this, &ExploreLatentSpaceScene::onButtonEvent);
  randomiseButton->setWidth(width);

  setAllButton = new ofxDatGuiButton("SET ALL");
  setAllButton->setPosition(0, buffer+randomiseButton->getY());
  setAllButton->onButtonEvent(this, &ExploreLatentSpaceScene::onButtonEvent);
  setAllButton->setWidth(width);

  toggleWidgetsButton = new ofxDatGuiButton("TOGGLE MODE");
  toggleWidgetsButton->setPosition(0, buffer+setAllButton->getY());
  toggleWidgetsButton->onButtonEvent(this, &ExploreLatentSpaceScene::onButtonEvent);
  toggleWidgetsButton->setWidth(width);

  speedSlider = new ofxDatGuiSlider("Traversal speed", 0, 1);
  speedSlider->setPosition(0, buffer+toggleWidgetsButton->getY());
  speedSlider->setWidth(width, 0.5);

  resetSpeedButton = new ofxDatGuiButton("RESET SPEED");
  resetSpeedButton->setPosition(0, buffer+speedSlider->getY());
  resetSpeedButton->onButtonEvent(this, &ExploreLatentSpaceScene::onButtonEvent);
  resetSpeedButton->setWidth(width);

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(100, ofGetHeight()-50);
  backButton->onButtonEvent(this, &ExploreLatentSpaceScene::onButtonEvent);

  setExportFolderButton = new ofxDatGuiButton("SET EXPORT FOLDER");
  setExportFolderButton->setPosition(ofGetWidth() - width - 50, ofGetHeight() - 100);
  setExportFolderButton->setWidth(width);
  setExportFolderButton->onButtonEvent(this, &ExploreLatentSpaceScene::onButtonEvent);

  recordButton = new ofxDatGuiButton("START RECORDING");
  recordButton->setStripeColor(ofColor(0,255,0));
  recordButton->setPosition(ofGetWidth() - width - 50, ofGetHeight() - 150);
  recordButton->setWidth(width);
  recordButton->onButtonEvent(this, &ExploreLatentSpaceScene::onButtonEvent);


  exportPictureButton = new ofxDatGuiButton("EXPORT IMAGE");
  exportPictureButton->setWidth(imWidth);
  exportPictureButton->setPosition(ofGetWidth()/2, ofGetHeight()/2 - imHeight/2 + imHeight);
  exportPictureButton->onButtonEvent(this, &ExploreLatentSpaceScene::onButtonEvent);


  gui.push_back(backButton);
  gui.push_back(latentVectorSelectSlider);
  gui.push_back(latentVectorSlider);
  gui.push_back(randomiseButton);
  gui.push_back(toggleWidgetsButton);
  gui.push_back(setAllButton);
  gui.push_back(setExportFolderButton);

  dialWidget.setup(150, 360, 125, ofColor(255, 255, 255), true);
  latentGraphWidget.setup(50, 520, 350, 175, ofColor(255, 255, 255), true);


  speedDialWidget.setup(150, 360, 125, ofColor(250, 218, 94), true);
  speedLatentGraphWidget.setup(50, 520, 350, 175,  ofColor(250, 218, 94), true);
  speedDialWidget.setLatentVector(&speedVector);
  speedLatentGraphWidget.setLatentVector(&speedVector);

}

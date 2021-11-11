#include "motionDetectionScene.h"


void MotionDetectionScene::refresh(){
  ModelManager * modelManager = ModelManager::getInstance();
  SceneManager::getInstance()->setShowNavBar(false);

  if(modelManager->getModelType() != MODEL_TYPE::GAN){
      cout << "Should not have gone here..." << endl;
      std::exit(EXIT_FAILURE);
  }

  nnWidth = modelManager->getImgWidth();
  nnHeight = modelManager->getImgHeight();
  latentDim = modelManager->getLatentVector();
  randomiseLatentVector();
  setPixelsToTrack();
  latentGraphWidget.setLatentVector(&latentVector);


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

  // allocate fbo and images with correct dimensions, no alpha channel
  ofLogVerbose() << "Allocating fbo and images ("
                 << nnWidth << ", " << nnHeight << ")";
  // imgIn.clear();
  imgOut.clear();

  if(ModelManager::getInstance()->getInputChannel() == 1){
      imgOut.allocate(nnWidth, nnHeight, OF_IMAGE_GRAYSCALE);
  }
  else{
      imgOut.allocate(nnWidth, nnHeight, OF_IMAGE_COLOR);
  }

  vidRecorder.setVideoCodec("mpeg4");
  vidRecorder.setVideoBitrate("800k");

  vidGrabber.setDeviceID(0);
  vidGrabber.setVerbose(true);
  vidGrabber.setDesiredFrameRate(60);
  vidGrabber.initGrabber(videoGrabberWidth,videoGrabberHeight);


  // shorten idle time to have model check for input more frequently,
  // this may increase responsiveness on faster machines but will use more cpu
  model.setIdleTime(10);

  // start the model background thread
  model.startThread();

  autoRun = true;
  bLearnBackground = true;

  hasChosenExportFolder = false;
  recording = false;
}

void MotionDetectionScene::setup(){
  setID(SCENE_TYPE::INTERACT_MOTION);
  randomiseLatentVector();

  ofSetFrameRate(60);
	ofSetVerticalSync(true);
	// ofBackground(54, 54, 54);
	ofSetLogLevel("ofxTensorFlow2", OF_LOG_VERBOSE);

	// use only a portion of the GPU memory & grow as needed
	if(!ofxTF2::setGPUMaxMemory(ofxTF2::GPU_PERCENT_70, true)) {
		ofLogError() << "failed to set GPU Memory options!";
	}
  addGui();

  distributionX = normal_distribution<float>(fboWidth /2, fboWidth * 2);
  distributionY = normal_distribution<float>(fboHeight / 2, fboHeight * 2);

  fbo.allocate(fboWidth, fboHeight, GL_RGB);
  imgIn.allocate(fboWidth, fboHeight, OF_IMAGE_COLOR);
  nextImgIn.allocate(fboWidth, fboHeight);
  lastImgIn.allocate(fboWidth, fboHeight);
  imgDiff.allocate(fboWidth, fboHeight);
  nextImgInGray.allocate(fboWidth, fboHeight);
  lastImgInGray.allocate(fboWidth, fboHeight);

  latentGraphWidget.setup(50, ofGetHeight()/2 , 350, 175, ofColor(255, 255, 255), true);

}

void MotionDetectionScene::startThread(){
  autoRun = true;
}

void MotionDetectionScene::stopThread(){
    autoRun = false;
}

void MotionDetectionScene::update(){

  for(int i = 0; i < gui.size(); i++){
    gui[i]->update();
  }

  vidGrabber.update();
  if(vidGrabber.isFrameNew()){
    lastImgIn.setFromPixels(imgIn.getPixels());
    lastImgInGray = lastImgIn;
    fbo.readToPixels(imgIn.getPixels());
    imgIn.update();
    nextImgIn.setFromPixels(imgIn.getPixels());
    nextImgInGray = nextImgIn;
    setLatentVector();
  }

  // start & stop the model
  if(!autoRun && model.isThreadRunning()) {
    model.stopThread();
  }
  else if(autoRun && !model.isThreadRunning()) {
    model.startThread();
  }
  latentGraphWidget.update();

  if(hasChosenExportFolder){
    recordButton->update();
    exportPictureButton->update();
  }

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
void MotionDetectionScene::draw(){
    for(int i = 0; i < gui.size(); i++){
      gui[i]->draw();
    }
    latentGraphWidget.draw();

    if(hasChosenExportFolder){
      recordButton->draw();
      exportPictureButton->draw();
    }

    fbo.begin();
    ofScale(float(fboWidth) / float(videoGrabberWidth), float(fboHeight) / float(videoGrabberHeight));
    vidGrabber.draw(0, 0);
    fbo.end();

  	ofPushMatrix();

  		if(!drawImage(imgOut, "imgOut", ofGetWidth()/2, ofGetHeight()/2 - imHeight/2, imWidth, imHeight)) {
  			cout << "imgOut not allocated !!" << endl;
  		}
      if(!drawImage(imgIn, "imgIn", 225 - imHeight / 8, ofGetHeight()/2 - 100 - imWidth/8, imWidth/4, imHeight/4)) {
        cout << "imgIn not allocated !!" << endl;
      }
  	ofPopMatrix();

}

void MotionDetectionScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
    vidGrabber.close();
    stopThread();
    update();
    SceneManager::getInstance()->setShowNavBar(true);
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACT_MENU);
  }
  else if(e.target == randomiseButton){
    randomiseLatentVector();
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

void MotionDetectionScene::randomiseLatentVector(){

  latentVector.clear();
  for (int i =0; i < latentDim; i++){
      float b = ofRandom(-1.f, 1.f);
      latentVector.push_back(b);
  }

  setPixelsToTrack();

}

void MotionDetectionScene::setPixelsToTrack(){
  pixelsToTrack.clear();
  for (int i =0; i < fboWidth*fboHeight; i++){
    // float x = distributionX(generator);
    // while(x < 0 || x >= fboWidth){
    //   x = distributionX(generator);
    // }
    // float y = distributionY(generator);
    // while(y < 0 || y >= fboHeight){
    //   y = distributionY(generator);
    // }
    int x = rand() % fboWidth;
    int y = rand() % fboHeight;
    int pixelIndex = y * fboWidth + x;
    // ofColor colour = pixels->getColor(pixelIndex);
    pixelsToTrack.push_back(pixelIndex);
  }

}

void MotionDetectionScene::setLatentVector(){

  // imgDiff.absDiff(lastImgInGray, nextImgInGray);
  // ofPixels pixels = imgDiff.getPixels();
  // ofPixels p1 = lastImgInGray.getPixels();
  // ofPixels p2 = nextImgInGray.getPixels();
  ofPixels p = imgIn.getPixels();
  for(int i =0; i < fboWidth*fboHeight; i++){
    int idx = pixelsToTrack[i];
    int dimIndex = idx % latentDim;

    ofColor c = p.getColor(idx);

    float cc = (c.r + c.g + c.b)/3;
    float newVal = ((cc - 127.5)/127.5);
    latentVector[dimIndex] = newVal;
  }
}


// private -------------------------------------------------------------
template <typename T>
bool MotionDetectionScene::drawImage(const T& img, string label, int x, int y , int width, int height) {
	if(img.isAllocated()) {
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
    ofDrawBitmapString(label, x, y - 15);

		// next position
		// ofTranslate(img.getWidth(), 0);

		return true;
	}
	return false;
}

void MotionDetectionScene::record(){
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

void MotionDetectionScene::addGui(){
  int width = 350;
  int buffer = 25;

  randomiseButton = new ofxDatGuiButton("RANDOMISE");
  randomiseButton->setPosition(50, 100);
  randomiseButton->onButtonEvent(this, &MotionDetectionScene::onButtonEvent);
  randomiseButton->setWidth(width);


  setExportFolderButton = new ofxDatGuiButton("SET EXPORT FOLDER");
  setExportFolderButton->setPosition(ofGetWidth() - width - 50, ofGetHeight() - 100);
  setExportFolderButton->setWidth(width);
  setExportFolderButton->onButtonEvent(this, &MotionDetectionScene::onButtonEvent);

  recordButton = new ofxDatGuiButton("START RECORDING");
  recordButton->setStripeColor(ofColor(0,255,0));
  recordButton->setPosition(ofGetWidth() - width - 50, ofGetHeight() - 150);
  recordButton->setWidth(width);
  recordButton->onButtonEvent(this, &MotionDetectionScene::onButtonEvent);

  exportPictureButton = new ofxDatGuiButton("EXPORT IMAGE");
  exportPictureButton->setPosition(ofGetWidth()/2, ofGetHeight()/2 - imHeight/2 + imHeight);
  exportPictureButton->setWidth(imWidth);
  exportPictureButton->onButtonEvent(this, &MotionDetectionScene::onButtonEvent);


  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(100, ofGetHeight()-50);
  backButton->onButtonEvent(this, &MotionDetectionScene::onButtonEvent);

  gui.push_back(backButton);
  gui.push_back(randomiseButton);
  gui.push_back(setExportFolderButton);

}

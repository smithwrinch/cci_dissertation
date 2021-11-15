#include "interactP2PInputScene.h"

void InteractP2PInputScene::refresh(){

  SceneManager::getInstance()->setShowNavBar(false);

  setupGui();

  // main model
  ModelManager * modelManager = ModelManager::getInstance();
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
        cout << "ERROR couldn't load model" << endl;
        std::exit(EXIT_FAILURE);
      }
      else{
        modelManager->setEpochsTrained(modelManager->getEpochsTrained()+1);
        modelManager->save();
      }
  }
  nnWidth = modelManager->getImgWidth();
  nnHeight = modelManager->getImgHeight();

  // allocate fbo and images with correct dimensions, no alpha channel
  ofLogVerbose() << "Allocating fbo and images ("
                 << nnWidth << ", " << nnHeight << ")";
  // fbo.allocate(nnWidth, nnHeight, GL_RGB);

  if(modelManager->getInputChannel() == 1){
    imgOut.allocate(nnWidth, nnHeight, OF_IMAGE_GRAYSCALE);
    imgIn2.allocate(nnHeight, nnHeight, OF_IMAGE_GRAYSCALE);
  }
  else{
    imgOut.allocate(nnWidth, nnHeight, OF_IMAGE_COLOR);
    imgIn2.allocate(nnHeight, nnHeight, OF_IMAGE_COLOR);
  }
  imgInCV.allocate(nnWidth, nnHeight);
  imgInGray.allocate(nnWidth, nnHeight);

  vidRecorder.setVideoCodec("mpeg4");
  vidRecorder.setVideoBitrate("800k");

  ofAddListener(vidRecorder.outputFileCompleteEvent, this, &InteractP2PInputScene::recordingComplete);



  model.setIdleTime(10);
  autoRun = false;
  recording = false;
  hasChosenExportFolder = false;
  hasChosenModel = false;
  exportDir = "";
  modelDir = "";
  msg = "";
}

//--------------------------------------------------------------
void InteractP2PInputScene::setup() {
  setID(SCENE_TYPE::INTERACT_P2P_P2P);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	// ofBackground(54, 54, 54);
	ofSetLogLevel("ofxTensorFlow2", OF_LOG_VERBOSE);

	// use only a portion of the GPU memory & grow as needed
	if(!ofxTF2::setGPUMaxMemory(ofxTF2::GPU_PERCENT_70, true)) {
		ofLogError() << "failed to set GPU Memory options!";
	}
}

void InteractP2PInputScene::startThread(){
  autoRun = true;
}

void InteractP2PInputScene::stopThread(){
    autoRun = false;
}

void InteractP2PInputScene::update() {
  if(hasChosenModel){

  	// start & stop the model
  	if(!autoRun && model.isThreadRunning()) {
  		model.stopThread();
      model2.stopThread();
  	}
  	else if(autoRun && !model.isThreadRunning()) {
  		model.startThread();
      model2.startThread();
  	}

    vidGrabber.update();
    if(vidGrabber.isFrameNew()){
      fbo.readToPixels(webcamOut.getPixels());
    }

    if(model2.readyForInput()){
		  input2 = ofxTF2::imageToTensor(webcamOut);
      model2.update(input2);
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
    if(model2.isOutputNew()){
      // pull output from model
      output2 = model2.getOutput();
      // output = cppflow::cast(input, TF_UINT8, TF_FLOAT);
      // write tensor to ofImage
      ofxTF2::tensorToImage(output2, imgIn);
      imgIn.update();
      // async update on model input
    	if(model.readyForInput()) {


        if(imgChannelToggle->getChecked()){
          imgIn2.setImageType(OF_IMAGE_GRAYSCALE);
        }
        imgIn2.setFromPixels(imgIn.getPixels());
        if(invertToggle->getChecked()){
          if(imgChannelToggle->getChecked()){
            imgInGray.setFromPixels(imgIn2.getPixels());
            imgInGray.invert();
            imgIn2.setFromPixels(imgInGray.getPixels());
          }
          else{
            imgInCV.setFromPixels(imgIn2.getPixels());
            imgInCV.invert();
            imgIn2.setFromPixels(imgInCV.getPixels());
          }
        }
        imgIn2.resize(nnWidth, nnHeight);
    		// read tensor from ofImage
        if(imgChannelToggle->getChecked()){
          imgIn2.setImageType(OF_IMAGE_COLOR);
        }

    		input = ofxTF2::imageToTensor(imgIn2);

    		// feed input into model
    		model.update(input);
        imgIn2.resize(int(imgWidthSlider->getValue()), int(imgHeightSlider->getValue()));

    		// end measurment
    		end = std::chrono::system_clock::now();
    		std::chrono::duration<double> diff = end - start;
    		ofLog() << "Run took: " << diff.count() << " s or ~"
    		        << (int)(1.0/diff.count()) << " fps";
    	}
    }

    for(int i = 0; i < gui.size(); i++){
      gui[i]->update();
    }
    if(hasChosenExportFolder){
      recordButton->update();
      exportPictureButton->update();
      exportPictureButton2->update();
    }

  }
  else{
    for(int i = 0; i < modelSelectGui.size(); i++){
      modelSelectGui[i]->update();
    }
  }
  if(!recording){
    backButton->update();
  }

}

//--------------------------------------------------------------
void InteractP2PInputScene::draw() {

  if(hasChosenModel){

    fbo.begin();
    ofScale(float(model2Width) / float(videoGrabberWidth), float(model2Height) / float(videoGrabberHeight));
    vidGrabber.draw(0, 0);
    fbo.end();
    ofPushMatrix();
    if(!drawImage(fbo, "webcam input", 50, 200, 200, 200)) {
      cout << "fbo not allocated !!" << std::endl;
    }
    if(!drawImage(imgIn, "pix2pix output->pix2pix input", 525 + 3 *drawWidth / 8, drawHeight + 100, drawWidth/4, drawHeight/4)) {
      cout << "imgIn not allocated !!" << std::endl;
    }
    if(!drawImage(imgOut, "Pix2Pix output", 525, 50, drawWidth, drawHeight)) {
      cout << "imgOut not allocated !!" << std::endl;
    }
    ofPopMatrix();

    // draw info texts
    ofSetColor(150);

    if(hasChosenExportFolder){
      recordButton->draw();
      exportPictureButton->draw();
      exportPictureButton2->draw();
    }
    for(int i = 0; i < gui.size(); i++){
      gui[i]->draw();
    }
  }
  else{
    for(int i = 0; i < modelSelectGui.size(); i++){
      modelSelectGui[i]->draw();
    }
    ofDrawBitmapString(msg, 300, 300);
  }
  if(!recording){
    backButton->draw();
  }

}

void InteractP2PInputScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
    stopThread();
    vidGrabber.close();
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
    ofSaveImage(imgIn,exportDir+"/"+ofGetTimestampString()+"_input.png");
  }
  else if(e.target == recordButton){
    record();
  }
  else if(e.target == continueButton){
      if(model2Dir != ""){
        refreshModel2();
        hasChosenModel = true;
        startThread();
        msg = "";
        vidGrabber.setDeviceID(0);
        vidGrabber.setVerbose(true);
        vidGrabber.setDesiredFrameRate(60);
        vidGrabber.initGrabber(videoGrabberWidth,videoGrabberHeight);
      }
      else{
        msg = "Please select a model first!";
      }
  }
  else if(e.target == modelSelectButton){
    ofFileDialogResult result = ofSystemLoadDialog("select model folder", true);
    if (result.bSuccess) {
      model2Dir = result.getPath();
    }
  }
}

// PRIVATE

//--------------------------------------------------------------
// draw image or fbo etc with border and label
// typename T must have draw(x,y), isAllocated(), getWidth(), getHeight()
template <typename T>
bool InteractP2PInputScene::drawImage(const T& img, string label, int x, int y, int width, int height) {
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
		ofDrawBitmapString(label, x, y- 15);

		// next position
		// ofTranslate(img.getWidth(), 0);

		return true;
	}
	return false;
}

void InteractP2PInputScene::setupGui(){

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(50, ofGetHeight()-50);
  backButton->onButtonEvent(this, &InteractP2PInputScene::onButtonEvent);

  exportPictureButton = new ofxDatGuiButton("EXPORT PICTURE");
  exportPictureButton->setPosition(525,50+drawHeight);
  exportPictureButton->setWidth(drawWidth);
  exportPictureButton->onButtonEvent(this, &InteractP2PInputScene::onButtonEvent);


  exportPictureButton2 = new ofxDatGuiButton("EXPORT PICTURE");
  exportPictureButton2->setPosition(525 + 3 *drawWidth / 8, drawHeight + 125);
  exportPictureButton2->setWidth(drawWidth/4);
  exportPictureButton2->onButtonEvent(this, &InteractP2PInputScene::onButtonEvent);

  int controlsWidth = 400;
  int controlsX = 100;

  int buttonsX = ofGetWidth() - controlsWidth;

  recordButton = new ofxDatGuiButton("START RECORDING");
  recordButton->setPosition(buttonsX, 650);
  recordButton->onButtonEvent(this, &InteractP2PInputScene::onButtonEvent);
  recordButton->setStripeColor(ofColor(0,255,0));

  setExportFolderButton = new ofxDatGuiButton("SET EXPORT FOLDER");
  setExportFolderButton->setPosition(buttonsX, recordButton->getY()+recordButton->getHeight()+25);
  setExportFolderButton->onButtonEvent(this, &InteractP2PInputScene::onButtonEvent);

  invertToggle = new ofxDatGuiToggle("INVERT");
  invertToggle->setPosition(50, 100);

  gui.clear();
  gui.push_back(setExportFolderButton);
  gui.push_back(invertToggle);

  // model select gui
  int width = 400;
  int x = ofGetWidth() / 2 - width / 2;

  modelSelectButton = new ofxDatGuiButton("SELECT MODEL");
  modelSelectButton->setPosition(x, ofGetHeight()/2 - modelSelectButton->getHeight() - 100);
  modelSelectButton->onButtonEvent(this, &InteractP2PInputScene::onButtonEvent);
  modelSelectButton->setWidth(width);

  imgWidthSlider = new ofxDatGuiSlider("IMAGE WIDTH", 28, 2048, 28);
  imgWidthSlider->setPosition(x, modelSelectButton->getY() +modelSelectButton->getHeight() + 10);
  imgWidthSlider->setPrecision(0);
  imgWidthSlider->setWidth(width, 0.5);

  imgHeightSlider = new ofxDatGuiSlider("IMAGE HEIGHT", 28, 2048, 28);
  imgHeightSlider->setPosition(x, imgWidthSlider->getY() +imgWidthSlider->getHeight() + 10);
  imgHeightSlider->setPrecision(0);
  imgHeightSlider->setWidth(width, 0.5);

  imgChannelToggle = new ofxDatGuiToggle("IMAGE GRAYSCALE");
  imgChannelToggle->setPosition(x, imgHeightSlider->getY() +imgHeightSlider->getHeight() + 10);
  imgChannelToggle->setWidth(width);

  continueButton = new ofxDatGuiButton("CONTINUE");
  continueButton->setPosition(x, imgChannelToggle->getY() + imgChannelToggle->getHeight() + 20);
  continueButton->onButtonEvent(this, &InteractP2PInputScene::onButtonEvent);
  continueButton->setWidth(width);

  modelSelectGui.push_back(modelSelectButton);
  modelSelectGui.push_back(imgWidthSlider);
  modelSelectGui.push_back(imgHeightSlider);
  modelSelectGui.push_back(imgChannelToggle);
  modelSelectGui.push_back(continueButton);


}

void InteractP2PInputScene::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
  cout << "The recoded video file is now complete." << endl;
}

void InteractP2PInputScene::refreshModel2(){
    model2.clear();
    if(!model2.load(model2Dir)){
        cout << "ERROR couldn't load input model" << endl;
        std::exit(EXIT_FAILURE);
    }
    imgIn.clear();
    webcamOut.clear();
    model2Width = int(imgWidthSlider->getValue());
    model2Height = int(imgHeightSlider->getValue());
    if(imgChannelToggle->getChecked()){
      imgIn.allocate(model2Width, model2Height, OF_IMAGE_GRAYSCALE);
    }
    else{
      imgIn.allocate(model2Width, model2Height, OF_IMAGE_COLOR);
    }
    webcamOut.allocate(model2Width, model2Height, OF_IMAGE_COLOR);
    fbo.allocate(model2Width, model2Height, GL_RGB);
}

void InteractP2PInputScene::record(){
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

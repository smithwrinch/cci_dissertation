#include "motionDetectionScene.h"


void MotionDetectionScene::refresh(){
  // neural network setup, bail out on error
  // the default model is edges2shoes and accepts [None, None, None, 3]
  randomiseLatentVector();
  ModelManager * modelManager = ModelManager::getInstance();



  if(modelManager->getModelType() != MODEL_TYPE::GAN){
      cout << "Should not have gone here..." << endl;
      std::exit(EXIT_FAILURE);
  }

  nnWidth = modelManager->getImgWidth();
  nnHeight = modelManager->getImgHeight();
  latentDim = modelManager->getLatentVector();


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

  autoRun = true;
  bSetBackground = true;
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

  // start & stop the model
  if(!autoRun && model.isThreadRunning()) {
    model.stopThread();
  }
  else if(autoRun && !model.isThreadRunning()) {
    model.startThread();
  }

  setLatentVector();

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

    // start new measurement
    start = std::chrono::system_clock::now();
  }

}
void MotionDetectionScene::draw(){
    for(int i = 0; i < gui.size(); i++){
      gui[i]->draw();
    }

    std::stringstream str;
  	ofPushMatrix();

  		if(!drawImage(imgOut, "imgOut", imWidth, imHeight)) {
  			cout << "imgOut not allocated !!" << std::endl;
  		}
  	ofPopMatrix();

}

void MotionDetectionScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
    stopThread();
    update();
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACT_MENU);
  }
  else if(e.target == randomiseButton){
    randomiseLatentVector();
  }
}

void MotionDetectionScene::randomiseLatentVector(){

  latentVector.clear();
  for (int i =0; i < latentDim; i++){
      float b = (float)rand() / (float)RAND_MAX;
      latentVector.push_back(b);
  }

}

// void MotionDetectionScene::updateLatentVector(){
//
//   for (int i =0; i < latentDim; i++){
//       latentVector[i] += speedVector[i]/ 100.f;
//       if(latentVector[i] > 1){
//         latentVector[i] = 1;
//       }
//       else if (latentVector[i] < 0){
//         latentVector[i] = 0;
//       }
//   }
//
// }


void MotionDetectionScene::setLatentVector(){
  // latentVector[latentVectorSelectSlider->getValue()] = latentVectorSlider->getValue();
}


// private -------------------------------------------------------------
template <typename T>
bool MotionDetectionScene::drawImage(const T& img, string label, int width, int height) {
	if(img.isAllocated()) {
		ofSetColor(255);
		ofFill();

		// draw image
		img.draw(ofGetWidth()/2 - width/2, ofGetHeight()/2 - height/2, width, height);

		// draw border
		ofNoFill();
		ofSetColor(200);
    ofSetLineWidth(1);
    ofDrawRectangle(ofGetWidth()/2 - width/2, ofGetHeight()/2 - height/2, width, height);

    // draw label
    ofDrawBitmapString(label, 10, height + 15);

		// next position
		ofTranslate(img.getWidth(), 0);

		return true;
	}
	return false;
}

void MotionDetectionScene::addGui(){
  int width = 350;
  int buffer = 25;

  randomiseButton = new ofxDatGuiButton("RANDOMISE");
  randomiseButton->setPosition(0, 50);
  randomiseButton->onButtonEvent(this, &MotionDetectionScene::onButtonEvent);
  randomiseButton->setWidth(width);


  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(100, ofGetHeight()-50);
  backButton->onButtonEvent(this, &MotionDetectionScene::onButtonEvent);

  gui.push_back(backButton);
  gui.push_back(randomiseButton);

}

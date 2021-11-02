#include "exploreLatentSpaceScene.h"


void ExploreLatentSpaceScene::refresh(){
  // neural network setup, bail out on error
  // the default model is edges2shoes and excepts [None, None, None, 3]
  ModelManager * modelManager = ModelManager::getInstance();

  if(modelManager->getModelType() != MODEL_TYPE::GAN){
      cout << "Should not have gone here..." << endl;
      std::exit(EXIT_FAILURE);
  }

  nnWidth = modelManager->getImgWidth();
  nnHeight = modelManager->getImgHeight();
  latentDim = modelManager->getLatentVector();

  latentVectorSelectSlider->setMax(latentDim);

  string modelDir = "saved_models/"+modelManager->getModelName()+
  "/saved_networks/ckpt/-"+ofToString(modelManager->getEpochsTrained())+"_generator";

  model.clear();
  if(!model.load(modelDir)) {

    string newModelDir = "saved_models/"+modelManager->getModelName()+
      "/saved_networks/ckpt/-"+ofToString(modelManager->getEpochsTrained()+1)+"_generator";
      // May have not updated getEpochsTrained
      if(!model.load(newModelDir)) {
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
  imgOut.allocate(nnWidth, nnHeight, OF_IMAGE_COLOR);


  // shorten idle time to have model check for input more frequently,
  // this may increase responsiveness on faster machines but will use more cpu
  model.setIdleTime(10);

  // start the model background thread
  model.startThread();

  autoRun = true;
}

void ExploreLatentSpaceScene::setup(){
  setID(SCENE_TYPE::EXPLORE_LATENT);
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
  latentGraphWidget.update();
  dialWidget.update();

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

    // write tensor to ofImage
    ofxTF2::tensorToImage(output, imgOut);
    imgOut.update();

    // start new measurement
    start = std::chrono::system_clock::now();
  }

}
void ExploreLatentSpaceScene::draw(){
    for(int i = 0; i < gui.size(); i++){
      gui[i]->draw();
    }
    dialWidget.draw();
    latentGraphWidget.draw();

    std::stringstream str;
  	ofPushMatrix();
  		// just to check fbo is reading correctly
  		//if(!drawImage(imgIn, "imgIn")) {
  		//	str << "imgIn not allocated !!" << srd::endl;
  		//}
  		if(!drawImage(imgOut, "imgOut")) {
  			cout << "imgOut not allocated !!" << std::endl;
  		}
  	ofPopMatrix();
    // gui.draw();

}

void ExploreLatentSpaceScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
    stopThread();
    update();
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::TRAIN);
  }
  else if(e.target == randomiseButton){
    randomiseLatentVector();
  }
}

void ExploreLatentSpaceScene::randomiseLatentVector(){

  latentVector.clear();
  for (int i =0; i < latentDim; i++){
      float b = (float)rand() / (float)RAND_MAX;
      latentVector.push_back(b);
  }

}


void ExploreLatentSpaceScene::setLatentVector(){
  latentVector[latentVectorSelectSlider->getValue()] = latentVectorSlider->getValue();
}


// private -------------------------------------------------------------
template <typename T>
bool ExploreLatentSpaceScene::drawImage(const T& img, string label) {
	if(img.isAllocated()) {
		ofSetColor(255);
		ofFill();

		// draw image
		img.draw(ofGetWidth()/2 - nnWidth/2, ofGetHeight()/2 - nnHeight/2);

		// draw border
		ofNoFill();
		ofSetColor(200);
		ofSetLineWidth(1);
		ofDrawRectangle(ofGetWidth()/2 - nnWidth/2, ofGetHeight()/2 - nnHeight/2, img.getWidth(), img.getHeight());

		// draw label
		ofDrawBitmapString(label, 10, img.getHeight() + 15);

		// next position
		ofTranslate(img.getWidth(), 0);

		return true;
	}
	return false;
}

void ExploreLatentSpaceScene::addGui(){
  int width = 350;
  latentVectorSelectSlider = new ofxDatGuiSlider("Latent vector idx", 0, latentDim);
  latentVectorSelectSlider->setPosition(0, 50);
  latentVectorSelectSlider->setPrecision(0);
  latentVectorSelectSlider->setWidth(width, 0.5);

  latentVectorSlider = new ofxDatGuiSlider("Latent vector value", 0, 1);
  latentVectorSlider->setPosition(0, 50+latentVectorSelectSlider->getHeight());
  latentVectorSlider->setWidth(width, 0.5);

  randomiseButton = new ofxDatGuiButton("RANDOMISE");
  randomiseButton->setPosition(0, 50+2*latentVectorSelectSlider->getHeight());
  randomiseButton->onButtonEvent(this, &ExploreLatentSpaceScene::onButtonEvent);
  randomiseButton->setWidth(width);

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(100, ofGetHeight()-50);
  backButton->onButtonEvent(this, &ExploreLatentSpaceScene::onButtonEvent);

  gui.push_back(backButton);
  gui.push_back(latentVectorSelectSlider);
  gui.push_back(latentVectorSlider);
  gui.push_back(randomiseButton);

  dialWidget.setup(150, 260, 125);
  latentGraphWidget.setup(50, 420, 350, 175);

  dialWidget.setLatentVector(&latentVector);
  latentGraphWidget.setLatentVector(&latentVector);
}

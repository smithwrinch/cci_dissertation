#include "inputGANScene.h"


void InputGANScene::setup(){
  setID(SCENE_TYPE::INTERACTGAN);
  randomiseLatentVector();

  ofSetFrameRate(60);
	ofSetVerticalSync(true);
	// ofBackground(54, 54, 54);
	ofSetWindowTitle("example_gan");
	ofSetLogLevel("ofxTensorFlow2", OF_LOG_VERBOSE);

	// use only a portion of the GPU memory & grow as needed
	if(!ofxTF2::setGPUMaxMemory(ofxTF2::GPU_PERCENT_70, true)) {
		ofLogError() << "failed to set GPU Memory options!";
	}

	// neural network setup, bail out on error
	// the default model is edges2shoes and excepts [None, None, None, 3]
	if(!model.load("default_models/carStyleGAN")) {
		std::exit(EXIT_FAILURE);
	}

	// allocate fbo and images with correct dimensions, no alpha channel
	ofLogVerbose() << "Allocating fbo and images ("
	               << nnWidth << ", " << nnHeight << ")";
	// fbo.allocate(nnWidth, nnHeight, GL_RGB);
	// imgIn.allocate(nnWidth, nnHeight, OF_IMAGE_COLOR);
	imgOut.allocate(nnWidth, nnHeight, OF_IMAGE_COLOR);


	// shorten idle time to have model check for input more frequently,
	// this may increase responsiveness on faster machines but will use more cpu
	model.setIdleTime(10);

	// start the model background thread
	model.startThread();

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(100, ofGetHeight()-50);
  backButton->onButtonEvent(this, &InputGANScene::onButtonEvent);

  gui.setup();
  gui.add(latentVectorSelect.setup("Select latent vector idx", 0, 0, 512));
  gui.add(latentVectorSet.setup("latent vector value", 0, 1, 0.001));
  gui.add(randomiseButton.setup("load field (l)"));
  randomiseButton.addListener(this, &InputGANScene::randomiseLatentVector);
}

void InputGANScene::startThread(){
  autoRun = true;
}

void InputGANScene::stopThread(){
    autoRun = false;
}

void InputGANScene::update(){
  backButton->update();

  // start & stop the model
  if(!autoRun && model.isThreadRunning()) {
    model.stopThread();
  }
  else if(autoRun && !model.isThreadRunning()) {
    model.startThread();
  }

  setLatentVectorAt(latentVectorSelect, latentVectorSet);

  // write fbo to ofImage
  // fbo.readToPixels(imgIn.getPixels());

  // async update on model input
  if(model.readyForInput()) {


    input = ofxTF2::vectorToTensor<float>(latentVector, {1, 512});
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
void InputGANScene::draw(){
    backButton->draw();

    std::stringstream str;
  	ofPushMatrix();
  		// just to check fbo is reading correctly
  		//if(!drawImage(imgIn, "imgIn")) {
  		//	str << "imgIn not allocated !!" << srd::endl;
  		//}
  		if(!drawImage(imgOut, "imgOut")) {
  			str << "imgOut not allocated !!" << std::endl;
  		}
  	ofPopMatrix();
    gui.draw();

}

void InputGANScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
    stopThread();
    update();
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::MENU);
  }
}

void InputGANScene::randomiseLatentVector(){

  latentVector.clear();
  int a = 512; //1 to 20
  for (int i =0; i < a; i++){
          int b = rand();
          latentVector.push_back(b);
  }

  latentVectorSelect = 0;
  latentVectorSet = latentVector[0];
}


void InputGANScene::setLatentVectorAt(int i, float val){
  latentVector[i] = val;
}


// private -------------------------------------------------------------
template <typename T>
bool InputGANScene::drawImage(const T& img, string label) {
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

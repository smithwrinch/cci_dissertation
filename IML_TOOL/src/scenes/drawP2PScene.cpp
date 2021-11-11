#include "drawP2PScene.h"

void DrawP2PScene::refresh(){
  // neural network setup, bail out on error
  // the default model is edges2shoes and excepts [None, None, None, 3]
  SceneManager::getInstance()->setShowNavBar(false);
  colors.clear();

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

  drawWidth = 512;
  drawHeight = 512;

  drawY = 25;
  drawX = (ofGetWidth() / 2) - drawWidth;

  // allocate fbo and images with correct dimensions, no alpha channel
  ofLogVerbose() << "Allocating fbo and images ("
                 << nnWidth << ", " << nnHeight << ")";
  fbo.allocate(nnWidth, nnHeight, GL_RGB);
  if(modelManager->getInputChannel() == 1){

    imgIn.allocate(nnWidth, nnHeight, OF_IMAGE_GRAYSCALE);
    imgOut.allocate(nnWidth, nnHeight, OF_IMAGE_GRAYSCALE);
  }
  else{
    imgIn.allocate(nnWidth, nnHeight, OF_IMAGE_COLOR);
    imgOut.allocate(nnWidth, nnHeight, OF_IMAGE_COLOR);
  }

  // search for the drawing tool config files in bin/data/draw
  setupDrawingTool("default_models/draw");
  setupGui();

  //video shit
  //    vidRecorder.setFfmpegLocation(ofFilePath::getAbsolutePath("ffmpeg")); // use this is you have ffmpeg installed in your data folder

  // run 'ffmpeg -codecs' to find out what your implementation supports (or -formats on some older versions)
  vidRecorder.setVideoCodec("mpeg4");
  vidRecorder.setVideoBitrate("800k");

  ofAddListener(vidRecorder.outputFileCompleteEvent, this, &DrawP2PScene::recordingComplete);

  // shorten idle time to have model check for input more frequently,
  // this may increase responsiveness on faster machines but will use more cpu
  model.setIdleTime(10);
  autoRun = true;
  recording = false;
  hasChosenExportFolder = false;
  controlMode = 0;
  exportDir = "";
}

//--------------------------------------------------------------
void DrawP2PScene::setup() {
  setID(SCENE_TYPE::INTERACT_DRAW_P2P);
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	// ofBackground(54, 54, 54);
	ofSetLogLevel("ofxTensorFlow2", OF_LOG_VERBOSE);

	// use only a portion of the GPU memory & grow as needed
	if(!ofxTF2::setGPUMaxMemory(ofxTF2::GPU_PERCENT_70, true)) {
		ofLogError() << "failed to set GPU Memory options!";
	}



}

void DrawP2PScene::startThread(){
  autoRun = true;
}

void DrawP2PScene::stopThread(){
    autoRun = false;
}

void DrawP2PScene::update() {

	// start & stop the model
	if(!autoRun && model.isThreadRunning()) {
		model.stopThread();
	}
	else if(autoRun && !model.isThreadRunning()) {
		model.startThread();
	}

	// write fbo to ofImage
	fbo.readToPixels(imgIn.getPixels());

	// async update on model input
	if(model.readyForInput()) {

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

    // output = cppflow::cast(input, TF_FLOAT, TF_UINT8);
    //
    // output = cppflow::mul(output, cppflow::tensor({0.5f}));
    // output = cppflow::add(output, cppflow::tensor({0.5f}));
    // // output = cppflow::cast(output, TF_FLOAT,TF_UINT32);
    // output = cppflow::mul(output, cppflow::tensor({255.f}));

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

  if(controlMode == 2){
    for(int i = 0; i < guiControls.size(); i++){
      guiControls[i]->update();
    }
  }
  if(hasChosenExportFolder){
    recordButton->update();
    exportPictureButton->update();
    exportPictureButton2->update();
  }
  if(!recording){
    backButton->update();
  }
  drawColor = colourPicker->getColor();
  drawRadius = brushRadius->getValue();
}

//--------------------------------------------------------------
void DrawP2PScene::draw() {

	// DISPLAY STUFF
	std::stringstream str;
	str << "ENTER : Toggle auto run " << (autoRun ? "(X)" : "( )") << std::endl;
	str << "DEL   : Clear drawing " << std::endl;
	str << "d     : Toggle draw mode " << (drawMode == 0 ? "(draw)" : "(boxes)") << std::endl;
	str << "c/v   : Change draw radius (" << drawRadius << ")" << std::endl;
	str << "z/x   : Change draw color " << std::endl;
	str << "i     : Get color from mouse" << std::endl;
	str << "p     : Remove colour from palette" << std::endl;
	str << "a     : Add colour to palette" << std::endl;
  str << "r     : Toggle recording" << std::endl;
  str << "g     : Toggle gui" << std::endl;

	// str << std::endl;
	// str << "Draw in the box on the left" << std::endl;
	// str << "or drag an image (PNG) into it" << std::endl;

	ofPushMatrix();
		if(!drawImage(fbo, "(draw in here \nor drag in image)",drawX, drawY, drawWidth, drawHeight)) {
			str << "fbo not allocated !!" << std::endl;
		}
		// just to check fbo is reading correctly
		//if(!drawImage(imgIn, "imgIn")) {
		//	str << "imgIn not allocated !!" << srd::endl;
		//}
		if(!drawImage(imgOut, "imgOut", drawX+drawWidth, drawY, drawWidth, drawHeight)) {
			str << "imgOut not allocated !!" << std::endl;
		}
	ofPopMatrix();

	// draw info texts
	ofSetColor(150);

  ofDrawBitmapString(ofToString((int)ofGetFrameRate()) + " fps", ofGetWidth() - 54, 30);
  if(controlMode == 0){
    ofDrawBitmapString(str.str(), drawX, drawY + drawHeight+40);
  }
  else if(controlMode == 1){
  	// draw colors
  	ofFill();
  	int x = 0;
  	int y = drawY + drawHeight + 40;

  	// draw current color
  	ofSetColor(drawColor);
  	ofDrawCircle(x + paletteDrawSize/2, y + paletteDrawSize/2, paletteDrawSize/2);
  	ofSetColor(200);
  	ofDrawBitmapString("Current draw color",
  	                   x + paletteDrawSize + 10, y + paletteDrawSize/2);
  	y += paletteDrawSize + 10;

  	// draw color palette
  	for(int i = 0; i < colors.size(); i++) {
  		ofSetColor(colors[i]);
  		ofDrawCircle(x + paletteDrawSize/2, y + paletteDrawSize/2, paletteDrawSize/2);

  		// draw outline if selected color
  		if(colors[i] == drawColor) {
  			ofPushStyle();
  			ofNoFill();
  			ofSetColor(255);
  			ofSetLineWidth(3);
  			ofDrawRectangle(x, y, paletteDrawSize, paletteDrawSize);
  			ofPopStyle();
  		}

  		x += paletteDrawSize;

  		// wrap around if doesn't fit on screen
  		if(x > ofGetWidth() - paletteDrawSize) {
  			x = 0;
  			y += paletteDrawSize;
  		}
  	}

  }

	// display drawing helpers
	ofNoFill();
  /*
  TODO: uncomment this and figure out why it removes all other images...
  */
	switch(drawMode) {
		case 0: // draw
			ofSetLineWidth(1);
			ofSetColor(ofColor::black);
			ofDrawCircle(ofGetMouseX(), ofGetMouseY(), drawRadius + 1);

			ofSetLineWidth(1);
			ofSetColor(drawColor);
			ofDrawCircle(ofGetMouseX(), ofGetMouseY(), drawRadius);
			break;
		case 1: // draw boxes
			if(ofGetMousePressed(0)) {
				ofSetLineWidth(1);
				ofSetColor(ofColor::black);
				ofDrawRectangle(mousePressPos.x - 1, mousePressPos.y - 1,
				                ofGetMouseX() - mousePressPos.x + 3,
				                ofGetMouseY() - mousePressPos.y + 3);

				ofSetLineWidth(1);
				ofSetColor(drawColor);
				ofDrawRectangle(mousePressPos.x, mousePressPos.y,
				                ofGetMouseX() - mousePressPos.x,
				                ofGetMouseY() - mousePressPos.y);
			}
			break;
	}
  ofFill();
  for(int i = 0; i < gui.size(); i++){
    gui[i]->draw();
  }
  if(controlMode == 2){
    for(int i = 0; i < guiControls.size(); i++){
      guiControls[i]->draw();
    }
  }
  if(hasChosenExportFolder){
    recordButton->draw();
    exportPictureButton->draw();
    exportPictureButton2->draw();
  }

  if(!recording){
    backButton->draw();
  }
}

//--------------------------------------------------------------
void DrawP2PScene::keyPressed(int key) {
	switch(key) {
		case 'd':
		case 'D':
			drawMode = 1 - drawMode;
			break;

		case 'c':
			if(drawRadius > 0) {
				drawRadius--;
        brushRadius->setValue(drawRadius);
			}
			break;

		case 'v':
			drawRadius++;
      brushRadius->setValue(drawRadius);
			break;

		case 'z':
			drawColorIndex--;
			if(drawColorIndex < 0) {
				drawColorIndex += colors.size(); // wrap around
			}
			drawColor = colors[drawColorIndex];
      colourPicker->setColor(drawColor);
			break;

		case 'x':
			drawColorIndex++;
			if(drawColorIndex >= colors.size()) {
				drawColorIndex -= colors.size(); // wrap around
			}
			drawColor = colors[drawColorIndex];
      colourPicker->setColor(drawColor);

			break;

		case 'i':
		case 'I':
			if(ofGetMouseX() < drawWidth && ofGetMouseY() < drawHeight) {
        int x_ = float(nnWidth) / float(drawWidth) * ofGetMouseX();
        int y_ =  float(nnHeight) / float(drawHeight) * ofGetMouseY();
				drawColor = imgIn.getColor((x_-drawX), (y_-drawY));
        colourPicker->setColor(drawColor);
			}
			break;

    case 'p':
		case 'P':
      if(colors.size() > 1){
        // so don't delete just one colour
        colors.erase(colors.begin() + drawColorIndex);
        drawColorIndex--;
  			if(drawColorIndex < 0) {
  				drawColorIndex += colors.size(); // wrap around
  			}
  			drawColor = colors[drawColorIndex];
        colourPicker->setColor(drawColor);
      }
			break;

    case 'a':
    case 'A':
      colors.push_back(colourPicker->getColor());
      break;

    case 'r':
    case 'R':
      record();
      break;


    case 'g':
    case 'G':
      controlMode ++;
      if(controlMode >= 3){
        controlMode = 0;
      }
      break;

		case OF_KEY_DEL:
			fbo.begin();
			ofClear(255);
			fbo.end();
			break;

		case OF_KEY_RETURN:
			autoRun ^= true;
			break;
	}
}

//--------------------------------------------------------------
void DrawP2PScene::mouseDragged( int x, int y, int button) {
	switch(drawMode) {
		case 0: // draw
			fbo.begin();
      ofScale(float(nnWidth) / float(drawWidth), float(nnHeight) / float(drawHeight));
      ofTranslate(-drawX, -drawY);
			ofSetColor(drawColor);
			ofFill();
			if(drawRadius > 0) {
				ofDrawCircle(x, y, drawRadius);
				ofSetLineWidth(drawRadius * 2);
			}
			else {
				ofSetLineWidth(0.1f);
			}
			ofDrawLine(x, y, ofGetPreviousMouseX(), ofGetPreviousMouseY());

      fbo.end();
			break;
		case 1: // draw boxes
			break;
	}
}

//--------------------------------------------------------------
void DrawP2PScene::mousePressed( int x, int y, int button) {
	mousePressPos = glm::vec2(x, y);
	mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void DrawP2PScene::mouseReleased(int x, int y, int button) {
	switch(drawMode) {
		case 0: // draw
			break;
		case 1: // draw boxes
			fbo.begin();
      ofScale(float(nnWidth) / float(drawWidth), float(nnHeight) / float(drawHeight));
      ofTranslate(-drawX, -drawY);
			ofSetColor(drawColor);
			ofFill();
			ofDrawRectangle(mousePressPos.x, mousePressPos.y,
			                x - mousePressPos.x, y - mousePressPos.y);
			fbo.end();
			break;
	}
}


//--------------------------------------------------------------
void DrawP2PScene::dragEvent(ofDragInfo dragInfo) {
	if(dragInfo.files.empty()) {
		return;
	}
	std::string file_path = dragInfo.files[0];
  cout << file_path << endl;

	// only PNGs work for some reason when Tensorflow is linked in
	ofImage img;
	img.load(file_path);
	if(img.isAllocated()) {
		fbo.begin();
		ofSetColor(255);
		ofFill();
		ofDrawRectangle(0, 0, fbo.getWidth(), fbo.getHeight()); // clear
		img.draw(0, 0, fbo.getWidth(), fbo.getHeight());
		fbo.end();
	}
}


void DrawP2PScene::onButtonEvent(ofxDatGuiButtonEvent e){
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
    ofSaveImage(imgOut,exportDir+"/"+currentDate()+"_"+currentTime()+".png");
  }
  else if(e.target == exportPictureButton2){
    ofSaveImage(imgIn,exportDir+"/"+currentDate()+"_"+currentTime()+"_doodle.png");
  }
  else if(e.target == toggleControlsButton){
    controlMode ++;
    if(controlMode >= 3){
      controlMode = 0;
    }
  }
  else if(e.target == addColourButton){
    colors.push_back(colourPicker->getColor());
  }
  else if(e.target == recordButton){
    record();
  }
}

// PRIVATE

//--------------------------------------------------------------
// setup the drawing tool by folder name
void DrawP2PScene::setupDrawingTool(std::string model_dir) {

	model_dir = ofToDataPath(model_dir);

	// load test image
	ofLogVerbose() << "Loading test image";
	ofImage img;
	img.load(ofFilePath::join(model_dir, "shoe.png"));
	if(img.isAllocated()) {
		fbo.begin();
		ofSetColor(255);
		img.draw(0, 0, fbo.getWidth(), fbo.getHeight());
		fbo.end();
	}
	else {
		ofLogError() << "Test image not found";
	}

	// load color palette for drawing
	ofLogVerbose() << "Loading color palette";
	colors.clear();
	ofBuffer buf;
	buf = ofBufferFromFile(ofFilePath::join(model_dir, "/palette.txt"));
	if(buf.size() > 0) {
		for(const auto& line : buf.getLines()) {
			ofLogVerbose() << line;
			if(line.size() == 6) { // if valid hex code
				colors.push_back(ofColor::fromHex(ofHexToInt(line)));
			}
		}
		drawColorIndex = 0;
		if(colors.size() > 0) {
			drawColor = colors[0];
		}
	}
	else {
		ofLogError() << "Palette info not found";
	}

	// load default brush info
	ofLogVerbose() << "Loading default brush info";
	buf = ofBufferFromFile(ofFilePath::join(model_dir, "/default_brush.txt"));
	if(buf.size() > 0) {
		auto str_info = buf.getLines().begin().asString();
		ofLogVerbose() << str_info;
		auto str_infos = ofSplitString(str_info, " ", true, true);
		if(str_infos[0] == "draw") {
			drawMode = 0;
		}
		else if(str_infos[0] == "box") {
			drawMode = 1;
		}
		else {
			ofLogError() << "Unknown draw mode: " << str_infos[0];
		}
		drawRadius = ofToInt(str_infos[1]);
	}
	else {
		ofLogError() << "Default brush info not found";
	}
}

//--------------------------------------------------------------
// draw image or fbo etc with border and label
// typename T must have draw(x,y), isAllocated(), getWidth(), getHeight()
template <typename T>
bool DrawP2PScene::drawImage(const T& img, string label, int x, int y, int width, int height) {
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

void DrawP2PScene::setupGui(){

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(100, ofGetHeight()-50);
  backButton->onButtonEvent(this, &DrawP2PScene::onButtonEvent);

  exportPictureButton = new ofxDatGuiButton("EXPORT PICTURE");
  exportPictureButton->setPosition(drawX+drawWidth,drawY+drawHeight);
  exportPictureButton->setWidth(drawHeight);
  exportPictureButton->onButtonEvent(this, &DrawP2PScene::onButtonEvent);


  exportPictureButton2 = new ofxDatGuiButton("EXPORT PICTURE");
  exportPictureButton2->setPosition(drawX,drawY+drawHeight);
  exportPictureButton2->setWidth(drawHeight);
  exportPictureButton2->onButtonEvent(this, &DrawP2PScene::onButtonEvent);

  int controlsWidth = 400;
  int controlsX = 100;

  int buttonsX = ofGetWidth() - controlsWidth;

  recordButton = new ofxDatGuiButton("START RECORDING");
  recordButton->setPosition(buttonsX, drawY+drawHeight+exportPictureButton->getHeight()*2);
  recordButton->onButtonEvent(this, &DrawP2PScene::onButtonEvent);
  recordButton->setStripeColor(ofColor(0,255,0));

  setExportFolderButton = new ofxDatGuiButton("SET EXPORT FOLDER");
  setExportFolderButton->setPosition(buttonsX, recordButton->getY()+recordButton->getHeight());
  setExportFolderButton->onButtonEvent(this, &DrawP2PScene::onButtonEvent);

  toggleControlsButton = new ofxDatGuiButton("TOGGLE CONTROLS GUI");
  toggleControlsButton->setPosition(buttonsX, setExportFolderButton->getY()+setExportFolderButton->getHeight());
  toggleControlsButton->onButtonEvent(this, &DrawP2PScene::onButtonEvent);


  brushRadius = new ofxDatGuiSlider("BRUSH RADIUS", 1, 100, 1);
  brushRadius->setPosition(controlsX, drawY+drawHeight+exportPictureButton2->getHeight());
  brushRadius->setWidth(400, 0.4);

  addColourButton = new ofxDatGuiButton("ADD COLOUR TO PALLETTE");
  addColourButton->setPosition(controlsX,  brushRadius->getY()+brushRadius->getHeight());
  addColourButton->setWidth(400, 0.4);
  addColourButton->onButtonEvent(this, &DrawP2PScene::onButtonEvent);

  colourPicker = new ofxDatGuiColorPicker("BRUSH COLOUR", ofColor::black);
  colourPicker->setPosition(controlsX,addColourButton->getY() + addColourButton->getHeight());
  colourPicker->setWidth(400, 0.4);

  gui.clear();
  gui.push_back(setExportFolderButton);
  gui.push_back(toggleControlsButton);
  guiControls.push_back(brushRadius);
  guiControls.push_back(colourPicker);
  guiControls.push_back(addColourButton);
}

void DrawP2PScene::recordingComplete(ofxVideoRecorderOutputFileCompleteEventArgs& args){
  cout << "The recoded video file is now complete." << endl;
}

void DrawP2PScene::record(){
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

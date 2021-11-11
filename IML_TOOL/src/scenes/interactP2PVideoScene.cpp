#include "interactP2PVideoScene.h"

void InteractP2PVideoScene::refresh(){
  SceneManager::getInstance()->setShowNavBar(false);
  hasChosenExportFolder = false;
  finished = false;
  running = false;
  exportDir = "";
  videoDir = "";
  msg = "";
}

//--------------------------------------------------------------
void InteractP2PVideoScene::setup() {
  setID(SCENE_TYPE::INTERACT_P2P_VIDEO);
  statusThread.setup("/tmp/tmp.txt", &msg, &finished);
  setupGui();
}



void InteractP2PVideoScene::update() {
  if(running){
    if(finished){
      running = false;
      pythonThread.stopThread();
      statusThread.stopThread();
      msg = "";
    }
  }
  else{
    for(int i = 0; i < gui.size(); i++){
      gui[i]->update();
    }
  }
}

//--------------------------------------------------------------
void InteractP2PVideoScene::draw() {
  if(running){
    ofDrawBitmapString(msg, 300, 200);
  }
  else{
    for(int i = 0; i < gui.size(); i++){
      gui[i]->draw();
    }
      ofDrawBitmapString(msg, 300, 300);
  }
}


void InteractP2PVideoScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
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
  else if(e.target == setVideoDirButton){
    ofFileDialogResult result = ofSystemLoadDialog("select video", false);
    if (result.bSuccess) {
      videoDir = result.getPath();
    }
  }
  else if(e.target == buildButton){
    if(!hasChosenExportFolder){
      msg = "Please select an export folder!";
    }
    else if (videoDir == ""){
      msg = "Please select a video file!";
    }
    else {
      beginPythonThread();
    }
  }

}


// PRIVATE
//--------------------------------------------------------------
void InteractP2PVideoScene::beginPythonThread(){

  ModelManager * modelManager = ModelManager::getInstance();
  string modelDir = "data/saved_models/"+modelManager->getModelName()+
  "/saved_networks/ckpt/-"+ofToString(modelManager->getEpochsTrained())+"_generator";

  int width = modelManager->getImgWidth();
  int height = modelManager->getImgHeight();

  if(modelManager->getStatus() == -10){
    // custom loaded
    modelDir = modelManager->getModelName();
  }
  pythonThread.setup(modelDir, "/tmp/tmp.txt", exportDir, videoDir, modelManager->getModelName(), width, height);
  running = true;
  finished = false;
  msg = "Initialising...";
  pythonThread.startThread();
  statusThread.startThread();

}

void InteractP2PVideoScene::setupGui(){

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(100, ofGetHeight()-50);
  backButton->onButtonEvent(this, &InteractP2PVideoScene::onButtonEvent);

  int controlsWidth = 400;
  int controlsX = ofGetWidth()/2 - controlsWidth / 2;

  setVideoDirButton = new ofxDatGuiButton("OPEN VIDEO FILE");
  setVideoDirButton->setPosition(controlsX, ofGetHeight()/2 - 50);
  setVideoDirButton->onButtonEvent(this, &InteractP2PVideoScene::onButtonEvent);
  setVideoDirButton->setWidth(controlsWidth);

  setExportFolderButton = new ofxDatGuiButton("SET EXPORT FOLDER");
  setExportFolderButton->setPosition(controlsX, setVideoDirButton->getY() + setVideoDirButton->getHeight() + 10);
  setExportFolderButton->onButtonEvent(this, &InteractP2PVideoScene::onButtonEvent);
  setExportFolderButton->setWidth(controlsWidth);

  buildButton = new ofxDatGuiButton("CREATE");
  buildButton->setPosition(controlsX,  setExportFolderButton->getY() + setExportFolderButton->getHeight() + 10);
  buildButton->onButtonEvent(this, &InteractP2PVideoScene::onButtonEvent);
  buildButton->setWidth(controlsWidth);

  gui.clear();
  gui.push_back(setVideoDirButton);
  gui.push_back(setExportFolderButton);
  gui.push_back(buildButton);
  gui.push_back(backButton);

}

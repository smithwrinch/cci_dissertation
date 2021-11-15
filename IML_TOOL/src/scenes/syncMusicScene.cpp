#include "syncMusicScene.h"

void SyncMusicScene::refresh(){
  SceneManager::getInstance()->setShowNavBar(false);
  state = 0;
  showingAdvanced = false;
  running = false;
  musicDir = "";
  videoDir = "";

  errorLabel->setLabel("");
  speedFpmSlider->setValue(12);
  pulseReactSlider->setValue(0.5);
  motionReactSlider->setValue(0.5);
  saveFramesToggle->setChecked(false);
  fpsSlider->setValue(43);
  pulsePercussiveToggle->setChecked(true);
  pulseHarmonicToggle->setChecked(false);
  motionPercussiveToggle->setChecked(false);
  motionHarmonicToggle->setChecked(true);
  motionRandomnessSlider->setValue(0.5);

  ModelManager * mm = ModelManager::getInstance();
  string base = "data/saved_models/"+mm->getModelName();

  txt_dir = base+"/outputs/temp.txt";
  statusThread.setup(txt_dir, &status, &finished);

  gan_dir = base+"/saved_networks/ckpt/-"+ofToString(mm->getEpochsTrained())+"_generator";

  setupPythonThread();
}

void SyncMusicScene::setup(){
  setID(SCENE_TYPE::INTERACT_SYNC_MUSIC);
  int topY = 50;
  int bufferY = 30;
  int width = 600;

  int centreX2 = ofGetWidth()/2 - width/2;

  stopPython = new ofxDatGuiButton("KILL PROCESS");
  stopPython->setPosition(centreX2, ofGetHeight()/2 - stopPython->getHeight()/2);
  stopPython->onButtonEvent(this, &SyncMusicScene::onButtonEvent);

  speedFpmSlider = new ofxDatGuiSlider("# Images to morph: ", 0, 40, 12);
  speedFpmSlider->setPosition(centreX2, 50);
  speedFpmSlider->setPrecision(0);

  pulseReactSlider = new ofxDatGuiSlider("Pulse strength", 0, 3, 0.5);
  pulseReactSlider->setPosition(centreX2, speedFpmSlider->getY() + bufferY);

  motionReactSlider = new ofxDatGuiSlider("Motion strength", 0, 3, 0.5);
  motionReactSlider->setPosition(centreX2, pulseReactSlider->getY() + bufferY);

  saveFramesToggle = new ofxDatGuiToggle("SAVE FRAMES");
  saveFramesToggle->setPosition(centreX2, motionReactSlider->getY() + bufferY);

  fpsSlider = new ofxDatGuiSlider("FPS", 1, 60, 43);
  fpsSlider->setPosition(centreX2, saveFramesToggle->getY() + bufferY);

  showAdvancedButton = new ofxDatGuiButton("Show advanced");
  showAdvancedButton->setPosition(centreX2, fpsSlider->getY() + bufferY);
  showAdvancedButton->onButtonEvent(this, &SyncMusicScene::onButtonEvent);

  pulsePercussiveToggle = new ofxDatGuiToggle("PULSE PERCUSSIVE");
  pulsePercussiveToggle->setPosition(centreX2, showAdvancedButton->getY() + bufferY);
  pulsePercussiveToggle->setChecked(true);

  pulseHarmonicToggle = new ofxDatGuiToggle("PULSE PERCUSSIVE");
  pulseHarmonicToggle->setPosition(centreX2, pulsePercussiveToggle->getY() + bufferY);

  motionPercussiveToggle = new ofxDatGuiToggle("MOTION PERCUSSIVE");
  motionPercussiveToggle->setPosition(centreX2, pulseHarmonicToggle->getY() + bufferY);

  motionHarmonicToggle = new ofxDatGuiToggle("MOTION HARMONIC");
  motionHarmonicToggle->setPosition(centreX2, motionPercussiveToggle->getY() + bufferY);
  motionHarmonicToggle->setChecked(true);

  motionRandomnessSlider = new ofxDatGuiSlider("MOTION RANDOMNESS", 0, 1);
  motionRandomnessSlider->setPosition(centreX2, motionHarmonicToggle->getY() + bufferY);

  speedFpmSlider->setWidth(width, 0.5);
  pulseReactSlider->setWidth(width, 0.5);
  motionReactSlider->setWidth(width, 0.5);
  showAdvancedButton->setWidth(width);
  saveFramesToggle->setWidth(width, 0.5);
  fpsSlider->setWidth(width, 0.5);
  pulsePercussiveToggle->setWidth(width, 0.5);
  pulseHarmonicToggle->setWidth(width, 0.5);
  // pulseSeperateButton->setWidth(width);
  motionPercussiveToggle->setWidth(width, 0.5);
  motionHarmonicToggle->setWidth(width, 0.5);
  // motionSeperateButton->setWidth(width);
  motionRandomnessSlider->setWidth(width, 0.5);
  stopPython->setWidth(width);



  errorLabel = new ofxDatGuiLabel("");
  int centreX = ofGetWidth()/2 - errorLabel->getWidth()/2;
  errorLabel->setPosition(centreX, 445);
  errorLabel->setLabelColor(ofColor(255, 0, 0));

  openMusicButton = new ofxDatGuiButton("OPEN .MP3");
  openMusicButton->onButtonEvent(this, &SyncMusicScene::onButtonEvent);
  openMusicButton->setPosition(centreX, bufferY + errorLabel->getY());

  fnameInput = new ofxDatGuiTextInput("OUT VIDEO NAME:", "song.mp4");
  fnameInput->setPosition(centreX, bufferY + openMusicButton->getY());

  videoPathButton = new ofxDatGuiButton("SAVE VIDEO TO");
  videoPathButton->setPosition(centreX, bufferY + fnameInput->getY());
  videoPathButton->onButtonEvent(this, &SyncMusicScene::onButtonEvent);

  createButton = new ofxDatGuiButton("CREATE");
  createButton->setPosition(centreX,  bufferY + videoPathButton->getY());
  createButton->onButtonEvent(this, &SyncMusicScene::onButtonEvent);

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(90, 670);
  backButton->onButtonEvent(this, &SyncMusicScene::onButtonEvent);

  gui.push_back(errorLabel);
  gui.push_back(openMusicButton);
  gui.push_back(fnameInput);
  gui.push_back(videoPathButton);
  gui.push_back(createButton);
  gui.push_back(backButton);
  gui.push_back(speedFpmSlider);
  gui.push_back(pulseReactSlider);
  gui.push_back(motionReactSlider);
  gui.push_back(showAdvancedButton);
  gui.push_back(saveFramesToggle);
  gui.push_back(fpsSlider);
  advanced.push_back(pulsePercussiveToggle);
  advanced.push_back(pulseHarmonicToggle);
  // advanced.push_back(pulseSeperateButton);
  advanced.push_back(motionPercussiveToggle);
  advanced.push_back(motionHarmonicToggle);
  // advanced.push_back(motionSeperateButton);
  advanced.push_back(motionRandomnessSlider);

}

void SyncMusicScene::update(){
  if(!running){
    for(int i =0; i < gui.size(); i++){
      gui[i]->update();
    }
    if(showingAdvanced){
      for(int i =0; i < advanced.size(); i++){
        advanced[i]->update();
      }
    }
  }
  else{
    stopPython->update();
    if(finished){
      finished = false;
      running = false;
      pythonThread.stopThread();
      statusThread.stopThread();
    }
  }

}

void SyncMusicScene::draw(){
  if(!running){

    for(int i =0; i < gui.size(); i++){
      gui[i]->draw();
    }
    if(showingAdvanced){
      for(int i =0; i < advanced.size(); i++){
        advanced[i]->draw();
      }
    }
  }
  else{
    stopPython->draw();
    ofDrawBitmapString(status, 350, 200);
  }
}

void SyncMusicScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
  BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::INTERACT_MENU);
  scene->refresh();
  SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACT_MENU);
  }
  else if(e.target == openMusicButton){
    ofFileDialogResult result = ofSystemLoadDialog("select music", false);
    if (result.bSuccess) {
      musicDir = result.getPath();
    }
  }
  else if(e.target == videoPathButton){
    ofFileDialogResult result = ofSystemLoadDialog("select video output folder", true);
    if (result.bSuccess) {
      videoDir = result.getPath();
    }
  }
  else if(e.target == createButton){
    if(videoDir == "" || musicDir == ""){
      errorLabel->setLabel("video directory or .mp3 missing");
    }
    else if(!checkNameValid()){
      errorLabel->setLabel("VIDEO NAME MUST END IN .MP4");
    }
    else{
      cout <<  videoDir << endl;
      cout << musicDir << endl;
      cout << fnameInput->getText() << endl;
      ofFile file;
      file.open(txt_dir);
      file.remove();
      file.close();
      running = true;
      setupPythonThread();
      pythonThread.startThread();
      statusThread.startThread();
    }
  }
  else if (e.target == showAdvancedButton){
    showingAdvanced = ! showingAdvanced;
  }
  if(e.target == stopPython){
    pythonThread.stopThread();
    statusThread.stopThread();
    running = false;
  }
}

bool SyncMusicScene::checkNameValid(){
  string s = fnameInput->getText().substr(fnameInput->getText().size() - 4);
  cout << s << endl;
  return s == ".mp4" || s == ".MP4";
}

void SyncMusicScene::setupPythonThread(){

  ModelManager * mm = ModelManager::getInstance();
  string fname = videoDir + "/" + fnameInput->getText();
  string song_dir = musicDir;
  int latent_dim = mm->getLatentVector();
  int fps = fpsSlider->getValue();
  float speedFpm = speedFpmSlider->getValue();
  float pulseReact = pulseReactSlider->getValue();
  float motionReact = motionReactSlider->getValue();
  bool saveFrames = saveFramesToggle->getChecked();
  bool pulsePercussive = pulsePercussiveToggle->getChecked();
  bool pulseHarmonic = pulseHarmonicToggle->getChecked();
  bool motionPercussive = motionPercussiveToggle->getChecked();
  bool motionHarmonic = motionHarmonicToggle->getChecked();
  float motionRandomness = motionRandomnessSlider->getValue();
  pythonThread.setup(gan_dir, txt_dir, fname, song_dir, latent_dim, fps,speedFpm, pulseReact, motionReact,
  saveFrames, pulsePercussive, pulseHarmonic, motionPercussive, motionHarmonic, motionRandomness);
}

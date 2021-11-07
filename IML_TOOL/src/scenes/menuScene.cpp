#include "menuScene.h"
void MenuScene::setup(){
  setID(SCENE_TYPE::MENU);
  newModelButton = new ofxDatGuiButton("NEW MODEL");
  newModelButton->setPosition(ofGetWidth()/2 - newModelButton->getWidth()/2, ofGetHeight()/2 - newModelButton->getHeight());
  newModelButton->onButtonEvent(this, &MenuScene::onButtonEvent);

  loadModelButton = new ofxDatGuiButton("LOAD MODEL");
  loadModelButton->setPosition(newModelButton->getX(), newModelButton->getY() + newModelButton->getHeight() + 20);
  loadModelButton->onButtonEvent(this, &MenuScene::onButtonEvent);


  playButton = new ofxDatGuiButton("PLAY");
  playButton->setPosition(loadModelButton->getX(), loadModelButton->getY() + loadModelButton->getHeight() + 50);
  playButton->onButtonEvent(this, &MenuScene::onButtonEvent);

  // state 1
  modelNameInput = new ofxDatGuiTextInput("NAME MODEL:", "");
  modelNameInput->setWidth(400, 0.5);
  modelNameInput->setPosition(ofGetWidth()/2 - modelNameInput->getWidth()/2, ofGetHeight()/2 - modelNameInput->getHeight());

  createModelButton = new ofxDatGuiButton("CONTINUE");
  createModelButton->setWidth(400, 0.5);
  createModelButton->setPosition(modelNameInput->getX(), modelNameInput->getY() + modelNameInput->getHeight() + 20);
  createModelButton->onButtonEvent(this, &MenuScene::onButtonEvent);

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(200, ofGetHeight() - 100);
  backButton->onButtonEvent(this, &MenuScene::onButtonEvent);

  errorLabel = new ofxDatGuiLabel("");
  errorLabel->setWidth(400, 0.5);
  errorLabel->setPosition(modelNameInput->getX(), modelNameInput->getY() - modelNameInput->getHeight());
  errorLabel->setLabelColor(ofColor(255, 0, 0));

  //state 2
  modelScroll = new ofxDatGuiScrollView("Saved Models", 10);
  modelScroll->setWidth(300, 0.5);
  modelScroll->setPosition(ofGetWidth()/2 - modelScroll->getWidth()/2, ofGetHeight()/2 - modelScroll->getHeight()/2);

  modelScroll->onScrollViewEvent(this, &MenuScene::onScrollViewEvent);

  scrollContinueButton = new ofxDatGuiButton("CONTINUE");
  scrollContinueButton->setPosition(ofGetWidth()/2 - scrollContinueButton->getWidth()/2 + 200 , modelScroll->getY() + modelScroll->getHeight() + 50);
  scrollContinueButton->onButtonEvent(this, &MenuScene::onButtonEvent);

  scrollDeleteButton = new ofxDatGuiButton("DELETE");
  scrollDeleteButton->setPosition(ofGetWidth()/2 - scrollContinueButton->getWidth()/2 - 200, modelScroll->getY() + modelScroll->getHeight() + 50);
  scrollDeleteButton->onButtonEvent(this, &MenuScene::onButtonEvent);
  scrollDeleteButton->setStripeColor(ofColor(255,0,0));

  scrollDeleteButton->setVisible(false);
  scrollContinueButton->setVisible(false);
  populateScroll();

  // state 3
  modelTypeLabel = new ofxDatGuiLabel("I WANT TO BUILD A...");
  modelTypeLabel->setPosition(ofGetWidth()/2 - modelTypeLabel->getWidth()/2, 100);


  pix2pixButton = new ofxDatGuiButton("IMAGE TO IMAGE TRANSLATION MODEL");
  pix2pixButton->setWidth(ofGetWidth() - 100, 1);
  pix2pixButton->setPosition(ofGetWidth()/2 - pix2pixButton->getWidth()/2, ofGetHeight()/2 - pix2pixButton->getHeight()/2);
  pix2pixButton->onButtonEvent(this, &MenuScene::onButtonEvent);
  pix2pixButton->setStripeColor(ofColor(2,132,30));
  pix2pixLabel = new ofxDatGuiLabel("develop a pix2pix network on labelled data");
  pix2pixLabel->setPosition(pix2pixButton->getX(), pix2pixButton->getY() + pix2pixButton->getHeight());
  pix2pixLabel->setWidth(pix2pixButton->getWidth());
  pix2pixLabel->setLabelColor(ofColor(150, 150, 150));


  ganButton = new ofxDatGuiButton("IMAGE GENERATION MODEL");
  ganButton->setWidth(ofGetWidth() - 100, 1);
  ganButton->setPosition(ofGetWidth()/2 - ganButton->getWidth()/2, pix2pixButton->getY() - 150);
  ganButton->onButtonEvent(this, &MenuScene::onButtonEvent);
  ganButton->setStripeColor(ofColor(253,106,2));
  ganLabel = new ofxDatGuiLabel("develop a generative adversarial network on unlabelled data");
  ganLabel->setPosition(ganButton->getX(), ganButton->getY() + ganButton->getHeight());
  ganLabel->setWidth(ganButton->getWidth());
  ganLabel->setLabelColor(ofColor(150, 150, 150));

  customButton = new ofxDatGuiButton("CUSTOM MODEL");
  customButton->setWidth(ofGetWidth() - 100, 1);
  customButton->setPosition(ofGetWidth()/2 - customButton->getWidth()/2, pix2pixButton->getY() + 150 + customButton->getHeight());
  customButton->setEnabled(false);
  customButton->setStripeColor(ofColor(100,100,100));
  customLabel = new ofxDatGuiLabel("A WIP...");
  customLabel->setPosition(customButton->getX(), customButton->getY() + customButton->getHeight());
  customLabel->setWidth(customButton->getWidth());
  customLabel->setLabelColor(ofColor(150, 150, 150));

}

void MenuScene::update(){
  //TODO should be switch statement
  if(state == 0){
    newModelButton->update();
    loadModelButton->update();
    playButton->update();
  }
  if(state == 1){
    createModelButton->update();
    modelNameInput->update();
    backButton->update();
    errorLabel->update();
  }

  if(state == 2){
    modelScroll->update();
    backButton->update();
    scrollContinueButton->update();
    scrollDeleteButton->update();
  }

  if(state == 3){
    ganButton->update();
    ganLabel->update();
    customButton->update();
    customLabel->update();
    pix2pixButton->update();
    pix2pixLabel->update();
    backButton->update();
    modelTypeLabel->update();
  }

}

void MenuScene::draw(){
  //TODO should be switch statement
  if(state == 0){
    newModelButton->draw();
    loadModelButton->draw();
    playButton->draw();
  }
  if(state == 1){
    createModelButton->draw();
    modelNameInput->draw();
    backButton->draw();
    errorLabel->draw();
  }

  if(state == 2){
    modelScroll->draw();
    backButton->draw();
    scrollContinueButton->draw();
    scrollDeleteButton->draw();
  }

  if(state == 3){
    ganButton->draw();
    ganLabel->draw();
    customButton->draw();
    customLabel->draw();
    pix2pixButton->draw();
    pix2pixLabel->draw();
    backButton->draw();
    modelTypeLabel->draw();
  }

}

void MenuScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if (e.target == newModelButton){
        // newModelButton->setLabel("NEW MODEL....");
        state = 1;
    }
  if (e.target == loadModelButton){
        // loadModelButton->setLabel("LOADING MODEL...");
        populateScroll();
        state = 2;
        refreshScroll();
    }
  if (e.target == createModelButton){
        // createModelButton->setLabel("SAVING MODEL...");
        if(checkTextValid() && checkFnameNew()){
          cout << "yeah" << endl;
          state = 3;
        }
    }

  if (e.target == backButton){
      ModelManager::getInstance()->reset();
      scrollDeleteButton->setVisible(false);
      scrollContinueButton->setVisible(false);
      state = 0;
    }
  if(e.target == ganButton){
    createModel(MODEL_TYPE::GAN);
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::ARCHITECTURE_MENU);
    state = 0;

  }
  if(e.target == pix2pixButton){
    createModel(MODEL_TYPE::PIX2PIX); // TODO this is causing error
    state = 0;
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::ARCHITECTURE_MENU);
  }

  if(e.target == scrollContinueButton){
    cout << "WE WILL NOW LOAD TO " << currentScroll << endl;
    state = 0;
    ModelManager::getInstance()->load(currentScroll);
    int status = ModelManager::getInstance()->getStatus();
    if(status <= 1){
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::ARCHITECTURE_MENU);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::ARCHITECTURE_MENU);
    }
    else if (status <= 2){
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_MENU);

    }

    else if (status <= 3){
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::TRAIN);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::TRAIN);
    }

  }
  if(e.target == scrollDeleteButton){
    cout << "WE WILL NOW DELETE " << currentScroll << endl;
    deleteModel(currentScroll);
    populateScroll();
    refreshScroll();
    currentScroll = "";
    scrollDeleteButton->setVisible(false);
    scrollContinueButton->setVisible(false);
  }
  if(e.target == playButton){

    sceneManager->changeSceneTo(SCENE_TYPE::PLAY_MODEL_SELECT);

    cout << sceneManager->getNumScenesAdded() << endl;
    cout << "PLAYING" << endl;
  }
}

void MenuScene::onScrollViewEvent(ofxDatGuiScrollViewEvent e)
{
    ofxDatGuiScrollView* parent = e.parent; // a pointer to the scrollview that dispatched the event //
    ofxDatGuiButton* button = e.target; // a pointer to the button that was selected //

    if(currentScroll == button->getLabel()){
        currentScroll = "";
        scrollDeleteButton->setVisible(false);
        scrollContinueButton->setVisible(false);
        refreshScroll();
    }
    else{


    scrollDeleteButton->setVisible(true);
    scrollContinueButton->setVisible(true);
    refreshScroll();
    button->setBorder(ofColor(255, 255, 255), 1);
    button->setStripeColor(ofColor(255,255,255));
    currentScroll = button->getLabel();

  }
}


bool MenuScene::checkTextValid(){
  string name = modelNameInput->getText();
  if(name.length() == 0){
    errorLabel->setLabel("PLEASE ENTER A NAME");
    return false;
  }
  // for (string::iterator it=name.begin(); it!=name.end(); ++it)
  // {
  //
  //   if ((*it) < 0x61 || (*it) > 0x71){
  //     // string contains characters other than a-z
  //     errorLabel->setLabel("ONLY CHARACTERS A-Z PERMITTED");
  //     return false;
  //   }
  // }
  for(int i = 0; i < name.length(); i++){
        if( !isalpha(name[i]) && !isdigit(name[i])){
            errorLabel->setLabel("ONLY CHARACTERS A-Z [0-9] PERMITTED");
            return false;
          }
      }
  return true;
}

bool MenuScene::checkFnameNew(){
  ofDirectory dir = ofDirectory("saved_models/"+modelNameInput->getText());
  if(dir.exists()){
    errorLabel->setLabel("FILE NAME ALREADY EXISTS!");
    return false;
  }
  return true;
}

void MenuScene::createModel(MODEL_TYPE::ID type){
  string fname = modelNameInput->getText();
  ofDirectory dir = ofDirectory("saved_models/"+fname);
  dir.create(true);

  ofDirectory dir2 = ofDirectory("saved_models/"+fname+"/images");
  ofDirectory dir3 = ofDirectory("saved_models/"+fname+"/outputs");
  ofDirectory dir4 = ofDirectory("saved_models/"+fname+"/checkpoints");
  ofDirectory dir5 = ofDirectory("saved_models/"+fname+"/python");
  ofDirectory dir6 = ofDirectory("saved_models/"+fname+"/saved_networks");
  dir2.create(true);
  dir3.create(true);
  dir4.create(true);
  dir5.create(true);
  dir6.create(true);

  ModelManager::getInstance()->setModelName(fname);
  ModelManager::getInstance()->setModelType(type);

  ModelManager::getInstance()->save();
}


void MenuScene::populateScroll(){
  ofDirectory dir = ofDirectory("saved_models");
  int size = dir.listDir();
  dir.sortByDate();
  modelScroll->clear();
  for (int i =size-1; i >= 0; i--){

    if(dir.getFile(i).isDirectory()==1){

        // eventually check config file is valid
        // and maybe show information
        modelScroll->add(dir.getFile(i).getBaseName());
        modelScroll->update();

        // ofDirectory d = dir.getFile(i);
        //
        // for (int x = 0; x < s; x++){
        //     if(d.getFile(i).isFile()==1){
        //       ofFilefile file = d.getFile(i);
        //
        //       cout << file.getBaseName() << endl;
        //
        //     }
        // }
    }
  }
}

void MenuScene::refreshScroll(){
  currentScroll = "";

  ModelManager * mm = ModelManager::getInstance();
  for (int i =0; i < modelScroll->getNumItems(); i++){
    ofxDatGuiScrollViewItem * button = modelScroll->getItemAtIndex(i);
    string label = button->getLabel();

    // ofDirectory dir = ofDirectory(label);
    mm->load(label);
    int type = mm->getModelType();
    if(type == 0){
      button->setStripeColor(ofColor(253,106,2));
    }
    else if(type == 1){
        button->setStripeColor(ofColor(2,132,30));
    }
    else{
      button->setStripeColor(ofColor(100,100,100));
    }
    button->setBorder(ofColor(200, 200, 200), 0);
  }
  mm->reset();
}

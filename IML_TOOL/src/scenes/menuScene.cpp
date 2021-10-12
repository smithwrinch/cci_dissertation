#include "menuScene.h"
void MenuScene::setup(){
  setID(SCENE_TYPE::MENU);
  newModelButton = new ofxDatGuiButton("NEW MODEL");
  newModelButton->setPosition(ofGetWidth()/2 - newModelButton->getWidth()/2, ofGetHeight()/2 - newModelButton->getHeight());
  newModelButton->onButtonEvent(this, &MenuScene::onButtonEvent);

  loadModelButton = new ofxDatGuiButton("LOAD MODEL");
  loadModelButton->setPosition(newModelButton->getX(), newModelButton->getY() + newModelButton->getHeight() + 20);
  loadModelButton->onButtonEvent(this, &MenuScene::onButtonEvent);

  // state 1
  modelNameInput = new ofxDatGuiTextInput("NAME MODEL:", "");
  modelNameInput->setPosition(ofGetWidth()/2 - modelNameInput->getWidth()/2, ofGetHeight()/2 - modelNameInput->getHeight());
  modelNameInput->setWidth(400, 0.5);
  createModelButton = new ofxDatGuiButton("CONTINUE");
  createModelButton->setPosition(modelNameInput->getX(), modelNameInput->getY() + modelNameInput->getHeight() + 20);
  createModelButton->onButtonEvent(this, &MenuScene::onButtonEvent);


  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(modelNameInput->getX(), ofGetHeight() - 100);
  backButton->onButtonEvent(this, &MenuScene::onButtonEvent);

  errorLabel = new ofxDatGuiLabel("");
  errorLabel->setPosition(modelNameInput->getX(), modelNameInput->getY() - modelNameInput->getHeight());

  errorLabel->setLabelColor(ofColor(255, 0, 0));

  //state 2
  modelScroll = new ofxDatGuiScrollView("Saved Models", 5);


  // state 3
  modelTypeLabel = new ofxDatGuiLabel("I WANT TO BUILD A...");
  modelTypeLabel->setPosition(ofGetWidth()/2 - modelTypeLabel->getWidth()/2, 100);


  pix2pixButton = new ofxDatGuiButton("IMAGE TO IMAGE TRANSLATION MODEL");
  pix2pixButton->setWidth(ofGetWidth() - 100, 1);
  pix2pixButton->setPosition(ofGetWidth()/2 - pix2pixButton->getWidth()/2, ofGetHeight()/2 - pix2pixButton->getHeight()/2);
  pix2pixButton->onButtonEvent(this, &MenuScene::onButtonEvent);
  pix2pixLabel = new ofxDatGuiLabel("develop a pix2pix network on labelled data");
  pix2pixLabel->setPosition(pix2pixButton->getX(), pix2pixButton->getY() + pix2pixButton->getHeight());
  pix2pixLabel->setWidth(pix2pixButton->getWidth());
  pix2pixLabel->setLabelColor(ofColor(150, 150, 150));


  ganButton = new ofxDatGuiButton("IMAGE GENERATION MODEL");
  ganButton->setWidth(ofGetWidth() - 100, 1);
  ganButton->setPosition(ofGetWidth()/2 - ganButton->getWidth()/2, pix2pixButton->getY() - 150);
  ganButton->onButtonEvent(this, &MenuScene::onButtonEvent);
  ganLabel = new ofxDatGuiLabel("develop a generative adversarial network on unlabelled data");
  ganLabel->setPosition(ganButton->getX(), ganButton->getY() + ganButton->getHeight());
  ganLabel->setWidth(ganButton->getWidth());
  ganLabel->setLabelColor(ofColor(150, 150, 150));

  customButton = new ofxDatGuiButton("CUSTOM MODEL");
  customButton->setWidth(ofGetWidth() - 100, 1);
  customButton->setPosition(ofGetWidth()/2 - customButton->getWidth()/2, pix2pixButton->getY() + 150 + customButton->getHeight());
  customButton->setEnabled(false);
  customLabel = new ofxDatGuiLabel("A WIP...");
  customLabel->setPosition(customButton->getX(), customButton->getY() + customButton->getHeight());
  customLabel->setWidth(customButton->getWidth());
  customLabel->setLabelColor(ofColor(150, 150, 150));

  populateScroll();
}

void MenuScene::update(){
  //TODO should be switch statement
  if(state == 0){
    newModelButton->update();
    loadModelButton->update();
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
    }
  if (e.target == createModelButton){
        // createModelButton->setLabel("SAVING MODEL...");
        if(checkTextValid() && checkFnameNew()){
          cout << "yeah" << endl;
          state = 3;
        }
    }

  if (e.target == backButton){
      state = 0;
    }
  if(e.target == ganButton){
    createModel(MODEL_TYPE::GAN);
    state = 0;
  }
  if(e.target == pix2pixButton){
    createModel(MODEL_TYPE::PIX2PIX);
    state = 0;
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
  ofDirectory dir4 = ofDirectory("saved_models/"+fname+"/saved_networks");
  ofDirectory dir5 = ofDirectory("saved_models/"+fname+"/python");
  dir2.create(true);
  dir3.create(true);
  dir4.create(true);
  dir5.create(true);

  ModelManager::getInstance().setModelName(fname);
  ModelManager::getInstance().setModelType(type);

  ModelManager::getInstance().save();
}

void MenuScene::populateScroll(){
  ofDirectory dir = ofDirectory("saved_models");
  dir.sortByDate();
  int size = dir.listDir();
  modelScroll->clear();
  for (int i =0; i < size; i++){

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

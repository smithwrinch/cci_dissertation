#include "interactCustomScene.h"

void InteractCustomScene::setup(){
  setID(SCENE_TYPE::INTERACT_CUSTOM);
  modelTypeLabel = new ofxDatGuiLabel("SELECT MODEL TYPE");
  modelTypeLabel->setPosition(ofGetWidth()/2 - modelTypeLabel->getWidth()/2, 100);


  pix2pixButton = new ofxDatGuiButton("IMAGE TO IMAGE TRANSLATION MODEL");
  pix2pixButton->setWidth(ofGetWidth() - 100, 1);
  pix2pixButton->setPosition(ofGetWidth()/2 - pix2pixButton->getWidth()/2, ofGetHeight()/2 - pix2pixButton->getHeight()/2);
  pix2pixButton->onButtonEvent(this, &InteractCustomScene::onButtonEvent);
  pix2pixButton->setStripeColor(ofColor(2,132,30));
  pix2pixLabel = new ofxDatGuiLabel("typically a pix2pix or autoencoder network");
  pix2pixLabel->setPosition(pix2pixButton->getX(), pix2pixButton->getY() + pix2pixButton->getHeight());
  pix2pixLabel->setWidth(pix2pixButton->getWidth());
  pix2pixLabel->setLabelColor(ofColor(150, 150, 150));


  ganButton = new ofxDatGuiButton("IMAGE GENERATION MODEL");
  ganButton->setWidth(ofGetWidth() - 100, 1);
  ganButton->setPosition(ofGetWidth()/2 - ganButton->getWidth()/2, pix2pixButton->getY() - 150);
  ganButton->onButtonEvent(this, &InteractCustomScene::onButtonEvent);
  ganButton->setStripeColor(ofColor(253,106,2));
  ganLabel = new ofxDatGuiLabel("typically a generative adversarial network");
  ganLabel->setPosition(ganButton->getX(), ganButton->getY() + ganButton->getHeight());
  ganLabel->setWidth(ganButton->getWidth());
  ganLabel->setLabelColor(ofColor(150, 150, 150));

  int width = 400;

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(50, ofGetHeight()-50);
  backButton->onButtonEvent(this, &InteractCustomScene::onButtonEvent);
  continueButton = new ofxDatGuiButton("CONTINUE");
  continueButton->setPosition(ofGetWidth() - 50 - continueButton->getWidth(), ofGetHeight() - 50);
  continueButton->onButtonEvent(this, &InteractCustomScene::onButtonEvent);
  setFolderButton = new ofxDatGuiButton("SELECT MODEL");
  setFolderButton->setPosition(ofGetWidth()/2 - width/2, 325);
  setFolderButton->setWidth(width);
  setFolderButton->onButtonEvent(this, &InteractCustomScene::onButtonEvent);

  grayscaleToggle = new ofxDatGuiToggle("IMAGE GRAYSCALE");
  grayscaleToggle->setPosition(ofGetWidth()/2 - width/2, 275);
  grayscaleToggle->setWidth(width);
  //gan
  imgSizeSlider = new ofxDatGuiSlider("OUTPUT IMAGE SIZE", 28, 2048);
  latentDimSlider = new ofxDatGuiSlider("LATENT DIMENSION", 1, 2048);

  imgSizeSlider->setPosition(ofGetWidth()/2 - width/2, 250);
  imgSizeSlider->setWidth(width, 0.5);
  latentDimSlider->setPosition(ofGetWidth()/2 - width/2, 225);
  latentDimSlider->setWidth(width, 0.5);
  //pix2pix
  //TODO DO FOR DIFFERENT INPUT/OUTPUT SIZES
  imgWidthSlider = new ofxDatGuiSlider("INPUT IMAGE WIDTH", 28, 2048);
  imgHeightSlider = new ofxDatGuiSlider("INPUT IMAGE HEIGHT", 28, 2048);

  imgWidthSlider->setPosition(ofGetWidth()/2 - width/2, 225);
  imgWidthSlider->setWidth(width, 0.5);
  imgHeightSlider->setPosition(ofGetWidth()/2 - width/2, 250);
  imgHeightSlider->setWidth(width, 0.5);
}

void InteractCustomScene::update(){
  backButton->update();
  continueButton->update();
  if(state == 0){
    pix2pixButton->update();
    pix2pixLabel->update();
    ganButton->update();
    ganLabel->update();
  }
  else{
    grayscaleToggle->update();
    continueButton->update();
    setFolderButton->update();
    if(state == 1){
      imgSizeSlider->update();
      latentDimSlider->update();
    }
    else if(state == 2){
      imgWidthSlider->update();
      imgHeightSlider->update();
    }
  }
}

void InteractCustomScene::draw(){
  backButton->draw();
  continueButton->draw();
  if(state == 0){
    pix2pixButton->draw();
    pix2pixLabel->draw();
    ganButton->draw();
    ganLabel->draw();
  }
  else{
    ofDrawBitmapString(msg, ofGetWidth()/2 - 50, ofGetHeight() - 100);
    setFolderButton->draw();
    grayscaleToggle->draw();
    continueButton->draw();
    if(state == 1){
      imgSizeSlider->draw();
      latentDimSlider->draw();
    }
    else if(state == 2){
      imgWidthSlider->draw();
      imgHeightSlider->draw();
    }
  }
}

void InteractCustomScene::toInteract(){
  ModelManager * model = ModelManager::getInstance();
  model->load("temp.xml");
  if(state == 1){
    model->setImgWidth(imgSizeSlider->getValue());
    model->setImgHeight(imgSizeSlider->getValue());
    model->setLatentVector(latentDimSlider->getValue());
    model->setModelType(MODEL_TYPE::GAN);
  }
  else if(state == 2){
    model->setImgWidth(imgWidthSlider->getValue());
    model->setImgHeight(imgHeightSlider->getValue());
    model->setModelType(MODEL_TYPE::PIX2PIX);
  }
  model->setStatus(-10);
  model->setModelName(dir);
  model->save();
}

void InteractCustomScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::MENU);
  }
  else if (e.target == ganButton){
    state = 1;
  }
  else if(e.target == pix2pixButton){
    state = 2;
  }
  else if (e.target == continueButton){
    if(dir == ""){
      msg = "Please select a model first...";
    }
    else{
      toInteract();
      state = 0;
      msg = "";
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::INTERACT_MENU);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACT_MENU);
    }
  }
  else if (e.target == setFolderButton){
    ofFileDialogResult result = ofSystemLoadDialog("select model folder", true);
    if (result.bSuccess) {
      dir = result.getPath();
    }
  }
}

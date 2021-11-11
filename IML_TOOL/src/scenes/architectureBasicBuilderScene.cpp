#include "architectureBasicBuilderScene.h"

void ArchitectureBasicBuilderScene::refresh(){
  ModelManager * model = ModelManager::getInstance();
  showingAdvanced = false;

  cout << "MODEL: " << model->getModelName() << endl;

  // imgWidthSlider->setValue(model->getImgWidth());
  // imgSizeSlider->setValue(model->getImgWidth());
  // imgHeightSlider->setValue(model->getImgHeight());
  setImageSizeChecked();


  learningRateSlider->setValue(model->getLearningRateX());
  learningRateSlider2->setValue(-(model->getLearningRateY()));
  maxEpochsSlider->setValue(model->getMaxEpochs());
  batchSizeSlider->setValue(model->getBatchSize());
  numLayersSlider->setValue(model->getNumLayers());
  latentDimSlider->setValue(model->getLatentVector());
  kernelSizeSlider->setValue(model->getKernelSize());
  betaSlider->setValue(model->getBeta());
  lambdaSlider->setValue(model->getLambda());

  discriminatorNoiseSlider->setValue(ofToFloat(model->getDiscriminatorNoise()));
  randomHorizontalToggle->setChecked(model->getRandomHorizontal());
  randomVerticalToggle->setChecked(model->getRandomVertical());
  cropSlider->setValue(model->getRandomCrop());
  brightnessSlider->setValue(ofToFloat(model->getRandomBrightness()));
  contrastSlider->setValue(ofToFloat(model->getRandomContrast()));

  cout << "LEARNING RATE X " << model->getLearningRateX() << endl;
  cout << "LEARNING RATE Y " << model->getLearningRateY() << endl;
}

void ArchitectureBasicBuilderScene::setup(){

  setID(SCENE_TYPE::ARCHITECTURE_BUILDER);

  int width = 600;
  float label_width = 0.5;
  int startY = 125;
  int btnBuffer = 200;

  ModelManager * model = ModelManager::getInstance();

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(btnBuffer, ofGetHeight() - 100);
  backButton->onButtonEvent(this, &ArchitectureBasicBuilderScene::onButtonEvent);

  continueButton = new ofxDatGuiButton("CONTINUE");
  continueButton->setPosition(ofGetWidth() - backButton->getWidth() - btnBuffer, ofGetHeight() - 100);
  continueButton->onButtonEvent(this, &ArchitectureBasicBuilderScene::onButtonEvent);
  //
  // imgWidthSlider = new ofxDatGuiSlider("IMAGE WIDTH: ", 32, 1024, model->getImgWidth());
  int centreX = ofGetWidth()/2 - width / 2;
  // imgWidthSlider->setPosition(centreX, startY);
  // imgWidthSlider->setPrecision(0);
  //
  //
  // imgSizeSlider = new ofxDatGuiSlider("IMAGE SIZE: ", 28, 64, model->getImgWidth());
  // imgSizeSlider->setPosition(centreX, startY);
  // imgSizeSlider->setPrecision(0);
  //
  //
  // imgHeightSlider = new ofxDatGuiSlider("IMAGE HEIGHT: ", 32, 1024, model->getImgHeight());
  // imgHeightSlider->setPosition(centreX, imgWidthSlider->getY() + imgWidthSlider->getHeight());
  // imgHeightSlider->setPrecision(0);


  button28 = new ofxDatGuiToggle("28x28");
  button28->setWidth(width /2 , label_width);
  button28->setPosition(ofGetWidth() /2  - width / 2, startY);
  button28->setChecked(true);
  button28->onToggleEvent(this, &ArchitectureBasicBuilderScene::onToggleEvent);

  button64G = new ofxDatGuiToggle("64x64");
  button64G->setWidth(width /2 , label_width);
  button64G->setPosition(ofGetWidth() /2  + width/2 - width / 2, startY);
  button64G->onToggleEvent(this, &ArchitectureBasicBuilderScene::onToggleEvent);

  ganButtons.push_back(button28);
  ganButtons.push_back(button64G);


  button32 = new ofxDatGuiToggle("32x32");
  button32->setWidth(100);
  button32->setPosition(ofGetWidth() / 2 - width / 2, startY);
  button32->onToggleEvent(this, &ArchitectureBasicBuilderScene::onToggleEvent);

  button64 = new ofxDatGuiToggle("64x64");
  button64->setChecked(true);
  button64->setWidth(100);
  button64->setPosition(ofGetWidth() / 2 + width / 6 - width / 2, startY);
  button64->onToggleEvent(this, &ArchitectureBasicBuilderScene::onToggleEvent);

  button128 = new ofxDatGuiToggle("128x128");
  button128->setWidth(100);
  button128->setPosition(ofGetWidth() / 2 + 2*width/6 - width / 2, startY);
  button128->onToggleEvent(this, &ArchitectureBasicBuilderScene::onToggleEvent);

  button256 = new ofxDatGuiToggle("256x256");
  button256->setWidth(100);
  button256->setPosition(ofGetWidth() / 2 + 3*width/6 - width / 2, startY);
  button256->onToggleEvent(this, &ArchitectureBasicBuilderScene::onToggleEvent);

  button512 = new ofxDatGuiToggle("512x512");
  button512->setWidth(100);
  button512->setPosition(ofGetWidth() / 2 + 4*width/6 - width / 2, startY);
  button512->onToggleEvent(this, &ArchitectureBasicBuilderScene::onToggleEvent);

  button1024 = new ofxDatGuiToggle("1024x1024");
  button1024->setWidth(100);
  button1024->setPosition(ofGetWidth() / 2 + 5*width/6 - width / 2, startY);
  button1024->onToggleEvent(this, &ArchitectureBasicBuilderScene::onToggleEvent);

  pix2pixButtons.push_back(button32);
  pix2pixButtons.push_back(button64);
  pix2pixButtons.push_back(button128);
  pix2pixButtons.push_back(button256);
  pix2pixButtons.push_back(button512);
  pix2pixButtons.push_back(button1024);


  inputRGBToggle = new ofxDatGuiToggle("IMAGE GRAYSCALE");
  inputRGBToggle->setPosition(centreX, button32->getY() + button32->getHeight());


  latentDimSlider = new ofxDatGuiSlider("LATENT DIM SIZE: ", 28, 1024, model->getLatentVector());
  latentDimSlider->setPosition(centreX, inputRGBToggle->getY() + inputRGBToggle->getHeight());
  latentDimSlider->setPrecision(0);

  //
  // outputRGBToggle = new ofxDatGuiToggle("OUTPUT IMAGE GRAYSCALE");
  // outputRGBToggle->setPosition(centreX, inputRGBToggle->getY() + inputRGBToggle->getHeight());

  // learningRateInput = new ofxDatGuiSlider("LEARNING RATE", 0.0000001, 0.00002, 0.001)
  learningRateSlider = new ofxDatGuiSlider("LEARNING RATE (Xe^-Y) X:", 1, 9, model->getLearningRateX());
  learningRateSlider->setPosition(centreX, latentDimSlider->getY() + latentDimSlider->getHeight()*2);
  learningRateSlider->setPrecision(0);

  learningRateSlider2 = new ofxDatGuiSlider("LEARNING RATE (Xe^-Y) Y:", 1, 9, -(model->getLearningRateY()));
  learningRateSlider2->setPosition(centreX, learningRateSlider->getY() + learningRateSlider->getHeight());
  learningRateSlider2->setPrecision(0);

  maxEpochsSlider = new ofxDatGuiSlider("MAX EPOCHS", 1, 50000, model->getMaxEpochs());
  maxEpochsSlider->setPosition(centreX, learningRateSlider2->getY() + learningRateSlider2->getHeight());
  maxEpochsSlider->setPrecision(0);

  batchSizeSlider = new ofxDatGuiSlider("BATCH SIZE", 1, 256, model->getBatchSize());
  batchSizeSlider->setPosition(centreX, maxEpochsSlider->getY() + maxEpochsSlider->getHeight());
  batchSizeSlider->setPrecision(0);

  showAdvanced = new ofxDatGuiButton("SHOW ADVANCED");
  showAdvanced->setPosition(centreX, batchSizeSlider->getY() + batchSizeSlider->getHeight());
  showAdvanced->onButtonEvent(this, &ArchitectureBasicBuilderScene::onButtonEvent);

  numLayersSlider = new ofxDatGuiSlider("# LAYERS", 4, 9, model->getNumLayers());
  numLayersSlider->setPosition(centreX, showAdvanced->getY() + batchSizeSlider->getHeight());
  numLayersSlider->setPrecision(0);


  kernelSizeSlider = new ofxDatGuiSlider("KERNEL SIZE", 1, 9, model->getKernelSize());
  kernelSizeSlider->setPosition(centreX, numLayersSlider->getY() + numLayersSlider->getHeight()*2);
  kernelSizeSlider->setPrecision(0);

  betaSlider = new ofxDatGuiSlider("BETA (*10^-2)", 0, 100, model->getBeta());
  betaSlider->setPosition(centreX, numLayersSlider->getY()+numLayersSlider->getHeight());
  betaSlider->setPrecision(0);

  lambdaSlider = new ofxDatGuiSlider("LAMBDA", 0, model->getLambda());
  lambdaSlider->setPosition(centreX, betaSlider->getY()+betaSlider->getHeight());
  lambdaSlider->setPrecision(0);

  discriminatorNoiseSlider = new ofxDatGuiSlider("Discriminator Noise", 0, 1, 0);
  discriminatorNoiseSlider->setPosition(centreX, lambdaSlider->getY()+lambdaSlider->getHeight());

  randomHorizontalToggle = new ofxDatGuiToggle("RANDOM HORIZONTAL FLIP");
  randomHorizontalToggle->setPosition(centreX, discriminatorNoiseSlider->getY()+discriminatorNoiseSlider->getHeight());

  randomVerticalToggle = new ofxDatGuiToggle("RANDOM VERTICAL FLIP");
  randomVerticalToggle->setPosition(centreX, randomHorizontalToggle->getY()+randomHorizontalToggle->getHeight());

  cropSlider = new ofxDatGuiSlider("RANDOM CROP AMOUNT (%)", 0, 50, 0);
  cropSlider->setPosition(centreX, randomVerticalToggle->getY()+randomVerticalToggle->getHeight());
  cropSlider->setPrecision(0);

  brightnessSlider = new ofxDatGuiSlider("RANDOM BRIGHTNESS DELTA", -0.3, 0.3, 0);
  brightnessSlider->setPosition(centreX, cropSlider->getY()+cropSlider->getHeight());
  // brightnessSlider->setPrecision(0);

  contrastSlider = new ofxDatGuiSlider("RANDOM CONTRAST FACTOR", 0, 2, 0);
  contrastSlider->setPosition(centreX, brightnessSlider->getY()+brightnessSlider->getHeight());

  // imgWidthSlider->setWidth(width, label_width);
  // imgHeightSlider->setWidth(width, label_width);
  inputRGBToggle->setWidth(width, label_width);
  // outputRGBToggle->setWidth(width, label_width);
  learningRateSlider->setWidth(width, label_width);
  learningRateSlider2->setWidth(width, label_width);
  maxEpochsSlider->setWidth(width, label_width);
  batchSizeSlider->setWidth(width, label_width);
  numLayersSlider->setWidth(width, label_width);
  showAdvanced->setWidth(width, label_width);
  kernelSizeSlider->setWidth(width, label_width);
  betaSlider->setWidth(width, label_width);
  lambdaSlider->setWidth(width, label_width);
  //
  discriminatorNoiseSlider->setWidth(width, label_width);
  randomHorizontalToggle->setWidth(width);
  randomVerticalToggle->setWidth(width);
  cropSlider->setWidth(width, label_width);
  brightnessSlider->setWidth(width, label_width);
  contrastSlider->setWidth(width, label_width);

  // imgSizeSlider->setWidth(width, label_width);
  latentDimSlider->setWidth(width, label_width);
}

void ArchitectureBasicBuilderScene::update(){
  inputRGBToggle->update();
  learningRateSlider->update();
  learningRateSlider2->update();
  maxEpochsSlider->update();
  batchSizeSlider->update();
  showAdvanced->update();
  if(showingAdvanced){

      discriminatorNoiseSlider->update();
      randomHorizontalToggle->update();
      randomVerticalToggle->update();
      cropSlider->update();
      brightnessSlider->update();
      contrastSlider->update();
  }
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    // outputRGBToggle->update();
    // imgWidthSlider->update();
    // imgHeightSlider->update();
    for(int i = 0; i < pix2pixButtons.size(); i++){
      pix2pixButtons[i]->update();
    }
    // outputRGBToggle->update();
    if(showingAdvanced){
      lambdaSlider->update();
      numLayersSlider->update();
      betaSlider->update();
    }
  }
  else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
    // imgSizeSlider->update();
    latentDimSlider->update();

    for(int i = 0; i < ganButtons.size(); i++){
      ganButtons[i]->update();
    }
  }
  backButton->update();
  continueButton->update();
}

void ArchitectureBasicBuilderScene::draw(){
  inputRGBToggle->draw();
  learningRateSlider->draw();
  learningRateSlider2->draw();
  maxEpochsSlider->draw();
  batchSizeSlider->draw();
  showAdvanced->draw();

  if(showingAdvanced){
      discriminatorNoiseSlider->draw();
      randomHorizontalToggle->draw();
      randomVerticalToggle->draw();
      cropSlider->draw();
      brightnessSlider->draw();
      contrastSlider->draw();
  }

  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){

    if(showingAdvanced){
      betaSlider->draw();
      lambdaSlider->draw();
      numLayersSlider->draw();
    }
    for(int i = 0; i < pix2pixButtons.size(); i++){
      pix2pixButtons[i]->draw();
    }
  }
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
    // imgSizeSlider->draw();
    latentDimSlider->draw();

    for(int i = 0; i < ganButtons.size(); i++){
      ganButtons[i]->draw();
    }
  }

  backButton->draw();
  continueButton->draw();
}


void ArchitectureBasicBuilderScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::ARCHITECTURE_MENU);
  }

  else if(e.target == continueButton){
    // TODO change to dataset
    setModel();
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_MENU);
  }
  else if(e.target == showAdvanced){
    showingAdvanced = ! showingAdvanced;
    if(showingAdvanced){
      showAdvanced->setLabel("Hide Advanced");
    }
    else{
      showAdvanced->setLabel("Show Advanced");

    }
  }
}

void ArchitectureBasicBuilderScene::onToggleEvent(ofxDatGuiToggleEvent e){
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    for(int i = 0; i < pix2pixButtons.size(); i++){
      pix2pixButtons[i]->setChecked(false);
    }
    e.target->setChecked(true);
    for(int i = 0; i < pix2pixButtons.size(); i++){
      if(pix2pixButtons[i]->getChecked()){
        numLayersSlider->setValue(i+4);
      }
    }
  }
  else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
    for(int i = 0; i < ganButtons.size(); i++){
      ganButtons[i]->setChecked(false);
    }
    e.target->setChecked(true);
  }
}

// assigns values to models config
void ArchitectureBasicBuilderScene::setModel(){
  ModelManager * model = model->getInstance();

  model->setStatus(1);
  model->setLearningRateX(learningRateSlider->getValue());
  model->setLearningRateY(-learningRateSlider2->getValue());
  model->setMaxEpochs(maxEpochsSlider->getValue());
  model->setBatchSize(batchSizeSlider->getValue());
  model->setKernelSize(kernelSizeSlider->getValue());

  model->setDiscriminatorNoise(discriminatorNoiseSlider->getValue());
  model->setRandomHorizontal(randomHorizontalToggle->getChecked());
  model->setRandomVertical(randomVerticalToggle->getChecked());
  model->setRandomCrop(cropSlider->getValue());
  model->setRandomBrightness(brightnessSlider->getValue());
  model->setRandomContrast(contrastSlider->getValue());
  // discriminatorNoiseSlider->update();
  // randomHorizontalToggle->update();
  // randomVerticalToggle->update();
  // cropSlider->update();
  // brightnessSlider->update();
  // contrastSlider->update();
  int channels = 3;
  if(inputRGBToggle->getChecked()){
    channels = 1;
  }

  model->setInputChannel(channels);

  model->setImgWidth(getImageSize());
  model->setImgHeight(getImageSize());
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    // int channels_ = 3;
    // if(outputRGBToggle->getChecked()){
    //   channels_ = 1;
    // }
    // model->setOutputChannel(channels_);

    model->setNumLayers(numLayersSlider->getValue());
    model->setBeta(int(betaSlider->getValue()));
    model->setLambda(lambdaSlider->getValue());
  }
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){

    // model->setImgWidth(imgSizeSlider->getValue());
    // model->setImgHeight(imgSizeSlider->getValue());
    model->setLatentVector(latentDimSlider->getValue());
  }


  model->save();
}

int ArchitectureBasicBuilderScene::getImageSize(){
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    for(int i = 0; i < pix2pixButtons.size(); i++){
      if(pix2pixButtons[i]->getChecked()){
        return pix2pixSize[i];
      }
    }
    return -1;
  }
  else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
    for(int i = 0; i < ganButtons.size(); i++){
      if(ganButtons[i]->getChecked()){
        return ganSize[i];
      }
    }
    return -1;
  }
  return -2;
}

void ArchitectureBasicBuilderScene::setImageSizeChecked(){
  int size = ModelManager::getInstance()->getImgWidth();

  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    for(int i = 0; i < pix2pixSize.size(); i++){
      pix2pixButtons[i]->setChecked(false);
      if(pix2pixSize[i] == size){
        pix2pixButtons[i]->setChecked(true);
      }
    }
  }
  else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
    for(int i = 0; i < ganSize.size(); i++){
      ganButtons[i]->setChecked(false);
      if(ganSize[i] == size){
        ganButtons[i]->setChecked(true);
      }
    }
  }

}

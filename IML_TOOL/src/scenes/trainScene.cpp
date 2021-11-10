

#include "trainScene.h"


//
void TrainingScene::refresh(){

    string img_dir = "saved_models/"+ModelManager::getInstance()->getModelName()+"/images";
    // num_images = DIR.listDir(img_dir);

    // training_img.allocate(1600, 1600, OF_IMAGE_COLOR);
    training_img.load("images/-1.png");
    graph_img.load("images/-1.png");
    // training_img.resize(400, 400);
    // threadedImageLoader.loadFromDisk(training_img, img_dir+"/"+ofToString(ModelManager::getInstance()->getEpochsTrained())+".png");

    // current_image = -1;
    epochManager.setup(&training_img, &graph_img, &threadedImageLoader, epochLabel, maxEpochsSlider);
    // imageLoader.startThread();
    ModelManager * model = ModelManager::getInstance();

    lossLoader.setup(&graph, &msg, "data/saved_models/" + model->getModelName() +"/saved_networks/");

    refreshTraining();

}


void TrainingScene::refreshTraining(){

  ModelManager * model = ModelManager::getInstance();
  int img_width = model->getImgWidth();
  int img_height = model->getImgHeight();
  int input_channel = model->getInputChannel();
  // int output_channel = model->getOutputChannel();
  int learning_rateX = model->getLearningRateX();
  int learning_rateY = model->getLearningRateY();
  int max_epochs = model->getMaxEpochs();
  int batch_size = model->getBatchSize();
  int num_layers = model->getNumLayers();
  int kernel_size = model->getKernelSize();
  float beta = model->getBeta();
  int lambda = model->getLambda();
  string dataset_dir = model->getDatasetDir();
  int latent_vector = model->getLatentVector();

  float disc_noise = ofToFloat(model->getDiscriminatorNoise());
  bool random_horizontal = model->getRandomHorizontal();
  bool random_vertical = model->getRandomVertical();
  int random_crop = model->getRandomCrop();
  float random_brightness = ofToFloat(model->getRandomBrightness());
  float random_contrast = ofToFloat(model->getRandomContrast());
  cout << random_contrast << endl;
  string pythonFile;

  //TODO create custom python files
  if(model->getModelType() == MODEL_TYPE::PIX2PIX){
    pythonFile = "python ../src/python/pix2pix_train.py ";
  }
  else if(model->getModelType() == MODEL_TYPE::GAN){
    pythonFile = "python ../src/python/dcgan.py ";
  }

  trainingThread.setup(pythonFile,
    img_width,
    img_height,
    input_channel,
    // output_channel,
    learning_rateX,
    learning_rateY,
    max_epochs,
    batch_size,
    num_layers,
    kernel_size,
    beta,
    lambda,
    latent_vector,
    dataset_dir,
    disc_noise,
    random_horizontal,
    random_vertical,
    random_crop,
    random_brightness,
    random_contrast
    );

  epochLabel->setLabel(ofToString(model->getEpochsTrained()) + "/" + ofToString(ModelManager::getInstance()->getMaxEpochs()));

  learningRateSlider->setValue(learning_rateX);
  learningRateSlider2->setValue(-learning_rateY);
  batchSizeSlider->setValue(batch_size);
  maxEpochsSlider->setValue(max_epochs);
  maxEpochsSlider->setMin(model->getEpochsTrained());

  discriminatorNoiseSlider->setValue(ofToFloat(model->getDiscriminatorNoise()));
  randomHorizontalToggle->setChecked(model->getRandomHorizontal());
  randomVerticalToggle->setChecked(model->getRandomVertical());
  cropSlider->setValue(model->getRandomCrop());
  brightnessSlider->setValue(ofToFloat(model->getRandomBrightness()));
  contrastSlider->setValue(ofToFloat(model->getRandomContrast()));
}

void TrainingScene::setup(){

  setID(SCENE_TYPE::TRAIN);

  graph.setup("LIVE LOSSES");
  graph.setDx(-1.0); // which means delta of time
  graph.setColor(ofColor::white);  // ofColor(255,255,255)
  graph.setLabel({"GENERATOR","DISCRIMINATOR"});

  epochLabel->setPosition(100, 450);
  epochLabel->setWidth(400);


  startTrainingButton->setPosition(ofGetWidth() / 2 - startTrainingButton->getWidth()/2, ofGetHeight() - 200);
  startTrainingButton->onButtonEvent(this, &TrainingScene::onButtonEvent);

  stopTrainingButton->setPosition(ofGetWidth() / 2 - stopTrainingButton->getWidth()/2, ofGetHeight() - 200);
  stopTrainingButton->onButtonEvent(this, &TrainingScene::onButtonEvent);

  resumeTrainingButton->setPosition(ofGetWidth() / 2 - resumeTrainingButton->getWidth()/2, ofGetHeight() - 200);
  resumeTrainingButton->onButtonEvent(this, &TrainingScene::onButtonEvent);


  restartTrainingButton->setPosition(ofGetWidth() / 2 - restartTrainingButton->getWidth()/2, ofGetHeight() - 150);
  restartTrainingButton->onButtonEvent(this, &TrainingScene::onButtonEvent);
  restartTrainingButton->setStripeColor(ofColor(255,0,0));


  saveModelButton->setPosition(ofGetWidth() / 2 - resumeTrainingButton->getWidth()/2, ofGetHeight() - 150);
  saveModelButton->onButtonEvent(this, &TrainingScene::onButtonEvent);

  backButton->setPosition(ofGetWidth() / 2 - 1.5 * backButton->getWidth(), ofGetHeight() - 100);
  backButton->onButtonEvent(this, &TrainingScene::onButtonEvent);


  confirmButton->setPosition(ofGetWidth() / 2 + confirmButton->getWidth()/2, ofGetHeight() - 200);
  confirmButton->onButtonEvent(this, &TrainingScene::onButtonEvent);
  confirmButton->setStripeColor(ofColor(255,0,0));

  unconfirmButton->setPosition(ofGetWidth() / 2 - 1.5 * backButton->getWidth(), ofGetHeight() - 200);
  unconfirmButton->onButtonEvent(this, &TrainingScene::onButtonEvent);

  playButton->setPosition(ofGetWidth() / 2 + playButton->getWidth()/2, ofGetHeight() - 100);
  playButton->onButtonEvent(this, &TrainingScene::onButtonEvent);

  int yControl = 275;
  learningRateSlider = new ofxDatGuiSlider("LEARNING RATE (Xe^-Y) X:", 1, 9,  ModelManager::getInstance()->getLearningRateX());
  learningRateSlider->setPosition(575, yControl);
  learningRateSlider->setWidth(450, 0.5);
  learningRateSlider->setPrecision(0);

  learningRateSlider2 = new ofxDatGuiSlider("LEARNING RATE (Xe^-Y) Y:", 1, 9, -( ModelManager::getInstance()->getLearningRateY()));
  learningRateSlider2->setPosition(575, yControl + learningRateSlider->getHeight());
  learningRateSlider2->setWidth(450, 0.5);
  learningRateSlider2->setPrecision(0);


  batchSizeSlider = new ofxDatGuiSlider("BATCH SIZE", 1, 256,  ModelManager::getInstance()->getBatchSize());
  batchSizeSlider->setPosition(575, yControl + 2*learningRateSlider->getHeight());
  batchSizeSlider->setWidth(450, 0.5);
  batchSizeSlider->setPrecision(0);

  maxEpochsSlider = new ofxDatGuiSlider("MAX EPOCHS", 1, 50000,  ModelManager::getInstance()->getMaxEpochs());
  maxEpochsSlider->setPosition(575, yControl + 3*learningRateSlider->getHeight());
  maxEpochsSlider->setWidth(450, 0.5);
  maxEpochsSlider->setPrecision(0);
  maxEpochsSlider->setMin(ModelManager::getInstance()->getEpochsTrained());

  discriminatorNoiseSlider = new ofxDatGuiSlider("Discriminator Noise", 0, 1, 0);
  discriminatorNoiseSlider->setPosition(575, yControl + 4*learningRateSlider->getHeight());
  discriminatorNoiseSlider->setWidth(450, 0.5);

  randomHorizontalToggle = new ofxDatGuiToggle("RANDOM HORIZONTAL FLIP");
  randomHorizontalToggle->setPosition(575, yControl + 5*learningRateSlider->getHeight());
  randomHorizontalToggle->setWidth(450);

  randomVerticalToggle = new ofxDatGuiToggle("RANDOM VERTICAL FLIP");
  randomVerticalToggle->setPosition(575, yControl + 6*learningRateSlider->getHeight());
  randomVerticalToggle->setWidth(450);

  cropSlider = new ofxDatGuiSlider("RANDOM CROP AMOUNT (%)", 0, 50, 0);
  cropSlider->setPosition(575, yControl + 7*learningRateSlider->getHeight());
  cropSlider->setPrecision(0);
  cropSlider->setWidth(450, 0.5);

  brightnessSlider = new ofxDatGuiSlider("RANDOM BRIGHTNESS DELTA", 0, 0.5, 0);
  brightnessSlider->setPosition(575, yControl + 8*learningRateSlider->getHeight());
  brightnessSlider->setWidth(450, 0.5);
  // brightnessSlider->setPrecision(0);

  contrastSlider = new ofxDatGuiSlider("RANDOM CONTRAST DELTA", 0, 0.5, 0);
  contrastSlider->setPosition(575, yControl + 9*learningRateSlider->getHeight());
  contrastSlider->setWidth(450, 0.5);


  toggleGraphButton->setPosition(701, 525);
  toggleGraphButton->onButtonEvent(this, &TrainingScene::onButtonEvent);

}

//--------------------------------------------------------------
void TrainingScene::update(){
  // cout << "LOL" <<"\n";
  /* open, read, and display the message from the FIFO */

  if(state == 0){
    if(ModelManager::getInstance()->getEpochsTrained() == 0){
      startTrainingButton->update();
    }
    else{
      resumeTrainingButton->update();
      playButton->update();
    }

    backButton->update();
    learningRateSlider->update();
    learningRateSlider2->update();
    batchSizeSlider->update();
    maxEpochsSlider->update();
    restartTrainingButton->update();
    discriminatorNoiseSlider->update();
    randomHorizontalToggle->update();
    randomVerticalToggle->update();
    cropSlider->update();
    brightnessSlider->update();
    contrastSlider->update();
  }
  else if(state == 1){
    stopTrainingButton->update();
    saveModelButton->update();
    toggleGraphButton->update();
  }
  else if(state == 2){
    confirmButton->update();
    unconfirmButton->update();
  }
  epochLabel->update();
  training_img.update();
  graph_img.update();
}

//--------------------------------------------------------------
void TrainingScene::draw(){
  graph.draw();
  ofFill();
  // cout << graph.getX() << endl;
  // cout << graph.getY() << endl;
  // cout << graph.getWidth() << endl;
  // cout << graph.getHeight() << endl;
  // ofDrawBitmapString(ofToString(ModelManager::getInstance()->getEpochsTrained()) + "/" + ofToString(ModelManager::getInstance()->getMaxEpochs()),
  // 500, 450);
  training_img.draw(100, 50, 400, 400);
  if(state == 0){
    if(ModelManager::getInstance()->getEpochsTrained() == 0){
      startTrainingButton->draw();
    }
    else{
      playButton->draw();
      resumeTrainingButton->draw();
    }
    backButton->draw();
    restartTrainingButton->draw();
    learningRateSlider->draw();
    learningRateSlider2->draw();
    batchSizeSlider->draw();
    maxEpochsSlider->draw();
    discriminatorNoiseSlider->draw();
    randomHorizontalToggle->draw();
    randomVerticalToggle->draw();
    cropSlider->draw();
    brightnessSlider->draw();
    contrastSlider->draw();
  }
  // training
  else if(state == 1){
    graph_img.draw(522, 275, 450, 250);
    toggleGraphButton->draw();
    stopTrainingButton->draw();
    saveModelButton->draw();

    ofDrawBitmapString(msg, 100, 500);
  }
  else if(state == 2){
    confirmButton->draw();
    unconfirmButton->draw();
  }
  epochLabel->draw();
}


void TrainingScene::onButtonEvent(ofxDatGuiButtonEvent e){

  if(e.target == startTrainingButton || e.target == resumeTrainingButton){
    ModelManager::getInstance()->setLearningRateX(learningRateSlider->getValue());
    ModelManager::getInstance()->setLearningRateY(-learningRateSlider2->getValue());
    ModelManager::getInstance()->setMaxEpochs(maxEpochsSlider->getValue());
    ModelManager::getInstance()->setBatchSize(batchSizeSlider->getValue());

    ModelManager::getInstance()->setDiscriminatorNoise(discriminatorNoiseSlider->getValue());
    ModelManager::getInstance()->setRandomHorizontal(randomHorizontalToggle->getChecked());
    ModelManager::getInstance()->setRandomVertical(randomVerticalToggle->getChecked());
    ModelManager::getInstance()->setRandomCrop(cropSlider->getValue());
    ModelManager::getInstance()->setRandomBrightness(brightnessSlider->getValue());
    ModelManager::getInstance()->setRandomContrast(contrastSlider->getValue());
    ModelManager::getInstance()->save();
    refreshTraining();
    trainingThread.startThread();
    lossLoader.startThread();
    epochManager.startThread();

    SceneManager::getInstance()->setShowNavBar(false);
    state = 1;
  }

  else if(e.target == stopTrainingButton){
    lossLoader.stopThread();
    trainingThread.stopThread();
    epochManager.stopThread();
    SceneManager::getInstance()->setShowNavBar(true);
    state = 0;
  }

  else if(e.target == restartTrainingButton){
    state = 2;
  }
  else if(e.target == confirmButton){
    // TODO: code for restart
    string basePath = "saved_models/"+ModelManager::getInstance()->getModelName();
    ofDirectory::removeDirectory(basePath+"/saved_networks/ckpt", true);
    ofDirectory::removeDirectory(basePath+"/saved_networks/losses", true);
    ofDirectory::removeDirectory(basePath+"/images", true);

    ofDirectory dd(basePath+"/images");
    dd.create();

    ModelManager::getInstance()->setEpochsTrained(0);
    ModelManager::getInstance()->setStatus(2);
    ModelManager::getInstance()->save();
    epochLabel->setLabel(ofToString(ModelManager::getInstance()->getEpochsTrained()) + "/" + ofToString(ModelManager::getInstance()->getMaxEpochs()));
    cout << "RESTART" << endl;
    state = 0;
  }
  else if(e.target == unconfirmButton){
    // trainingThread.stopThread();
    // lossLoader.stopThread();
    // imageLoader.stopThread();
    // TODO: code for restart
    state = 0;
  }

  else if(e.target == saveModelButton){
    saveModelAtCurrentEpoch();
  }
  else if(e.target == toggleGraphButton){
    graphToShow++;
    if(graphToShow > 2){
      graphToShow = 0;
    }
    string name = "graph.png";
    switch(graphToShow){
      case 0:
        name = "graph.png";
        break;
      case 1:
        name = "graphG.png";
        break;
      case 2:
        name = "graphD.png";
        break;
    }
    threadedImageLoader.loadFromDisk(graph_img, "saved_models/" + ModelManager::getInstance()->getModelName()+"/saved_networks/losses/"+name);
  }
  else if(e.target == backButton){
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::DATASET_MENU);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_MENU);
  }

  else if(e.target == playButton){
      BaseScene * scene = SceneManager::getInstance()->getScene(SCENE_TYPE::INTERACT_MENU);
      scene->refresh();
      SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::INTERACT_MENU);
  }

}

void TrainingScene::saveModelAtCurrentEpoch(){
  ofFileDialogResult result = ofSystemLoadDialog("Select output folder", true);
  if (result.bSuccess) {
    string path = result.getPath();
    ofDirectory dir_ = ofDirectory("saved_models/"+ModelManager::getInstance()->getModelName()+"/saved_networks/ckpt/");

    string newFolder = ModelManager::getInstance()->getModelName() + "_"+ofToString(ModelManager::getInstance()->getEpochsTrained());

    ofDirectory dir__ = ofDirectory(path + "/" + newFolder);
    dir__.create(true);
    dir_.copyTo(path + "/" + newFolder +"/", true, true);
  }
}

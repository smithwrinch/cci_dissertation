

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

    lossLoader.setup(&graph, "data/saved_models/" + model->getModelName() +"/saved_networks/" );

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
    string pythonFile;
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
      latent_vector, dataset_dir);

    epochLabel->setLabel(ofToString(model->getEpochsTrained()) + "/" + ofToString(ModelManager::getInstance()->getMaxEpochs()));

    learningRateSlider->setValue(learning_rateX);
    learningRateSlider2->setValue(-learning_rateY);
    batchSizeSlider->setValue(batch_size);
    maxEpochsSlider->setValue(max_epochs);
    maxEpochsSlider->setMin(model->getEpochsTrained());

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

  learningRateSlider = new ofxDatGuiSlider("LEARNING RATE (Xe^-Y) X:", 1, 9,  ModelManager::getInstance()->getLearningRateX());
  learningRateSlider->setPosition(575, 300);
  learningRateSlider->setWidth(450, 0.5);
  learningRateSlider->setPrecision(0);

  learningRateSlider2 = new ofxDatGuiSlider("LEARNING RATE (Xe^-Y) Y:", 1, 9, -( ModelManager::getInstance()->getLearningRateY()));
  learningRateSlider2->setPosition(575, 300 + learningRateSlider->getHeight());
  learningRateSlider2->setWidth(450, 0.5);
  learningRateSlider2->setPrecision(0);


  batchSizeSlider = new ofxDatGuiSlider("BATCH SIZE", 1, 256,  ModelManager::getInstance()->getBatchSize());
  batchSizeSlider->setPosition(575, 300 + 2*learningRateSlider->getHeight());
  batchSizeSlider->setWidth(450, 0.5);
  batchSizeSlider->setPrecision(0);

  maxEpochsSlider = new ofxDatGuiSlider("MAX EPOCHS", 1, 50000,  ModelManager::getInstance()->getMaxEpochs());
  maxEpochsSlider->setPosition(575, 300 + 3*learningRateSlider->getHeight());
  maxEpochsSlider->setWidth(450, 0.5);
  maxEpochsSlider->setPrecision(0);
  maxEpochsSlider->setMin(ModelManager::getInstance()->getEpochsTrained());


  toggleGraphButton->setPosition(699, 526);
  toggleGraphButton->onButtonEvent(this, &TrainingScene::onButtonEvent);

}

//--------------------------------------------------------------
void TrainingScene::update(){
  // cout << "LOL" <<"\n";
  /* open, read, and display the message from the FIFO */
  if(state != 2){
    backButton->update();
    playButton->update();
  }
  if(state == 0){
    if(ModelManager::getInstance()->getEpochsTrained() == 0){
      startTrainingButton->update();
    }
    else{
      resumeTrainingButton->update();
    }

    learningRateSlider->update();
    learningRateSlider2->update();
    batchSizeSlider->update();
    maxEpochsSlider->update();
    restartTrainingButton->update();
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
  // cout << graph.getX() << endl;
  // cout << graph.getY() << endl;
  // cout << graph.getWidth() << endl;
  // cout << graph.getHeight() << endl;
  // ofDrawBitmapString(ofToString(ModelManager::getInstance()->getEpochsTrained()) + "/" + ofToString(ModelManager::getInstance()->getMaxEpochs()),
  // 500, 450);
  training_img.draw(100, 50, 400, 400);
  if(state != 2){
    backButton->draw();
    playButton->draw();
  }
  if(state == 0){
    if(ModelManager::getInstance()->getEpochsTrained() == 0){
      startTrainingButton->draw();
    }
    else{
      resumeTrainingButton->draw();
    }
    restartTrainingButton->draw();
    learningRateSlider->draw();
    learningRateSlider2->draw();
    batchSizeSlider->draw();
    maxEpochsSlider->draw();
  }
  else if(state == 1){
    graph_img.draw(522, 275, 450, 250);
    toggleGraphButton->draw();
    stopTrainingButton->draw();
    saveModelButton->draw();
  }
  else if(state == 2){
    confirmButton->draw();
    unconfirmButton->draw();
  }
  epochLabel->draw();
}


void TrainingScene::onButtonEvent(ofxDatGuiButtonEvent e){

  if(e.target == startTrainingButton || e.target == resumeTrainingButton){
    trainingThread.startThread();
    lossLoader.startThread();
    epochManager.startThread();

    SceneManager::getInstance()->setShowNavBar(false);

    ModelManager::getInstance()->setLearningRateX(learningRateSlider->getValue());
    ModelManager::getInstance()->setLearningRateY(-learningRateSlider2->getValue());
    ModelManager::getInstance()->setMaxEpochs(maxEpochsSlider->getValue());
    ModelManager::getInstance()->setBatchSize(batchSizeSlider->getValue());

    ModelManager::getInstance()->save();
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

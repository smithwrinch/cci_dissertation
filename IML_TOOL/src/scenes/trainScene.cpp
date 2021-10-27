

#include "trainScene.h"

//
void TrainingScene::refresh(){
    lossLoader.setup(&graph);

    string img_dir = "saved_models/"+ModelManager::getInstance()->getModelName()+"/images";
    // num_images = DIR.listDir(img_dir);

    // training_img.allocate(1600, 1600, OF_IMAGE_COLOR);
    training_img.load("images/-1.png");
    training_img.resize(400, 400);

    // current_image = -1;
    imageLoader.setup(&training_img, &threadedImageLoader);
    // imageLoader.startThread();



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
}

void TrainingScene::setup(){

  /*
  ERROR IN HERE SOMEWHERE
  */
  setID(SCENE_TYPE::TRAIN);



  graph.setup("LIVE LOSSES");
  graph.setDx(-1.0); // which means delta of time
  graph.setColor(ofColor::white);  // ofColor(255,255,255)
  graph.setLabel({"GENERATOR","DISCRIMINATOR"});


  startTrainingButton->setPosition(ofGetWidth() / 2 - startTrainingButton->getWidth()/2, ofGetHeight() - 200);
  startTrainingButton->onButtonEvent(this, &TrainingScene::onButtonEvent);

  stopTrainingButton->setPosition(ofGetWidth() / 2 - stopTrainingButton->getWidth()/2, ofGetHeight() - 200);
  stopTrainingButton->onButtonEvent(this, &TrainingScene::onButtonEvent);

  resumeTrainingButton->setPosition(ofGetWidth() / 2 - resumeTrainingButton->getWidth()/2, ofGetHeight() - 200);
  resumeTrainingButton->onButtonEvent(this, &TrainingScene::onButtonEvent);


  restartTrainingButton->setPosition(ofGetWidth() / 2 - restartTrainingButton->getWidth()/2, ofGetHeight() - 150);
  restartTrainingButton->onButtonEvent(this, &TrainingScene::onButtonEvent);
  restartTrainingButton->setStripeColor(ofColor(255,0,0));

  backButton->setPosition(ofGetWidth() / 2 - 1.5 * backButton->getWidth(), ofGetHeight() - 100);
  backButton->onButtonEvent(this, &TrainingScene::onButtonEvent);


  confirmButton->setPosition(ofGetWidth() / 2 + confirmButton->getWidth()/2, ofGetHeight() - 200);
  confirmButton->onButtonEvent(this, &TrainingScene::onButtonEvent);
  confirmButton->setStripeColor(ofColor(255,0,0));

  unconfirmButton->setPosition(ofGetWidth() / 2 - 1.5 * backButton->getWidth(), ofGetHeight() - 200);
  unconfirmButton->onButtonEvent(this, &TrainingScene::onButtonEvent);

  playButton->setPosition(ofGetWidth() / 2 + playButton->getWidth()/2, ofGetHeight() - 100);
  playButton->onButtonEvent(this, &TrainingScene::onButtonEvent);
  /*
  /ERROR
  */

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
    restartTrainingButton->update();
  }
  else if(state == 1){
    stopTrainingButton->update();
  }
  else if(state == 2){
    confirmButton->update();
    unconfirmButton->update();
  }

  training_img.update();
}

//--------------------------------------------------------------
void TrainingScene::draw(){
  graph.draw();
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
  }
  else if(state == 1){
    stopTrainingButton->draw();
  }
  else if(state == 2){
    confirmButton->draw();
    unconfirmButton->draw();
  }
}

void TrainingScene::onButtonEvent(ofxDatGuiButtonEvent e){

  if(e.target == startTrainingButton || e.target == resumeTrainingButton){
    trainingThread.startThread();
    lossLoader.startThread();
    imageLoader.startThread();
    state = 1;
  }

  if(e.target == stopTrainingButton){
    lossLoader.stopThread();
    trainingThread.stopThread();
    imageLoader.stopThread();
    state = 0;
  }

  if(e.target == restartTrainingButton){
    state = 2;
  }
  if(e.target == confirmButton){
    // TODO: code for restart
    string basePath = "saved_models/"+ModelManager::getInstance()->getModelName();
    ofDirectory::removeDirectory(basePath+"/saved_networks/ckpt", true);
    ofDirectory::removeDirectory(basePath+"/images", true);
    ModelManager::getInstance()->setEpochsTrained(0);
    ModelManager::getInstance()->save();
    cout << "RESTART" << endl;
    state = 0;
  }
  if(e.target == unconfirmButton){
    // trainingThread.stopThread();
    // lossLoader.stopThread();
    // imageLoader.stopThread();
    // TODO: code for restart
    state = 0;
  }

}

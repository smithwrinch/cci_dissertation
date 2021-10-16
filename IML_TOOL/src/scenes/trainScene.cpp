#include "trainScene.h"

void TrainingScene::setup(){

  /*
  ERROR IN HERE SOMEWHERE
  */
  setID(SCENE_TYPE::TRAIN);
  // graph.setup("LIVE LOSSES");
  // graph.setDx(-1.0); // which means delta of time
  // graph.setColor(ofColor::white);  // ofColor(255,255,255)
  lossLoader.setup("/tmp/gen", &graph);
  // lossLoader.startThread();

  /*
  /ERROR
  */

  string img_dir = ModelManager::getInstance()->getModelName()+"/images";
  num_images = DIR.listDir(img_dir);


  training_img.loadImage(ModelManager::getInstance()->getModelName()+"/images/0.png");
  training_img.resize(400, 400);

  current_image = -1;
  imageLoader.setup(img_dir, &current_image,&DIR,&training_img);
  // imageLoader.startThread();

  startTrainingButton->setPosition(ofGetWidth() / 2 - startTrainingButton->getWidth()/2, ofGetHeight() - 200);

  ModelManager * model = ModelManager::getInstance();
  int img_width = model->getImgWidth();
  int img_height = model->getImgHeight();
  int input_channel = model->getInputChannel();
  int output_channel = model->getOutputChannel();
  float learning_rate = model->getLearningRate();
  int max_epochs = model->getMaxEpochs();
  int batch_size = model->getBatchSize();
  int num_layers = model->getNumLayers();
  int kernel_size = model->getKernelSize();
  float beta = model->getBeta();
  int lambda = model->getLambda();

  string pythonFile = "python ../src/python/pix2pix_train.py";
  trainingThread.setup(pythonFile,
    img_width,
    img_height,
    input_channel,
    output_channel,
    learning_rate,
    max_epochs,
    batch_size,
    num_layers,
    kernel_size,
    beta,
    lambda);
}

//--------------------------------------------------------------
void TrainingScene::update(){
  // cout << "LOL" <<"\n";
  /* open, read, and display the message from the FIFO */
  // graph.add(current_loss);
}

//--------------------------------------------------------------
void TrainingScene::draw(){
  graph.draw();
  training_img.update();
  training_img.draw(100, 50);
}

void TrainingScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == startTrainingButton){
    // trainingThread.startThread();
    // lossLoader.startThread();
    // imageLoader.startThread();
  }
}

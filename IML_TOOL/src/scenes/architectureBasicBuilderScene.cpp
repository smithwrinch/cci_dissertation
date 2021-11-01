#include "architectureBasicBuilderScene.h"

// parser.add_argument("--max_epochs", help="epochs to train for", type=int, default=10000, required=False)
// parser.add_argument("--batch_size", help="batch size", type=int, default=1, required=False)
// parser.add_argument("--learning_rate", help="learning rate for discriminator and generator", type=float, default=2e-4, required=False)
// parser.add_argument("--image_width", help="image width for input and output", type=int, default=256, required=False)
// parser.add_argument("--image_height", help="image height for input and output", type=int, default=256, required=False)
// parser.add_argument("--input_channel", help="number of channels for input image", type=int, default=3, required=False)
// parser.add_argument("--output_channel", help="number of channels for output image", type=int, default=3, required=False)
// parser.add_argument("--num_layers", help="[advanced] number of layers in generator decoder and encoder", type=int, default=7, required=False)
// parser.add_argument("--kernel_size", help="[advanced] kernel size for convolutional layers", type=int, default=4, required=False)
// parser.add_argument("--beta", help="[advanced] momentum for ADAM optimiser", type=float, default=0.5, required=False)
// parser.add_argument("--lambda_", help="[advanced] variable to improve structural loss", type=int, default=100, required=False)
// parser.add_argument("--img_save_dir", help="Directory to save images to", default="../../bin/data/default_save/", required=False)
// parser.add_argument("--checkpoint_save_dir", help="Directory to save checkpoints to", default="../../bin/data/default_save/", required=False)

void ArchitectureBasicBuilderScene::refresh(){
  ModelManager * model = ModelManager::getInstance();

  cout << "MODEL: " << model->getModelName() << endl;

  imgWidthSlider->setValue(model->getImgWidth());
  imgSizeSlider->setValue(model->getImgWidth());
  imgHeightSlider->setValue(model->getImgHeight());
  learningRateSlider->setValue(model->getLearningRateX());
  learningRateSlider2->setValue(-model->getLearningRateY());
  maxEpochsSlider->setValue(model->getMaxEpochs());
  batchSizeSlider->setValue(model->getBatchSize());
  numLayersSlider->setValue(model->getNumLayers());
  latentDimSlider->setValue(model->getLatentVector());
  kernelSizeSlider->setValue(model->getKernelSize());
  betaSlider->setValue(model->getBeta());
  lambdaSlider->setValue(model->getLambda());

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
  backButton->setPosition(btnBuffer, ofGetHeight() - 200);
  backButton->onButtonEvent(this, &ArchitectureBasicBuilderScene::onButtonEvent);

  continueButton = new ofxDatGuiButton("CONTINUE");
  continueButton->setPosition(ofGetWidth() - backButton->getWidth() - btnBuffer, ofGetHeight() - 200);
  continueButton->onButtonEvent(this, &ArchitectureBasicBuilderScene::onButtonEvent);

  imgWidthSlider = new ofxDatGuiSlider("IMAGE WIDTH: ", 32, 1024, model->getImgWidth());
  int centreX = ofGetWidth()/2 - width / 2;
  imgWidthSlider->setPosition(centreX, startY);
  imgWidthSlider->setPrecision(0);


  imgSizeSlider = new ofxDatGuiSlider("IMAGE SIZE: ", 28, 64, model->getImgWidth());
  imgSizeSlider->setPosition(centreX, startY);
  imgSizeSlider->setPrecision(0);


  imgHeightSlider = new ofxDatGuiSlider("IMAGE HEIGHT: ", 32, 1024, model->getImgHeight());
  imgHeightSlider->setPosition(centreX, imgWidthSlider->getY() + imgWidthSlider->getHeight());
  imgHeightSlider->setPrecision(0);



  inputRGBToggle = new ofxDatGuiToggle("IMAGE GRAYSCALE");
  inputRGBToggle->setPosition(centreX, imgHeightSlider->getY() + imgHeightSlider->getHeight());
  //
  // outputRGBToggle = new ofxDatGuiToggle("OUTPUT IMAGE GRAYSCALE");
  // outputRGBToggle->setPosition(centreX, inputRGBToggle->getY() + inputRGBToggle->getHeight());

  // learningRateInput = new ofxDatGuiSlider("LEARNING RATE", 0.0000001, 0.00002, 0.001)
  learningRateSlider = new ofxDatGuiSlider("LEARNING RATE (Xe^-Y) X:", 1, 9, model->getLearningRateX());
  learningRateSlider->setPosition(centreX, inputRGBToggle->getY() + inputRGBToggle->getHeight()*2);
  learningRateSlider->setPrecision(0);

  learningRateSlider2 = new ofxDatGuiSlider("LEARNING RATE (Xe^-Y) Y:", 1, 9, model->getLearningRateY());
  learningRateSlider2->setPosition(centreX, learningRateSlider->getY() + learningRateSlider->getHeight());
  learningRateSlider2->setPrecision(0);

  maxEpochsSlider = new ofxDatGuiSlider("MAX EPOCHS", 1, 50000, model->getMaxEpochs());
  maxEpochsSlider->setPosition(centreX, learningRateSlider2->getY() + learningRateSlider2->getHeight());
  maxEpochsSlider->setPrecision(0);

  batchSizeSlider = new ofxDatGuiSlider("BATCH SIZE", 1, 256, model->getBatchSize());
  batchSizeSlider->setPosition(centreX, maxEpochsSlider->getY() + maxEpochsSlider->getHeight());
  batchSizeSlider->setPrecision(0);

  numLayersSlider = new ofxDatGuiSlider("# LAYERS", 4, 9, model->getNumLayers());
  numLayersSlider->setPosition(centreX, batchSizeSlider->getY() + batchSizeSlider->getHeight());
  numLayersSlider->setPrecision(0);


  latentDimSlider = new ofxDatGuiSlider("LATENT DIM SIZE: ", 28, 1024, model->getLatentVector());
  latentDimSlider->setPosition(centreX, batchSizeSlider->getY() + batchSizeSlider->getHeight());
  latentDimSlider->setPrecision(0);

  kernelSizeSlider = new ofxDatGuiSlider("KERNEL SIZE", 1, 9, model->getKernelSize());
  kernelSizeSlider->setPosition(centreX, numLayersSlider->getY() + numLayersSlider->getHeight()*2);
  kernelSizeSlider->setPrecision(0);

  betaSlider = new ofxDatGuiSlider("BETA (*10^-2)", 0, 100, model->getBeta());
  betaSlider->setPosition(centreX, kernelSizeSlider->getY()+kernelSizeSlider->getHeight());
  betaSlider->setPrecision(0);

  lambdaSlider = new ofxDatGuiSlider("LAMBDA", 0, model->getLambda());
  lambdaSlider->setPosition(centreX, betaSlider->getY()+betaSlider->getHeight());
  lambdaSlider->setPrecision(0);

  imgWidthSlider->setWidth(width, label_width);
  imgHeightSlider->setWidth(width, label_width);
  inputRGBToggle->setWidth(width, label_width);
  // outputRGBToggle->setWidth(width, label_width);
  learningRateSlider->setWidth(width, label_width);
  learningRateSlider2->setWidth(width, label_width);
  maxEpochsSlider->setWidth(width, label_width);
  batchSizeSlider->setWidth(width, label_width);
  numLayersSlider->setWidth(width, label_width);
  // advancedButton->update();
  kernelSizeSlider->setWidth(width, label_width);
  betaSlider->setWidth(width, label_width);
  lambdaSlider->setWidth(width, label_width);


  imgSizeSlider->setWidth(width, label_width);
  latentDimSlider->setWidth(width, label_width);
}

void ArchitectureBasicBuilderScene::update(){
  inputRGBToggle->update();
  learningRateSlider->update();
  learningRateSlider2->update();
  maxEpochsSlider->update();
  batchSizeSlider->update();
  // advancedButton->update();
  kernelSizeSlider->update();
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    // outputRGBToggle->update();
    imgWidthSlider->update();
    imgHeightSlider->update();
    // outputRGBToggle->update();
    lambdaSlider->update();
    numLayersSlider->update();
    betaSlider->update();
  }
  else if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
    imgSizeSlider->update();
    latentDimSlider->update();
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
  // advancedButton->draw();
  kernelSizeSlider->draw();
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){

    betaSlider->draw();
    imgWidthSlider->draw();
    imgHeightSlider->draw();
    // outputRGBToggle->draw();
    lambdaSlider->draw();
    numLayersSlider->draw();
  }
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::GAN){
    imgSizeSlider->draw();
    latentDimSlider->draw();
  }

  backButton->draw();
  continueButton->draw();
}

void ArchitectureBasicBuilderScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::ARCHITECTURE_MENU);
  }

  if(e.target == continueButton){
    // TODO change to dataset
    setModel();
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::DATASET_MENU);
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
  int channels = 3;
  if(inputRGBToggle->getChecked()){
    channels = 1;
  }

  model->setInputChannel(channels);

  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    model->setImgWidth(imgWidthSlider->getValue());
    model->setImgHeight(imgHeightSlider->getValue());
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

    model->setImgWidth(imgSizeSlider->getValue());
    model->setImgHeight(imgSizeSlider->getValue());
    model->setLatentVector(latentDimSlider->getValue());
  }


  model->save();
}

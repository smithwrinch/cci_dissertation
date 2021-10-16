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

void ArchitectureBasicBuilderScene::setup(){

  setID(SCENE_TYPE::ARCHITECTURE_BUILDER);

  int width = 600;
  float label_width = 0.5;
  int startY = 125;
  int btnBuffer = 200;

  backButton = new ofxDatGuiButton("BACK<-");
  backButton->setPosition(btnBuffer, ofGetHeight() - 200);
  backButton->onButtonEvent(this, &ArchitectureBasicBuilderScene::onButtonEvent);

  continueButton = new ofxDatGuiButton("CONTINUE");
  continueButton->setPosition(ofGetWidth() - backButton->getWidth() - btnBuffer, ofGetHeight() - 200);
  continueButton->onButtonEvent(this, &ArchitectureBasicBuilderScene::onButtonEvent);

  imgWidthSlider = new ofxDatGuiSlider("IMAGE WIDTH: ", 32, 1024, 256);
  int centreX = ofGetWidth()/2 - width / 2;
  imgWidthSlider->setPosition(centreX, startY);
  imgWidthSlider->setPrecision(0);

  imgHeightSlider = new ofxDatGuiSlider("IMAGE HEIGHT: ", 32, 1024, 256);
  imgHeightSlider->setPosition(centreX, imgWidthSlider->getY() + imgWidthSlider->getHeight());
  imgHeightSlider->setPrecision(0);

  inputRGBToggle = new ofxDatGuiToggle("INPUT IMAGE GRAYSCALE");
  inputRGBToggle->setPosition(centreX, imgHeightSlider->getY() + imgHeightSlider->getHeight());

  outputRGBToggle = new ofxDatGuiToggle("OUTPUT IMAGE GRAYSCALE");
  outputRGBToggle->setPosition(centreX, inputRGBToggle->getY() + inputRGBToggle->getHeight());

  // learningRateInput = new ofxDatGuiSlider("LEARNING RATE", 0.0000001, 0.00002, 0.001)
  learningRateSlider = new ofxDatGuiSlider("LEARNING RATE (Xe^-Y) X:", 1, 9, 2);
  learningRateSlider->setPosition(centreX, outputRGBToggle->getY() + outputRGBToggle->getHeight()*2);
  learningRateSlider->setPrecision(0);

  learningRateSlider2 = new ofxDatGuiSlider("LEARNING RATE (Xe^-Y) Y:", 1, 9, 5);
  learningRateSlider2->setPosition(centreX, learningRateSlider->getY() + learningRateSlider->getHeight());
  learningRateSlider2->setPrecision(0);

  maxEpochsSlider = new ofxDatGuiSlider("MAX EPOCHS", 1, 50000, 1000);
  maxEpochsSlider->setPosition(centreX, learningRateSlider2->getY() + learningRateSlider2->getHeight());
  maxEpochsSlider->setPrecision(0);

  batchSizeSlider = new ofxDatGuiSlider("BATCH SIZE", 1, 128, 1);
  batchSizeSlider->setPosition(centreX, maxEpochsSlider->getY() + maxEpochsSlider->getHeight());
  batchSizeSlider->setPrecision(0);

  numLayersSlider = new ofxDatGuiSlider("# LAYERS", 4, 9, 7);
  numLayersSlider->setPosition(centreX, batchSizeSlider->getY() + batchSizeSlider->getHeight());
  numLayersSlider->setPrecision(0);

  kernelSizeSlider = new ofxDatGuiSlider("KERNEL SIZE", 1, 9, 4);
  kernelSizeSlider->setPosition(centreX, numLayersSlider->getY() + numLayersSlider->getHeight()*2);
  kernelSizeSlider->setPrecision(0);

  betaSlider = new ofxDatGuiSlider("BETA", 0, 1);
  betaSlider->setPosition(centreX, kernelSizeSlider->getY()+kernelSizeSlider->getHeight());

  lambdaSlider = new ofxDatGuiSlider("LAMBDA", 0, 200);
  lambdaSlider->setPosition(centreX, betaSlider->getY()+betaSlider->getHeight());
  lambdaSlider->setPrecision(0);

  imgWidthSlider->setWidth(width, label_width);
  imgHeightSlider->setWidth(width, label_width);
  inputRGBToggle->setWidth(width, label_width);
  outputRGBToggle->setWidth(width, label_width);
  learningRateSlider->setWidth(width, label_width);
  learningRateSlider2->setWidth(width, label_width);
  maxEpochsSlider->setWidth(width, label_width);
  batchSizeSlider->setWidth(width, label_width);
  numLayersSlider->setWidth(width, label_width);
  // advancedButton->update();
  kernelSizeSlider->setWidth(width, label_width);
  betaSlider->setWidth(width, label_width);
  lambdaSlider->setWidth(width, label_width);
}

void ArchitectureBasicBuilderScene::update(){
  imgWidthSlider->update();
  imgHeightSlider->update();
  inputRGBToggle->update();
  outputRGBToggle->update();
  learningRateSlider->update();
  learningRateSlider2->update();
  maxEpochsSlider->update();
  batchSizeSlider->update();
  numLayersSlider->update();
  // advancedButton->update();
  kernelSizeSlider->update();
  betaSlider->update();
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    lambdaSlider->update();
  }
  backButton->update();
  continueButton->update();
}

void ArchitectureBasicBuilderScene::draw(){
  imgWidthSlider->draw();
  imgHeightSlider->draw();
  inputRGBToggle->draw();
  outputRGBToggle->draw();
  learningRateSlider->draw();
  learningRateSlider2->draw();
  maxEpochsSlider->draw();
  batchSizeSlider->draw();
  numLayersSlider->draw();
  // advancedButton->draw();
  kernelSizeSlider->draw();
  betaSlider->draw();
  if(ModelManager::getInstance()->getModelType() == MODEL_TYPE::PIX2PIX){
    lambdaSlider->draw();
  }
  backButton->draw();
  continueButton->draw();
}

void ArchitectureBasicBuilderScene::onButtonEvent(ofxDatGuiButtonEvent e){
  if(e.target == backButton){
    setModel();
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::MENU);
  }

  if(e.target == continueButton){
    // TODO change to dataset
    setModel();
    SceneManager::getInstance()->changeSceneTo(SCENE_TYPE::TRAIN);
  }
}

// assigns values to models config
void ArchitectureBasicBuilderScene::setModel(){
  ModelManager * model = model->getInstance();
  model->setImgWidth(imgWidthSlider->getValue());
  model->setImgHeight(imgHeightSlider->getValue());
  int channels = 3;
  if(inputRGBToggle->getChecked()){
    channels = 1;
  }
  model->setInputChannel(channels);
  int channels_ = 3;
  if(outputRGBToggle->getChecked()){
    channels_ = 1;
  }
  model->setOutputChannel(channels);
  float learning_rate = pow(learningRateSlider->getValue(), learningRateSlider2->getValue());
  model->setLearningRate(learning_rate);
  model->setMaxEpochs(maxEpochsSlider->getValue());
  model->setBatchSize(batchSizeSlider->getValue());
  model->setNumLayers(numLayersSlider->getValue());
  model->setKernelSize(kernelSizeSlider->getValue());
  model->setBeta(betaSlider->getValue());
  model->setLambda(lambdaSlider->getValue());
  model->save();
}

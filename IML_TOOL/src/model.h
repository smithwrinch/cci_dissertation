// deals with config.xml
#pragma once
#include "sceneTypes.h"

#include "ofxXmlSettings.h"
// singleton approach relies on robust scene switching
// decided on singleton class so I wouldn't have to pass it into functions
// probably a bad design decision but here we are

// sorry
class ModelManager{
  public:

    static ModelManager * singleton;

    static ModelManager *  getInstance(){
      if(singleton == nullptr){
        singleton = new ModelManager();
      }
      return singleton;
    }

    void reset(){
      config.loadFile("_config.xml");
    }

    void save(){
      config.saveFile("saved_models/"+getModelName()+"/config.xml");
    }
    void load(string fname){
      config.loadFile("saved_models/"+fname+"/config.xml");
    }
    void setModelType(MODEL_TYPE::ID a){
      config.setValue("config:model_type", a);
    }
    void setModelName(string s){
      config.setValue("config:name", s);
    }
    void setEpochsTrained(int i){
      config.setValue("config:epochs_trained_for", i);
    }
    void setDataset(string s){
      config.setValue("config:dataset_url", s);
    }
    void setStatus(int i){
      config.setValue("config:status", i);
    }
    // architecture ----------------------------------------
    void setImgWidth(int i){
      config.setValue("config:image_width", i);
    }
    void setImgHeight(int i){
      config.setValue("config:image_height", i);
    }
    void setInputChannel(int i){
      config.setValue("config:input_channel", i);
    }
    void setOutputChannel(int i){
      config.setValue("config:output_channel", i);
    }
    void setLearningRateX(int f){
      config.setValue("config:learning_rateX", f);
    }

    void setLearningRateY(int f){
      config.setValue("config:learning_rateY", f);
    }
    void setMaxEpochs(int i){
      config.setValue("config:max_epochs", i);
    }
    void setBatchSize(int i){
      config.setValue("config:batch_size", i);
    }
    void setNumLayers(int i){
      config.setValue("config:num_layers", i);
    }
    void setKernelSize(int i){
      config.setValue("config:kernel_size", i);
    }
    void setBeta(int f){
      config.setValue("config:beta", f);
    }
    void setLambda(int i){
      config.setValue("config:lambda", i);
    }
    void setLatentVector(int i){
      config.setValue("config:latent_vector", i);
    }
    void setDatasetDir(string dir){
      config.setValue("config:dataset_dir", dir);
    }
    void setDiscriminatorNoise(float f){
      config.setValue("config:disc_noise", f);
    }
    void setRandomHorizontal(bool b){
      int out = 0;
      if(b){
        out = 1;
      }
      config.setValue("config:random_horizontal", out);
    }
    void setRandomVertical(bool b){
      int out = 0;
      if(b){
        out = 1;
      }
      config.setValue("config:random_vertical", out);
    }
    void setRandomCrop(int i){
      config.setValue("config:random_crop", i);
    }
    void setRandomBrightness(float f){
      config.setValue("config:random_brightness", f);
    }

    void setRandomContrast(float f){
      config.setValue("config:random_contrast", f);
    }

    // all purpose set
    // wrote everything else out to be more robust to refactor
    void set(string key, string i){
        config.setValue("config:"+key, i);
    }


    MODEL_TYPE::ID getModelType(){
      int modelType = config.getValue("config:model_type", -1);
      MODEL_TYPE::ID val = static_cast<MODEL_TYPE::ID>(modelType);
      return val;
    }

    string getModelName(){
      return config.getValue("config:name", "");
    }

    int getEpochsTrained(){
      return config.getValue("config:epochs_trained_for", 0);
    }

    //todo chaneg according to data structure
    string getDataset(){
      return config.getValue("config:dataset_url", "");
    }

    int getStatus(){
      return config.getValue("config:status", -1);
    }

    int getImgWidth(){
      int default_;
      if(singleton->getModelType() == MODEL_TYPE::GAN){
        default_ = 28;
      }
      else{
        default_ = 256;
      }
      return config.getValue("config:image_width", default_);
    }
    int getImgHeight(){
      int default_;
      if(singleton->getModelType() == MODEL_TYPE::GAN){
        default_ = 28;
      }
      else{
        default_ = 256;
      }
      return config.getValue("config:image_height", default_);
    }
    int getInputChannel(){
      return config.getValue("config:input_channel", -1);
    }
    int getOutputChannel(){
      return config.getValue("config:output_channel", -1);
    }
    int getLearningRateX(){
      int default_;
      if(singleton->getModelType() == MODEL_TYPE::GAN){
        default_ = 1;
      }
      else{
        default_ = 2;
      }
      return config.getValue("config:learning_rateX", default_);
    }
    int getLearningRateY(){
      int default_;
      if(singleton->getModelType() == MODEL_TYPE::GAN){
        default_ = 4;
      }
      else{
        default_ = 5;
      }
      return config.getValue("config:learning_rateY", default_);
    }
    int getMaxEpochs(){
      return config.getValue("config:max_epochs", 1000);
    }
    int getBatchSize(){
      int default_;
      if(singleton->getModelType() == MODEL_TYPE::GAN){
        default_ = 128;
      }
      else{
        default_ = 1;
      }
      return config.getValue("config:batch_size", default_);
    }
    int getNumLayers(){
      return config.getValue("config:num_layers", 7);
    }
    int getKernelSize(){
      return config.getValue("config:kernel_size", 4);
    }
    int getBeta(){
      return config.getValue("config:beta", 50);
    }
    int getLambda(){
      return config.getValue("config:lambda", 100);
    }
    int getLatentVector(){
      return config.getValue("config:latent_vector", 128);
    }
    string getDatasetDir(){
      return config.getValue("config:dataset_dir", "ERROR");
    }


    float getDiscriminatorNoise(){
      return config.getValue("config:disc_noise", 0);
    }
    bool getRandomHorizontal(){

      int i = config.getValue("config:random_horizontal", 0);
      return i == 1;
    }
    bool getRandomVertical(){
      int i = config.getValue("config:random_vertical", 0);
      return i == 1;
    }
    int getRandomCrop(){
      return config.getValue("config:random_crop", 0);
    }
    string getRandomBrightness(){
      return config.getValue("config:random_brightness", "0");
    }

    string getRandomContrast(){
      return config.getValue("config:random_contrast", "0");
    }

    // all purpose get (useful for temp variables)
    // only used by dataset creation screen atm to store portential dataset
    string get(string val){
        return config.getValue("config:"+val, "ERROR");
    }


  private:
    ofxXmlSettings config;
    ModelManager(){};

};

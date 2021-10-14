#pragma once
// reads config.xml
#include "sceneTypes.h"

#include "ofxXmlSettings.h"
class ModelManager{
  public:

    static ModelManager * singleton;

    static ModelManager *  getInstance(){
      if(singleton == nullptr){
        singleton = new ModelManager();
      }
      return singleton;
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

  private:
    ofxXmlSettings config;
    ModelManager(){};

};

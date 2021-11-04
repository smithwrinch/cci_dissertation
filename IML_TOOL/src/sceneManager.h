#pragma once

#include <vector>
#include "baseScene.h"
#include "sceneTypes.h"
class SceneManager{

  public:

    static SceneManager * singleton;

    static SceneManager *  getInstance(){
      if(singleton == nullptr){
        singleton = new SceneManager();
      }
      return singleton;
    }
    void addScene(BaseScene * scene){
        scenes.push_back(scene);
    }
    void changeSceneTo(SCENE_TYPE::ID id){
      for(int i = 0; i < scenes.size(); i++){
        BaseScene * scene = scenes[i];
        if(scene->getID() == id){
          currentScene = scene;
          break;
        }
      }
    }
    int getNumScenesAdded(){
      return scenes.size();
    }
    BaseScene * getCurrentScene(){
      return currentScene;
    }
    SCENE_TYPE::ID getCurrentSceneID(){
      return currentScene->getID();
    }
    BaseScene * getScene(SCENE_TYPE::ID id){
      for(int i = 0; i < scenes.size(); i++){
        BaseScene * scene = scenes[i];
        if(scene->getID() == id){
          return scene;
        }
      }
      // lol
      return nullptr;
    }
    bool showNavBar(){
      return showNavBar;
    }
    bool setNavBar(bool b){
      showNavBar = b;
    }
  private:
    SceneManager(){
      // as singleton
    };
    BaseScene * currentScene;
    vector<BaseScene*> scenes;
    bool showNavBar = false;
};

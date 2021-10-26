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
      cout << "Tried changing scene to " << id << " but it doesn't exist..." << endl;
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
  private:
    SceneManager(){
      // as singleton
    };
    BaseScene * currentScene;
    vector<BaseScene*> scenes;
};

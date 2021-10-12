#pragma once

#include <vector>
#include "baseScene.h"
#include "sceneTypes.h"
class SceneManager{
  public:
    static SceneManager& getInstance(){
        static SceneManager theInstance;
        return theInstance;
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
    BaseScene * getCurrentScene(){
      return currentScene;
    }
    SCENE_TYPE::ID getCurrentSceneID(){
      return currentScene->getID();
    }
  private:
    SceneManager(){

    };
    BaseScene * currentScene;
    vector<BaseScene*> scenes;
};

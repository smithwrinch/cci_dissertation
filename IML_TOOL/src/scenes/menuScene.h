#include "../baseScene.h"
#include <filesystem> // foe removing fules revursively
#include "../sceneManager.h"
class MenuScene : public BaseScene{
  public:
    void setup();
    void update();
    void draw();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:
    SceneManager * sceneManager = SceneManager::getInstance();
    ofxDatGuiButton* newModelButton;
    ofxDatGuiButton* loadModelButton;
    ofxDatGuiButton* playButton;
    ofxDatGuiButton* interactButton;

    ofxDatGuiTextInput* modelNameInput;
    ofxDatGuiButton* createModelButton;
    ofxDatGuiButton* backButton;
    ofxDatGuiLabel* errorLabel;

    ofxDatGuiLabel* modelTypeLabel;

    ofxDatGuiButton* ganButton;
    ofxDatGuiLabel* ganLabel;

    ofxDatGuiButton* pix2pixButton;
    ofxDatGuiLabel* pix2pixLabel;

    ofxDatGuiButton* customButton;
    ofxDatGuiLabel* customLabel;

    ofxDatGuiScrollView* modelScroll;
    ofxDatGuiButton* scrollDeleteButton;
    ofxDatGuiButton* scrollContinueButton;
    string currentScroll = "";


    void onScrollViewEvent(ofxDatGuiScrollViewEvent e);
    void populateScroll();
    void refreshScroll();
    void createModel(MODEL_TYPE::ID);
    bool checkFnameNew();
    bool checkTextValid();

    void deleteModel(string name){
      filesystem::remove_all("../bin/data/saved_models/"+name);
      cout << "saved_models/"+name << endl;
    };
    int state = 0; // 0 for start, 1 for save, 2 for load, 3 for model type selection



};

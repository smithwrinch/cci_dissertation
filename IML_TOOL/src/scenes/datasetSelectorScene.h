#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"
#include "trainScene.h"
#include <filesystem>

class DatasetSelectorScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void refresh();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:


    ofxDatGuiScrollView* datasetScroll;
    ofxDatGuiButton* scrollDeleteButton;
    ofxDatGuiButton* scrollContinueButton;
    ofxDatGuiButton* backButton;

    string currentScroll = "";


    void onScrollViewEvent(ofxDatGuiScrollViewEvent e);
    void populateScroll();
    void refreshScroll();
};

/*
 img width 256
 img height 256
 img size 28
 lr 2-5 // 1-4
 batch size 1 // 128
 layers 7
 latent dim 128
 kernel size 4
 beta 50
 lambda 100
*/

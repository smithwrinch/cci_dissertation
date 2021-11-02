#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"

class InteractMenuScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void refresh();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:
    //GAN
    vector <ofxDatGuiButton*> ganButtons;
    ofxDatGuiButton * exploreLatentSpaceButton;
    ofxDatGuiButton * musicSyncButton;
    ofxDatGuiButton * motionDetectionButton;

    //P2P
    vector <ofxDatGuiButton*> p2pButtons;
    ofxDatGuiButton * mlInputButton;
    ofxDatGuiButton * drawButton;
    ofxDatGuiButton * webcamInputButton;
    ofxDatGuiButton * videoInputButton;


    ofxDatGuiButton * exportTrainingGifButton;

    ofxDatGuiButton * backButton;
};

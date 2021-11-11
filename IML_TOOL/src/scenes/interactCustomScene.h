#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"

class InteractCustomScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:
    void toInteract();
    ofxDatGuiButton * pix2pixButton;
    ofxDatGuiLabel * pix2pixLabel;
    ofxDatGuiButton * ganButton;
    ofxDatGuiLabel * ganLabel;
    ofxDatGuiLabel * modelTypeLabel;
    ofxDatGuiButton * backButton;

    ofxDatGuiSlider * imgSizeSlider;
    ofxDatGuiSlider * latentDimSlider;
    ofxDatGuiSlider * imgWidthSlider;
    ofxDatGuiSlider * imgHeightSlider;

    ofxDatGuiToggle * grayscaleToggle;
    ofxDatGuiButton * continueButton;
    ofxDatGuiButton * setFolderButton;

    string msg;
    string dir;

    int state = 0; // 0 model type; 1 gan; 2 p2p

};

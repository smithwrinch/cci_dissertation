#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"
#include "trainScene.h"
#include<cmath>
class ArchitectureBasicBuilderScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:
    void setModel();

    ofxDatGuiSlider* imgWidthSlider;
    ofxDatGuiSlider* imgHeightSlider;
    ofxDatGuiToggle* inputRGBToggle;
    ofxDatGuiToggle* outputRGBToggle;
    ofxDatGuiSlider* learningRateSlider;
    ofxDatGuiSlider* learningRateSlider2;
    ofxDatGuiSlider* maxEpochsSlider;
    ofxDatGuiSlider* batchSizeSlider;
    ofxDatGuiSlider* numLayersSlider;
    // ofxDatGuiButton advancedButton; // to hide/show advanced features
    ofxDatGuiSlider* kernelSizeSlider;
    ofxDatGuiSlider* betaSlider;
    ofxDatGuiSlider* lambdaSlider;


    ofxDatGuiSlider * imgSizeSlider;
    ofxDatGuiSlider * latentDimSlider;

    ofxDatGuiButton* backButton;
    ofxDatGuiButton* continueButton;
};

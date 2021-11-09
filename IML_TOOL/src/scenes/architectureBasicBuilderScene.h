# pragma once
#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"
#include "trainScene.h"
#include<cmath>

class ArchitectureBasicBuilderScene : public BaseScene {
  public:
    void refresh();
    void setup();
    void update();
    void draw();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:
    void onToggleEvent(ofxDatGuiToggleEvent e);
    void setModel();

    int getImageSize();
    void setImageSizeChecked();

    ofxDatGuiToggle*  button28;
    ofxDatGuiToggle * button64G;

    ofxDatGuiToggle*  button32;
    ofxDatGuiToggle*  button64;
    ofxDatGuiToggle * button128;
    ofxDatGuiToggle * button256;
    ofxDatGuiToggle * button512;
    ofxDatGuiToggle * button1024;


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


    ofxDatGuiSlider* discriminatorNoiseSlider;
    ofxDatGuiToggle* randomHorizontalToggle;
    ofxDatGuiToggle* randomVerticalToggle;
    ofxDatGuiSlider* cropSlider;
    ofxDatGuiSlider* brightnessSlider;
    ofxDatGuiSlider* contrastSlider;

    ofxDatGuiButton * showAdvanced;
    bool showingAdvanced = false;

    ofxDatGuiButton* backButton;
    ofxDatGuiButton* continueButton;

    vector<ofxDatGuiToggle*> ganButtons;
    vector<ofxDatGuiToggle*> pix2pixButtons;

    const vector<int> pix2pixSize = {32, 64, 128, 256, 512, 1024};
    const vector<int> ganSize = {28, 64};

};

#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"

class DatasetManualBuilderScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:

    vector <ofxDatGuiComponent*> gui;
    ofxDatGuiButton * buildButton;
    ofxDatGuiButton * manualButton;
    ofxDatGuiButton * backButton;

    ofxDatGuiLabel * label1;
    ofxDatGuiLabel * label2;

    ofxDatGuiTextInput * inputText;
    ofxDatGuiTextInput * outputText;
};

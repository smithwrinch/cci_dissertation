#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"
#include "architectureBasicBuilderScene.h"

class ArchitectureTypeSelectScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void refresh();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:
    ofxDatGuiButton * buildButton;
    ofxDatGuiButton * selectButton;


    ofxDatGuiButton * rebuildArchitectureButton;
    ofxDatGuiButton * confirmButton;
    ofxDatGuiButton * unconfirmButton;
    int state;
};

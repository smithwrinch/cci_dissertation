#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"

class ArchitectureTypeSelectScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:
    ofxDatGuiButton * buildButton;
    ofxDatGuiButton * selectButton;
};

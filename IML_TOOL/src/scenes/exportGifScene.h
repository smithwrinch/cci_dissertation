#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"
#include "architectureBasicBuilderScene.h"

class ExportGifScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void refresh();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:
    bool checkValid();
    ofxDatGuiLabel * errorLabel;
    ofxDatGuiButton * outputFolderButton;
    ofxDatGuiButton * buildButton;
    ofxDatGuiButton * backButton;

    vector <ofxDatGuiComponent*> gui;

    string gifDir;
    string imgsDir;
};

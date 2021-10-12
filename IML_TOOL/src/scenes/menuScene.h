#include "../baseScene.h"

class MenuScene : public BaseScene{
  public:
    void setup();
    void update();
    void draw();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:
    ofxDatGuiButton* newModelButton;
    ofxDatGuiButton* loadModelButton;

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

    void populateScroll();
    void createModel(MODEL_TYPE::ID);
    bool checkFnameNew();
    bool checkTextValid();

    int state = 0; // 0 for start, 1 for save, 2 for load, 3 for model type selection

};

#include "../baseScene.h"
#include "../sceneManager.h"
#include "../sceneTypes.h"
#include "inputPix2PixScene.h"
#include "inputGANScene.h"

class PlayModelSelectorScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void onButtonEvent(ofxDatGuiButtonEvent e);
    void mousePressed(int x, int y, int button);

  private:
    ofImage edges2shoesImg;
    ofImage landscapeGANImg;
    ofxDatGuiButton * edges2shoesButton;
    ofxDatGuiButton * landscapeGANButton;

    ofxDatGuiButton* backButton;

    int imSize = 256;
};

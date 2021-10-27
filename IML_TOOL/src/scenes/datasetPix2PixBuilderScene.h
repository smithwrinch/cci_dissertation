#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"

class DatasetPix2PixBuilderScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void refresh();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:
    ofImage im1;
    ofImage im2;
    ofImage im3;
    ofImage im4;
    ofDirectory d;
};

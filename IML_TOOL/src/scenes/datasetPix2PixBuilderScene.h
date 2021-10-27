#include "../baseScene.h"
#include "../sceneManager.h"
#include "../model.h"
#include "../sceneTypes.h"
#include "ofxOpenCv.h"

class DatasetPix2PixBuilderScene : public BaseScene {
  public:
    void setup();
    void update();
    void draw();
    void refresh();
    void onButtonEvent(ofxDatGuiButtonEvent e);

  private:
    ofImage im1;
    ofxCvColorImage colorImg;
    ofxCvGrayscaleImage grayImage, grayBg, grayDiff;
    ofxCvContourFinder contourFinder;
    ofDirectory d;

    int width = 400;
    int height = 400;
};

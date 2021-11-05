#include <vector>
#include "math.h"
#include "ofMain.h"
using std::vector;
class LatentGraphWidget {
  public:
    void setup(int x, int y, int w, int h, ofColor colour= ofColor(255,255,255));
    void update();
    void draw();
    void reset();
    void setLatentVector(vector<float> *vec);
  private:
    int x_, y_, width_, height_;
    vector <float> * latentVector;

    int brushRadius = 3;
    void setLatentVectorFromMouse();

    ofColor colour_;

};

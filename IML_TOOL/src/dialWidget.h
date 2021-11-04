#include <vector>
#include "math.h"
#include "ofMain.h"
using std::vector;
class DialWidget {
  public:
    void setup(int x, int y, int radius);
    void update();
    void draw();
    void reset();
    void setLatentVector(vector<float> *vec);
  private:
    void setLatentVectorFromMouse();

    int centreX, centreY, radius;
    vector <float> * latentVector;
    float startTheta;
    float endTheta;
    bool mouseBeenPressed = false;
    bool mouseInArea = false;
    int offsetIndexTarget;
    int offsetIndex;

    float line1X;
    float line1Y;
    float line2X;
    float line2Y;
};
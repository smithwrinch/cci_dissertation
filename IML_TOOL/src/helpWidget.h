#pragma once

#include <vector>
#include "math.h"
#include "ofMain.h"
using std::vector;
class HelpWidget {
  public:
    // TODO overload setup to take ofxDatGuiComponent pointer.
    void setup(int x, int y, string msg, int w=25, int h=25);
    void draw();
    void reset();
  private:
    int x_, y_, width_, height_;
    string msg_;
    ofImage logo;

    const int width = 250;
    int height = 400;

};

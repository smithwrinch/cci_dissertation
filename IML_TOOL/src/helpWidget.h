#pragma once

#include <vector>
#include "math.h"
#include "ofMain.h"
using std::vector;
class HelpWidget {
  public:
    void setup(int x, int y, string msg, int w=25, int h=25);
    // void update();
    void draw();
    void reset();
  private:
    int x_, y_, width_, height_;
    string msg_;
    ofImage logo;

    const int width = 200;
    const int height = 100;

};

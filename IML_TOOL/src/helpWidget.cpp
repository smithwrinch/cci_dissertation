#include "latentGraphWidget.h"

void HelpWidget::setup(int x, int y, string msg, int w, int h){
  x_ = x;
  y_ = y;
  width_ = w;
  height_ = h;
  msg_ = msg;
  logo.load("images/logo.png");
  logo.update();
}

// void HelpWidget::update(){
//
// }


void HelpWidget::draw(){
  logo.draw(x_, y_, width_, height_);
  if(ofGetMouseX() > x_ && ofGetMouseX() < x_ + width_){
    if(ofGetMouseY() > y_ && ofGetMouseY() < y_ + height_){
      ofDrawRectangle(ofGetMouseX(), ofGetMouseY(), width ,height);
      ofDrawBitmapString(msg_, x_+10, y_+10);
    }
  }
}

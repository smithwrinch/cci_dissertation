#include "helpWidget.h"

void HelpWidget::setup(int x, int y, string msg, int w, int h){
  x_ = x;
  y_ = y;
  width_ = w;
  height_ = h;
  msg_ = msg;
  logo.load("images/logo.png");
  logo.update();
  // 31
  int size = msg_.size();
  int cnt = 0;
  for (int i = 0; i < size; i++){
    if(i % 29 == 0 && i !=0){
      msg_.insert(i+cnt, "\n");
      cnt ++;
    }
  }
  height = (cnt+1)*14;
}

// void HelpWidget::update(){
//
// }


void HelpWidget::draw(){
  logo.draw(x_, y_, width_, height_);
  if(ofGetMouseX() > x_ && ofGetMouseX() < x_ + width_){
    if(ofGetMouseY() > y_ && ofGetMouseY() < y_ + height_){
      ofPushStyle();
      ofSetColor(20,20,20);
      ofDrawRectangle(ofGetMouseX(), ofGetMouseY(), width ,height);
      ofSetColor(255,255,255);
      ofDrawBitmapString(msg_, ofGetMouseX()+10, ofGetMouseY()+10);
      ofPopStyle();
    }
  }
}

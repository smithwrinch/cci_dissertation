#include "latentGraphWidget.h"

void LatentGraphWidget::setup(int x, int y, int w, int h, ofColor c, bool half, int max){
  x_ = x;
  y_ = y;
  width_ = w;
  height_ = h;
  colour_ = c;
  isHalf_ = half;
  max_ = max;
}

void LatentGraphWidget::update(){
  if(ofGetMousePressed()){
    setLatentVectorFromMouse();
  }

}


void LatentGraphWidget::setLatentVector(vector<float> *vec){
  latentVector = vec;
}

void LatentGraphWidget::draw(){

    ofSetColor(colour_);
    ofDrawBitmapString("0", x_ -15, y_ + height_/2);
    ofDrawBitmapString(ofToString(max_), x_ -15, y_);
    ofDrawBitmapString(ofToString(-max_), x_ -15, y_ + height_- 5);

   ofDrawRectangle(x_, y_, width_, height_);

   if(isHalf_){
     // center line
     ofSetColor(127, 127, 127);
     ofDrawLine(x_, y_+height_/2, x_+width_, y_+height_/2);
     ofSetColor(colour_);
   }

   int size = latentVector->size();
   for(int i = 0; i < size; i ++){
     float radius_ = latentVector->at(i);
     float x = i*width_/size;

     if(isHalf_){
      ofDrawLine(x + x_, y_ +height_ , x+x_, y_ + (0.5-radius_/(2*max_)) * height_);
     }
     else{

     ofDrawLine(x + x_, y_ +height_ , x+x_, y_ + (1-(radius_/max_)) * height_);
      }

   }


   float x = ofGetMouseX();
   float y = ofGetMouseY();
   if(x >= x_ && x <= x_+width_){
       if(y >= y_ && y <= y_ +height_){
        ofDrawCircle(x, y, brushRadius);
     }
   }

}

void LatentGraphWidget::setLatentVectorFromMouse(){
    float x = ofGetMouseX();
    float y = ofGetMouseY();
    int size = latentVector->size();
    if(x >= x_ && x <= x_+width_){
        if(y >= y_ && y <= y_ +height_){
          // in graph box
          float new_y = (y - y_)/height_;
          int idx = ((x-x_)*size / width_);

          for(int i =1; i <= brushRadius; i++){
            int new_idx = idx + i;
            int new_idx2 = idx - i;
            if(new_idx < 0){
              new_idx = 0;
            }
            if(new_idx >= size){
              new_idx = size - 1;
            }
            if(new_idx2 < 0){
              new_idx2 = 0;
            }
            if(new_idx2 >= size){
              new_idx2 = size - 1;
            }
            if(isHalf_){
              latentVector->at(new_idx) = ((1- new_y)* 2*max_) - max_;
              latentVector->at(new_idx2) = ((1- new_y)* 2*max_) - max_;
            }
            else{
              latentVector->at(new_idx) = (1- new_y);
              latentVector->at(new_idx2) = (1- new_y);
            }

        }

        }
    }
}

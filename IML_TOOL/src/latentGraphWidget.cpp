#include "latentGraphWidget.h"

void LatentGraphWidget::setup(int x, int y, int w, int h){
  x_ = x;
  y_ = y;
  width_ = w;
  height_ = h;
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

   ofDrawRectangle(x_, y_, width_, height_);

   int size = latentVector->size();
   for(int i = 0; i < size; i ++){
     float radius_ = latentVector->at(i);

     float x = i*width_/size;

     ofDrawLine(x + x_, y_ +height_ , x+x_, y_ + radius_ * height_);
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
            latentVector->at(new_idx) = new_y;
            latentVector->at(new_idx2) = new_y;
        }

        }
    }
}

#include "dialWidget.h"

void DialWidget::setup(int x, int y, int r, ofColor c, bool half, int m){
  centreX = x;
  centreY = y;
  radius = r;
  colour = c;
  isHalf = half;
  max = m;

  offsetIndex = 0;

  mouseAngle = 0;
  mouseLineX = radius * cos(mouseAngle);
  mouseLineY = radius * sin(mouseAngle);
}

void DialWidget::update(){

  float x = ofGetMouseX();
  float y = ofGetMouseY();
  float x_ = x - centreX;
  float y_ = y - centreY;

  if(sqrt(x_*x_ + y_*y_) <= radius){
    // in circle
    mouseInArea = true;
    mouseAngle = atan(y_/x_);
    if(x_ < 0){
      mouseAngle = mouseAngle + PI;
    }
    mouseLineX = radius * cos(mouseAngle);
    mouseLineY = radius * sin(mouseAngle);

    if(ofGetMousePressed()){
      if(mouseBeenPressed){

      }
      else{
        mouseBeenPressed = true;
        startTheta = atan(y_/x_);
        if(x_ < 0){
          startTheta = startTheta + PI;
        }
      }
    }
    else{
      if(mouseBeenPressed){

        endTheta = atan(y_/x_);
        if(x_ < 0){
          endTheta = endTheta + PI;
        }
        setLatentVectorFromMouse();
        mouseBeenPressed = false;
      }
    }
  }
  else{
    startTheta = endTheta = 0;
    mouseInArea = false;
  }
}

void DialWidget::setLatentVector(vector<float> *vec){
  latentVector = vec;
}

void DialWidget::draw(){
   ofDrawCircle(centreX, centreY, radius);

   int size = latentVector->size();
   for(int i = 0; i < size; i ++){
     float radius_ = latentVector->at(i);
     float theta = ((PI*2) / size);
     float angle = (theta * i);

     if(isHalf){
        float abs_radius = abs(radius_);
        float x = (radius/max) * (0.5 + abs_radius/2) * cos(angle);
        float y = (radius/max) * (0.5 + abs_radius/2) * sin(angle);
        int coeff = 1;
        if(radius_ < 0){
          coeff = -1;
          ofSetColor(0, 0, 0);
        }
        x*= coeff;
        y*= coeff;
        ofDrawLine(centreX,centreY, x+centreX, y+centreY);
        ofSetColor(colour);
     }
     else{
      float x = (radius/max) * radius_ * cos(angle);
      float y = (radius/max) * radius_ * sin(angle);
      ofDrawLine(centreX,centreY, x+centreX, y+centreY);
     }
   }

   // if(isHalf){
      // ofSetColor(127, 127, 127);
      // ofDrawCircle(centreX, centreY, radius/2);
      // ofSetColor(colour);

   // }

   if(mouseInArea){
      ofSetColor(ofColor(255, 215, 0));
      float x = radius * cos(startTheta);
      float y = radius * sin(startTheta);
      ofDrawLine(centreX,centreY, x+centreX, y+centreY);


      ofDrawLine(centreX,centreY, mouseLineX+centreX, mouseLineY+centreY);

      ofSetColor(ofColor(255, 155, 0));
      float x_ = radius * cos(endTheta);
      float y_ = radius * sin(endTheta);
      ofDrawLine(centreX,centreY, x_+centreX, y_+centreY);

      ofSetColor(ofColor(255, 215, 0, 50));
      ofFill();
      ofDrawTriangle(centreX, centreY, x_+centreX , y_+centreY, x+centreX, y+centreY);
      ofNoFill();
      ofSetColor(colour);


   }

}

void DialWidget::setLatentVectorFromMouse(){
  //passed as argument to save recalculating
  float theta = startTheta - endTheta;

  offsetIndexTarget = int(theta*latentVector->size() / (2*PI)) % latentVector->size();

  rotate(latentVector->begin(), latentVector->begin() + offsetIndexTarget, latentVector->end());


}

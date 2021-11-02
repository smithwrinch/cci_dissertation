#include "dialWidget.h"

void DialWidget::setup(int x, int y, int r){
  centreX = x;
  centreY = y;
  radius = r;

  offsetIndex = 0;
}

void DialWidget::update(){

  float x = ofGetMouseX();
  float y = ofGetMouseY();
  float x_ = x - centreX;
  float y_ = y - centreY;

  if(sqrt(x_*x_ + y_*y_) <= radius){
    // in circle
    mouseInArea = true;

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

     float x = radius * radius_ * cos(angle);
     float y = radius * radius_ * sin(angle);
     ofDrawLine(centreX,centreY, x+centreX, y+centreY);
   }

   if(mouseInArea){
      ofSetColor(ofColor(255, 215, 0));
      float x = radius * cos(startTheta);
      float y = radius * sin(startTheta);
      ofDrawLine(centreX,centreY, x+centreX, y+centreY);


      ofSetColor(ofColor(255, 155, 0));
      float x_ = radius * cos(endTheta);
      float y_ = radius * sin(endTheta);
      ofDrawLine(centreX,centreY, x_+centreX, y_+centreY);

      ofSetColor(ofColor(255, 215, 0, 50));
      ofFill();
      ofDrawTriangle(centreX, centreY, x_+centreX , y_+centreY, x+centreX, y+centreY);
      ofNoFill();
      ofSetColor(ofColor(255,255,255));


   }

}

void DialWidget::setLatentVectorFromMouse(){
  //passed as argument to save recalculating
  float theta = startTheta - endTheta;

  offsetIndexTarget = int(theta*latentVector->size() / (2*PI)) % latentVector->size();


  rotate(latentVector->begin(), latentVector->begin() + offsetIndexTarget, latentVector->end());


}

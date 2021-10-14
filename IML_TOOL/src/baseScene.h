#pragma once
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxDatGui.h"
#include "sceneTypes.h"
#include "model.h"

#include "ofxXmlSettings.h"
#include <stdlib.h>
#include <math.h>

class BaseScene{
	public:
		virtual void setup(){};
		virtual void update(){};
		virtual void draw(){};
    virtual SCENE_TYPE::ID getID(){
      return id;
    }
    virtual void setID(SCENE_TYPE::ID a){
      id = a;
    }
    // virtual void onButtonEvent(ofxDatGuiButtonEvent e) {};
		virtual void keyPressed(int key){};
		// virtual void keyReleased(int key);
		// virtual void mouseMoved(int x, int y);
		virtual void mouseDragged(int x, int y, int button){};
		virtual void mousePressed(int x, int y, int button){};
		virtual void mouseReleased(int x, int y, int button){};
		virtual void mouseEntered(int x, int y){};
		virtual void mouseExited(int x, int y){};
		// virtual void windowResized(int w, int h);
		virtual void dragEvent(ofDragInfo dragInfo){};
		// virtual void gotMessage(ofMessage msg);
    private:
      SCENE_TYPE::ID id;
		// VectorField vectorField;
    // vector<Particle *> particles;
		// ofImage img; //for screenshots

		// int maxParticles = 20000;
};

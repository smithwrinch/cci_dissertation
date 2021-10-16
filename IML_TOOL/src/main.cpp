#define MACRO_VARIABLE_TO_STRING(Variable) (void(Variable),#Variable)

#include "ofMain.h"
#include "ofApp.h"
#include "sceneManager.h"
#include "model.h"

SceneManager * SceneManager::singleton = nullptr;
ModelManager * ModelManager::singleton = nullptr;

// ofColor ganColor = ofColor();
// ofColor pix2pixColor = ofColor(2,132,30);
// ofColor customColor = ofColor();

//========================================================================
int main( ){

	ofGLFWWindowSettings settings;
	settings.resizable = false;
	settings.setSize(1024, 768);
	ofCreateWindow(settings);
	// ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context
	// glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}

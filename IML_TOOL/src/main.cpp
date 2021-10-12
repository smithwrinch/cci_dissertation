#include "ofMain.h"
#include "ofApp.h"


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

#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "cv.h"
#include "cvaux.h"
#include "ofxGui.h"

// uncomment this to read from two kinects simultaneously
//#define USE_TWO_KINECTS

class testApp : public ofBaseApp {
public:

	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

	ofxKinect kinect;

	ofxCvColorImage colorImg;

	ofxCvGrayscaleImage grayImage; // grayscale depth image
	ofxCvGrayscaleImage grayThreshNear; // the near thresholded image
	ofxCvGrayscaleImage grayThreshFar; // the far thresholded image

	ofxCvGrayscaleImage grayScaleImg;

	ofxCvContourFinder contourFinder;

	bool bThreshWithOpenCV;

	int nearThreshold;
	int farThreshold;

	int angle;

	ofShader maskShader;

	ofFbo myFbo;

    CvBGStatModel* gauss_bgModel;
    ofxCvGrayscaleImage     gauss_foregroundImg;
    ofxCvColorImage     gauss_backgroundImg;

    CvBGStatModel* fgd_bgModel;
    ofxCvGrayscaleImage     fgd_foregroundImg;
    ofxCvColorImage     fgd_backgroundImg;
    CvGaussBGStatModelParams* params;

    ofxFloatSlider slider;
};

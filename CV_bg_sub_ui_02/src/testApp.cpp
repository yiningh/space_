#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	//ofSetLogLevel(OF_LOG_VERBOSE);
	ofEnableAlphaBlending();

	kinect.setRegistration(true);
	kinect.init();
	kinect.open();

	if(kinect.isConnected()) {
		ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
	}

	colorImg.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	grayThreshNear.allocate(kinect.width, kinect.height);
	grayThreshFar.allocate(kinect.width, kinect.height);
	grayScaleImg.allocate(kinect.width, kinect.height);
	diffImage.allocate(kinect.width, kinect.height);
	grayImage.allocate(kinect.width, kinect.height);
	cvGrayImage.allocate(kinect.width, kinect.height);
	bgImage.allocate(kinect.width, kinect.height);


	nearThreshold = 230;
	farThreshold = 70;
	bThreshWithOpenCV = false;

	ofSetFrameRate(60);

	angle = 0;
	kinect.setCameraTiltAngle(angle);
	myFbo.allocate(400, 300);

	maskShader.load("composite");
	maskShader.setUniformTexture("Tex0", myFbo.getTextureReference(), 0);
    maskShader.end();
	maskShader.begin();
    maskShader.setUniformTexture("Tex1", grayScaleImg.getTextureReference(), 1);

    //GUI
    gui.setup();
    gui.add(dil.setup("dilate", 0, 0, 6, 300, 10));
    gui.add(blur.setup("blur", 0, 0, 25, 300, 10));
    gui.add(bgThres.setup("bg_thres", 50, 0.0, 100.0, 300,10));
    gui.add(darken.setup("darken", 0, 0, 100, 300,10));
    gui.loadFromFile("settings.xml");
    bGetBackground = true;
}

//--------------------------------------------------------------
void testApp::update() {
	ofBackground(100, 100, 100);
	kinect.update();

	if(kinect.isFrameNew()) {
		grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
		grayScaleImg.setFromPixels(kinect.getDepthPixels(), kinect.width,kinect.height);

		colorImg.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
		cvGrayImage = colorImg;

        if(bGetBackground){
            bgImage = cvGrayImage;
            bGetBackground = false;
        }

		if(bThreshWithOpenCV) {
			grayThreshNear = grayImage;
			grayThreshFar = grayImage;
			grayThreshNear.threshold(nearThreshold, true);
			grayThreshFar.threshold(farThreshold);
			cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
		} else {
		    diffImage.absDiff(bgImage, cvGrayImage);
            diffImage.threshold(bgThres);
            diffImage.invert();

            for(int i = 0; i < dil; i++){
                grayImage.dilate();
			}
			grayImage.blurGaussian( blur*2 + 1 );

			unsigned char * pix = grayImage.getPixels();
			unsigned char * diffPix = diffImage.getPixels();

			int numPixels = grayImage.getWidth() * grayImage.getHeight();
			for(int i = 0; i < numPixels; i++) {
				if(pix[i] < nearThreshold && pix[i] > farThreshold) {
                    pix[i] = pix[i] - darken*2;
				} else {
					pix[i] = 255;
				}
			}
			for(int i = 0; i < numPixels; i++) {
                if( diffPix[i] == 255 ){
                    diffPix[i] = pix[i];
                }else{
                    diffPix[i] = diffPix[i];
                }
            }
		}
	}
}

//--------------------------------------------------------------
void testApp::draw() {
    ofSetWindowTitle("FPS: " + ofToString(ofGetFrameRate()));
	ofSetColor(255, 255, 255);

    myFbo.begin();
    ofClear(255,255,255,0);
    //grayScaleImg.draw(0, 0, 400, 300); //kinect.drawDepth
    myFbo.end();
    myFbo.draw(10, 10);
    kinect.draw(420, 10, 400, 300);

    grayImage.draw(10, 320, 400, 300);
    //contourFinder.draw(10, 320, 400, 300);

    grayScaleImg.draw(420,320,400,300);
    colorImg.draw(420, 320, 400, 300);

    maskShader.begin();
        glActiveTexture(GL_TEXTURE0_ARB);
        myFbo.getTextureReference().bind();

        glActiveTexture(GL_TEXTURE1_ARB);
        diffImage.getTextureReference().bind();
        glBegin(GL_QUADS);

        glMultiTexCoord2d(GL_TEXTURE0_ARB, 0, 0);
        glMultiTexCoord2d(GL_TEXTURE1_ARB, 0, 0);
        glVertex2f(420,320);
        glMultiTexCoord2d(GL_TEXTURE0_ARB, myFbo.getWidth(), 0);
        glMultiTexCoord2d(GL_TEXTURE1_ARB, grayImage.getWidth(), 0);
        glVertex2d(820, 320);
        glMultiTexCoord2d(GL_TEXTURE0_ARB, myFbo.getWidth(), myFbo.getHeight());
        glMultiTexCoord2d(GL_TEXTURE1_ARB, grayImage.getWidth(), grayImage.getHeight());
        glVertex2d(820, 620);
        glMultiTexCoord2d(GL_TEXTURE0_ARB, 0, myFbo.getHeight());
        glMultiTexCoord2d(GL_TEXTURE1_ARB, 0, grayImage.getHeight());
        glVertex2f(420,620);

        glEnd();

        glActiveTexture(GL_TEXTURE1_ARB);
        diffImage.getTextureReference().unbind();
        glActiveTexture(GL_TEXTURE0_ARB);
        myFbo.getTextureReference().unbind();

    maskShader.end();

    ofPushMatrix();{
        ofTranslate(820, 10);
        diffImage.draw(0,320,400,300);
        /*
        for(int i=0; i<contourFinder.nBlobs; i++){
            ofSetColor(255,0,0);
            ofxCvBlob& blob = contourFinder.blobs[i];
            ofBeginShape();
            for(int j=0; j<blob.pts.size(); j++){
                ofVertex(blob.pts[j]);
            }
            ofEndShape();
        }
        */
    }ofPopMatrix();

    gui.draw();
	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;

    if(kinect.hasAccelControl()) {
        reportStream << "accel is: " << ofToString(kinect.getMksAccel().x, 2) << " / "
        << ofToString(kinect.getMksAccel().y, 2) << " / "
        << ofToString(kinect.getMksAccel().z, 2) << endl;
    } else {
        reportStream << "Note: this is a newer Xbox Kinect or Kinect For Windows device," << endl
		<< "motor / led / accel controls are not currently supported" << endl << endl;
    }

	reportStream << "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
	<< "using opencv threshold = " << bThreshWithOpenCV <<" (press spacebar)" << endl
	<< "set near threshold " << nearThreshold << " (press: + -)" << endl
	<< "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
	<< ", fps: " << ofGetFrameRate() << endl
	<< "press c to close the connection and o to open it again, connection is: " << kinect.isConnected() << endl;

    if(kinect.hasCamTiltControl()) {
    	reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
        << "press 1-5 & 0 to change the led mode" << endl;
    }

	ofDrawBitmapString(reportStream.str(), 20, 652);

}


//--------------------------------------------------------------
void testApp::exit() {
    gui.saveToFile("settings.xml");
	kinect.setCameraTiltAngle(0); // zero the tilt on exit
	kinect.close();
}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch (key) {
		case ' ':
			//bThreshWithOpenCV = !bThreshWithOpenCV;
			bGetBackground = true;
			break;

        case 'z':
            bGetBackground = true;
            break;

		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;

		case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;

		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;

		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;

		case 'w':
			kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
			break;

		case 'o':
			kinect.setCameraTiltAngle(angle); // go back to prev tilt
			kinect.open();
			break;

		case 'c':
			kinect.setCameraTiltAngle(0); // zero the tilt
			kinect.close();
			break;

		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;

		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}

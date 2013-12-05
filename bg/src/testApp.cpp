#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(ofColor(ofMap(mouseX, 0, ofGetWindowWidth(), 0 , 255),255,ofMap(mouseY, 0, ofGetWindowHeight(), 0, 255)));
    ofSetColor(0);
    ofCircle(ofGetWindowWidth()/2, ofGetWindowHeight()/2, 40);

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    if (key == 'm' || key == 'M'){
        ofSetFullscreen(true);
    }
    if (key == 'n' || key == 'N'){
        ofSetFullscreen(false);
    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){

}

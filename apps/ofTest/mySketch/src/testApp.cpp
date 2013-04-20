#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
  
  MAX_HANDS = 2;
  MAX_CHANGE = 8;

  x = 0;
  z = 0;
  y_change = 0;
  y = 0.0f;

  for (int i = 0; i < MAX_HANDS; i++) {
    hands[i] = NULL;
  }

  //ofSetLogLevel(OF_LOG_VERBOSE);
  ofSetLogLevel(OF_LOG_NOTICE);

  openNIDevice.setup();
  openNIDevice.addImageGenerator();
  openNIDevice.addDepthGenerator();
  openNIDevice.setRegister(true);
  openNIDevice.setMirror(true);
    
  // setup the hand generator
  openNIDevice.addHandsGenerator();

  // required for wave gesture .. but crashes NI .. eventually
  //openNIDevice.addUserGenerator();
  //openNIDevice.addGestureGenerator();

  // you can use this to get a list of gestures
  // prints to console and/or you can use the returned vector  
  //vector<string> gestureNames = openNIDevice.getAvailableGestures();

  // add all focus gestures (ie., wave, click, raise arm)
	openNIDevice.addAllHandFocusGestures();

  // or you can add them one at a time
  // Wave Click RaiseHand MovingHand
  //openNIDevice.addGesture("Click");
  //openNIDevice.addGesture("Wave");

  openNIDevice.setMaxNumHands(MAX_HANDS);
  // openNIDevice.setMaxNumUsers(1); // only used with skeleton stuff
    
  ofAddListener(openNIDevice.gestureEvent,this,&testApp::handEvent); 

  openNIDevice.start();
      
  verdana.loadFont(ofToDataPath("verdana.ttf"), 10);
}

//--------------------------------------------------------------
void testApp::update(){
  openNIDevice.update();
  int num_hands = openNIDevice.getNumTrackedHands();
  // get tracked hands and stuff them into our array
  for (int i = 0; i < MAX_HANDS; i++) {
    if (i < num_hands) {
      ofxOpenNIHand & hand = openNIDevice.getTrackedHand(i);
      hands[i] = & hand;
    } else {
      hands[i] = NULL;
    }
  }

  // hand 0 will be position. it's the one on the right
  // hand 1 will be for height.. it's the other one

  // if we have both hands, sort them
  if (hands[0] && hands[1]) {
    // order them right to left. f@*# it assume there are 2 at most
    if (hands[0]->getPosition().x < hands[1]->getPosition().x) {
      // sort hands
      ofxOpenNIHand * tmp = hands[1];
      hands[1] = hands[0];
      hands[0] = tmp;
    }
  }

  // if we have 1 hand here, we can update position
  if (hands[0]) {
    x = floor(hands[0]->getPosition().x);
    z = floor(hands[0]->getPosition().y);
  }

  // if we have a second hand
  if (hands[1]) {
    // only for feedback, not used otherwise
    y = hands[1]->getPosition().y; 

    // calculate change for y
    y_norm = hands[1]->getPosition().normalized().y;
    // if outside deadzone
    if (y_norm < 0.25f || y_norm > 0.75f) {
      int y_change = floor(MAX_CHANGE * (y_norm - 0.5f));
      // terrain_modification_function_call_here(x, z, y_change)
    }
  }    
}

//--------------------------------------------------------------
void testApp::draw(){
	//ofSetColor(255, 255, 255);
    
  //ofPushMatrix();
  // draw debug (ie., image, depth, skeleton)
  //openNIDevice.drawDebug();
  //openNIDevice.drawHands();
  openNIDevice.drawDepth();
  //ofPopMatrix();

  //int numUsers = openNIDevice.getNumTrackedUsers();
  //for (int j = 0; j < numUsers; j++){
  //  openNIDevice.drawSkeleton(j);
  //}

  ofSetColor(255,0,0);
  // height
  string msg = ofToString(y) + " ["+ ofToString(y_change) +"]";
	verdana.drawString(msg, 10, y - 5);

  // dot(s)
  ofRect(x -2, z - 2, 4, 4);
  ofRect(9, y - 2, 4, 4);

  // position
  msg = ofToString(x) + ":" + ofToString(z);
	verdana.drawString(msg, x, z - 5);
    
  // draw some info regarding frame counts etc
	ofSetColor(255, 0, 255);
	//string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate()) + " Device FPS: " + ofToString(openNIDevice.getFrameRate());
  msg = "Device FPS: " + ofToString(floor(openNIDevice.getFrameRate()));
	verdana.drawString(msg, 10, openNIDevice.getNumDevices() * 480 - 10);
}

//--------------------------------------------------------------
void testApp::handEvent(ofxOpenNIGestureEvent & event){
  // show hand event messages in the console
	if(event.gestureName == "Wave" || event.gestureName == "Click") {
    ofLogNotice() << event.gestureName;
    // export data .. to csv? .. so you can import it in .. excel?
  }
}

//--------------------------------------------------------------
void testApp::exit(){
    openNIDevice.stop();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

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
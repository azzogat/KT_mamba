#include "testApp.h"
#include "GL\glew.h"

//--------------------------------------------------------------
void testApp::setup() {
  
  MAX_HANDS = 2;
  MAX_CHANGE = 8;
  DIM_X = ofGetWidth();
  DIM_Y = ofGetHeight();
  TOP_MARGIN = 60;  // using margins to correct apparent sensor inaccuracy
  RIGHT_MARGIN = 40;
  LEFT_MARGIN = 50;
  BOTTOM_MARGIN = 10;
  DIM_X_CORRECTED = DIM_X - LEFT_MARGIN - RIGHT_MARGIN;
  DIM_Y_CORRECTED = DIM_Y - TOP_MARGIN - BOTTOM_MARGIN;

  dead_range = 0.2f; // dead zone for input
  live_margin = (1.0f - dead_range) * 0.5f;
  radius = 0.2f;

  // init with 0
  x = y = z = x_norm = y_norm = z_norm = y_change = 0;
  // ..and NULL
  for (int i = 0; i < MAX_HANDS; i++) {
    hands[i] = NULL;
  }

  //ofSetLogLevel(OF_LOG_VERBOSE); // ..no, STFU
  ofSetLogLevel(OF_LOG_NOTICE);

  glewInit();

  if (!glewIsSupported("GL_VERSION_2_0")) {
    float test = 5;
  }
    
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

  terrain = Terrain::Create(20,20,64,64,ofVec3f(0,0,0));
  
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

    // calculate change for y
    y = hands[1]->getPosition().y;
    y_norm = (y - TOP_MARGIN) / DIM_Y_CORRECTED;

    // if outside deadzone
    if (y_norm < live_margin || y_norm > 1.0f - live_margin ) {
      // normalise all values to whatever makes sense..

      // get height values
      y_change = y_norm;  // use y_change to do calculations on y_norm
      if (y_norm > live_margin) y_change -= dead_range; // if y_norm > live_range, it includes dead_range; take it out 
      y_change -= live_margin;  // center the range on 0 (so range is -live_margin .. +live_margin)
      y_change = -y_change * MAX_CHANGE; // reverses y-axis and normalizes to MAX_CHANGE

      // get position values
      // sensor doesn't go lower than ~50 nor higher than ~620 in x
      // sensor doesn't go lower than ~60 nor higher than ~470 in y
      // fix it! ..
      x_norm = (x - LEFT_MARGIN) / DIM_X_CORRECTED;
      z_norm = (z - TOP_MARGIN) / DIM_Y_CORRECTED;

      // terrain_modification_function_call_here(change_in_y, x, z, fancy_radius_thingey)
      terrain->AdjustHeight(y_change, x_norm, z_norm, radius); // normalised radius to window X dimension, because .. why not
    } else {
      // dead zone .. no change (for feedback purposes)
      y_change = 0;
    }
  }    

  terrain->Update();
}

//--------------------------------------------------------------
void testApp::draw(){

  //* DEPTH STUFF
  glPushMatrix();
  ofSetColor(0, 0, 255);
  openNIDevice.drawDepth();
  glPopMatrix();
  //*/

  // TERRAIN
  glMatrixMode(GL_PROJECTION);
	
  glLoadIdentity();
  gluPerspective(60,ofGetWidth()/ofGetHeight(),1,1000);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0,20,20,0,0,0,0,1,0);
  glPushMatrix();
    terrain->Draw();
  glPopMatrix();

	// GUI RELATED CRAP  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
	glOrtho(0, ofGetWidth(), ofGetHeight(), 0, -ofGetHeight(), ofGetHeight());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

  /* GUI CRAP
	  glPushMatrix();
		  m_gui.Draw();
	  glPopMatrix();
  //*/

  //* INPUT FEEDBACK CRAP
    //ofSetColor(255, 255, 255);
    //int numUsers = openNIDevice.getNumTrackedUsers();
    //for (int j = 0; j < numUsers; j++){
    //  openNIDevice.drawSkeleton(j);
    //}
    glPushMatrix();
    ofSetColor(255,0,0);
    // height
    string msg = ofToString(y,2) + " ["+ ofToString(y_norm,2) +" : " + ofToString(y_change,2) + "]";
	  verdana.drawString(msg, 10, y - 5);

    // dot(s)
    ofRect(x -2, z - 2, 4, 4);
    ofRect(9, y - 2, 4, 4);
    ofRect(5, DIM_Y * live_margin - 1, 20, 2); // high threshold
    ofRect(5, DIM_Y * (1.0f - live_margin) - 1, 20, 2); // low threshold

    // position
    msg = ofToString(x) + ":" + ofToString(z);
	  verdana.drawString(msg, x, z - 5);
    
    // draw some info regarding frame counts etc
    msg = "Device FPS: " + ofToString(floor(openNIDevice.getFrameRate()));
	  verdana.drawString(msg, 10, openNIDevice.getNumDevices() * 480 - 10);
    glPopMatrix();
  //*/
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
  if (key == 98 || true) {
    terrain->AdjustHeight(0.5f,0.5f,0.5f,0.1f);
  }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

	// check what the point is over
	KT_PRESSED pressed = m_gui.GetAtPoint(0,ofVec2f(x,y));
	if(pressed != KT_NONE)
	{
		printf("Point over button \n");
		switch( pressed )
		{
			case KT_RAISE:
				// raise terrain
			break;
			case KT_LOWER:
				// lower terrain
			break;
		}
	}
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
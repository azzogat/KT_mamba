#include "testApp.h"
#include "GL\glew.h"

//--------------------------------------------------------------
void testApp::setup() {
  
  MAX_HANDS = 2;
  MAX_CHANGE = 2;

  // using margins to correct apparent sensor inaccuracy (could be unique to device)
  // sensor doesn't go lower than ~50 nor higher than ~620 in x
  // sensor doesn't go lower than ~60 nor higher than ~470 in y
  margin[0] = 0.125f;       // top
  margin[1] = 0.03125f;     // right
  margin[2] = 0.02083333f;  // bottom
  margin[3] = 0.078125f;    // left

  ofWidth = ofGetWidth();
  ofHeight = ofGetHeight();
  xDimension = 1.0f - margin[3] - margin[1];
  yDimension = 1.0f - margin[0] - margin[2];

  radius = 0.2f;
  deadZone = 0.2f; // dead zone for input
  liveLower = (1.0f - deadZone) * 0.5f;
  liveUpper = (1.0f + deadZone) * 0.5f;

  // init with 0
  x = y = z = yChange = 0;
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
  // depth threshold doesn't seem to work .. or just doesn't work the way i think it deos
  //depthThreshold = ofxOpenNIDepthThreshold(0, 0, false, true, true, true, true);
  //openNIDevice.addDepthThreshold(depthThreshold);

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
  //openNIDevice.addGesture("Wave");

  openNIDevice.setMaxNumHands(MAX_HANDS);
  // openNIDevice.setMaxNumUsers(1); // only used with skeleton stuff
    
  ofAddListener(openNIDevice.gestureEvent,this,&testApp::handEvent); 

  openNIDevice.start();

  verdana.loadFont(ofToDataPath("verdana.ttf"), 8);

  terrain = Terrain::Create(20,20,64,64,ofVec3f(0,0,0));
  
}

//--------------------------------------------------------------
void testApp::update(){
  openNIDevice.update();

  // NOTE:
  // openNIDevice.getNumTrackedHands() often returns a number greater
  // than actual hands in the array of hands OpenNI is tracking...
  // ... this �@!#$ up on getTrackeHand(messed_up_index) ...

  // get tracked hands and stuff them into our array
  for (int i = 0; i < MAX_HANDS; i++) {
    if (i < openNIDevice.getNumTrackedHands()) {
      ofxOpenNIHand & hand = openNIDevice.getTrackedHand(i);
      hands[i] = & hand;
      // thought depth threshold would make it more accurate or faster
      // instead, it seems to .. doesn't (slower & no perceivable limit imposed)
      //if(!i) {
      //  ofxOpenNIDepthThreshold &dt = openNIDevice.getDepthThreshold(i);
      //  dt.setNearThreshold(hand.getWorldPosition().z-50);
      //  dt.setFarThreshold(hand.getWorldPosition().z+50);
      //}
    } else {
      hands[i] = NULL;
    }
  }

  // hand 0 will be position. it's the one on the right
  // hand 1 will be for height.. it's the other one

  // if we have both hands, sort them right to left
  if (hands[0] && hands[1]) {
    // f@*# it assume there are exactly 2 at this point
    if (hands[0]->getPosition().x < hands[1]->getPosition().x) {
      // sort hands
      ofxOpenNIHand * tmp = hands[1];
      hands[1] = hands[0];
      hands[0] = tmp;
    }
  }

  // if we have 1 hand here, we can update position
  if (hands[0]) {
    x = hands[0]->getPosition().x / ofWidth;
    x = (x - margin[3]) / xDimension; // adjust for margins
    x = min(max(x,0.0f),1.0f); // restrict to 0 and 1
    z = hands[0]->getPosition().y / ofHeight;
    z = (z - margin[0]) / yDimension; // adjust for margins
    z = min(max(z,0.0f),1.0f); // restrict to 0 and 1
  } else {
    x = z = 0.0f;
  }


  // if we have a second, hand we have height too
  if (hands[1]) {
    y = hands[1]->getPosition().y / ofHeight;
    y = (y - margin[0]) / yDimension; // adjust for margins
    y = min(max(y,0.0f),1.0f); // restrict to 0 and 1

    // if outside deadzone
    if (y < liveLower || y > liveUpper ) {
      // get height values
      yChange = y;  // use y_change to do calculations on y_norm
      // if y > liveLower, it includes deadZone; take it out
      if (y > liveLower) yChange -= deadZone; 
      yChange -= liveLower;  // center the range on 0 (so range is -live_margin .. +live_margin)
      y = min(max(y,0.0f),1.0f); // restrict to -1 and 1
      yChange = -yChange * MAX_CHANGE; // reverses y-axis and normalizes to MAX_CHANGE

      // terrain_modification_function_call_here(change_in_y, x, z, fancy_radius_thingey)
      terrain->AdjustHeight(yChange, x, z, radius);
    } else {
      // dead zone .. no change (for feedback purposes)
      yChange = 0.0f;
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

      ofSetColor(0,255,0);
      // draw some info regarding frame counts etc
      string msg = "Device FPS: " + ofToString(floor(openNIDevice.getFrameRate()));
	    verdana.drawString(msg, 10, openNIDevice.getNumDevices() * 480 - 10);

      // height
      ofSetColor(255,0,0);
      msg = ofToString(y,2) + " [" + ofToString(yChange,2) + "]";
	    verdana.drawString(msg, 10, y * ofHeight - 2);
      // position
      msg = ofToString(x,2) + ":" + ofToString(z,2);
	    verdana.drawString(msg, x * ofWidth + 2, z * ofHeight - 2);

      // dot(s)
      ofCircle(x * ofWidth, z * ofHeight, 2);
      ofCircle(8, y * ofHeight, 2);
      // thresholds
      ofSetColor(255,255,255);
      ofRect(5, ofHeight * liveLower, 6, 1); // high threshold
      ofRect(5, ofHeight * liveUpper, 6, 1); // low threshold
    
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
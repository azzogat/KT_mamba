#include "testApp.h"
#include "GL\glew.h"

//--------------------------------------------------------------
void testApp::setup() {
    

  ofSetLogLevel(OF_LOG_VERBOSE);

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
  openNIDevice.addGestureGenerator();
    // add all focus gestures (ie., wave, click, raise arm)
	openNIDevice.addAllHandFocusGestures();
    
    // or you can add them one at a time
    //vector<string> gestureNames = openNIDevice.getAvailableGestures(); // you can use this to get a list of gestures
                                                                         // prints to console and/or you can use the returned vector
  openNIDevice.addGesture("Click");
  openNIDevice.setMaxNumHands(4);
    
  ofAddListener(openNIDevice.gestureEvent,this,&testApp::handEvent); 

  openNIDevice.start();

  verdana.loadFont(ofToDataPath("verdana.ttf"), 24);

  terrain = Terrain::Create(20,20,64,64,ofVec3f(0,0,0));
  
}

//--------------------------------------------------------------
void testApp::update(){
    openNIDevice.update();
    terrain->Update();
}

//--------------------------------------------------------------
void testApp::draw(){
	//ofSetColor(255, 255, 255);
 //   
 //   ofPushMatrix();
 //   // draw debug (ie., image, depth, skeleton)
 //   //openNIDevice.drawDebug();
 //   ofPopMatrix();
 //   
 //   ofPushMatrix();
 //   // get number of current hands
 //   int numHands = openNIDevice.getNumTrackedHands();
 //   
 //   // iterate through users
 //   for (int i = 0; i < numHands; i++){
 //       
 //       // get a reference to this user
 //       ofxOpenNIHand & hand = openNIDevice.getTrackedHand(i);
 //       
 //       // get hand position
 //       ofPoint & handPosition = hand.getPosition();
 //       // do something with the positions like:
 //       
 //       // draw a rect at the position (don't confuse this with the debug draw which shows circles!!)
 //       ofSetColor(255,0,0);
 //       ofRect(handPosition.x, handPosition.y, 10, 10);
 //   }
    glMatrixMode(GL_PROJECTION);
	
    glLoadIdentity();
    gluPerspective(60,ofGetWidth()/ofGetHeight(),1,1000);


    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0,20,20,0,0,0,0,1,0);
    glPushMatrix();
        terrain->Draw();
    glPopMatrix();
	    
	// GUI CRAP
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, ofGetWidth(), ofGetHeight(), 0, -ofGetHeight(), ofGetHeight());
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
		m_gui.Draw();
	glPopMatrix();
    //ofPopMatrix();
    
    // draw some info regarding frame counts etc
	//ofSetColor(0, 255, 0);
	//string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate()) + " Device FPS: " + ofToString(openNIDevice.getFrameRate());
    
	//verdana.drawString(msg, 20, openNIDevice.getNumDevices() * 480 - 20);
}

//--------------------------------------------------------------
void testApp::handEvent(ofxOpenNIGestureEvent & event){
    // show hand event messages in the console
	if(event.gestureName == "Wave")
		ofLogNotice() << event.gestureName << "for hand" << event.gestureName << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void testApp::exit(){
    openNIDevice.stop();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
  if (key == 98) {
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
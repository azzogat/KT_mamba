#include "testApp.h"
#include "GL\glew.h"
#include "terrainShader.h"
#include "ofFileUtils.h"
//--------------------------------------------------------------
void testApp::setup() {
  
  maxHands = 2;
  maxChange = 1.0f;
  maxRadius = 0.4f;
  uiZone = 0.7f;

  hasLeft = false;
  hasRight = false;

  // using margins to correct for sensor inaccuracy (could be unique to device)
  // sensor doesn't go lower than ~50 nor higher than ~620 in x
  // sensor doesn't go lower than ~60 nor higher than ~470 in y
  margin[TOP] = 0.125f;         // top
  margin[RIGHT] = 0.03125f;     // right
  margin[BOTTOM] = 0.02083333f; // bottom
  margin[LEFT] = 0.078125f;     // left

  // Hand coords params: offset x, y, z, range x, y, z
  hand[R] = HandCoords(0.3f, 0.0f, 0.8f, 0.6f, 0.7f, 0.4f);
  hand[R].setDead(margin);     // add dead margins
  hand[R].set(0.5f,0.5f,0.5f); // center
  hand[L] = HandCoords(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
  hand[L].setDead(margin);

  radius = 0.0f;
  yChange = 0.0f;
  deadZone = 0.15f; // dead zone for input
  // upper and lower thresholds derived from deadZone
  liveLower = (1.0f - deadZone) * 0.5f;
  liveUpper = (1.0f + deadZone) * 0.5f;

  hoverTimer = 0;
  hoverTimerDelay = 20;

  for (int i = 0; i < maxHands; i++) {
    tracked_hands[i] = NULL;
  }

  //ofSetLogLevel(OF_LOG_VERBOSE); // ..no, STFU
  ofSetLogLevel(OF_LOG_NOTICE);

  glewInit();

  if (!glewIsSupported("GL_VERSION_2_0")) {
    float test = 5;
  }
    
  openNIDevice.setup();
  openNIDevice.addDepthGenerator();
  openNIDevice.setMirror(true);
  openNIDevice.addHandsGenerator();
	openNIDevice.addAllHandFocusGestures();
  openNIDevice.setMaxNumHands(maxHands);
    
  ofAddListener(openNIDevice.gestureEvent,this,&testApp::handEvent); 

  openNIDevice.start();

  verdana.loadFont(ofToDataPath("verdana.ttf"), 10);

  ofBuffer vsh = ofBufferFromFile("terrain.vsh");
  const char* vshp= vsh.getBinaryBuffer();

  ofBuffer psh = ofBufferFromFile("terrain.psh");
  const char* pshp= psh.getBinaryBuffer();

  terrain = Terrain::Create(20,20,64,64,ofVec3f(0,0,0));

  unsigned int m_vertexId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(m_vertexId,1,&vshp,NULL);
  glCompileShader(m_vertexId);


  GLint isCompiled = 0;
  glGetShaderiv(m_vertexId, GL_COMPILE_STATUS, &isCompiled);
  
  if (isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(m_vertexId, GL_INFO_LOG_LENGTH, &maxLength);

    //The maxLength includes the NULL character
    std::vector<GLchar> infoLog(maxLength);
    glGetShaderInfoLog(m_vertexId, maxLength, &maxLength, &infoLog[0]);
    std::string error(&infoLog[0]);
  }

  unsigned int m_fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(m_fragmentId,1,&pshp,NULL);
  glCompileShader(m_fragmentId);

  isCompiled = 0;
  glGetShaderiv(m_fragmentId, GL_COMPILE_STATUS, &isCompiled);

  if (isCompiled == GL_FALSE) {
    GLint maxLength = 0;
    glGetShaderiv(m_fragmentId, GL_INFO_LOG_LENGTH, &maxLength);

    //The maxLength includes the NULL character
    std::vector<GLchar> infoLog(maxLength);
    glGetShaderInfoLog(m_fragmentId, maxLength, &maxLength, &infoLog[0]);
    std::string error(&infoLog[0]);
  }
  
  program_id = glCreateProgram();
  
  glAttachShader(program_id,m_vertexId);
  glAttachShader(program_id,m_fragmentId);

  glBindAttribLocation(program_id,0,"pos");
  glBindAttribLocation(program_id,1,"normal");
  glBindAttribLocation(program_id,2,"uv");
  glBindAttribLocation(program_id,3,"color");

  glLinkProgram(program_id);

  GLint isLinked = 0;
  glGetProgramiv(program_id,GL_LINK_STATUS,&isLinked);

  GLint maxLength = 0;
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &maxLength);

  //The maxLength includes the NULL character
  std::vector<GLchar> infoLog(maxLength);

}

//--------------------------------------------------------------
void testApp::update(){
    openNIDevice.update();

    // get tracked hands and stuff them into our array
    for (int i = 0; i < maxHands; i++) {
      if (i < openNIDevice.getNumTrackedHands()) { // redundant after hack

        // getNumTrackedHands() often returned too high a number for a few
        // frames after losing a hand. This caused a runtime error on
        // getTrackedHand() when passing an index which doesn't exist in the
        // array. We added a hack to OpenNI to check first and return a dummy
        // ofxOpenNIHand with an ID of -2 if the index is out of bounds
        ofxOpenNIHand & hand = openNIDevice.getTrackedHand(i);
        if (hand.getID() == -2)
        {
          break;
        }
        tracked_hands[i] = & hand;
      } else {
        tracked_hands[i] = NULL;
      }
    }

    // hand 0 will be position. it's the one on the right
    // hand 1 will be for height.. it's the other one

    // if we have both hands, sort them right to left
    if (tracked_hands[0] && tracked_hands[1]) {
      // f@*# it assume there are exactly 2 at this point
      if (tracked_hands[0]->getPosition().x < tracked_hands[1]->getPosition().x) {
        // sort hands
        ofxOpenNIHand * tmp = tracked_hands[1];
        tracked_hands[1] = tracked_hands[0];
        tracked_hands[0] = tmp;
      }
    }

    // use right hand to update position
    if (tracked_hands[0]) {
      x = tracked_hands[0]->getPosition().x / 640; 
      y = tracked_hands[0]->getPosition().y / 480;
      z = tracked_hands[0]->getWorldPosition().z * 0.001; // z in mm; metres now

      // feed coordinates into the right hand's own coordinate system
      hand[R].set(x, y, z); // this normalizes and scales the value
      // using hand[R].x y or z now gives you an adjusted value

      // if outside deadzone
      if (hand[R].y < liveLower || hand[R].y > liveUpper ) {
        // get height values
        yChange = hand[R].y;  // use y_change to do calculations on y_norm
        // if y > liveLower, it includes deadZone; take it out
        if (yChange > liveLower) yChange -= deadZone; 
        yChange -= liveLower;  // any value below liveLower is negative
        yChange = -yChange * maxChange; // reverse y and normalize to maxChange
      } else {
        // dead zone .. no change (for feedback purposes)
        yChange = 0.0f;
      }
      hasRight = true;
    } else {
      yChange = 0.0f;
      hand[R].x = hand[R].z = hand[R].y = 0.5f;
      hasRight = false;
    }

    // use left hand to control radius, and for UI interaction
    if (tracked_hands[1]) {
      x = tracked_hands[1]->getPosition().x / 640;
      y = tracked_hands[1]->getPosition().y / 480;
      hand[L].set(x, y, 0.0f);

      radius = 1.0f - hand[L].y / uiZone; // invert y and normalize to uiZone
      radius = min(max(radius,0.0f),1.0f) * maxRadius; // scale to maxRadius   

      // terrain modification function call (change_in_y, x, z, brush_radius)
      terrain->AdjustHeight(yChange, hand[R].x, hand[R].z, radius);
      hasLeft = true;
    } else {
      hasLeft = false;
      radius = 0.0f;
    }

    if (!radius) {
      // UI stuff
      // check what the point is over
	    KT_PRESSED pressed = m_gui.GetAtPoint(0,ofVec2f(hand[L].x * windowWidth, hand[L].y * windowHeight));
	    if(pressed != KT_NONE)
	    {
		    //printf("Point over button \n");
        if(hoverTimer++ == hoverTimerDelay) {
		      switch( pressed )
		      {
			      case KT_RESET:
				      // reset terrain
					    terrain->Reset();
			      break;
            case KT_EXPORT:
				      // export terrain
              ofLogNotice() << "EXPORT TERRAIN";
			      break;
		      }
        }
	    } else {
        hoverTimer = 0;
      }
    }

    // terrain highlight & update
    terrain->HighLightPosition(hand[R].x,hand[R].z,max(radius,0.05f));
    terrain->Update();
}

//--------------------------------------------------------------
void testApp::draw(){

  //glPushMatrix();
  //ofSetColor(0, 0, 255);
  openNIDevice.drawDepth();
  //openNIDevice.drawHands();
  //glPopMatrix();

  ofMatrix4x4 matview;
  matview.makeIdentityMatrix();
  matview.makeLookAtViewMatrix(ofVec3f(0,20,15),ofVec3f(0,0,0),ofVec3f(0,1,0));
  ofMatrix4x4 matProjection;
  matProjection.makePerspectiveMatrix(60,(float)windowWidth/(float)windowHeight,1,1000);


  glUseProgram(program_id);
  int matloc = glGetUniformLocation(program_id,"mToV");
  glUniformMatrix4fv(matloc,1,false,matview.getPtr());
  matloc = glGetUniformLocation(program_id,"vToP");
  glUniformMatrix4fv(matloc,1,false,matProjection.getPtr());


  terrain->Draw(program_id);
  glUseProgram(0);

  //GUI CRAP
	glPushMatrix();
	  m_gui.Draw();
	glPopMatrix();

  //* INPUT FEEDBACK CRAP
  glPushMatrix();
    ofSetColor(0,255,0);
    // draw some info regarding frame counts etc
    string msg = "Device FPS: " + ofToString(floor(openNIDevice.getFrameRate()));
	  verdana.drawString(msg, 6, openNIDevice.getNumDevices() * windowHeight - 6);
    // hand indicator
    ofSetColor(255,0,0);
    if(hasRight) ofSetColor(255,127,39);
    if(hasLeft) ofSetColor(0,255,0);
    ofCircle(45,windowHeight-40,20);
    // height
    yChange?ofSetColor(255,127,39):ofSetColor(255,0,0);
    ofCircle(windowWidth - 10, hand[R].y * windowHeight, 8);
    msg = ofToString(yChange,2);
	  verdana.drawString(msg, windowWidth - 50, hand[R].y * windowHeight + 4);
    // radius
    radius?ofSetColor(255,127,39):ofSetColor(255,0,0);
    ofCircle(30, hand[L].y * (windowHeight - 25) + 25, radius*30);
    msg = ofToString(radius,2);
	  verdana.drawString(msg, 45, hand[L].y * (windowHeight - 25) + 29);
    // left hand
    if(!radius && hasLeft) {
      ofCircle(hand[L].x * windowWidth, hand[L].y * windowHeight, 6);
      // hover indicator
      if(hoverTimer) {
        ofSetColor(0,0,255);
        if(hoverTimer >= hoverTimerDelay) ofSetColor(255,0,255);
        ofCircle(45,windowHeight-40,min(20,20 * hoverTimer/hoverTimerDelay));
      }
    }
    // thresholds
    ofSetColor(255,255,255);
    ofRect(windowWidth - 19, windowHeight * liveLower, 18, 4); // high threshold
    ofRect(windowWidth - 19, windowHeight * liveUpper, 18, -4); // low threshold 
    ofRect(1, windowHeight * uiZone, 19, 4); // low threshold 
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
  if (key == 98) { // which key is 98?! :/
    float x = (float)mouseX / (float)windowWidth;
    float y = (float)mouseY / (float)windowHeight;
    terrain->AdjustHeight(0.1f,x,y,0.1f);
  }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

  mouseX = x;
  mouseY = y;

	// check what the point is over
	KT_PRESSED pressed = m_gui.GetAtPoint(0,ofVec2f(x,y));
	if(pressed != KT_NONE)
	{
		switch( pressed )
		{
			case KT_RESET:
				// reset terrain
        terrain->Reset();
			break;
      case KT_EXPORT:
				// reset terrain
        ofLogNotice() << "EXPORT TERRAIN";
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
  RECT clientRect;

  windowWidth = w;
  windowHeight = h;
}
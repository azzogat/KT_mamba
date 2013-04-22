#include "testApp.h"
#include "GL\glew.h"
#include "terrainShader.h"
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

  unsigned int m_vertexId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(m_vertexId,1,const_cast<const char**>(&terrainVShader),NULL);
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
  glShaderSource(m_fragmentId,1,const_cast<const char**>(&terrainPShader),NULL);
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

  program_id = 0;

  program_id = glCreateProgram();
  glAttachShader(program_id,m_vertexId);
  glAttachShader(program_id,m_fragmentId);
  glLinkProgram(program_id);

  GLint isLinked = 0;
  glGetProgramiv(program_id, GL_LINK_STATUS, (int *)&isLinked);

  GLint maxLength = 0;
  glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &maxLength);

    //The maxLength includes the NULL character
  std::vector<GLchar> infoLog(maxLength);
  glGetProgramInfoLog(program_id, maxLength, &maxLength, &infoLog[0]);
}

//--------------------------------------------------------------
void testApp::update(){
    openNIDevice.update();
    float x = (float)mouseX / (float)windowWidth;
    float y = (float)mouseY / (float)windowHeight;
    terrain->HighLightPosition(x,y,0.1f);
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

  ofMatrix4x4 matview;
  matview.makeIdentityMatrix();
  matview.makeLookAtViewMatrix(ofVec3f(0,20,20),ofVec3f(0,0,0),ofVec3f(0,1,0));
  ofMatrix4x4 matProjection;
  matProjection.makePerspectiveMatrix(60,(float)ofGetWidth()/(float)ofGetHeight(),1,1000);


  glUseProgram(program_id);
  int matloc = glGetUniformLocation(program_id,"mToV");
  glUniformMatrix4fv(matloc,1,false,matview.getPtr());
  matloc = glGetUniformLocation(program_id,"vToP");
  glUniformMatrix4fv(matloc,1,true,matProjection.getPtr());


  terrain->Draw();
  glUseProgram(0);

	    
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
  RECT clientRect;


  windowWidth = w;
  windowHeight = h;
}
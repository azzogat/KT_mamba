#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "Terrain.h"
#include "HandCoords.h"
#include "KTGui.h"

#define MAX_DEVICES 2

enum Hand {
  R, // Right
  L  // Left
};

class testApp : public ofBaseApp{

	KTGui	m_gui;
public:
    
	void setup();
	void update();
	void draw();
  void exit();
    
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

private:
    
    void handEvent(ofxOpenNIGestureEvent & event);

    Terrain* terrain;
  	ofxOpenNI openNIDevice;

    ofTrueTypeFont verdana;
    int windowWidth;
    int windowHeight;
    unsigned int program_id;

    ofxOpenNIHand * tracked_hands[2];
    HandCoords hand[2];

    ofxOpenNIDepthThreshold depthThreshold;
    
    float margin[4]; // used to correct for aparent dead zone on edges of sensor
    float x, y, z, yChange, radius, maxRadius, deadZone, liveUpper, liveLower, maxChange, uiZone;
    int maxHands, hoverTimer, hoverTimerDelay;
    bool hasLeft, hasRight;
};

#endif

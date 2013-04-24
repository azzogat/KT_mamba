#ifndef _TEST_APP
#define _TEST_APP

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "Terrain.h"
#include "KTGui.h"

 #define MAX_DEVICES 2

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

    ofxOpenNIHand * hands[2];

    ofxOpenNIDepthThreshold depthThreshold;
    
    float margin[4]; // used to correct for aparent dead zone on edges of sensor
    float x, y, z, radius, yChange, deadZone, liveUpper, liveLower, reduced_x;
    float xDimension, yDimension, ofHeight, ofWidth, left_hand_x, left_hand_y;
    int MAX_HANDS, MAX_CHANGE, hover_timer, hover_timer_delay;
    bool has_left, has_right;
};

#endif

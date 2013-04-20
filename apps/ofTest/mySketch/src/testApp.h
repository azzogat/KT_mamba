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

    ofxOpenNIHand * hands[2];
    
    float x, y, z, x_norm, y_norm, z_norm, y_change, dead_range, live_margin, radius;
    int MAX_HANDS, MAX_CHANGE, DIM_X, DIM_Y, DIM_X_CORRECTED, DIM_Y_CORRECTED, LEFT_MARGIN, RIGHT_MARGIN, TOP_MARGIN, BOTTOM_MARGIN;
};

#endif

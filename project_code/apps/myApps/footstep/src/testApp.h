#pragma once

#include "ofMain.h"

#include "ball.h"

#include "ofxTween.h"

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

        float clamp;
        ofxTween::ofxEasingType easingType;
        ofxEasingQuad easeQuad;
    
    private:
        ofImage     m_gMapView;
        ofImage     m_gStreetView;
    
        double      m_longitude;
        double      m_latitude;
    
        ofColor     m_tmpPalette;
        string      m_str_latitude;
        string      m_str_longitude;
        ofEasyCam   m_cam;
    
        double getMoveX();
        double getMoveY();
    
    
    
    
    
    
};

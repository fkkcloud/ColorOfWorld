#pragma once

#include "ofMain.h"
#include "ofxTween.h"
#include "Map.h"
#include "ofxLeapMotion.h"
#include <fstream>
#include "ofxMSAPerlin.h"
#include "ofxPostProcessing.h"

typedef struct {
    float latitude;
    float longitude;
    string name;
    int city_id;
} City;

typedef struct {
    string street;
    string city;
} GeoData;


class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
        void exit();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    private:
        // Leap object
        ofxLeapMotion   leap;
    
        // Leap hand count
        vector <ofxLeapMotionSimpleHand> myHands;
    
        // Leap Palm Position
        ofPoint         palmPos;
    
        // Modest Map object
        Map             m_map;
    
        // ofxTween variables for map function
        float           clamp;
        ofxEasingQuad   easeQuad;
        ofxTween::ofxEasingType easingType;

        // Google MapView ofImages
        ofImage         m_gMapView;
        ofImage         m_gStreetView;
    
        // Location informations
        double          m_longitude;
        double          m_latitude;
        string          m_str_latitude;
        string          m_str_longitude;
        string          m_cityName;
        int             m_cityId;
        vector <City> cityLocations;
    
        // store informations
        vector < vector < ofColor > > streetPalettes;
        deque < deque < ofColor > > colors;
        deque < deque < ofPoint > > points;
    
        // Color Search Toggle
        bool            colorSearchEnabled;
    
        // Tint Enabled
        int             tint;
    
        // Sphere parameter
        float m_earth_radius;
        float m_earth_xpos;
        float m_earth_ypos;
        float m_earth_zpos;
        float m_earth_speed;
        bool  display_earth;
    
        // tmp color
        ofColor         m_tmpPalette;
    
        // Post Processing
        ofxPostProcessing post;
    
        // Light
        //ofLight light;
    
        // Mesh
        //ofMesh boxMesh;
    
        // fbo object
        ofFbo fboMap;

        // dataCache
        vector < ofPoint > coordinates;
        vector < ofPoint > onMapPos;
        vector < ofPoint > onMapClr;
        map < string, GeoData > streetData;
        map < string, float > elevationData;
        map < string, ofImage > imageLib;
    
        // elapsedTime
        float m_etime;
    
        // image match enabled
        bool m_imageMatchEnabled;
        string whichClr;
    
        // music
        ofSoundPlayer  bgm;
    
//-------------------------------------------------------
        // private method : movement calculation
        double getMoveX( float );
        double getMoveY( float );
    
        // private method : random city selector
        City randomCityLocation();
    
        // private method : gesture debug purpose
        void gestureDbug();
    
        // interpolated line generator
        ofPolyline lifeStream( ofVec3f, ofVec3f, int );
    
        // custom 3d noise
        ofVec3f noise3d( ofVec3f, int);
    
        // fboDraw function
    void fboDraw();
    
    
};

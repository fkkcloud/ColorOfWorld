#include "testApp.h"
#include "string.h"

#define MAX_WIDTH 0.6
#define MIN_WIDTH 0.4


//--------------------------------------------------------------
void testApp::setup(){
    
    ofEnableSmoothing();
	ofEnableAlphaBlending();
	ofSetVerticalSync(true);
    
    ofSetFrameRate(60);
    
    ofSetWindowTitle( "Color Palette Exporter" );
    
    m_latitude = 40.829729;
    m_longitude = -73.990972;
    
    clamp = false;
    easingType = ofxTween::easeInOut;
}

//-----------------------
double testApp::getMoveX(){
    if ( mouseX <= ofGetWidth() * MIN_WIDTH ){
        double output = ofMap(mouseX, 0, ofGetWidth() * MIN_WIDTH, 0.001, 0);
        cout << " DOWN DOWN " << output << endl;
        return output;
    } else if ( mouseX > ofGetWidth() * MIN_WIDTH && mouseX <= ofGetWidth() * MAX_WIDTH ){
        double output = 0;
        cout << " STATIC " << output << endl;
        return output;
    } else if ( mouseX > ofGetWidth() * MAX_WIDTH ){
        double output = ofMap(mouseX, ofGetWidth() * MAX_WIDTH, ofGetWidth(), 0, 0.001);
        cout << "UP UP UP" << output << endl;
        return output;
    }
}

double testApp::getMoveY(){
    if ( mouseY <= ofGetHeight() * MIN_WIDTH ){
        double output = ofMap(mouseY, 0, ofGetHeight() * MIN_WIDTH, 0.001, 0);
        cout << " DOWN DOWN " << output << endl;
        return output;
    } else if ( mouseY > ofGetHeight() * MIN_WIDTH && mouseY <= ofGetHeight() * MAX_WIDTH ){
        double output = 0;
        cout << " STATIC " << output << endl;
        return output;
    } else if ( mouseY > ofGetWidth() * MAX_WIDTH ){
        double output = ofMap(mouseY, ofGetHeight() * MAX_WIDTH, ofGetHeight(), 0, 0.001);
        cout << "UP UP UP" << output << endl;
        return output;
    }
}

//--------------------------------------------------------------
void testApp::update(){
    
    // Convert to string updated location
    m_longitude = m_longitude - getMoveX();
    m_latitude = m_latitude - getMoveY();
    m_str_latitude = ofToString( m_latitude);
    m_str_longitude = ofToString( m_longitude);
    
    // Update path for map view
    string imgMapPath = "http://maps.googleapis.com/maps/api/staticmap?center="
    + m_str_latitude
    + ","
    + m_str_longitude
    + "&zoom=14&size=120x120&sensor=false&maptype=satellite";
    //m_gMapView.loadImage(imgMapPath);
    
    // Update path for street view
    string imgStreetPath = "http://maps.googleapis.com/maps/api/streetview?size=200x200&location="
    + m_str_latitude
    + ","
    + m_str_longitude
    + "&fov=90&heading=235&pitch=10&sensor=false";
    //m_gStreetView.loadImage(imgStreetPath);
    
    std::cout.precision(16);
    std::cout << m_latitude << ", " << m_longitude << std::endl;
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground( ofColor(10, 20, 30) );
    
    // General Information
    ofSetColor( 220, 220, 200 );
    ofDrawBitmapString( ofToString(ofGetFrameRate()) + "fps", 10, 20 );
    ofDrawBitmapString( "Longitude : " + m_str_longitude, 10, 35 );
    ofDrawBitmapString( "Latitude : " + m_str_latitude, 10, 50 );
    
    // Map View Draw
    ofSetColor( 255, 255, 255 );
    
    m_cam.begin();
    
    ofPushMatrix();
    
    ofRotate( -45, 1, 0, 0 );
    m_gMapView.draw( -300, 20, 10 );

    ofPopMatrix();
    
    m_cam.end();
    
    // Street View Draw
    m_gStreetView.draw( 520, 100 );
    

    // Color Palette
    int k, j;
    for ( k = 0; k < 5; k++ )
    {
        for ( j = 0; j < 5; j++ )
        {
            m_tmpPalette = m_gStreetView.getColor( k + 140, j + 140 );
            ofSetColor( m_tmpPalette );
            ofRect( 545 + j * 30, 330 + k * 30, 24, 24 );
        }
    }
    
}



//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
    //cout << "Moved " << x << "," << y << endl;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    //cout << "dragged " << x << "," << y << endl;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    //cout << "released at " << x << "," << y << endl;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    
}
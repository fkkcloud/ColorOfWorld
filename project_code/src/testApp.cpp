#include "testApp.h"
#include "string.h"
#include "OpenStreetMapProvider.h"
#include "GeoUtils.h"


// FOR MOUSE INTERACTION
#define MAX_WIDTH 0.6
#define MIN_WIDTH 0.4

// FOR LEAP INTERACTION
#define LEAP_THRES 6    // -6 ~ 6 in x-axia and y-axiz will be static zone
#define LEAP_ACTIV 90   // -6~-96 or 6~96 would be moving zone
#define XOFFSET 300
#define ZOFFSET 200

// FBO
#define FBO 1

// MAP MOVEMENT SPEED
#define MOVEMENT 0.0001

// PALETTE DRAWER PARAMETER
#define MAPSIZE 100
#define XORIGIN 140
#define YORIGIN 100
#define PIXELSIZE 5

#define PALETTEBRUSH   1
#define PALETTEMAP     0
#define USEAPI         0
#define SPHEREMAP      0
#define COLORCUBELIMIT 27

// TURBULENCE
#define AMPLITUDE       5
#define OCCTAVE         4
#define FREQ            0.4
#define VERTEX_COUNT    5


//--------------------------------------------------------------
void testApp::setup(){
    
    // OF settings
    ofSetVerticalSync(true);
	ofSetFrameRate(60);
    ofEnableSmoothing();
	ofEnableAlphaBlending();
    //ofNoFill();
    ofSetWindowTitle( "Color Palette Exporter" );
    
    // bgm setting
    bgm.loadSound("bgm.mp3");
    bgm.play();
    
    // Street Data Open
    ofBuffer file = ofBufferFromFile("cityData");
    while (!file.isLastLine()) {

        vector <string> values = ofSplitString(file.getNextLine(), ",");
        float lat = ofToFloat(values[0]);
        float lon = ofToFloat(values[1]);
        coordinates.push_back(ofPoint(lat, lon));
        string str = ofToString(lat) + "," + ofToString(lon);
        GeoData geoData = {values[2], values[3]};
        streetData[str] = geoData;

    }
    
    // Elevation Data Buffer
    ofBuffer file2 = ofBufferFromFile("elevationData.txt");
    while (!file2.isLastLine()) {
    
        vector <string> values = ofSplitString(file2.getNextLine(), ",");
        float lat = ofToFloat(values[0]);
        float lon = ofToFloat(values[1]);
        string str = ofToString(lat) + "," + ofToString(lon);
        elevationData[str] = ofToFloat(values[2]);
    
    }
    
    // Image Lib
    ofImage imageRed;
    if( ! imageRed.loadImage("red.jpg")) printf("Red loading failed");
    ofImage imageGreen;
    if( ! imageGreen.loadImage("green.jpg")) printf("Green loading failed");
    ofImage imageBlue;
    if( ! imageBlue.loadImage("blue.png")) printf("Blue loading failed");
    ofImage imageBlack;
    if( ! imageBlack.loadImage("black.jpg")) printf("Black loading failed");
    imageLib["red"] = imageRed;
    imageLib["green"] = imageGreen;
    imageLib["blue"] = imageBlue;
    imageLib["black"] = imageBlack;
    
    /*
    
    // ofxPostProcessing
    post.init( ofGetWidth(), ofGetHeight() );
    post.createPass<FxaaPass>()->setEnabled(false);
    post.createPass<BloomPass>()->setEnabled(false);
    post.createPass<DofPass>()->setEnabled(false);
    post.createPass<KaleidoscopePass>()->setEnabled(false);
    post.createPass<NoiseWarpPass>()->setEnabled(false);
    post.createPass<PixelatePass>()->setEnabled(false);
    post.createPass<EdgePass>()->setEnabled(false);
    post.createPass<VerticalTiltShifPass>()->setEnabled(false);
    post.createPass<GodRaysPass>()->setEnabled(false);
     
     */
    
    
    // ofFbo allocation
    fboMap.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
    fboMap.begin();
    ofClear(255, 255, 255, 0);
    fboMap.end();
    
    // Map setup
    m_map.setup(std::shared_ptr<OpenStreetMapProvider>(new OpenStreetMapProvider()),
              ofGetWidth() * 0.45,
              ofGetWidth() * 0.5  );
    
    // Map zoom setup
	m_map.setZoom(15);

    // Leap setup
    leap.open();
    
    // Leap gesture setup
	leap.setupGestures();   // we enable our gesture detection here

    // City location setup ofPoint(Latitude, Longitude)
    City seoul          = { 37.58330, 127.05000, "Seoul", 0 };
    City shanghai       = { 31.23300, 121.45000, "Shanghai", 1 };
    City tokyo          = { 35.68330, 139.73330, "Tokyo", 2 };
    City lasvegas       = { 14.86667, -88.06667, "Las Vegas", 3 };
    City sanfrancisco   = { 37.7749300, -122.4194200, "San Francisco", 4 };
    
    cityLocations.push_back( seoul        );      // Seoul
    cityLocations.push_back( shanghai     );      // Shanghai
    cityLocations.push_back( tokyo        );      // Tokyo
    cityLocations.push_back( lasvegas     );      // Las Vegas
    cityLocations.push_back( sanfrancisco );   // San Francisco
    
    // Multidimensional Deque
    for (int i = 0; i < cityLocations.size(); i++) {
        deque<ofColor> vectorCd;
        deque<ofPoint> vectorPt;
        for (int i = 0; i < 27; i++){
            vectorCd.push_back( ofColor(0, 0, 0, 0) );
            vectorPt.push_back( ofPoint(0, 0, 0) );
        }
        colors.push_back(vectorCd);
        points.push_back(vectorPt);
    }

    // Initial random city location
    //City cityLocation = randomCityLocation(); //randomCityLocation();
    m_latitude = cityLocations[4].latitude;
    m_longitude = cityLocations[4].longitude;
    m_cityName = cityLocations[4].name;
    m_cityId   = cityLocations[4].city_id;
    
    // Initial Map Setup
    m_map.setGeoLocationCenter(GeoLocation(m_latitude , m_longitude));
    
    // ofxTween initialize
    clamp = true;
    easingType = ofxTween::easeInOut;
    
    // Color Search Toggle
    colorSearchEnabled = false;
    
    // Image Match Toggle
    m_imageMatchEnabled = false;
    
    // Default tint value
    tint = 100;
    
    // Light
    //light.setPosition(1000, 1000, 2000);
    
    // debug datas
    /*
    for (int i = 0; i < coordinates.size(); i++) {
        string str = ofToString(coordinates[i][0]) + "," + ofToString(coordinates[i][1]);
        cout << streetData[str].street << endl;
        cout << streetData[str].city << endl;
        cout << streetData[str].street.size() << endl;
        cout << streetData[str].city.size() << endl;
        cout << elevationData[str] << endl;
        //cout << onMapPos[i] << endl;
    }
    */
    
    // PALETTEEMAP DATA STORE
    if ( PALETTEMAP ) {
        // Declare & Store for colors
        for (int i = 0; i < MAPSIZE; i++) {
            vector<ofColor> row; // Create an empty row
            for (int j = 0; j < MAPSIZE; j++) {
                row.push_back(ofColor(0, ofRandom( 0, 5 ), ofRandom( 200, 255 )));
            }
            streetPalettes.push_back(row); // Add the row to the main vector
        }
        
         // Map Street View Color
         for ( int i = 0; i < MAPSIZE; i++ ) {
            for (int j = 0; j < MAPSIZE; j++) {
                double x = XORIGIN + PIXELSIZE * i;
                double z = YORIGIN + PIXELSIZE * j;
                ofPoint location = m_map.pointToGeolocation( ofVec2d( x, z ) );
                
                if ( USEAPI ) {
                    string imgStreetPath = "http://maps.googleapis.com/maps/api/streetview?size=20x20&location="
                    + ofToString(location.x)
                    + ","
                    + ofToString(location.y)
                    + "&sensor=false"
                    + "&key=AIzaSyCj52TZirpdBIbGvdtcFXW4B8ADVUHbmyg";
                    //cout << imgStreetPath << endl;
                    if (m_gStreetView.loadImage(imgStreetPath) ){
                        cout << i << ", " << j << " image loaded." << endl;
                        streetPalettes[i][j] = m_gStreetView.getColor( 15, 17 );

                    } else {
                        cout << "image load failed." << endl;
                    }
                }
            }
         }
    }
    
    
    
    
    
}


/*
//Option 1 - mouse movement

//--------------------------------------------------------------
double testApp::getMoveX(){
    if ( mouseX <= ofGetWidth() * MIN_WIDTH ){
        double output = ofxTween::map(mouseX, 0, ofGetWidth() * MIN_WIDTH, MOVEMENT, 0, clamp, easeQuad, easingType);
        cout << " LEFT " << output << endl;
        return output;
    } else if ( mouseX > ofGetWidth() * MIN_WIDTH && mouseX <= ofGetWidth() * MAX_WIDTH ){
        double output = 0;
        cout << " STATIC " << output << endl;
        return output;
    } else if ( mouseX > ofGetWidth() * MAX_WIDTH ){
        double output = ofxTween::map(mouseX, ofGetWidth() * MAX_WIDTH, ofGetWidth(), 0, -MOVEMENT, clamp, easeQuad, easingType);

        cout << " RIGHT " << output << endl;
        return output;
    }
}

double testApp::getMoveY(){
    if ( mouseY <= ofGetHeight() * MIN_WIDTH ){
        double output = ofxTween::map(mouseY, 0, ofGetHeight() * MIN_WIDTH, -MOVEMENT, 0, clamp, easeQuad, easingType);
        cout << " UP " << output << endl;
        return output;
    } else if ( mouseY > ofGetHeight() * MIN_WIDTH && mouseY <= ofGetHeight() * MAX_WIDTH ){
        double output = 0;
        cout << " STATIC " << output << endl;
        return output;
    } else if ( mouseY > ofGetHeight() * MAX_WIDTH ){
        double output = ofxTween::map(mouseY, ofGetHeight() * MAX_WIDTH, ofGetHeight(), 0, MOVEMENT, clamp, easeQuad, easingType);
        cout << " DOWN " << output << endl;
        return output;
    }
}
*/


// Option 2 - leap motion movement

//--------------------------------------------------------------
double testApp::getMoveX( float x ){
    if ( palmPos.x <= -LEAP_THRES - XOFFSET ){
        double output = ofxTween::map(palmPos.x, -LEAP_THRES - LEAP_ACTIV - XOFFSET, -LEAP_THRES - XOFFSET, MOVEMENT, 0, clamp, easeQuad, easingType);
        //cout << " LEFT " << output << endl;
        return output;
    } else if ( palmPos.x > -LEAP_THRES - XOFFSET && palmPos.x <= LEAP_THRES - XOFFSET ){
        double output = 0;
        //cout << " STATIC " << output << endl;
        return output;
    } else if ( palmPos.x > LEAP_THRES - XOFFSET ){
        double output = ofxTween::map(palmPos.x, LEAP_THRES - XOFFSET, LEAP_THRES + LEAP_ACTIV - XOFFSET, 0, -MOVEMENT, clamp, easeQuad, easingType);
        //cout << " RIGHT " << output << endl;
        return output;
    }
}

double testApp::getMoveY( float z ){
    if ( palmPos.z <= -LEAP_THRES + ZOFFSET){
        double output = ofxTween::map(palmPos.z, -LEAP_THRES - LEAP_ACTIV + ZOFFSET, -LEAP_THRES + ZOFFSET, -MOVEMENT, 0, clamp, easeQuad, easingType);
        //cout << " UP " << output << endl;
        return output;
    } else if ( palmPos.z > -LEAP_THRES + ZOFFSET && palmPos.z <= LEAP_THRES+ZOFFSET ){
        double output = 0;
        //cout << " STATIC " << output << endl;
        return output;
    } else if ( palmPos.z > LEAP_THRES + ZOFFSET ){
        double output = ofxTween::map(palmPos.z, LEAP_THRES + ZOFFSET, LEAP_THRES + LEAP_ACTIV + ZOFFSET, 0, MOVEMENT, clamp, easeQuad, easingType);
        //cout << " DOWN " << output << endl;
        return output;
    }
}

//--------------------------------------------------------------
void testApp::fboDraw(){
    // Map draw
    /*
    if ( !display_earth ){
        m_map.draw();
    }
     */
    
    // PALETTE BRUSH-----------------------------
    if ( PALETTEBRUSH && !display_earth ) {
        for (int i = 0; i < points[m_cityId].size(); i++) {
            ofSetColor( colors[m_cityId][i] );
            ofVec2d pos = m_map.geoLocationToPoint( GeoLocation(points[m_cityId][i].x, points[m_cityId][i].y) );
            ofCircle(pos.x, pos.y, 3.5);
        }
    }
    
    // Frames----------------------------
    ofSetColor( 10, 20, 30 );
    ofRect(0, 0, 140, ofGetHeight() );
    ofRect(0, 0, ofGetWidth(), 100 );
    ofRect(0, 650, ofGetWidth(), ofGetHeight() );
    ofRect(650, 0, ofGetWidth(), ofGetHeight() );
    
    
    // PALETTE BRUSH DATA ------------
    if ( PALETTEBRUSH ) {
        if ( colorSearchEnabled ){
            for ( int k = 0; k < myHands.size(); k++ ){
                for ( int i = 0; i < myHands[k].fingers.size(); i++ ){
                    ofPoint fingerPos = myHands[k].fingers[i].pos;
                    double x = fingerPos.x + ofGetWidth() * 0.5 -XOFFSET;
                    double z = fingerPos.z + ofGetHeight() * 0.5 +ZOFFSET - 180;
                    ofPoint location = m_map.pointToGeolocation( ofVec2d( x , z ) );
                    //ofSetColor( 220, 250, 255 );
                    //ofDrawBitmapString( ofToString(location.x) , ofPoint( x - 10, z + 25) );
                    //ofDrawBitmapString( ofToString(location.y), ofPoint( x- 10, z + 50) );
                    
                    // Update path for street view
                    string imgStreetPath = "http://maps.googleapis.com/maps/api/streetview?size=50x50&location="
                    + ofToString(location.x)
                    + ","
                    + ofToString(location.y)
                    + "&sensor=false"
                    + "&key=AIzaSyCj52TZirpdBIbGvdtcFXW4B8ADVUHbmyg";
                    
                    m_gStreetView.loadImage(imgStreetPath);
                    m_tmpPalette = m_gStreetView.getColor( 10, 13 );
                    
                    if ( m_tmpPalette.r > 220 && m_tmpPalette.g > 220 && m_tmpPalette.b > 220 ){
                        // DO NOTHING
                    } else {
                        if ( colors[m_cityId].size() > COLORCUBELIMIT ){
                            colors[m_cityId].pop_front();
                            points[m_cityId].pop_front();
                        }
                        m_tmpPalette.setSaturation( 200 );
                        m_tmpPalette.setBrightness( 300 );
                        colors[m_cityId].push_back( m_tmpPalette );
                        points[m_cityId].push_back( ofPoint( location.x, location.y ) );
                    }
                    
                    // Draw finger position box with palette
                    ofSetColor(m_tmpPalette);
                    ofNoFill();
                    ofRect(x, z, 10, 10);
                    ofFill();
                    
                    // Palette and Street View information
                    ofRect( 60, 80 * i + 100, 75, 75 );
                    ofSetColor( 255, 255, 255 );
                    m_gStreetView.draw( 10, 80 * i + 100, 75, 75 );
                    
                    
                }
            }
        }
    }
}

//--------------------------------------------------------------


//--------------------------------------------------------------
City testApp::randomCityLocation(){
    int randVal = ofRandom(cityLocations.size());
    cout << randVal << endl;
    return cityLocations[randVal];
}



//--------------------------------------------------------------


//--------------------------------------------------------------
void testApp::update(){
    // get elapsed time
    m_etime = ofGetElapsedTimef();
    //cout << m_etime << endl;
    
    // Leap gesture setup for update function
    leap.updateGestures();  // check for gesture updates
	leap.markFrameAsOld();	//IMPORTANT! - tell ofxLeapMotion that the frame is no longer new.
    
    // Leap hand initialize
    myHands = leap.getSimpleHands();
    
    // Store palm position
    for (int i = 0; i < myHands.size(); i++) {
        palmPos = myHands[0].handPos; // no matter how mand hands, it will use first one
        palmPos = ofVec3f( palmPos.x - XOFFSET, palmPos.y, palmPos.z + ZOFFSET );
    }
    
    
    // FIST GESTURE
    /*
    for (int i = 0; i < myHands.size(); i++){
        if ( myHands[i].fingers.size() < 1 ){
            m_imageMatchEnabled = true;
        } else {
            m_imageMatchEnabled = false;
        }
    }
    */
    if (m_imageMatchEnabled) {
        ofColor sumClr;
        
        for (int i = 0; i < colors[m_cityId].size(); i++) {
            ofColor tmp = colors[m_cityId][i];
            sumClr = ofColor(sumClr.r + tmp.r, sumClr.g + tmp.g, sumClr.b + tmp.b);
        }
        sumClr = ofColor(sumClr.r / colors[m_cityId].size(), sumClr.g / colors[m_cityId].size(), sumClr.b / colors[m_cityId].size() );
        
        if ( (sumClr.r > sumClr.g) && (sumClr.r > sumClr.b) ){
            whichClr = "red";
        } else if ( (sumClr.g > sumClr.r) && (sumClr.g > sumClr.b) ){
            whichClr = "green";
        } else if ( (sumClr.b > sumClr.r) && (sumClr.b > sumClr.g) ){
            whichClr = "blue";
        } else {
            whichClr = "black";
        }
    }
    
    
    // Enable/Disable Color Search
    if (leap.iGestures == 9){
        colorSearchEnabled = true;
    } else if (leap.iGestures == 10){
        colorSearchEnabled = false;
        m_imageMatchEnabled = false;
    }
    
    /*
    if (leap.iGestures == 6){
        City cityLocation = randomCityLocation();
        m_latitude  = cityLocation.latitude;
        m_longitude = cityLocation.longitude;
        m_cityName  = cityLocation.name;
        m_cityId   = cityLocation.city_id;
    }
    */
    
    // Navigated location
    if ( !colorSearchEnabled ){
        m_latitude  -= getMoveY(palmPos.x);
        m_longitude -= getMoveX(palmPos.z);
    }
    
    // location to string
    m_str_latitude = ofToString( m_latitude);
    m_str_longitude = ofToString( m_longitude);


    /*
    // Finger grab return random city locations
    for ( int i = 0; i < myHands.size(); i++ ){
        if (myHands[i].fingers.size() < 1) {
            // ACTION
        }
    }
    */
    
    if ( display_earth ){
        // SPHEARE DRAW BIG
        m_earth_radius = 300;
        m_earth_xpos   = ofGetWidth() * 0.5;
        m_earth_ypos   = ofGetHeight() * 0.5;
        m_earth_zpos   = 0;
        m_earth_speed  = 0.1;
    } else {
        // SPHEARE DRAW SMALL
        m_earth_radius = 200;
        m_earth_xpos   = ofGetWidth() * 0.7;
        m_earth_ypos   = ofGetHeight() * 0.5;
        m_earth_zpos   = 0;
        m_earth_speed  = 0.125;
    }
    
    
    // Map update
    m_map.setGeoLocationCenter(GeoLocation(m_latitude , m_longitude));
    

    
#define THRES 0.01
    // Art Map update
    // Get position from geoLocation
    onMapPos.clear();
    onMapClr.clear();
    for (int i = 0; i < coordinates.size(); i++) {
        double x = coordinates[i][0];
        double y = coordinates[i][1];
        ofPoint center(m_latitude, m_longitude);
        if ( (x < center.x + THRES) && (y < center.y + THRES + 0.004) && (x >= center.x - THRES) && (y >= center.y - THRES - 0.004) ) {
            GeoLocation geoLoc = GeoLocation( x, y );
            ofPoint pt = m_map.geoLocationToPoint( geoLoc );
            onMapPos.push_back( pt );
            onMapClr.push_back( ofPoint(x,y) );
        }
    }

    
//-----------------------------------------
    /*
    // Update path for map view
    string imgMapPath = "http://maps.googleapis.com/maps/api/staticmap?center="
    + m_str_latitude
    + ","
    + m_str_longitude
    + "&zoom=14&size=120x120&sensor=false&maptype=satellite";
    m_gMapView.loadImage(imgMapPath);
    
    // Update path for street view
    string imgStreetPath = "http://maps.googleapis.com/maps/api/streetview?size=200x200&location="
    + m_str_latitude
    + ","
    + m_str_longitude
    + "&fov=90&heading=235&pitch=10&sensor=false";
    m_gStreetView.loadImage(imgStreetPath);
    */
    
//-----------------------------------------

}

//--------------------------------------------------------------
void testApp::draw(){
    // Background Color
    ofBackgroundGradient( ofColor(50, 150, 255), ofColor(5, 10, 45), OF_GRADIENT_CIRCULAR );
    //glEnable(GL_CULL_FACE);
    //light.enable();
    

    /*
    // FBO for map
    if ( FBO ) {
        ofPushMatrix();
        ofRotate(60, 1, 0, 1);
        ofTranslate( 0, -500);
        fboDraw();
        ofPopMatrix();
    } else {
        fboDraw();
    }
    */
    
    // Color Match-------------------------------------------
    if (m_imageMatchEnabled) {
        imageLib[whichClr].draw(ofPoint(ofGetWidth()/3, ofGetHeight()/3.7), 400, 400);
    }
    
    
    
    
    // DRAW ART MAP------------------------------------------------------------------------------------------------
    string cityName;
    ofPushMatrix();
    ofRotate(50, 1, 0, 0);
    ofTranslate(390, 190);
    
        for (int i = 0; i < onMapClr.size(); i++) {
            
            
            string str = ofToString(onMapClr[i][0]) + "," + ofToString(onMapClr[i][1]);
            
            srand(streetData[str].street.size()+1);
            int r = rand() % (streetData[str].street.size()+10);
            srand(streetData[str].street.size()+2);
            int g = rand() % (streetData[str].street.size()+20);
            srand(streetData[str].street.size()+3);
            int b = rand() % (streetData[str].street.size()+30);
            
            ofPoint center    = m_map.geoLocationToPoint(GeoLocation(m_latitude, m_longitude));
            double dist = ofDist(center.x, center.y, onMapPos[i].x, onMapPos[i].y);
            
            float alpha = ofxTween::map(dist, 40, 280, 180, 0, true, easeQuad, easingType); // alpha gets smaller towars end
            float radius = ofxTween::map(dist, 40, 280, 4.5, 0.5, true, easeQuad, easingType); // radius gets smaller towards end
            float height = ofxTween::map(dist, 40, 280, 15, 0, true, easeQuad, easingType); // sphere shape
            float alpha2 = ofxTween::map(dist, 70, 280, 255, 0, true, easeQuad, easingType); // alpha for string
            
            float elevation = ofxTween::map(elevationData[str], 0, 280, -15, 50, true, easeQuad, easingType);
            float h_radius = ofxTween::map(elevationData[str], 0, 280, 0, -1.5, true, easeQuad, easingType);

            ofColor clr = ofColor( r * 56, g * 56, b * 36 );
            clr.setSaturation(320);
            clr.setBrightness(200);
            ofSetColor( clr, alpha);
            

            ofCircle(onMapPos[i].x, onMapPos[i].y, height + elevation, radius + h_radius);
            //ofBox(onMapPos[i].x, onMapPos[i].y, height + elevation, radius, radius, radius);
            //ofNoFill();
            //ofBox(onMapPos[i].x, onMapPos[i].y, height + elevation, radius * 1.2, radius * 1.2, radius * 1.2);
            //ofFill();
            
            if ( i % 200 == 0 ) {
                ofSetColor( clr * 30, alpha2 );
                ofDrawBitmapString(streetData[str].street, onMapPos[i].x+15, onMapPos[i].y, height + elevation + 30);
                ofLine(onMapPos[i].x, onMapPos[i].y, height + elevation, onMapPos[i].x+15, onMapPos[i].y, height + elevation + 30);
            }
            if ( i % 500 == 0 ){
                cityName = streetData[str].city;
            }
            
            
            

        }
    
    
    // PALETTE BRUSH DATA ------------
    if ( PALETTEBRUSH ) {
        if ( colorSearchEnabled ){
            for ( int k = 0; k < myHands.size(); k++ ){
                for ( int i = 0; i < myHands[k].fingers.size(); i++ ){
                    ofPoint fingerPos = myHands[k].fingers[i].pos;
                    double x = fingerPos.x + ofGetWidth() * 0.5 -XOFFSET;
                    double z = fingerPos.z + ofGetHeight() * 0.5 +ZOFFSET - 180;
                    ofPoint location = m_map.pointToGeolocation( ofVec2d( x , z ) );
                    //ofSetColor( 220, 250, 255 );
                    //ofDrawBitmapString( ofToString(location.x) , ofPoint( x - 10, z + 25) );
                    //ofDrawBitmapString( ofToString(location.y), ofPoint( x- 10, z + 50) );
                    
                    // Update path for street view
                    string imgStreetPath = "http://maps.googleapis.com/maps/api/streetview?size=50x50&location="
                    + ofToString(location.x)
                    + ","
                    + ofToString(location.y)
                    + "&sensor=false"
                    + "&key=AIzaSyCj52TZirpdBIbGvdtcFXW4B8ADVUHbmyg";
                    
                    m_gStreetView.loadImage(imgStreetPath);
                    m_tmpPalette = m_gStreetView.getColor( 10, 13 );
                    
                    if ( m_tmpPalette.r > 220 && m_tmpPalette.g > 220 && m_tmpPalette.b > 220 ){
                        // DO NOTHING
                    } else {
                        if ( colors[m_cityId].size() > COLORCUBELIMIT ){
                            colors[m_cityId].pop_front();
                            points[m_cityId].pop_front();
                        }
                        m_tmpPalette.setSaturation( 200 );
                        m_tmpPalette.setBrightness( 300 );
                        colors[m_cityId].push_back( m_tmpPalette );
                        points[m_cityId].push_back( ofPoint( location.x, location.y ) );
                    }
                    
                    // Draw finger position box with palette
                    ofSetColor(m_tmpPalette);
                    
                    float height;
                    for (int i = 0; i < onMapClr.size(); i++) {
                        float dist = ofDist( onMapPos[i].x, onMapPos[i].y, x, z );
                        if ( dist <= 7 ){
                            string str = ofToString(onMapClr[i][0]) + "," + ofToString(onMapClr[i][1]);
                            height = ofxTween::map(elevationData[str], 0, 280, -15, 50, true, easeQuad, easingType);
                        }
                    }
                    
                    ofNoFill();
                    ofRect(x + 5, z + 5, height, 13, 13);
                    
                    ofPushMatrix();
                    int up = m_etime;
                        ofTranslate(0, 0, (up*3)%6 * 10 );
                        //ofRotate(m_etime * 5, 0, 1, 0);
                        ofSetColor(m_tmpPalette, 100);
                        ofFill();
                        ofBox(x , z , height, 8, 8, 8);
                        ofNoFill();
                        ofBox(x , z , height, 9, 9, 9);
                    ofPopMatrix();
                    
                    
                    ofFill();
                    ofSetColor(m_tmpPalette);
                    ofLine(x + 10, z + 10 , height, x + 10, z + 10, palmPos.z * 0.25 + height);
                    ofRect(x +4, z +4, height, 11, 11);
                    ofRect(x +2, z +2, palmPos.z * 0.25 + height, 4, 4);
                    
                    // Palette and Street View information
                    ofPushMatrix();
                    ofRotate(-50, 1, 0, 0);
                    ofTranslate(-100, -30);
                    ofRect( 60, 80 * i + 100, 75, 75 );
                    ofSetColor(255, 255, 255, 200);
                    m_gStreetView.draw( 10, 80 * i + 100, 75, 75 );
                    ofPopMatrix();
                }
            }
        }
    }
    
    
    
    ofPopMatrix();
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(cityName, 10, 20);
    //--------------------------------------------------------------------------------------------------------------
    
    
    
    
    
    // COLOR BOX---------------------------------

    if ( PALETTEBRUSH ){
    
        // COLOR BOX

        int offset;
        for (int city = 0; city < points.size(); city++ ){  // by city loop
            
            switch (city) {
                case 0:
                    offset = 0;
                    break;
                case 1:
                    offset = 1;
                    break;
                case 2:
                    offset = 2;
                    break;
                case 3:
                    offset = 3;
                    break;
                case 4:
                    offset = 4;
                    break;
                case 5:
                    offset = 5;
                    break;
                default:
                    break;
            }
            
            int p_idx = 0;
            //ofSetupScreenOrtho( ofGetWidth(), ofGetHeight() );
            for (int i = 0; i < 3; i++ ) { // each unit loop
                for (int j = 0; j < 3; j ++) {
                    for (int k = 0; k < 3; k++) {
                        ofColor clr = colors[city][p_idx];
                        
                        if ( (clr.r + clr.g + clr.b) > 20 ){ // filtering initial data
                            ofSetColor( ofColor(clr.r, clr.g, clr.b, 200) );
                            ofPushMatrix();
                            ofTranslate(ofGetWidth() * 0.25 + offset * 125, 700);
                            ofBox( 50 + 16 * i,
                                  16 * j,
                                  16 * k,
                                  12,
                                  12,
                                  12);
                            ofPopMatrix();
                        }
                        
                        p_idx++;
                    }
                    
                    }
                }
            
        }
        
        for (int i = 0; i < cityLocations.size(); i++ ){
            int offset;
            for (int j = 0; j < i+1; j++) {
                offset = i * 125;
            }
            ofSetColor( 200, 200, 250);
            ofDrawBitmapString(cityLocations[i].name, ofGetWidth() * 0.25 + 20 + offset, 680);
        }
    
}

    // PALETTE MAP DRAW-------------------------
    if ( PALETTEMAP ) {
        
        if ( colorSearchEnabled ){
            for ( int i = 0; i < MAPSIZE; i++ ) {
                for (int j = 0; j < MAPSIZE; j++) {
                    double x = XORIGIN + PIXELSIZE * i;
                    double z = YORIGIN + PIXELSIZE * j;
                    ofSetColor( streetPalettes[i][j] );
                    ofRect(x, z, PIXELSIZE * 0.9 , PIXELSIZE * 0.9);
               }
            }
        }
    }
    

    // DRAW SPHERE EARTH--------------------------------------------------------
    if ( SPHEREMAP ) {
        ofPushMatrix();
        
        //translate so that the center of the screen is 0,0
        ofTranslate( m_earth_xpos ,  m_earth_ypos);
        ofSetColor(255, 255, 255, 2.5);
        
        //draw a translucent wireframe sphere (ofNoFill() is on)
        ofPushMatrix();
        //add an extra spin at the rate of 1 degree per frame
        ofRotate(ofGetFrameNum() * m_earth_speed, 0, 1, 0);
        ofNoFill();
        ofDrawSphere(0, 0, 0, m_earth_radius);
        ofFill();
        ofPopMatrix();
        
        
        //City & Line DRAW ----------------------------------------------------
        ofSetColor(255);
        for(unsigned int i = 0; i < cityLocations.size(); i++){
            
            //three rotations
            //two to represent the latitude and lontitude of the city
            //a third so that it spins along with the spinning sphere
            ofQuaternion latRot, longRot, spinQuat;
            latRot.makeRotate(cityLocations[i].latitude, 1, 0, 0);
            longRot.makeRotate(cityLocations[i].longitude, 0, 1, 0);
            spinQuat.makeRotate(ofGetFrameNum() * m_earth_speed, 0, 1, 0);
            
            //our starting point is 0,0, on the surface of our sphere, this is where the meridian and equator meet
            ofVec3f center = ofVec3f(0,0, m_earth_radius);
            //multiplying a quat with another quat combines their rotations into one quat
            //multiplying a quat to a vector applies the quat's rotation to that vector
            //so to to generate our point on the sphere, multiply all of our quaternions together then multiple the centery by the combined rotation
            ofVec3f worldPoint = latRot * longRot * spinQuat * center;
            
            ofSetColor( 255, 50, 20 );
            ofRect(worldPoint, 5, 5);
            
            ofSetColor( 60, 200, 255, 200 );
            //set the bitmap text mode billboard so the points show up correctly in 3d
            ofDrawBitmapString(cityLocations[i].name, worldPoint );
        }
        
        
        // PALETTE LINES DRAW on EARTH --------------------------------
        for(unsigned int i = 0; i < points[m_cityId].size(); i++){
            
            ofQuaternion latRot, longRot, spinQuat;
            latRot.makeRotate(points[m_cityId][i].x, 1, 0, 0);
            longRot.makeRotate(points[m_cityId][i].y, 0, 1, 0);
            spinQuat.makeRotate(ofGetFrameNum() * m_earth_speed, 0, 1, 0);
            
            ofVec3f center = ofVec3f(0,0, m_earth_radius);
            ofVec3f worldPoint = latRot * longRot * spinQuat * center;
            
            ofSetColor( colors[m_cityId][i] );
            vector <ofPoint> vertices;
            for (int i = 0; i <= VERTEX_COUNT; i++) {
                float amplitude = ofxTween::map(i, 0, VERTEX_COUNT, 0, AMPLITUDE, clamp, easeQuad, easingType );
                msa::Perlin pNoise2( OCCTAVE, FREQ, AMPLITUDE, ofRandom(123) );
                ofPoint currPt = worldPoint * i/VERTEX_COUNT;
                float x = currPt.x + pNoise2.get( currPt.x + ofRandom(100), currPt.y, currPt.z );
                float y = currPt.y + pNoise2.get( currPt.x, currPt.y + ofRandom(100), currPt.z );
                float z = currPt.z + pNoise2.get( currPt.x, currPt.y, currPt.z + ofRandom(100) );
                vertices.push_back( ofPoint( x, y, z ) );
            }
            ofPolyline pline( vertices );
            //pline.getSmoothed(1);
            pline.draw();
            
            ofRect(worldPoint, 5, 5);
        }

        
        // CURRENT POSITION DRAW------------------------------------------
        ofQuaternion latRot, longRot, spinQuat;
        latRot.makeRotate(m_latitude, 1, 0, 0);
        longRot.makeRotate(m_longitude, 0, 1, 0);
        spinQuat.makeRotate(ofGetFrameNum() * m_earth_speed, 0, 1, 0);

        ofVec3f center = ofVec3f(0,0, m_earth_radius);
        ofVec3f worldPoint = latRot * longRot * spinQuat * center;
        
        ofSetColor( 5, 50, 220 );
        ofRect(worldPoint, 5, 5);
        
        ofPopMatrix();
        
    }

    /*
    // General Information draw
    ofSetColor( 220, 220, 220 );
    ofDrawBitmapString( ofToString(ofGetFrameRate()) + "fps", 10, 20 );
    ofDrawBitmapString( "Longitude : " + m_str_longitude, 10, 35 );
    ofDrawBitmapString( "Latitude : " + m_str_latitude, 10, 50 );
    */
    
    /*
    ofDrawBitmapStringHighlight(ofToString(m_map.getGeoLocationCenter()),
                                ofGetWidth() / 2,
                                ofGetHeight() / 2);
     */
    
    /*
    for (unsigned i = 0; i < post.size(); ++i)
    {
        if (post[i]->getEnabled()) ofSetColor(0, 255, 255);
            else ofSetColor(255, 0, 0);
                ostringstream oss;
        oss << i << ": " << post[i]->getName() << (post[i]->getEnabled()?" (on)":" (off)");
        ofDrawBitmapString(oss.str(), 10, 20 * (i + 2));
    
    }
     */

    testApp::gestureDbug();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    /*
    if ( key == 'k' ){
        City cityLocation = randomCityLocation();
        m_latitude  = cityLocation.latitude;
        m_longitude = cityLocation.longitude;
        m_cityName  = cityLocation.name;
        m_cityId   = cityLocation.city_id;
    }
     */
    
    if ( key == 't' ){
        tint = 255;
    }
    if ( key == 'r' ){
        tint = 100;
    }
    if ( key == 'e' ){
        display_earth = !display_earth;
    }
    if ( key == 'm' ){
        m_imageMatchEnabled = !m_imageMatchEnabled;
    }
   
    
    unsigned idx = key - '0';
    if (idx < post.size()) post[idx]->setEnabled(!post[idx]->getEnabled());
    
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

void testApp::exit(){
    leap.close();
    cout << "EXIT" << endl;
}

void testApp::gestureDbug(){
    string msg;
    
    switch (leap.iGestures)
    {
        case 1:
            msg = "Screen Tap";
            break;
            
        case 2:
            msg = "Key Tap";
            break;
            
        case 3:
            msg = "Swipe Right";
            break;
            
        case 4:
            msg = "Swipe Left";
            break;
            
        case 5:
            msg = "Swipe Down";
            break;
            
        case 6:
            msg = "Swipe Up";
            break;
            
        case 7:
            msg = "Swipe Forward";
            break;
            
        case 8:
            msg = "Swipe Backwards";
            break;
            
        case 9:
            msg = "Circle Left";
            break;
            
        case 10:
            msg = "Circle Right";
            break;
            
        default:
            msg = "Waiting for hand movement...";
            break;
    }
    ofSetColor(120, 150, 180);
    ofDrawBitmapString(msg, 10, 80);  // let's draw our message to the screen
}

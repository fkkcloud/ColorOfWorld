Implemented interactive map view with Leap Motion
"Right Circle Finger" motion enables color palette collect from the map's street view
Collected Color will be stored bottom left
Each fingers street view and defined color palette will also displayed very left side

All the layouy is just temp and it is just tech demo

Download Palette Exporter Tester App 2 (run on OSX only)   
http://fkkcloud.com/apps/ColorPaletteExporter_tester2.zip

http://www.youtube.com/watch?v=Yclz-w1nxiE

![Example Image1](../project_images/demoScreenShot.jpg?raw=true "Example Image1")


```

void testApp::update(){
    
    // Convert to string updated location
    m_str_latitude = ofToString( m_latitude - mouseY * 0.00001 );
    m_str_longitude = ofToString( m_longitude - mouseX * 0.00001 );
    
    // Update path for map view
    string imgMapPath = "http://maps.googleapis.com/maps/api/staticmap?center="
    + m_str_latitude
    + ","
    + m_str_longitude
    + "&zoom=14&size=400x400&sensor=false&maptype=satellite";
    m_gMapView.loadImage(imgMapPath);
    
    // Update path for street view
    string imgStreetPath = "http://maps.googleapis.com/maps/api/streetview?size=200x200&location="
    + m_str_latitude
    + ","
    + m_str_longitude
    + "&fov=90&heading=235&pitch=10&sensor=false";
    m_gStreetView.loadImage(imgStreetPath);

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
    m_gMapView.draw( 100, 100 );
    
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
  ```

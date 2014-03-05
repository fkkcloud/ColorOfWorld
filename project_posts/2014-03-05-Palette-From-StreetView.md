Implemented interactive map view / street view based on mouse's position adding to latitude and longitude.

With the loaded street view, it will put out some color palettes. (Color Filter Algorithm is not applied yet)

![Example Image](../project_images/scan01.png?raw=true "Example Image")
![Example Image](../project_images/scan02.png?raw=true "Example Image")
![Example Image](../project_images/scan03.png?raw=true "Example Image")
![Example Image](../project_images/scan04.png?raw=true "Example Image")


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
            ofRect( 545 + j * 30, 330 + k * 30, 24, 24 );
        }
    }
}

```

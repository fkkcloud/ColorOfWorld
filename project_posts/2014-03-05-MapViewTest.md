So, it was quite simple to bringing in interactive mapview from google map API into openframeworks but it seems to be a bit slow to load in image but good for Leap Motion interactivity.


```

void testApp::update(){
    string str_latitude = ofToString( latitude - mouseY * 0.00001 );
    string str_longitude = ofToString( longitude - mouseX * 0.00001 );
    string imgPath = "http://maps.googleapis.com/maps/api/staticmap?center="
    + str_latitude
    + ","
    + str_longitude
    + "&zoom=14&size=400x400&sensor=false";
    gMapView.loadImage(imgPath);
}

```

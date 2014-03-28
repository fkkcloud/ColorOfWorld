//
//  ball.h
//  footstep
//
//  Created by Pro on 3/4/14.
//
//

#ifndef footstep_ball_h
#define footstep_ball_h

#include "ofMain.h"

class Ball {
public:
    // Constructor
    Ball();
    
    // Methods
    
    void moveTo(int _xDestiny, int _yDestiny);
    void draw();
    
    // Properties
    int x;
    int y;
    ofColor color;
};
#endif

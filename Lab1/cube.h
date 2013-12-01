//
//  Cube
//
//  Extension of DrawableObject to facilitate the drawing of the cube primitive.
//
#ifndef CUBE_H
#define CUBE_H

#define CUBE_SIZE 1

#include <iostream>
#include "platform.h"
#include "drawable_object.h"

using namespace std;

class Cube : public DrawableObject {
private:
    void init();
public:
	Cube();
    ~Cube();
};

#endif

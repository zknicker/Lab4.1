//
//  Cube
//
//  Extension of DrawableObject to facilitate the drawing of the cube primitive.
//
#ifndef PLANE_H
#define PLANE_H

#define PLANE_SIZE 1

#include <iostream>
#include "platform.h"
#include "drawable_object.h"

using namespace std;

class Plane : public DrawableObject {
private:
    void init();
public:
	Plane();
    ~Plane();
};

#endif

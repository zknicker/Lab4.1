//
//  Torus
//
//  Extension of DrawableObject to facilitate the drawing of the torus primitive.
//
#ifndef TORUS_H
#define TORUS_H

#include <iostream>
#include "platform.h"
#include "drawable_object.h"

using namespace std;

class Torus : public DrawableObject {
private:
	GLuint slices;
	GLuint stacks;
	GLuint num_vertices;
	GLuint num_indices;
    
    void init();
public:
	Torus();
    ~Torus();
};

#endif

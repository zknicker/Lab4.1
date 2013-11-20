//
//  Sphere
//
//  Extension of DrawableObject to facilitate the drawing of the sphere primitive.
//
#ifndef SPHERE_H
#define SPHERE_H

#include <iostream>
#include "platform.h"
#include "DrawableObject.h"

using namespace std;

class Sphere : public DrawableObject {
private:
	GLuint slices;
	GLuint stacks;
	GLfloat radius;
	GLuint num_vertices;
	GLuint num_indices;

    void init();
public:
	Sphere();
    ~Sphere();
};

#endif

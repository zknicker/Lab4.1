//
//  Cylinder
//
//  Extension of DrawableObject to facilitate the drawing of the cylinder primitive.
//
#ifndef CYLINDER_H
#define CYLINDER_H

#include <iostream>
#include "platform.h"
#include "DrawableObject.h"

using namespace std;

class Cylinder : public DrawableObject {
private:
	GLuint slices;
	GLuint stacks;
	GLuint height;
	GLfloat top_radius;
	GLfloat bot_radius;
	GLuint num_vertices;
	GLuint num_indices;

    void init();
public:
	Cylinder();
    ~Cylinder();
};

#endif

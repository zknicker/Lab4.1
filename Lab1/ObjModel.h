//
//  OBJ Model
//
//  Extension of DrawableObject to facilitate the drawing of a .OBJ model.
//
#ifndef OBJ_MODEL_H
#define OBJ_MODEL_H

#include <iostream>
#include "platform.h"
#include "DrawableObject.h"

using namespace std;

class ObjModel : public DrawableObject {
private:
	GLuint slices;
	GLuint stacks;
	GLfloat radius;
	GLuint num_vertices;
	GLuint num_indices;

    void init();
public:
	ObjModel();
    ~ObjModel();
};

#endif

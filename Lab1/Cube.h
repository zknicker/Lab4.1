//
//  Defines a drawable cube.
//

#ifndef CUBE_H
#define CUBE_H

#include <iostream>
#include "platform.h"
#include "DrawableObject.h"

using namespace std;

class Cube : public DrawableObject {
private:
    void init();
public:
	Cube();
    ~Cube();
};

#endif

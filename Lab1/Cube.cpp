//
//  Cube.cpp
//  Shadows
//
//  Created by Zach Knickerbocker on 11/14/13.
//  Copyright (c) 2013 Zach Knickerbocker. All rights reserved.
//

#include "Cube.h"
#include "platform.h"

using namespace std;

Cube::Cube() {
    cout << "Here\n";
    setAmbient(0.5f, 0.8f, 0.2f);
    cout << ambient[0];
}

Cube::~Cube() { }
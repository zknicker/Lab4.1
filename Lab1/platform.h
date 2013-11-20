//
//  Cross-platform OpenGL dependency includes.
//

#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef __APPLE__ // Apple includes.
#include <unistd.h>
#include "glew.h"
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#include "glm/glm.hpp"
#include "glm/ext.hpp"

#else // Windows includes.
#include <direct.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#define getcwd _getcwd
#endif

// Sorry, no Linux includes. But, if you define them in
// here, they'll be used everywhere.

#endif
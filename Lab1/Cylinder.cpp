//
//  Cylinder
//
//  Extension of DrawableObject to facilitate the drawing of the cylinder primitive.
//

#include "cylinder.h"
#include "platform.h"

using namespace std;

Cylinder::Cylinder() {
	// Cylinder geometry settings.
	slices = 32;
	stacks = 6;
	height = 1;
	top_radius = 0.5f;
	bot_radius = 1.0f;
	num_vertices = stacks * slices;
	num_indices = stacks * slices * 6;

	// Rendering settings.
	draw_elements_mode = GL_TRIANGLES;
    setAmbient(0, 0.2, 0);
    setDiffuse(0, 0.7, 0);
    setSpecular(0.3, 0.3, 0.3);
    setSpecularPower(5);
    setShader(PHONG_SHADER);
    
    init();
    updateBuffers();
}

Cylinder::~Cylinder() { }

void Cylinder::init() {
	vertices.resize(num_vertices);
    indices.resize(num_indices);

	GLfloat const dA = 2 * PI / (float)(slices - 1); // delta angle
	GLfloat const dR = (float)(top_radius - bot_radius) / (float)(stacks - 1); // delta radius
    GLuint cur_stack, cur_slice;

    for(cur_stack = 0; cur_stack < stacks; cur_stack++) {
		for(cur_slice = 0; cur_slice < slices; cur_slice++) {
			
			int vert = cur_stack * slices + cur_slice;

			float const x = cos(cur_slice * dA);
			float const y = cur_stack * 1.0 / (float)(stacks - 1) - (height / 2.0f);
			float const z = sin(cur_slice * dA);
			float const rad = (bot_radius + (cur_stack * dR)) / 2.0;

			// Set x, y, and z coordinates for current vertex.
			vertices[vert].point[0] = x * rad;
			vertices[vert].color[0] = rand() % 2;
			vertices[vert].normal[0] = x;

			vertices[vert].point[1] = y;
			vertices[vert].color[1] = rand() % 2;
			vertices[vert].normal[1] = 0;

			vertices[vert].point[2] = z * rad;
			vertices[vert].color[2] = rand() % 2;
			vertices[vert].normal[2] = z;

			// The 4th dimension of the coordinate is not used.
			vertices[vert].point[3] = 1;
			vertices[vert].color[3] = 1;
			vertices[vert].normal[3] = 0;
		}
	}

	int index = 0;
    for(cur_stack = 0; cur_stack < stacks - 1; cur_stack++) {
		for(cur_slice = 0; cur_slice < slices - 1; cur_slice++) {

            indices[index++] = cur_stack * slices + cur_slice;
            indices[index++] = cur_stack * slices + (cur_slice + 1);
            indices[index++] = (cur_stack + 1) * slices + (cur_slice);

            indices[index++] = cur_stack * slices + (cur_slice + 1);
            indices[index++] = (cur_stack + 1) * slices + (cur_slice + 1);
            indices[index++] = (cur_stack + 1) * slices + (cur_slice);
		}
    }
}
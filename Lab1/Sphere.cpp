//
//  Sphere
//
//  Extension of DrawableObject to facilitate the drawing of the sphere primitive.
//

#include "sphere.h"
#include "platform.h"

using namespace std;

Sphere::Sphere() {
	// Cylinder geometry settings.
	slices = 24;
	stacks = 24;
	radius = 0.5f;
	num_vertices = slices * stacks;
	num_indices = num_vertices * 4;

	// Rendering settings.
	draw_elements_mode = GL_QUADS;
    setAmbient(0, 0.2, 0.2);
    setDiffuse(0, 0.7, 0.7);
    setSpecular(0.3, 0.3, 0.3);
    setSpecularPower(5);
    setShader(PHONG_SHADER);
    
    init();
    updateBuffers();
}

Sphere::~Sphere() { }

void Sphere::init() {
	vertices.resize(num_vertices);
    indices.resize(num_indices);

	float const dStacks = 1.0f / (float)(stacks - 1);
    float const dSlices = 1.0f / (float)(slices - 1);
    int cur_stack, cur_slice;

    for(cur_stack = 0; cur_stack < stacks; cur_stack++) {
		for(cur_slice = 0; cur_slice < slices; cur_slice++) {
			
            float const x = cos(2 * PI * cur_slice * dSlices) * sin(PI * cur_stack * dStacks);
            float const y = sin(-1 * PI / 2.0f + PI * cur_stack * dStacks);
            float const z = sin(2 * PI * cur_slice * dSlices) * sin(PI * cur_stack * dStacks);

			float const tx = cos(2 * PI * cur_slice * dSlices) * sin(PI * cur_stack * dStacks);
            float const ty = sin(-1 * PI / 2.0f + PI * cur_stack * dStacks);
            float const tz = sin(2 * PI * cur_slice * dSlices) * sin(PI * cur_stack * dStacks);

			int vert = cur_stack * slices + cur_slice;

			// Set x, y, and z coordinates for current vertex.
            vertices[vert].point[0] = x * radius;
			vertices[vert].color[0] = rand() % 2;
			vertices[vert].normal[0] = x;

            vertices[vert].point[1] = y * radius;
			vertices[vert].color[1] = rand() % 2;
			vertices[vert].normal[1] = y;

            vertices[vert].point[2] = z * radius;
			vertices[vert].color[2] = rand() % 2;
			vertices[vert].normal[2] = z;

			// The 4th dimension of the coordinate is not used.
			vertices[vert].point[3] = 1;
			vertices[vert].color[3] = 1;
			vertices[vert].normal[3] = 0;

			// Texturing
			vertices[vert].texcoord[0] = 1.0f - (float) cur_slice / (float) slices;
			vertices[vert].texcoord[1] = (float) cur_stack / (float) stacks;
		}
	}

	int index = 0;
    for(cur_stack = 0; cur_stack < stacks - 1; cur_stack++) {
		for(cur_slice = 0; cur_slice < slices - 1; cur_slice++) {
			
            indices[index++] = cur_stack * slices + cur_slice;
            indices[index++] = cur_stack * slices + (cur_slice + 1);
            indices[index++] = (cur_stack + 1) * slices + (cur_slice + 1);
            indices[index++] = (cur_stack + 1) * slices + cur_slice;
		}
    }
}
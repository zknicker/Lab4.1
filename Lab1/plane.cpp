//
//  Plane
//
//  Extension of DrawableObject to facilitate the drawing of the cube primitive.
//

#include "plane.h"
#include "platform.h"

using namespace std;

Plane::Plane() {
	draw_elements_mode = GL_QUADS;

    setAmbient(0, 0, 0.2);
    setDiffuse(0, 0, 0.7);
    setSpecular(0.3, 0.3, 0.3);
    setSpecularPower(5);
    setShader(PHONG_SHADER);
    
    init();
    updateBuffers();
}

Plane::~Plane() { }

void Plane::init() {
    float half_plane_size = PLANE_SIZE / 2.0;

    vertices.resize(4);
    indices.resize(4);

	vertices[0].point[0] = -1;	vertices[0].point[1] = -1;	vertices[0].point[2] = -1;	vertices[0].point[3] = 1; // Front Lower Left
	vertices[1].point[0] = -1;	vertices[1].point[1] = 1;	vertices[1].point[2] = 1;	vertices[1].point[3] = 1; // Front Upper left
	vertices[2].point[0] = 1;	vertices[2].point[1] = -1;	vertices[2].point[2] = -1;	vertices[2].point[3] = 1; // Front Lower Right
	vertices[3].point[0] = 1;	vertices[3].point[1] = 1;	vertices[3].point[2] = 1;	vertices[3].point[3] = 1; // Front Upper Right

	vertices[0].normal[0] = 0;	vertices[0].normal[1] = 0;	vertices[0].normal[2] = 1;	vertices[0].normal[3] = 0;
	vertices[1].normal[0] = 0;	vertices[1].normal[1] = 0;	vertices[1].normal[2] = 1;	vertices[1].normal[3] = 0;
	vertices[2].normal[0] = 0;	vertices[2].normal[1] = 0;	vertices[2].normal[2] = 1;	vertices[2].normal[3] = 0;
	vertices[3].normal[0] = 0;	vertices[3].normal[1] = 0;	vertices[3].normal[2] = 1;	vertices[3].normal[3] = 0;
    
	vertices[0].texcoord[0] = 0;	vertices[0].texcoord[1] = 0;
	vertices[1].texcoord[0] = 0;	vertices[1].texcoord[1] = 1;
	vertices[2].texcoord[0] = 1;	vertices[2].texcoord[1] = 0;
	vertices[3].texcoord[0] = 1;	vertices[3].texcoord[1] = 1;
    
	indices[0] = 0;     indices[1] = 1;     indices[2] = 3;     indices[3] = 2; // Top Face
}
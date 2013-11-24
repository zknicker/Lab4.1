//
//  Cube
//
//  Extension of DrawableObject to facilitate the drawing of the cube primitive.
//

#include "Cube.h"
#include "platform.h"

using namespace std;

Cube::Cube() {
	draw_elements_mode = GL_QUADS;

    setAmbient(0, 0, 0.2);
    setDiffuse(0, 0, 0.7);
    setSpecular(0.3, 0.3, 0.3);
    setSpecularPower(5);
    setShader(PHONG_SHADER);
    
    init();
    updateBuffers();
}

Cube::~Cube() { }

void Cube::init() {
    float half_cube_size = CUBE_SIZE / 2.0;

    vertices.resize(8);
    indices.resize(24);

	vertices[0].point[0] = -half_cube_size;		vertices[0].point[1] = -half_cube_size;		vertices[0].point[2] = -half_cube_size;		vertices[0].point[3] = 1; // Back Lower Left
	vertices[1].point[0] = -half_cube_size;		vertices[1].point[1] = -half_cube_size;		vertices[1].point[2] = half_cube_size;		vertices[1].point[3] = 1; // Front Lower Left
	vertices[2].point[0] = -half_cube_size;		vertices[2].point[1] = half_cube_size;		vertices[2].point[2] = -half_cube_size;		vertices[2].point[3] = 1; // Back Upper Left
	vertices[3].point[0] = -half_cube_size;		vertices[3].point[1] = half_cube_size;		vertices[3].point[2] = half_cube_size;		vertices[3].point[3] = 1; // Front Upper left
	vertices[4].point[0] = half_cube_size;		vertices[4].point[1] = -half_cube_size;		vertices[4].point[2] = -half_cube_size;		vertices[4].point[3] = 1; // Back Lower Right
	vertices[5].point[0] = half_cube_size;		vertices[5].point[1] = -half_cube_size;		vertices[5].point[2] = half_cube_size;		vertices[5].point[3] = 1; // Front Lower Right
	vertices[6].point[0] = half_cube_size;		vertices[6].point[1] = half_cube_size;		vertices[6].point[2] = -half_cube_size;		vertices[6].point[3] = 1; // Back Upper Right
	vertices[7].point[0] = half_cube_size;		vertices[7].point[1] = half_cube_size;		vertices[7].point[2] = half_cube_size;		vertices[7].point[3] = 1; // Front Upper Right
	
	// Note: normals are computed lazily. Technically, there should be multiple normals per vertex such that the cube looks like an actual cube.
	// But, for the sake of keeping this code quite a bit smaller (8 vertexes above as opposed to 24), we're sticking with this. It looks satisfactory.
	vertices[0].normal[0] = -half_cube_size;	vertices[0].normal[1] = -half_cube_size;	vertices[0].normal[2] = -half_cube_size;	vertices[0].normal[3] = 0;
	vertices[1].normal[0] = -half_cube_size;	vertices[1].normal[1] = -half_cube_size;	vertices[1].normal[2] = half_cube_size;		vertices[1].normal[3] = 0;
	vertices[2].normal[0] = -half_cube_size;	vertices[2].normal[1] = half_cube_size;		vertices[2].normal[2] = -half_cube_size;	vertices[2].normal[3] = 0;
	vertices[3].normal[0] = -half_cube_size;	vertices[3].normal[1] = half_cube_size;		vertices[3].normal[2] = half_cube_size;		vertices[3].normal[3] = 0;
	vertices[4].normal[0] = half_cube_size;		vertices[4].normal[1] = -half_cube_size;	vertices[4].normal[2] = -half_cube_size;	vertices[4].normal[3] = 0;
	vertices[5].normal[0] = half_cube_size;		vertices[5].normal[1] = -half_cube_size;	vertices[5].normal[2] = half_cube_size;		vertices[5].normal[3] = 0;
	vertices[6].normal[0] = half_cube_size;		vertices[6].normal[1] = half_cube_size;		vertices[6].normal[2] = -half_cube_size;	vertices[6].normal[3] = 0;
	vertices[7].normal[0] = half_cube_size;		vertices[7].normal[1] = half_cube_size;		vertices[7].normal[2] = half_cube_size;		vertices[7].normal[3] = 0;
    
	vertices[0].color[0] = 1;	vertices[0].color[1] = 0;	vertices[0].color[2] = 0;	vertices[0].color[3] = 1;
	vertices[1].color[0] = 1;	vertices[1].color[1] = 1;	vertices[1].color[2] = 0;	vertices[1].color[3] = 1;
	vertices[2].color[0] = 1;	vertices[2].color[1] = 0;	vertices[2].color[2] = 1;	vertices[2].color[3] = 1;
	vertices[3].color[0] = 0;	vertices[3].color[1] = 1;	vertices[3].color[2] = 0;	vertices[3].color[3] = 1;
	vertices[4].color[0] = 0;	vertices[4].color[1] = 1;	vertices[4].color[2] = 1;	vertices[4].color[3] = 1;
	vertices[5].color[0] = 0;	vertices[5].color[1] = 0;	vertices[5].color[2] = 1;	vertices[5].color[3] = 1;
	vertices[6].color[0] = 0;	vertices[6].color[1] = 1;	vertices[6].color[2] = 0;	vertices[6].color[3] = 1;
	vertices[7].color[0] = 0;	vertices[7].color[1] = 0;	vertices[7].color[2] = 1;	vertices[7].color[3] = 1;
    
	vertices[0].texcoord[0] = 0;	vertices[0].texcoord[1] = 1;
	vertices[1].texcoord[0] = 0;	vertices[1].texcoord[1] = 0;
	vertices[2].texcoord[0] = 1;	vertices[2].texcoord[1] = 0;
	vertices[3].texcoord[0] = 1;	vertices[3].texcoord[1] = 1;
	vertices[4].texcoord[0] = 1;	vertices[4].texcoord[1] = 1;
	vertices[5].texcoord[0] = 1;	vertices[5].texcoord[1] = 0;
	vertices[6].texcoord[0] = 0;	vertices[6].texcoord[1] = 0;
	vertices[7].texcoord[0] = 0;	vertices[7].texcoord[1] = 1;
    
	indices[0] = 2;     indices[1] = 3;     indices[2] = 7;     indices[3] = 6; // Top Face
	indices[4] = 0;     indices[5] = 1;     indices[6] = 5;     indices[7] = 4; // Bottom Face
	indices[8] = 1;     indices[9] = 3;     indices[10] = 7;	indices[11] = 5; // Front Face
	indices[12] = 0;	indices[13] = 2;	indices[14] = 6;	indices[15] = 4; // Back Face
	indices[16] = 0;	indices[17] = 1;	indices[18] = 3;	indices[19] = 2; // Left Face
	indices[20] = 4;	indices[21] = 5;	indices[22] = 7;	indices[23] = 6; // Right Face
}
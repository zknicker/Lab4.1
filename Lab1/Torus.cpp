//
//  Torus
//
//  Extension of DrawableObject to facilitate the drawing of the torus primitive.
//

#include "Torus.h"
#include "platform.h"

using namespace std;

Torus::Torus() {
	// Torus geometry settings.
	slices = 40;
	stacks = 40;
	num_vertices = slices * stacks;
	num_indices = slices * 6 * stacks;
    
	// Rendering settings.
	draw_elements_mode = GL_TRIANGLES;
    setAmbient(0.1, 0.1, 0.3);
    setDiffuse(0.0, 0.0, 0.7);
    setSpecular(0.5, 0.5, 0.5);
    setSpecularPower(5);
    setShader(PHONG_SHADER);
    
    init();
    updateBuffers();
}

Torus::~Torus() { }

void cross(float a0, float a1, float a2, float b0, float b1, float b2, float*a_cross_b) {
    a_cross_b[0] = a1*b2-b1*a2;
    a_cross_b[1] = a2*b0-b2*a0;
    a_cross_b[2] = a0*b1-a1*b0;
}

void Torus::init() {
    cout << "Init torus...\n";
	vertices.resize(num_vertices);
    indices.resize(num_indices);
 
    float istep = 2 * 3.14159 / (float)(slices);
    float jstep = 2 * 3.14159 / (float)(stacks);
 
    for (int j =0; j < stacks; j++) {
        for (int i=0; i < slices; i++) {
            int idx = j * slices + i; // mesh[j][i]
 
            float s = i * istep;
            float t = j * jstep;
 
            // first vertex
            float x = 3*cos(s)+cos(t)*cos(s);
            float y = 3*sin(s)+cos(t)*sin(s);
            float z = sin(t);
 
            vertices[idx].point[0] = x;
            vertices[idx].point[1] = y;
            vertices[idx].point[2] = z;
 
            float tangentx = -3*sin(s)-cos(t)*sin(s);
            float tangenty =  3*cos(s)+cos(t)*cos(s);
            float tangentz =  0;
            float dx_dt = -sin(t)*cos(s);
            float dy_dt = -sin(t)*sin(s);
            float dz_dt = cos(t);
 
            float normal[3];
            float binormal[3];
 
            cross(tangentx, tangenty, tangentz, dx_dt, dy_dt, dz_dt, normal);
            cross(normal[0], normal[1], normal[2], tangentx, tangenty, tangentz, binormal);
 
            vertices[idx].normal[0] = normal[0];
            vertices[idx].normal[1] = normal[1];
            vertices[idx].normal[2] = normal[2];
 
            vertices[idx].tangent[0] = tangentx;
            vertices[idx].tangent[1] = tangenty;
            vertices[idx].tangent[2] = tangentz;
            
            vertices[idx].point[3] = 1.0;  vertices[idx].normal[3] = 0.0;
 
            float mag = sqrt(tangentx*tangentx+tangenty*tangenty+tangentz*tangentz);
            vertices[idx].color[0] = tangentx/mag;
            vertices[idx].color[1] = tangenty/mag;
            vertices[idx].color[2] = tangentz/mag;
            vertices[idx].color[3] = 1.0;
            
            vertices[idx].texcoord[0] = i / (float) stacks;
            vertices[idx].texcoord[1] = j / (float) slices;
        }
    }
    
    // now create the index array
    int n = 0;
    for (int j =0; j < stacks; j++) {
        for (int i=0; i < slices; i++) {
 
            int i1 = i;
            int i2 = (i+1) % slices;
            int j1 = j;
            int j2 = (j+1) % stacks;
 
            int idx1 = j1* slices + i1;
            int idx2 = j1* slices + i2;
            int idx3 = j2* slices + i2;
            int idx4 = j2* slices + i1;
 
            indices[n++] = idx1;
            indices[n++] = idx2;
            indices[n++] = idx4;
 
            indices[n++] = idx2;
            indices[n++] = idx3;
            indices[n++] = idx4;
        }
    }
}
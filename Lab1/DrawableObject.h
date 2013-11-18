//
//  DrawableObject.h
//  Shadows
//
//  Created by Zach Knickerbocker on 11/18/13.
//  Copyright (c) 2013 Zach Knickerbocker. All rights reserved.
//

#ifndef DRAWABLE_OJBECT_H
#define DRAWABLE_OBJECT_H

#include <iostream>
#include <vector>
#include "platform.h"

using namespace std;
using namespace glm;

class DrawableObject {
protected:
    // Vertices
    vector<vec4> vertices;
    vector<vec4> normals;
    vector<vec2> uvs;
    vector<vec3> tangents;
    
    // Lighting
    float ambient[3];
    float diffuse[3];
    float specular[3];
    float specular_power;
	
    // Texturing
    int use_texture;
    
    // Transformations
    glm::mat4 model;
	glm::mat4 trackball;
    
    // Drawing
	GLuint array_buffer;
	GLuint element_array_buffer;
	GLenum draw_elements_mode;
	GLsizei draw_elements_count;
	GLenum draw_elements_type;
    
public:
	DrawableObject();
    ~DrawableObject();
    
    void setAmbient(float r, float g, float b);
    void setDiffuse(float r, float g, float b);
    void setSpecular(float r, float g, float b);
    void setSpecularPower(float power);
    void setUseTexture(int should_use);
    
    float* getAmbient();
    float* getDiffuse();
    float* getSpecular();
    float getSpecularPower();
    int getUseTexture();
    
    void setModelMatrix(glm::mat4 model);
    void setTrackballMatrix(glm::mat4 trackball);
    
    void setVertices(vector<vec4> vertices);
    void setNormals(vector<vec4> normals);
    void setTextureCoords(vector<vec2> uv);
    void setTangents(vector<vec3> tangents);
    
    void draw(Scene *scene, int shader);
};

#endif

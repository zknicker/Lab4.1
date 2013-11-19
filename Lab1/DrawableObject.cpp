//
//  DrawableObject.cpp
//  Shadows
//
//  Created by Zach Knickerbocker on 11/18/13.
//  Copyright (c) 2013 Zach Knickerbocker. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stack>
#include <math.h>
#include <iostream>
#include <vector>
#include "platform.h"
#include "DrawableObject.h"
#include "shader_utils.h"
#include "primitives.h"

DrawableObject::DrawableObject() { cout << "Instantiated drawable object.";
    // Store vertex and index data in the GPU.
    glGenBuffers(1, &array_buffer_id);
    glGenBuffers(1, &element_array_buffer_id);
}

DrawableObject::~DrawableObject() { }

void DrawableObject::updateBuffers() {
    // Store vertex and index data in the GPU.
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
    
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), &indices[0], GL_STATIC_DRAW);
    
    // Reset binded buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DrawableObject::setAmbient(float r, float g, float b) {
    ambient[0] = r;
    ambient[1] = g;
    ambient[2] = b;
}

void DrawableObject::setDiffuse(float r, float g, float b) {
    diffuse[0] = r;
    diffuse[1] = g;
    diffuse[2] = b;
}

void DrawableObject::setSpecular(float r, float g, float b) {
    specular[0] = r;
    specular[1] = g;
    specular[2] = b;
}

void DrawableObject::setSpecularPower(float power) {
    specular_power = power;
}

void DrawableObject::setUseTexture(int use) {
    use_texture = use;
}

void DrawableObject::setTextureId(int texture) {
    texture_id = texture;
}

void DrawableObject::setShader(int shader) {
    this->shader = shader;
}

void DrawableObject::draw(Scene *scene, int shader) {
    GLuint program, verticesId, normalsId, textureCoordsId;
    
	if (shader == PHONG_SHADER) {
        program = scene->phong_shader.program;
		glUniform3f(scene->phong_shader.ambientMatId, ambient[0], ambient[1], ambient[2]);
		glUniform3f(scene->phong_shader.diffuseMatId, diffuse[0], diffuse[1], diffuse[2]);
		glUniform3f(scene->phong_shader.specularMatId, specular[0], specular[1], specular[2]);
		glUniform1f(scene->phong_shader.specularPowerId, specular_power);
		glUniform1i(scene->phong_shader.use_texture_id, use_texture);
		updatePhongShader(scene);
        
	} else if (shader == SHADOW_SHADER) {
		program = scene->shadow_shader.program;
		updateShadowShader(scene);
        
	} else if (shader == PICKER_SHADER) {
        program = scene->picking_shader.program;
		updatePickingShader(scene);
	} else {
        cout << "Fatal error: invalid shader specified while drawing object.";
        exit(1);
    }
    
    glUseProgram(program);
    verticesId = glGetAttribLocation(program, "vertex_model");
    normalsId = glGetAttribLocation(program, "normal_model");
    textureCoordsId = glGetAttribLocation(program, "tex_coord");
    
	// Bind buffers.
	glBindBuffer(GL_ARRAY_BUFFER, array_buffer_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_id);
    
    glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
    
    glVertexAttribPointer(verticesId, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Vertices
    glVertexAttribPointer(normalsId, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)16); // Normals
    glVertexAttribPointer(textureCoordsId, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)48); // Texture Coords
    
    // Draw the cube.
	glDrawElements(draw_elements_mode, (int) indices.size(), GL_SHORT, (char*) NULL + 0);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    
	// Unbind buffers.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
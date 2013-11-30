//
//  DrawableObject
//
//  Creates and draws a drawable object to the screen. This is the base class
//  for all objects in the scene.
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

DrawableObject::DrawableObject() {
    // Store vertex and index data in the GPU.
    glGenBuffers(1, &array_buffer_id);
    glGenBuffers(1, &element_array_buffer_id);

	// Field defaults.
	use_texture = 0;
    use_texture_glow = 0;
    texture_glow_power = 1;
	texture = 0;
    reflect_cubemap = 0;
    light_texture = 0;
	draw_elements_mode = GL_QUADS;
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

void DrawableObject::setUseTexture(int should_use) {
    use_texture = should_use;
}

void DrawableObject::setTexture(int texture) {
    this->texture = texture;
}

void DrawableObject::setReflectCubemap(int reflect_cubemap) {
    this->reflect_cubemap = reflect_cubemap;
}

void DrawableObject::setLightTexture(int light_texture) {
    this->light_texture = light_texture;
}

void DrawableObject::setUseTextureGlow(int should_use) {
    use_texture_glow = should_use;
}

void DrawableObject::setTextureGlowPower(float power) {
    texture_glow_power = power;
}

void DrawableObject::setShader(int shader) {
    this->shader = shader;
}

void DrawableObject::draw(Scene *scene, int shader) {
	// Setup shader vars.
    GLuint program, vertex_model_id, normal_model_id, tex_coord_id, tangent_id;

	if (shader == PHONG_SHADER) {
        program = scene->phong_shader.program;
		glUseProgram(program);
		glUniform3f(scene->phong_shader.ambientMatId, ambient[0], ambient[1], ambient[2]);
		glUniform3f(scene->phong_shader.diffuseMatId, diffuse[0], diffuse[1], diffuse[2]);
		glUniform3f(scene->phong_shader.specularMatId, specular[0], specular[1], specular[2]);
		glUniform1f(scene->phong_shader.specularPowerId, specular_power);
        glUniform1f(scene->phong_shader.textureGlowPowerId, texture_glow_power);
		glUniform1i(scene->phong_shader.use_texture_id, use_texture);
		glUniform1i(scene->phong_shader.reflect_cubemap_id, reflect_cubemap);
		glUniform1i(scene->phong_shader.light_texture_id, light_texture);
		glUniform1i(scene->phong_shader.glow_id, use_texture_glow);
		updatePhongShader(scene);
        
	} else if (shader == SHADOW_SHADER) {
		program = scene->shadow_shader.program;
		glUseProgram(program);
		updateShadowShader(scene);
        
	} else if (shader == PICKER_SHADER) {
        program = scene->picking_shader.program;
		glUseProgram(program);
		updatePickingShader(scene);
	} else if (shader == BUMPMAP_SHADER) {
		program = scene->bumpmap_shader.program;
		glUseProgram(program);
		glUniform3f(scene->bumpmap_shader.ambientMatId, ambient[0], ambient[1], ambient[2]);
		glUniform3f(scene->bumpmap_shader.diffuseMatId, diffuse[0], diffuse[1], diffuse[2]);
		glUniform3f(scene->bumpmap_shader.specularMatId, specular[0], specular[1], specular[2]);
		glUniform1f(scene->bumpmap_shader.specularPowerId, specular_power);
		updateBumpMapShader(scene);
	} else {
        cout << "Fatal error: invalid shader specified while drawing object.";
        exit(1);
    }
        
	vertex_model_id = glGetAttribLocation(program, "vertex_model");
    normal_model_id = glGetAttribLocation(program, "normal_model");
    tex_coord_id = glGetAttribLocation(program, "tex_coord");

	// Bind buffers.
	glBindBuffer(GL_ARRAY_BUFFER, array_buffer_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_id);
    
	// Configure shader attribute pointers.
    glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
    
    glVertexAttribPointer(vertex_model_id, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Vertices
    glVertexAttribPointer(normal_model_id, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)16); // Normals
    glVertexAttribPointer(tex_coord_id, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)48); // Texture Coords

	if (shader == BUMPMAP_SHADER) {
		tangent_id = glGetAttribLocation(program, "tangent");
		glVertexAttribPointer(tangent_id, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)64); // Tangent
	}

    // Draw the cube.
	glDrawElements(draw_elements_mode, indices.size(), GL_UNSIGNED_SHORT, (char*) NULL + 0);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
    
	// Unbind buffers.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
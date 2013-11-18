#include <stdio.h>
#include <stdlib.h>
#include <stack> 
#include <math.h>
#include <iostream>
#include <vector>

#include "platform.h"
#include "primitives.h"
#include "shader_utils.h"
#include "normal_utils.h"

Vertex cube[CUBE_VERTICES];
GLushort cube_indices[CUBE_INDICES];
Vertex cylinder[CYLINDER_VERTICES];
GLushort cylinder_indices[CYLINDER_INDICES];
Vertex sphere[SPHERE_VERTICES];
GLushort sphere_indices[SPHERE_INDICES];

// VBOs for cube, cylinder, and sphere primitives.
GLuint primitives[6];

using namespace std;
using namespace glm;

Object cube_object;

/* Initialize VBOs for primitives.
 * -------------------------------------------------------------------------- */
void initPrimitives() 
{
	// Initialize VBOs.
	glGenBuffers(6, primitives);

	// Initialize primitives (each init function populates its primitivess).
	initCube();
	initCylinder();
	initSphere();

	// Copy cube verts, indices to video card.
	glBindBuffer(GL_ARRAY_BUFFER, primitives[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 8, cube, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitives[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 24, cube_indices, GL_STATIC_DRAW);

	// Copy cylinder verts, indices to video card.
	glBindBuffer(GL_ARRAY_BUFFER, primitives[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * CYLINDER_SLICES * CYLINDER_STACKS, cylinder, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitives[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * CYLINDER_INDICES, cylinder_indices, GL_STATIC_DRAW);

	// Copy sphere verts, indices to video card.
	glBindBuffer(GL_ARRAY_BUFFER, primitives[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * SPHERE_SLICES * SPHERE_STACKS, sphere, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitives[5]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * SPHERE_SLICES * SPHERE_STACKS * 4, sphere_indices, GL_STATIC_DRAW);

	// Reset binded buffer.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
} 

/* Initalize Cube - initializes cube vertices, default colors, and indices.
 * -------------------------------------------------------------------------- */
void initCube() {
	float half_cube_size = CUBE_SIZE / 2.0;

	cube[0].point[0] = -half_cube_size;		cube[0].point[1] = -half_cube_size;		cube[0].point[2] = -half_cube_size;		cube[0].point[3] = 1; // Back Lower Left
	cube[1].point[0] = -half_cube_size;		cube[1].point[1] = -half_cube_size;		cube[1].point[2] = half_cube_size;		cube[1].point[3] = 1; // Front Lower Left
	cube[2].point[0] = -half_cube_size;		cube[2].point[1] = half_cube_size;		cube[2].point[2] = -half_cube_size;		cube[2].point[3] = 1; // Back Upper Left
	cube[3].point[0] = -half_cube_size;		cube[3].point[1] = half_cube_size;		cube[3].point[2] = half_cube_size;		cube[3].point[3] = 1; // Front Upper left
	cube[4].point[0] = half_cube_size;		cube[4].point[1] = -half_cube_size;		cube[4].point[2] = -half_cube_size;		cube[4].point[3] = 1; // Back Lower Right
	cube[5].point[0] = half_cube_size;		cube[5].point[1] = -half_cube_size;		cube[5].point[2] = half_cube_size;		cube[5].point[3] = 1; // Front Lower Right
	cube[6].point[0] = half_cube_size;		cube[6].point[1] = half_cube_size;		cube[6].point[2] = -half_cube_size;		cube[6].point[3] = 1; // Back Upper Right
	cube[7].point[0] = half_cube_size;		cube[7].point[1] = half_cube_size;		cube[7].point[2] = half_cube_size;		cube[7].point[3] = 1; // Front Upper Right
	
	// Note: normals are computed lazily. Technically, there should be multiple normals per vertex such that the cube looks like an actual cube.
	// But, for the sake of keeping this code quite a bit smaller (8 vertexes above as opposed to 24), we're sticking with this. It looks satisfactory.
	cube[0].normal[0] = -half_cube_size;	cube[0].normal[1] = -half_cube_size;	cube[0].normal[2] = -half_cube_size;	cube[0].normal[3] = 0;
	cube[1].normal[0] = -half_cube_size;	cube[1].normal[1] = -half_cube_size;	cube[1].normal[2] = half_cube_size;		cube[1].normal[3] = 0;
	cube[2].normal[0] = -half_cube_size;	cube[2].normal[1] = half_cube_size;		cube[2].normal[2] = -half_cube_size;	cube[2].normal[3] = 0;
	cube[3].normal[0] = -half_cube_size;	cube[3].normal[1] = half_cube_size;		cube[3].normal[2] = half_cube_size;		cube[3].normal[3] = 0;
	cube[4].normal[0] = half_cube_size;		cube[4].normal[1] = -half_cube_size;	cube[4].normal[2] = -half_cube_size;	cube[4].normal[3] = 0;
	cube[5].normal[0] = half_cube_size;		cube[5].normal[1] = -half_cube_size;	cube[5].normal[2] = half_cube_size;		cube[5].normal[3] = 0;
	cube[6].normal[0] = half_cube_size;		cube[6].normal[1] = half_cube_size;		cube[6].normal[2] = -half_cube_size;	cube[6].normal[3] = 0;
	cube[7].normal[0] = half_cube_size;		cube[7].normal[1] = half_cube_size;		cube[7].normal[2] = half_cube_size;		cube[7].normal[3] = 0;

	cube[0].color[0] = 1;	cube[0].color[1] = 0;	cube[0].color[2] = 0;	cube[0].color[3] = 1;
	cube[1].color[0] = 1;	cube[1].color[1] = 1;	cube[1].color[2] = 0;	cube[1].color[3] = 1;
	cube[2].color[0] = 1;	cube[2].color[1] = 0;	cube[2].color[2] = 1;	cube[2].color[3] = 1;
	cube[3].color[0] = 0;	cube[3].color[1] = 1;	cube[3].color[2] = 0;	cube[3].color[3] = 1;
	cube[4].color[0] = 0;	cube[4].color[1] = 1;	cube[4].color[2] = 1;	cube[4].color[3] = 1;
	cube[5].color[0] = 0;	cube[5].color[1] = 0;	cube[5].color[2] = 1;	cube[5].color[3] = 1;
	cube[6].color[0] = 0;	cube[6].color[1] = 1;	cube[6].color[2] = 0;	cube[6].color[3] = 1;
	cube[7].color[0] = 0;	cube[7].color[1] = 0;	cube[7].color[2] = 1;	cube[7].color[3] = 1;

	cube[0].texcoord[0] = 1;	cube[0].texcoord[1] = 1;
	cube[1].texcoord[0] = 1;	cube[1].texcoord[1] = 0;
	cube[2].texcoord[0] = 1;	cube[2].texcoord[1] = 1;
	cube[3].texcoord[0] = 1;	cube[3].texcoord[1] = 0;
	cube[4].texcoord[0] = 0;	cube[4].texcoord[1] = 1;
	cube[5].texcoord[0] = 0;	cube[5].texcoord[1] = 0;
	cube[6].texcoord[0] = 0;	cube[6].texcoord[1] = 1;
	cube[7].texcoord[0] = 0;	cube[7].texcoord[1] = 0;

	cube_indices[0] = 2;	cube_indices[1] = 3;	cube_indices[2] = 7;	cube_indices[3] = 6; // Top Face
	cube_indices[4] = 0;	cube_indices[5] = 1;	cube_indices[6] = 5;	cube_indices[7] = 4; // Bottom Face
	cube_indices[8] = 1;	cube_indices[9] = 3;	cube_indices[10] = 7;	cube_indices[11] = 5; // Front Face
	cube_indices[12] = 0;	cube_indices[13] = 2;	cube_indices[14] = 6;	cube_indices[15] = 4; // Back Face 
	cube_indices[16] = 0;	cube_indices[17] = 1;	cube_indices[18] = 3;	cube_indices[19] = 2; // Left Face
	cube_indices[20] = 4;	cube_indices[21] = 5;	cube_indices[22] = 7;	cube_indices[23] = 6; // Right Face
}

/* Draw Cube - draws a cube. Takes a Color parameter to draw the object as 
 * a solid color.
 * -------------------------------------------------------------------------- */
void drawCube(Scene *scene, bool shadow, bool texture) {
	
    GLuint c0, c1, c2;
    
    Object cubeObject;
    cubeObject.ambient_mat[0] = 0.0; cubeObject.ambient_mat[0] = 0.0; cubeObject.ambient_mat[0] = 0.2;
    
    
	if (!shadow) {
		glUniform3f(scene->phong_shader.ambientMatId, 0, 0, 0.2);
		glUniform3f(scene->phong_shader.diffuseMatId, 0, 0, 0.7);
		glUniform3f(scene->phong_shader.specularMatId, 0.3, 0.3, 0.3);
		glUniform1f(scene->phong_shader.specularPowerId, 5);
		if (texture) glUniform1i(scene->phong_shader.use_texture_id, 1);
		else glUniform1i(scene->phong_shader.use_texture_id, 0);
		updatePhongShader(scene);
        
        c0 = glGetAttribLocation(scene->phong_shader.program, "vertex_model");
        c1 = glGetAttribLocation(scene->phong_shader.program, "normal_model");
        c2 = glGetAttribLocation(scene->phong_shader.program, "tex_coord");

	} else {
		glUseProgram(scene->shadow_shader.program);
		updateShadowShader(scene);
        
        c0 = glGetAttribLocation(scene->shadow_shader.program, "vertex_model");
        c1 = glGetAttribLocation(scene->shadow_shader.program, "normal_model");
        c2 = glGetAttribLocation(scene->shadow_shader.program, "tex_coord");
	}

	// Bind cube buffers.
	glBindBuffer(GL_ARRAY_BUFFER, primitives[0]); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitives[1]);

    glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
    
    vector<glm::vec3> vertices, normals;
    vector<glm::vec2> uvs;
    vertices.resize(CUBE_VERTICES);
    normals.resize(CUBE_VERTICES);
    uvs.resize(8);
    for(int i = 0; i < CUBE_VERTICES; i++) {
        vertices[i] = vec3(cube[i].point[0], cube[i].point[1], cube[i].point[2]);
        normals[i] = vec3(cube[i].normal[0], cube[i].normal[1], cube[i].normal[2]);
        uvs[i] = vec2(cube[i].texcoord[0], cube[i].texcoord[1]);
    }
    
    vector<glm::vec3> tangents;
    vector<glm::vec3> bitangents;
    //computeTangentBasis(vertices, uvs, normals, tangents, bitangents);
    
    glVertexAttribPointer(c0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Vertices
    glVertexAttribPointer(c1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)16); // Normals
    glVertexAttribPointer(c2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)48); // Texture Coords

    // Draw the cube.
	glDrawElements(GL_QUADS, CUBE_INDICES, GL_UNSIGNED_SHORT, (char*) NULL + 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

	// Unbind buffers.
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/* Draw Cube - draws a cube. Takes a Color parameter to draw the object as 
 * a solid color.
 * -------------------------------------------------------------------------- */
void drawObject(Scene *scene, Object *object, int shader) {
	
    GLuint program, verticesId, normalsId, textureCoordsId;
    
	if (shader == PHONG_SHADER) {
        program = scene->phong_shader.program;
		glUniform3f(scene->phong_shader.ambientMatId, object->ambient_mat[0], object->ambient_mat[1], object->ambient_mat[2]);
		glUniform3f(scene->phong_shader.diffuseMatId, object->diffuse_mat[0], object->diffuse_mat[1], object->diffuse_mat[2]);
		glUniform3f(scene->phong_shader.specularMatId, object->specular_mat[0], object->specular_mat[1], object->specular_mat[2]);
		glUniform1f(scene->phong_shader.specularPowerId, object->specular_pow);
		glUniform1i(scene->phong_shader.use_texture_id, object->use_texture);
		updatePhongShader(scene);

	} else if (shader == SHADOW_SHADER) {
		program = scene->shadow_shader.program;
		updateShadowShader(scene);

	} else if (shader == PICKER_SHADER) {
        program = scene->picking_shader.program;
		int r = (object->id & 0x000000FF) >>  0;
		int g = (object->id & 0x0000FF00) >>  8;
		int b = (object->id & 0x00FF0000) >> 16;
		glUniform3f(scene->phong_shader.ambientMatId, r, g, b);
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
	glBindBuffer(GL_ARRAY_BUFFER, object->array_buffer); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->element_array_buffer);

    glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Vertices
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)16); // Normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)48); // Texture Coords

    // Draw the cube.
	glDrawElements(object->draw_elements_mode, object->draw_elements_count, object->draw_elements_type, (char*) NULL + 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

	// Unbind buffers.
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/* Initalize Cylinder - initializes cylinder vertices, default colors, 
 * and indices.
 * -------------------------------------------------------------------------- */
void initCylinder() {
    float const dA = 2 * PI / (float)(CYLINDER_SLICES - 1); // delta angle
	float const dR = (float)(CYLINDER_TOPRAD - CYLINDER_BOTRAD) / (float)(CYLINDER_STACKS - 1); // delta radius
    int cur_stack, cur_slice;

    for(cur_stack = 0; cur_stack < CYLINDER_STACKS; cur_stack++) {
		for(cur_slice = 0; cur_slice < CYLINDER_SLICES; cur_slice++) {
			
			int vert = cur_stack * CYLINDER_SLICES + cur_slice;

			float const x = cos(cur_slice * dA);
			float const y = cur_stack * 1.0 / (float)(CYLINDER_STACKS - 1) - (CYLINDER_HEIGHT / 2.0f);
			float const z = sin(cur_slice * dA);
			float const rad = (CYLINDER_BOTRAD + (cur_stack * dR)) / 2.0;

			// Set x, y, and z coordinates for current vertex.
			cylinder[vert].point[0] = x * rad;
			cylinder[vert].color[0] = rand() % 2;
			cylinder[vert].normal[0] = x;

			cylinder[vert].point[1] = y;
			cylinder[vert].color[1] = rand() % 2;
			cylinder[vert].normal[1] = 0;

			cylinder[vert].point[2] = z * rad;
			cylinder[vert].color[2] = rand() % 2;
			cylinder[vert].normal[2] = z;

			// The 4th dimension of the coordinate is not used.
			cylinder[vert].point[3] = 1;
			cylinder[vert].color[3] = 1;
			cylinder[vert].normal[3] = 0;
		}
	}

	int index = 0;
    for(cur_stack = 0; cur_stack < CYLINDER_STACKS - 1; cur_stack++) {
		for(cur_slice = 0; cur_slice < CYLINDER_SLICES - 1; cur_slice++) {

            cylinder_indices[index++] = cur_stack * CYLINDER_SLICES + cur_slice;
            cylinder_indices[index++] = cur_stack * CYLINDER_SLICES + (cur_slice + 1);
            cylinder_indices[index++] = (cur_stack + 1) * CYLINDER_SLICES + (cur_slice);

            cylinder_indices[index++] = cur_stack * CYLINDER_SLICES + (cur_slice + 1);
            cylinder_indices[index++] = (cur_stack + 1) * CYLINDER_SLICES + (cur_slice + 1);
            cylinder_indices[index++] = (cur_stack + 1) * CYLINDER_SLICES + (cur_slice);
		}
    }
}

/* Draw Cylinder - draws a cylinder. Takes a Color parameter to draw the
 * object as a solid color.
 * -------------------------------------------------------------------------- */
void drawCylinder(Scene *scene, bool shadow) {
	drawCylinder(scene, NULL, shadow);
}
void drawCylinder(Scene *scene, Color *color, bool shadow) {
	
    GLuint c0, c1, c2;
    
	if (!shadow) {
		glUniform3f(scene->phong_shader.ambientMatId, 0, 0.2, 0);
		glUniform3f(scene->phong_shader.diffuseMatId, 0, 0.7, 0);
		glUniform3f(scene->phong_shader.specularMatId, 0.3, 0.3, 0.3);
		glUniform1f(scene->phong_shader.specularPowerId, 5);
		glUniform1i(scene->phong_shader.use_texture_id, 0);
		updatePhongShader(scene);
        
        c0 = glGetAttribLocation(scene->phong_shader.program, "vertex_model");
        c1 = glGetAttribLocation(scene->phong_shader.program, "normal_model");
        c2 = glGetAttribLocation(scene->phong_shader.program, "tex_coord");

	} else {
		glUseProgram(scene->shadow_shader.program);
		updateShadowShader(scene);
        
        c0 = glGetAttribLocation(scene->shadow_shader.program, "vertex_model");
        c1 = glGetAttribLocation(scene->shadow_shader.program, "normal_model");
        c2 = glGetAttribLocation(scene->shadow_shader.program, "tex_coord");
	}

	// Bind cube buffers.
	glBindBuffer(GL_ARRAY_BUFFER, primitives[2]); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitives[3]);
	
    glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
    
    glVertexAttribPointer(c0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Vertices
    glVertexAttribPointer(c1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)16); // Normals
    glVertexAttribPointer(c2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)48); // Texture
	
	// Draw the cylinder.
	glDrawElements(GL_TRIANGLES, CYLINDER_INDICES, GL_UNSIGNED_SHORT, (char*) NULL + 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

	// Unbind buffers.
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/* Initalize Sphere - initializes sphere vertices, default colors, 
 * and indices.
 * -------------------------------------------------------------------------- */
void initSphere() {
	float const dStacks = 1.0f / (float)(SPHERE_STACKS - 1);
    float const dSlices = 1.0f / (float)(SPHERE_SLICES - 1);
    int cur_stack, cur_slice;

    for(cur_stack = 0; cur_stack < SPHERE_STACKS; cur_stack++) {
		for(cur_slice = 0; cur_slice < SPHERE_SLICES; cur_slice++) {
			
            float const x = cos(2 * PI * cur_slice * dSlices) * sin(PI * cur_stack * dStacks);
            float const y = sin(-1 * PI / 2.0f + PI * cur_stack * dStacks);
            float const z = sin(2 * PI * cur_slice * dSlices) * sin(PI * cur_stack * dStacks);

			int vert = cur_stack * SPHERE_SLICES + cur_slice;

			// Set x, y, and z coordinates for current vertex.
            sphere[vert].point[0] = x * SPHERE_RADIUS;
			sphere[vert].color[0] = rand() % 2;
			sphere[vert].normal[0] = x;

            sphere[vert].point[1] = y * SPHERE_RADIUS;
			sphere[vert].color[1] = rand() % 2;
			sphere[vert].normal[1] = y;

            sphere[vert].point[2] = z * SPHERE_RADIUS;
			sphere[vert].color[2] = rand() % 2;
			sphere[vert].normal[2] = z;

			// The 4th dimension of the coordinate is not used.
			sphere[vert].point[3] = 1;
			sphere[vert].color[3] = 1;
			sphere[vert].normal[3] = 0;
		}
	}

	int index = 0;
    for(cur_stack = 0; cur_stack < SPHERE_STACKS - 1; cur_stack++) {
		for(cur_slice = 0; cur_slice < SPHERE_SLICES - 1; cur_slice++) {
			
            sphere_indices[index++] = cur_stack * SPHERE_SLICES + cur_slice;
            sphere_indices[index++] = cur_stack * SPHERE_SLICES + (cur_slice + 1);
            sphere_indices[index++] = (cur_stack + 1) * SPHERE_SLICES + (cur_slice + 1);
            sphere_indices[index++] = (cur_stack + 1) * SPHERE_SLICES + cur_slice;
		}
    }
}

/* Draw Sphere - draws a sphere.
 * -------------------------------------------------------------------------- */
void drawSphere(Scene *scene, bool shadow) {
	drawSphere(scene, NULL, shadow);
}

/* Draw Sphere - draws a sphere. Takes a Color parameter to draw the
 * object as a solid color.
 * -------------------------------------------------------------------------- */
void drawSphere(Scene *scene, Color *color, bool shadow) {

    GLuint c0, c1, c2;
    
	if (!shadow) {
		glUseProgram(scene->phong_shader.program);
		glUniform3f(scene->phong_shader.ambientMatId, 0, 0.2, 0.2);
		glUniform3f(scene->phong_shader.diffuseMatId, 0, 0.7, 0.7);
		glUniform3f(scene->phong_shader.specularMatId, 0.3, 0.3, 0.3);
		glUniform1f(scene->phong_shader.specularPowerId, 5);
		glUniform1i(scene->phong_shader.use_texture_id, 0);
		updatePhongShader(scene);
        
        c0 = glGetAttribLocation(scene->phong_shader.program, "vertex_model");
        c1 = glGetAttribLocation(scene->phong_shader.program, "normal_model");
        c2 = glGetAttribLocation(scene->phong_shader.program, "tex_coord");

	} else {
		glUseProgram(scene->shadow_shader.program);
		updateShadowShader(scene);
        
        c0 = glGetAttribLocation(scene->shadow_shader.program, "vertex_model");
        c1 = glGetAttribLocation(scene->shadow_shader.program, "normal_model");
        c2 = glGetAttribLocation(scene->shadow_shader.program, "tex_coord");
	}

	// Bind cube buffers.
	glBindBuffer(GL_ARRAY_BUFFER, primitives[4]); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitives[5]);
	
    glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

    glVertexAttribPointer(c0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Vertices
    glVertexAttribPointer(c1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)16); // Normals
    glVertexAttribPointer(c2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)48); // Texture
	
	// Draw the cylinder.
	glDrawElements(GL_QUADS, SPHERE_INDICES, GL_UNSIGNED_SHORT, (char*) NULL + 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

	// Unbind buffers.
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
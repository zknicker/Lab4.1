/* ------------------------------------------------------------
 * Lab 4 - Textures, Shadows, and a Trackball
 * CSE 5542
 * Zach Knickerbocker
 *
 * This program draws a scene which uses shader-implemented
 * lighting, shadows, and texture mapping. A trackball is also
 * implemented which enables rotation of the scene on all axis.
 *
 * (q) quit the program.
 * (w) enable wireframes.
 * (v) draw trackball.
 * (p) disable shadows.
 * (left mouse button) hold and drag to rotate the camera.
 * (right mouse button) hold and drag to zoom.
 * ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <stack> 
#include <math.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include <GL/glew.h> // includes gl.h
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "shader_utils.h"
#include "obj_utils.h"
#include "primitives.h"
#include "trackball.h"

using namespace glm;
using namespace std;

/* Window settings. */
int const window_size = 1200;

/* Trackball and mouse vars. */
int press_x, press_y;
float last_pos[3] = {0.0f, 0.0f, 0.0f};
Trackball trackball = { 0.0, {0.0, 1.0, 0.0}};

/* The .OBJ that is imported and drawn. */
vector<Vertex> obj;
GLuint obj_buffer;

/* The scene struct that describes the scene being drawn. */
Scene scene;

/* Draw flags. */
bool draw_wireframes = false;
bool draw_shadows = true;
bool draw_trackball = false;

/* Initializes the desired model's (.obj) VBO and prepares it to be drawn.
 * -------------------------------------------------------------------------- */
void initObj(string obj_path) {
	// Read our .obj file
	ifstream ifile(obj_path);
	obj = LoadOBJ(ifile);

	// Load it into a VBO
	glGenBuffers(1, &obj_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj_buffer);
	glBufferData(GL_ARRAY_BUFFER, obj.size() * sizeof(Vertex), &obj[0], GL_STATIC_DRAW);
}

/* Draws the desired model (.obj).
 * -------------------------------------------------------------------------- */
void drawObj(bool shadow) {
	// Matrices
	scene.model = glm::scale(scene.model, vec3(0.1, 0.1, 0.1));
	
	if (!shadow) {
		glUseProgram(scene.phong_shader.program);
		glUniform3f(scene.phong_shader.ambientMatId, 0.2, 0, 0);
		glUniform3f(scene.phong_shader.diffuseMatId, 0.7, 0, 0);
		glUniform3f(scene.phong_shader.specularMatId, 0.3, 0.3, 0.3);
		glUniform1f(scene.phong_shader.specularPowerId, 5);
		glUniform1i(scene.phong_shader.use_texture_id, 0);
		glUniform3f(scene.phong_shader.lightId, scene.light_pos.x, scene.light_pos.y, scene.light_pos.z);
		updatePhongShader(&scene);

	} else {
		glUseProgram(scene.shadow_shader.program);
		glUniform3f(scene.shadow_shader.lightId, scene.light_pos.x, scene.light_pos.y, scene.light_pos.z);
		updateShadowShader(&scene);
	}
	
	// Drawing
    glBindBuffer(GL_ARRAY_BUFFER, obj_buffer);
    glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Vertices
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)16); // Normals

    glDrawArrays(GL_TRIANGLES, 0, obj.size());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

/* Main animation function. Draws the screen.
 * -------------------------------------------------------------------------- */
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(scene.phong_shader.program);

	// Camera vars.
	vec3 position = vec3(0, 2.0, -4.0);
	vec3 center = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);

    // Setup Model, View, and Projection matrices.
	scene.projection = glm::perspective(80.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	scene.view = glm::lookAt(position, center, up);
    scene.model = glm::mat4(1.0);

	// Update the light.
    scene.light_pos = glm::vec3(0.0, 4.0, -6.0);
	glUniform3f(scene.phong_shader.lightMatId, 1.0, 1.0, 1.0);

	// Create the transformation matrix stack.
	stack<mat4> model_stack;
	model_stack.push(scene.model);

	glEnable(GL_STENCIL_TEST); 
	glStencilFunc(GL_ALWAYS, 1, ~0); 
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Draw trackball rotation.
	scene.model = scene.model * trackball.trackball_rot;
	model_stack.push(scene.model);

	// Update plane normal.
	vec4 plane_normal = trackball.trackball_rot * vec4(0.0, 1.0, 0.0, 1.0);
	scene.shadow_shader.plane_normal = vec3(plane_normal.x, plane_normal.y, plane_normal.z);

	// Draw trackball.
	if (draw_trackball) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		scene.model = glm::scale(scene.model, vec3(5.0, 5.0, 5.0));
		drawSphere(&scene, false);
		scene.model = model_stack.top();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glEnable(GL_POLYGON_OFFSET_FILL); // fight the z (z-fighting fix)!
	glPolygonOffset(1,1); 

	// Draw ground.
	scene.model = glm::scale(scene.model, vec3(10.0, 0.1, 10.0));
	scene.model = glm::translate(scene.model, vec3(0.0, -0.5f, 0.0));
	drawCube(&scene, false, true);
	scene.model = model_stack.top();
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_STENCIL_TEST);
	
	// Global transform all objects upward.
	scene.model = glm::translate(scene.model, vec3(0.0, 0.8, 0.0));
	model_stack.push(scene.model);

	// Draw wireframes?
	if (draw_wireframes) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // Draw objects.
	scene.model = model_stack.top();
	scene.model = glm::translate(scene.model, vec3(-0.75, 0.0, 0.0));
	drawCube(&scene, false, false);
	scene.model = glm::translate(scene.model, vec3(-1.5, 0.0, 0.0));
	drawSphere(&scene, false);
	scene.model = model_stack.top();
	scene.model = glm::translate(scene.model, vec3(0.75, 0.0, 0.0));
	drawCylinder(&scene, false);
	scene.model = glm::translate(scene.model, vec3(1.5, -0.5, 0.0));
	drawObj(false);

	// Draw shadows.
	if (draw_shadows) {
		glEnable(GL_STENCIL_TEST); 
		glStencilFunc(GL_EQUAL, 1, ~0); 
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

		scene.model = model_stack.top();
		scene.model = glm::translate(scene.model, vec3(-0.75, 0.0, 0.0));
		drawCube(&scene, true, false);
		scene.model = glm::translate(scene.model, vec3(-1.5, 0.0, 0.0));
		drawSphere(&scene, true);
		scene.model = model_stack.top();
		scene.model = glm::translate(scene.model, vec3(0.75, 0.0, 0.0));
		drawCylinder(&scene, true);
		scene.model = glm::translate(scene.model, vec3(1.5, -0.5, 0.0));
		drawObj(true);

		glDisable(GL_BLEND);
		glDisable(GL_STENCIL_TEST);
	}

	// Reset wireframe?
	if (draw_wireframes) glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glutSwapBuffers();
}

void getObjectClicked(int x, int y) {
	cout << "mouse clicked";

	glUseProgram(scene.picking_shader.program);

	// Clear the screen in white
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Only the positions are needed (not the UVs and normals)
    glEnableVertexAttribArray(0);
	/*
    glm::mat4 RotationMatrix = glm::toMat4(orientations[i]);
    glm::mat4 TranslationMatrix = translate(mat4(), positions[i]);
    glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix;

    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform
    glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);

    // Convert "i", the integer mesh ID, into an RGB color
    int r = (i & 0x000000FF) >>  0;
    int g = (i & 0x0000FF00) >>  8;
    int b = (i & 0x00FF0000) >> 16;

    // OpenGL expects colors to be in [0,1], so divide by 255.
    glUniform4f(pickingColorID, r/255.0f, g/255.0f, b/255.0f, 1.0f);

    // 1rst attribute buffer : vertices
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0,                  // attribute
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

    // Draw the triangles !
    glDrawElements(
            GL_TRIANGLES,      // mode
            indices.size(),    // count
            GL_UNSIGNED_SHORT,   // type
            (void*)0           // element array buffer offset
    );

    glDisableVertexAttribArray(0);

    // Wait until all the pending drawing commands are really done.
    // Ultra-mega-over slow ! 
    // There are usually a long time between glDrawElements() and
    // all the fragments completely rasterized.
    glFlush();
    glFinish(); 

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Read the pixel at the center of the screen.
    // You can also use glfwGetMousePos().
    // Ultra-mega-over slow too, even for 1 pixel, 
    // because the framebuffer is on the GPU.
    unsigned char data[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Convert the color back to an integer ID
    int pickedID = 
            data[0] + 
            data[1] * 256 +
            data[2] * 256*256;

    if (pickedID == 0x00ffffff){ // Full white, must be the background !
            message = "background";
    }else{
            std::ostringstream oss;
            oss << "mesh " << pickedID;
            message = oss.str();
    }
	glClearColor(1.0, 0.0, 0.0, 0.0);
	glutSwapBuffers();
	*/
}

/* Responses to mouse click events.
 * -------------------------------------------------------------------------- */
void mouseController(int button, int state, int x, int y) {
	if(button==GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			//press_x = x; press_y = y;
			//mapToTrackball(x, y, window_size, last_pos);
			getObjectClicked(x, y);
		}
	}
}

/* Responses to mouse movement events.
 * -------------------------------------------------------------------------- */
void motionController(int x, int y) {
	//updateTrackball(x, y, window_size, last_pos, &trackball);
}

/* Responses to keyboard key down events.
 * -------------------------------------------------------------------------- */
void keyboardController(unsigned char key, int x, int y)
{
	if (key == 'q') {
		exit(1);
	}

	if (key == 'w') {
		draw_wireframes = !draw_wireframes;
	}

	if (key == 'v') {
		draw_trackball = !draw_trackball;
	}

	if (key == 'p') {
		draw_shadows = !draw_shadows;
	}
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_STENCIL);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(window_size,window_size);
	glutCreateWindow("Lab 4: Trackball, Textures, and Shadows");

	cout << "Lab 4: Trackball, Textures, and Shadows\nCSE 5542\nZach Knickerbocker\n\n";
	cout << "----------------------------------------\n";
	checkGraphics();
	cout << "\n----------------------------------------\n";
	cout << "Initializing application.\n";

	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMouseFunc(mouseController); 
	glutMotionFunc(motionController);
	glutKeyboardFunc(keyboardController);

	glewInit();
	if (!glewIsSupported("GL_VERSION_3_0")) {
		cout << "Could not detect OpenGL 3.0. This could mean trouble.\n";
	}

	cout << "Loading the cube, cylinder, and sphere primitives.\n";
	initPrimitives();

	// Get the obj from the command line param.
	cout << "Loading custom object: pumpkin.obj (887kb)\n";
	initObj("pumpkin.obj");

	// Setup the shaders.
	cout << "Initializing phong shdaer... ";
	setupPhongShader(&scene.phong_shader);
	cout << "Initializing shadow shader... ";
	setupShadowShader(&scene.shadow_shader);
	cout << "Initializing picking shader... ";
	setupPickingShader(&scene.picking_shader);

	// Setup texture 1.
	cout << "Loading the custom texture: tiles.ppm (5190kb)\n";
	readTexture(&scene, "tiles.ppm");

	// Load texture 1.
	cout << "tiles.ppm loaded. Loading the texture to the GPU.\n";
	glGenTextures(1, &scene.phong_shader.tex_one_id); 
	glBindTexture(GL_TEXTURE_2D, scene.phong_shader.tex_one_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scene.tex_one_image_width, scene.tex_one_image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &scene.tex_one_image_v[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glUniform1i(scene.phong_shader.tex_one_sampler, 0);

	cout << "\n----------------------------------------\n";
	cout << "(q) to quit.\n(w) to toggle wireframes.\n";
	cout << "(v) to draw the trackball.\n";
	cout << "(p) to disable shadows.\n";
	cout << "(left mouse button) rotate the scene.";
	glutMainLoop();

	return 0;
}
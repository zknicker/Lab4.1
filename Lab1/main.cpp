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
 * (left mouse button) hold and drag to rotate the scene.
 * ------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <stack> 
#include <math.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

#include "platform.h"
#include "shader_utils.h"
#include "obj_utils.h"
#include "primitives.h"
#include "trackball.h"
#include "Cube.h"

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

Cube *myCube;

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
    
    GLuint c0, c1, c2;
    
	// Matrices
	scene.model = glm::scale(scene.model, vec3(0.1, 0.1, 0.1));
	
	if (!shadow) {
		glUseProgram(scene.phong_shader.program);
		glUniform3f(scene.phong_shader.ambientMatId, 0.2, 0, 0);
		glUniform3f(scene.phong_shader.diffuseMatId, 0.7, 0, 0);
		glUniform3f(scene.phong_shader.specularMatId, 0.3, 0.3, 0.3);
		glUniform1f(scene.phong_shader.specularPowerId, 5);
		glUniform1i(scene.phong_shader.use_texture_id, 0);
		updatePhongShader(&scene);
        
        c0 = glGetAttribLocation(scene.phong_shader.program, "vertex_model");
        c1 = glGetAttribLocation(scene.phong_shader.program, "normal_model");
        c2 = glGetAttribLocation(scene.phong_shader.program, "tex_coord");

	} else {
		glUseProgram(scene.shadow_shader.program);
		updateShadowShader(&scene);
        
        c0 = glGetAttribLocation(scene.shadow_shader.program, "vertex_model");
        c1 = glGetAttribLocation(scene.shadow_shader.program, "normal_model");
        c2 = glGetAttribLocation(scene.shadow_shader.program, "tex_coord");
	}
	
	// Drawing
    glBindBuffer(GL_ARRAY_BUFFER, obj_buffer);
    glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

    glVertexAttribPointer(c0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // Vertices
    glVertexAttribPointer(c1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)16); // Normals
    glVertexAttribPointer(c2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)48); // Texture Coords

    int size = (int) obj.size();
    glDrawArrays(GL_TRIANGLES, 0, size);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
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
	scene.projection = glm::perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	scene.view = glm::lookAt(position, center, up);
    scene.model = glm::mat4(1.0);

	// Update the light.
    scene.light_pos = glm::vec3(0.0, 4.0, -10.0);
	glUniform3f(scene.phong_shader.lightMatId, 1.0, 1.0, 1.0);

	// Create the transformation matrix stack.
	stack<mat4> model_stack;
	model_stack.push(scene.model);

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
    
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    
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
	myCube->draw(&scene, PHONG_SHADER);
    //drawCube(&scene, false, false);
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

/* Responses to mouse click events.
 * -------------------------------------------------------------------------- */
void mouseController(int button, int state, int x, int y) {
	if(button==GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			press_x = x; press_y = y;
			mapToTrackball(x, y, window_size, last_pos);
		}
	}
}

/* Responses to mouse movement events.
 * -------------------------------------------------------------------------- */
void motionController(int x, int y) {
	updateTrackball(x, y, window_size, last_pos, &trackball);
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
    char buffer[1000];
    cout << "Current working directory is: " << getcwd(buffer, 1000) << "\n";
    cout << "Make sure that the relevant file dependencies are included here.\n";
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
	cout << "Initializing phong shader... ";
	setupPhongShader(&scene.phong_shader);
	cout << "Initializing shadow shader... ";
	setupShadowShader(&scene.shadow_shader);
	cout << "Initializing picking shader... ";
	setupPickingShader(&scene.picking_shader);

	// Setup texture 1.
	cout << "Loading the custom texture: tiles.ppm (5190kb)\n";
	readTexture(&scene, "tiles.ppm");

    myCube = new Cube();
    
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
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
#include "Cylinder.h"
#include "Sphere.h"

using namespace glm;
using namespace std;

/* Window settings. */
int const window_size = 1200;

/* Trackball and mouse vars. */
mat4 trackball_rot;

/* The .OBJ that is imported and drawn. */
vector<Vertex> obj;
GLuint obj_buffer;

/* The scene struct that describes the scene being drawn. */
Scene scene;

/* Draw flags. */
bool draw_wireframes = false;
bool draw_shadows = false;
bool draw_trackball = false;

Cube *myCube;
Cube *bumpedCube;
Cylinder *myCylinder;
Sphere *mySphere;
Sphere *bumpedSphere;

// User Defined Variables
GLUquadricObj *quadratic;

const float PI2 = 2.0*3.1415926535f;

Matrix4fT   Transform   = {  1.0f,  0.0f,  0.0f,  0.0f,
                             0.0f,  1.0f,  0.0f,  0.0f,
                             0.0f,  0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  0.0f,  1.0f };

Matrix3fT   LastRot     = {  1.0f,  0.0f,  0.0f,
                             0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  1.0f };

Matrix3fT   ThisRot     = {  1.0f,  0.0f,  0.0f,
                             0.0f,  1.0f,  0.0f,
                             0.0f,  0.0f,  1.0f };

ArcBallT    ArcBall(window_size, window_size);
Point2fT    MousePt;


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
double myrot = 0.0;
/* Draws a six sided cube representing the environment.
 * -------------------------------------------------------------------------- */
void drawEnvironment(stack<mat4> *model_stack) {
	double room_size = 50.0;
	double wall_width = 0.05;
	
	model_stack->push(scene.model);

	// Using texture 0
	glActiveTexture(GL_TEXTURE0 + 0);

	glEnable(GL_POLYGON_OFFSET_FILL); // fight the z (z-fighting fix)!
	glPolygonOffset(1,1);
    
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, ~0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Draw negative Y wall.
	scene.model = glm::scale(scene.model, vec3(room_size, wall_width, room_size));
	scene.model = glm::translate(scene.model, vec3(0.0, -room_size * 0.5 * (1.0 / wall_width), 0.0));
	glBindTexture(GL_TEXTURE_2D, scene.phong_shader.cubemap_textures[3]);
	myCube->draw(&scene, PHONG_SHADER);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_STENCIL_TEST);

	// Draw positive y wall.
	scene.model = glm::translate(scene.model, vec3(0.0, room_size * (1.0 / wall_width), 0.0));
	scene.model = glm::rotate(scene.model, 180.0f, vec3(0.0, 1.0, 0.0));
	scene.model = glm::rotate(scene.model, 180.0f, vec3(0.0, 0.0, 1.0));
	glBindTexture(GL_TEXTURE_2D, scene.phong_shader.cubemap_textures[2]);
	myCube->draw(&scene, PHONG_SHADER);

	// Draw positive X wall.
	scene.model = model_stack->top();
	scene.model = glm::rotate(scene.model, -90.0f, vec3(0.0, 0.0, 1.0));
	scene.model = glm::translate(scene.model, vec3(0.0, -room_size / 2.0, 0.0));
	scene.model = glm::rotate(scene.model, -90.0f, vec3(0.0, 1.0, 0.0));
	scene.model = glm::scale(scene.model, vec3(room_size, wall_width, room_size));
	glBindTexture(GL_TEXTURE_2D, scene.phong_shader.cubemap_textures[0]);
	myCube->draw(&scene, PHONG_SHADER);

	// Draw negative X wall.
	scene.model = glm::translate(scene.model, vec3(0.0, room_size * (1.0 / wall_width), 0.0));
	scene.model = glm::rotate(scene.model, 180.0f, vec3(0.0, 0.0, 1.0));
	glBindTexture(GL_TEXTURE_2D, scene.phong_shader.cubemap_textures[1]);
	myCube->draw(&scene, PHONG_SHADER);
	
	// Draw positive Z wall.
	scene.model = model_stack->top();
	scene.model = glm::rotate(scene.model, -90.0f, vec3(1.0, 0.0, 0.0));
	scene.model = glm::translate(scene.model, vec3(0.0, -room_size / 2.0, 0.0));
	scene.model = glm::scale(scene.model, vec3(room_size, wall_width, room_size));
	glBindTexture(GL_TEXTURE_2D, scene.phong_shader.cubemap_textures[4]);
	myCube->draw(&scene, PHONG_SHADER);

	// Draw negative Z wall.
	scene.model = glm::translate(scene.model, vec3(0.0, room_size * (1.0 / wall_width), 0.0));
	scene.model = glm::rotate(scene.model, 180.0f, vec3(1.0, 0.0, 0.0));
	scene.model = glm::rotate(scene.model, 180.0f, vec3(0.0, 1.0, 0.0));
	glBindTexture(GL_TEXTURE_2D, scene.phong_shader.cubemap_textures[5]);
	myCube->draw(&scene, PHONG_SHADER);

	model_stack->pop();
}

float sphere_rot = 0.0f;
/* Main animation function. Draws the screen.
 * -------------------------------------------------------------------------- */
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(scene.phong_shader.program);

	// Camera vars.
	scene.camera_pos = glm::vec3(0, 2.0, -10.0);
	vec3 center = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);

    // Setup Model, View, and Projection matrices.
	scene.projection = glm::perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	scene.view = glm::lookAt(scene.camera_pos, center, up);
    scene.model = glm::mat4(1.0);

	// Update the light.
    scene.light_pos = glm::vec3(0.0, 2.0, -4.0);

	// Create the transformation matrix stack.
	stack<mat4> model_stack;
	model_stack.push(scene.model);

	// Draw trackball rotation.
	//scene.model = scene.model * trackball_rot;
	mat4 trackball_rotation = mat4(
		Transform.M[0], Transform.M[1], Transform.M[2], Transform.M[3],
		Transform.M[4], Transform.M[5], Transform.M[6], Transform.M[7],
		Transform.M[8], Transform.M[9], Transform.M[10], Transform.M[11],
		Transform.M[12], Transform.M[13], Transform.M[14], Transform.M[15]);
	scene.model = scene.model * trackball_rotation;
	model_stack.push(scene.model);

	// Update plane normal.
	//vec4 plane_normal = trackball_rot * vec4(0.0, 1.0, 0.0, 1.0);
	vec4 plane_normal = vec4(0.0, 1.0, 0.0, 1.0);
	scene.shadow_shader.plane_normal = vec3(plane_normal.x, plane_normal.y, plane_normal.z);

	// Draw trackball.
	if (draw_trackball) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		scene.model = glm::scale(scene.model, vec3(5.0, 5.0, 5.0));
		drawSphere(&scene, false);
		scene.model = model_stack.top();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
    
	// Draw environment.
	//drawEnvironment(&model_stack);
	scene.model = model_stack.top();
    
	// Global transform all objects.
	scene.model = glm::translate(scene.model, vec3(0.0, 0.0, 0.0));
	scene.model = glm::scale(scene.model, vec3(3.0, 3.0, 3.0));
	model_stack.push(scene.model);

	// Draw wireframes?
	if (draw_wireframes) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // Draw objects.
	scene.model = model_stack.top();
	//glActiveTexture(GL_TEXTURE0 + 2);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, scene.phong_shader.cubemap_texture);
	//myCylinder->draw(&scene, PHONG_SHADER);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, scene.bumpmap_texture.id);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, scene.bumpmap_gradients_texture.id);
	scene.model = glm::scale(scene.model, vec3(2.0, 0.001, 1.0));
	bumpedCube->draw(&scene, BUMPMAP_SHADER);

	// Reset wireframe?
	if (draw_wireframes) glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    glutSwapBuffers();
}

int left_mouse_depressed = 0;
int right_mouse_depressed = 0;
int last_x_movement = 0;
int last_y_movement = 0;

/* Responses to mouse click events.
 * -------------------------------------------------------------------------- */
void mouseController(int button, int state, int x, int y) {

	// Rotation
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			MousePt.s.X = (GLfloat) x;
            MousePt.s.Y = (GLfloat) y;

			LastRot = ThisRot;
			ArcBall.click(&MousePt);
		}
	}
}

/* Responses to mouse movement events.
 * -------------------------------------------------------------------------- */
void motionController(int x, int y) {
	MousePt.s.X = (GLfloat) x;
    MousePt.s.Y = (GLfloat) y;
	Quat4fT     ThisQuat;

    ArcBall.drag(&MousePt, &ThisQuat);
    Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);
    Matrix3fMulMatrix3f(&ThisRot, &LastRot);
    Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);
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
	
	if (key == 'd') {
		sphere_rot += 1.0f;
	}

	if (key == 'a') {
		sphere_rot -= 1.0f;
	}
}

void loadCubeMap() {

	glEnable(GL_TEXTURE_CUBE_MAP); 
	glGenTextures(1, &scene.phong_shader.cubemap_texture); 
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, scene.phong_shader.cubemap_texture); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); 
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_REPLACE);

	Texture cubemap[6];
	cubemap[0] = Texture();
	readTexture(&cubemap[0], "space_positive_x.ppm");
	cout << "Successfully loaded cubemap positive X texture.\n";
	cubemap[1] = Texture();
	readTexture(&cubemap[1], "space_negative_x.ppm");
	cout << "Successfully loaded cubemap negative X texture.\n";
	cubemap[2] = Texture();
	readTexture(&cubemap[2], "space_positive_y.ppm");
	cout << "Successfully loaded cubemap positive Y texture.\n";
	cubemap[3] = Texture();
	readTexture(&cubemap[3], "space_negative_y.ppm");
	cout << "Successfully loaded cubemap negative Y texture.\n";
	cubemap[4] = Texture();
	readTexture(&cubemap[4], "space_positive_z.ppm");
	cout << "Successfully loaded cubemap positive Z texture.\n";
	cubemap[5] = Texture();
	readTexture(&cubemap[5], "space_negative_z.ppm");
	cout << "Successfully loaded cubemap negative Z texture.\n";

	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, cubemap[i].width, cubemap[i].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cubemap[i].image[0]);
	}

	scene.phong_shader.cubemap_sampler = glGetUniformLocation(scene.phong_shader.program, "cube_map_sampler"); 
	glUniform1i(scene.phong_shader.cubemap_sampler, 2);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + 0);
	for (int i = 0; i < 6; i++) {
		GLuint *texture_id = &scene.phong_shader.cubemap_textures[i];
		glGenTextures(1, texture_id);
		glBindTexture(GL_TEXTURE_2D, *texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cubemap[i].width, cubemap[i].height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &cubemap[i].image[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
}

int time_since_start = 0;
void idleController() {
	glutPostRedisplay();
}

int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA | GLUT_STENCIL);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(window_size,window_size);
	glutCreateWindow("Lab 5: Space, The Final Frontier");

	cout << "Lab 5: Space, The Final Frontier\nCSE 5542\nZach Knickerbocker\n\n";
	cout << "----------------------------------------\n";
	checkGraphics();
	cout << "\n----------------------------------------\n";
    char buffer[1000];
    cout << "Current working directory is: " << getcwd(buffer, 1000) << "\n";
    cout << "Make sure that the relevant file dependencies are included here.\n";
	cout << "\n----------------------------------------\n";
	cout << "Initializing application.\n";

	glutDisplayFunc(display);
	glutIdleFunc(idleController);
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
	//cout << "Loading custom object: pumpkin.obj (887kb)\n";
	//initObj("pumpkin.obj");

	// Setup the shaders.
	cout << "Initializing phong shader... ";
	setupPhongShader(&scene.phong_shader);
	cout << "Initializing shadow shader... ";
	setupShadowShader(&scene.shadow_shader);
	cout << "Initializing bump mapping shader... ";
	setupBumpMapShader(&scene.bumpmap_shader);

	// Setup bumpmap textures.
	cout << "Reading bumpmap texture.\n";
	readTexture(&scene.bumpmap_texture, "earth.ppm");
	cout << "Loading bumpmap texture.\n";
	loadTexture(&scene.bumpmap_texture);
	cout << "Calculating gradient texture for bumpmap.\n";
	calculateGradientsTexture(&scene.bumpmap_texture, &scene.bumpmap_gradients_texture, 20);
	cout << "Loading gradient texture.\n";
	loadTexture(&scene.bumpmap_gradients_texture);

    myCube = new Cube();
	myCube->setUseTexture(1);
    bumpedCube = new Cube();
	bumpedCube->setShader(BUMPMAP_SHADER);
	bumpedCube->setAmbient(0.25, 0.25, 0.25);
	bumpedCube->setDiffuse(0.8, 0.8, 0.8);
	myCylinder = new Cylinder();
	myCylinder->setUseTexture(1);
	myCylinder->setLightTexture(1);
	myCylinder->setReflectCubemap(1);
	mySphere = new Sphere();
	mySphere->setUseTexture(1);
	mySphere->setLightTexture(1);
	mySphere->setReflectCubemap(0);
	bumpedSphere = new Sphere();
	bumpedSphere->setShader(BUMPMAP_SHADER);
	bumpedSphere->setUseTexture(1);
	bumpedSphere->setLightTexture(1);
	bumpedSphere->setReflectCubemap(0);

	// Load CubeMap textures.
	//cout << "Loading the 6 cubemap textures.\n";
	//loadCubeMap();

	cout << "\n----------------------------------------\n";
	cout << "(q) to quit.\n(w) to toggle wireframes.\n";
	cout << "(v) to draw the trackball.\n";
	cout << "(p) to disable shadows.\n";
	cout << "(left mouse button) rotate the scene.";
	glutMainLoop();

	return 0;
}
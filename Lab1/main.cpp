/* ------------------------------------------------------------
 * Lab 5: Space, The Final Frontier
 * CSE 5542
 * Zach Knickerbocker
 *
 * This program draws a scene which utilizes techniques from
 * previous labs, as well as both cube mapping and bump
 * maaping. Specifically, the floating spheres are textured
 * using a cube map of a space scene, and the torus is textured
 * with an Earth which has been bump mapped. The gradients for
 * said bump map were calculated using central difference.
 *
 * Extra points:
 *   - When bump mapping is disabled, the torus is drawn with
 *     a flat earth texture that glowingly pulses. That is,
 *     it shifts between a normally lit earth texture and a
 *     glowing/bloomed earth texture. This glowing is 
 *	   accomplished via a setting in the phong shader which 
 *     results in the outputted texture-colored fragment having 
 *     been modified to appear as if glowing by some 
 *     "glow power". See the phong_tex.frag to see this 
 *     implementation.
 *
 * (q) quit the program.
 * (w) enable wireframes.
 * (v) draw/don't draw trackball.
 * (b) disable/enable bump mapping.
 * (g) disable/enable glow.
 *
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
#include "trackball.h"
#include "cube.h"
#include "cylinder.h"
#include "sphere.h"
#include "torus.h"
#include "plane.h"

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
bool draw_bumpmap = true;
bool draw_glow = true;
bool draw_star = false;

/* Object in the scene. */
Cube *wallCube;
Sphere *trackballSphere;
Torus *bumpedTorus;
Sphere *sun;
Plane *sun_corona;
Plane *sun_halo;
vector<Sphere *> rotating_spheres;

/* Scene animation variable representing stage in animation. */
float anim;

/* Trackball vars 
   Attribution: http://nehe.gamedev.net/tutorial/arcball_rotation/19003/ */
mat4 trackball_rotation;
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
	wallCube->draw(&scene, PHONG_SHADER);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glDisable(GL_STENCIL_TEST);

	// Draw positive y wall.
	scene.model = glm::translate(scene.model, vec3(0.0, room_size * (1.0 / wall_width), 0.0));
	scene.model = glm::rotate(scene.model, 180.0f, vec3(0.0, 1.0, 0.0));
	scene.model = glm::rotate(scene.model, 180.0f, vec3(0.0, 0.0, 1.0));
	glBindTexture(GL_TEXTURE_2D, scene.phong_shader.cubemap_textures[2]);
	wallCube->draw(&scene, PHONG_SHADER);

	// Draw positive X wall.
	scene.model = model_stack->top();
	scene.model = glm::rotate(scene.model, -90.0f, vec3(0.0, 0.0, 1.0));
	scene.model = glm::translate(scene.model, vec3(0.0, -room_size / 2.0, 0.0));
	scene.model = glm::rotate(scene.model, -90.0f, vec3(0.0, 1.0, 0.0));
	scene.model = glm::scale(scene.model, vec3(room_size, wall_width, room_size));
	glBindTexture(GL_TEXTURE_2D, scene.phong_shader.cubemap_textures[0]);
	wallCube->draw(&scene, PHONG_SHADER);

	// Draw negative X wall.
	scene.model = glm::translate(scene.model, vec3(0.0, room_size * (1.0 / wall_width), 0.0));
	scene.model = glm::rotate(scene.model, 180.0f, vec3(0.0, 0.0, 1.0));
	glBindTexture(GL_TEXTURE_2D, scene.phong_shader.cubemap_textures[1]);
	wallCube->draw(&scene, PHONG_SHADER);
	
	// Draw positive Z wall.
	scene.model = model_stack->top();
	scene.model = glm::rotate(scene.model, -90.0f, vec3(1.0, 0.0, 0.0));
	scene.model = glm::translate(scene.model, vec3(0.0, -room_size / 2.0, 0.0));
	scene.model = glm::scale(scene.model, vec3(room_size, wall_width, room_size));
	glBindTexture(GL_TEXTURE_2D, scene.phong_shader.cubemap_textures[4]);
	wallCube->draw(&scene, PHONG_SHADER);

	// Draw negative Z wall.
	scene.model = glm::translate(scene.model, vec3(0.0, room_size * (1.0 / wall_width), 0.0));
	scene.model = glm::rotate(scene.model, 180.0f, vec3(1.0, 0.0, 0.0));
	scene.model = glm::rotate(scene.model, 180.0f, vec3(0.0, 1.0, 0.0));
	glBindTexture(GL_TEXTURE_2D, scene.phong_shader.cubemap_textures[5]);
	wallCube->draw(&scene, PHONG_SHADER);

	model_stack->pop();
}

// Warning! Hacks ahead! :o
float star_anim = 0;
void drawStar(stack<mat4> *model_stack) {
	star_anim += .0009;

	scene.camera_pos = glm::vec3(0.1 * sin(star_anim), 0.1 * sin(star_anim), -9.0);
	vec3 center = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);

    // Setup Model, View, and Projection matrices.
	scene.projection = glm::perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	scene.view = glm::lookAt(scene.camera_pos, center, up);

	scene.star_shader.shade_corona = 0;
	scene.star_shader.shade_halo = 0;
	scene.star_shader.time = star_anim;
	scene.star_shader.spectral_lookup = 0.0 + abs(0.06f * sin(star_anim * .005f));

	// Sun
	scene.model = model_stack->top();
	scene.model = glm::scale(scene.model, vec3(5.0, 5.0, 5.0));
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + 10);
	glBindTexture(GL_TEXTURE_2D, scene.sun_surface.id);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + 11);
	glBindTexture(GL_TEXTURE_2D, scene.star_colorshift.id);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + 12);
	glBindTexture(GL_TEXTURE_2D, scene.star_colorgraph.id);
	sun->draw(&scene, STAR_SHADER);
	
	model_stack->push(scene.model);

	// Corona
	scene.star_shader.shade_corona = 1;
	scene.model = inverse(trackball_rotation) * scene.model;
	scene.model = glm::translate(scene.model, vec3(0.0, 0.1, -0.50));
	scene.model = glm::scale(scene.model, vec3(2.0, 2.0, 2.0));
	scene.model = glm::rotate(scene.model, 135.0f, vec3(1.0, 0.0, 0.0));

	glDisable(GL_DEPTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + 10);
	glBindTexture(GL_TEXTURE_2D, scene.corona.id);
	glActiveTexture(GL_TEXTURE0 + 12);
	glBindTexture(GL_TEXTURE_2D, scene.star_colorgraph.id);
	sun_corona->draw(&scene, STAR_SHADER);

	scene.star_shader.shade_corona = 0;

	// Halo
	scene.model = model_stack->top();
	model_stack->pop();
	scene.model = inverse(trackball_rotation) * scene.model;
	scene.model = glm::translate(scene.model, vec3(0.0, 0.1, -0.55));
	scene.model = glm::scale(scene.model, vec3(0.6, 0.45, 0.6));
	scene.model = glm::rotate(scene.model, 135.0f, vec3(1.0, 0.0, 0.0));
	scene.star_shader.shade_halo = 1;

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + 10);
	glBindTexture(GL_TEXTURE_2D, scene.sun_halo.id);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0 + 11);
	glBindTexture(GL_TEXTURE_2D, scene.halo_colorshift.id);
	glActiveTexture(GL_TEXTURE0 + 12);
	glBindTexture(GL_TEXTURE_2D, scene.star_colorgraph.id);
	sun_halo->draw(&scene, STAR_SHADER);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH);
	scene.star_shader.shade_halo = 0;

}

/* Main animation function. Draws the screen.
 * -------------------------------------------------------------------------- */
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(scene.phong_shader.program);

	// Camera vars.
	scene.camera_pos = glm::vec3(0, 2.5, -12.0);
	vec3 center = vec3(0, 0, 0);
	vec3 up = vec3(0, 1, 0);

    // Setup Model, View, and Projection matrices.
	scene.projection = glm::perspective(60.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	scene.view = glm::lookAt(scene.camera_pos, center, up);
    scene.model = glm::mat4(1.0);

	// Update the light.
    scene.light_pos = glm::vec3(0.0, 2.0, -15.0);
    
	// Create the transformation matrix stack.
	stack<mat4> model_stack;
	model_stack.push(scene.model);

	// Draw trackball rotation.
	//scene.model = scene.model * trackball_rot;
	trackball_rotation = mat4(
		Transform.M[0], Transform.M[1], Transform.M[2], Transform.M[3],
		Transform.M[4], Transform.M[5], Transform.M[6], Transform.M[7],
		Transform.M[8], Transform.M[9], Transform.M[10], Transform.M[11],
		Transform.M[12], Transform.M[13], Transform.M[14], Transform.M[15]);
	scene.model = scene.model * trackball_rotation;
	model_stack.push(scene.model);

	// Update plane normal.
    vec4 plane_normal = trackball_rot * vec4(0.0, 1.0, 0.0, 1.0);
	scene.shadow_shader.plane_normal = vec3(plane_normal.x, plane_normal.y, plane_normal.z);

	// Draw trackball.
	if (draw_trackball) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		scene.model = glm::scale(scene.model, vec3(15.0, 15.0, 15.0));
		trackballSphere->draw(&scene, PHONG_SHADER);
		scene.model = model_stack.top();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
    
	// Draw environment.
	drawEnvironment(&model_stack);
	scene.model = model_stack.top();
    
	if (draw_star) {
		drawStar(&model_stack);
	} else {
		// Global transform all objects.
		scene.model = glm::translate(scene.model, vec3(0.0, 0.0, 0.0));
		model_stack.push(scene.model);

		// Draw wireframes?
		if (draw_wireframes) glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

		scene.model = glm::rotate(scene.model, -15.0f, vec3(1.0, 0.0, 0.0));
		model_stack.push(scene.model);
    
		// Draw spheres.
		scene.model = model_stack.top();
		glActiveTexture(GL_TEXTURE0 + 2);
		glBindTexture(GL_TEXTURE_CUBE_MAP, scene.phong_shader.cubemap_texture);
		for (int i = 0; i < rotating_spheres.size(); i++) {
			float r = 5;
			float variance = i * 360.0 / rotating_spheres.size();
			float x = 2 * r * sin(anim + variance);
			float z = 0.0;
			float y = r * sin(2 * anim + variance);
			float s = 1.0 + i / rotating_spheres.size() * 2.0;
			glm::scale(scene.model, vec3(s, s, s));
			scene.model = glm::rotate(scene.model, variance, vec3(0.0, 1.0, 0.0));
			scene.model = glm::translate(scene.model, vec3(x, y, z));
			rotating_spheres[i]->draw(&scene, PHONG_SHADER);
			scene.model = model_stack.top();
		}
    
		scene.model = model_stack.top();
    
		// Draw torus.
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, scene.bumpmap_texture.id);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0 + 1);
		glBindTexture(GL_TEXTURE_2D, scene.bumpmap_gradients_texture.id);
		scene.model = glm::rotate(scene.model, 90.0f, vec3(1.0, 0.0, 0.0));
		if (draw_bumpmap) {
			bumpedTorus->draw(&scene, BUMPMAP_SHADER);
		} else {
			if (draw_glow) {
				glEnable(GL_TEXTURE_2D);
				glActiveTexture(GL_TEXTURE0 + 0);
				bumpedTorus->setTexture(scene.phong_shader.cubemap_textures[3]);
				bumpedTorus->setUseTextureGlow(1);
				bumpedTorus->setTextureGlowPower(abs(sin(anim * 8)));
			} else {
				glEnable(GL_TEXTURE_2D);
				glActiveTexture(GL_TEXTURE0 + 0);
				bumpedTorus->setTexture(scene.bumpmap_texture.id);
				bumpedTorus->setUseTextureGlow(0);
			}
			bumpedTorus->draw(&scene, PHONG_SHADER);
		}
	}

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
	Quat4fT ThisQuat;

    ArcBall.drag(&MousePt, &ThisQuat);
    Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);
    Matrix3fMulMatrix3f(&ThisRot, &LastRot);
    Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);
}

/* Responses to keyboard key down events.
 * -------------------------------------------------------------------------- */
void keyboardController(unsigned char key, int x, int y)
{
	if (key == 'q' || key == 'Q') {
		exit(1);
	}

	if (key == 'w' || key == 'W') {
		draw_wireframes = !draw_wireframes;
	}

	if (key == 'v' || key == 'V') {
		draw_trackball = !draw_trackball;
	}

	if (key == 'p' || key == 'P') {
		draw_shadows = !draw_shadows;
	}
	
	if (key == 'b' || key == 'B') {
		draw_bumpmap = !draw_bumpmap;
	}
    
    if (key == 'g' || key == 'G') {
        draw_glow = !draw_glow;
    }
	
    if (key == 's' || key == 'S') {
        draw_star = !draw_star;
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

int old_time_since_start = 0;
void idleController() {
    // Get delta time (thanks to http://gamedev.stackexchange.com/questions/13008/how-to-get-and-use-delta-time)
    int time_since_start = glutGet(GLUT_ELAPSED_TIME);
    int delta_time = time_since_start - old_time_since_start;
    old_time_since_start = time_since_start;
    
    anim += .0001 * delta_time;
    if (anim > 360) anim -= 360;
    
	glutPostRedisplay();
}

void loadStarTexture(Texture *texture, char *ppm, GLuint tex_num) {
	glGenTextures(1, &texture->id);
	readTexture(texture, ppm);

	glActiveTexture(GL_TEXTURE0 + tex_num);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texture->image[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	cout << "Successfully loaded " << ppm << ".\n";
}

void loadStarTextures() {
	// Setup texture uniforms.
	glEnable(GL_TEXTURE_2D);
	glUseProgram(scene.star_shader.program);

	loadStarTexture(&scene.sun_surface, "sun_surface.ppm", 10); 
	loadStarTexture(&scene.corona, "corona.ppm", 10); 
	loadStarTexture(&scene.sun_halo, "sun_halo.ppm", 10); 
	loadStarTexture(&scene.star_colorshift, "star_colorshift.ppm", 11); 
	loadStarTexture(&scene.halo_colorshift, "halo_colorshift.ppm", 11); 
	loadStarTexture(&scene.star_colorgraph, "star_colorgraph.ppm", 12); 
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
    
	// Setup the shaders.
	cout << "Initializing phong shader... ";
	setupPhongShader(&scene.phong_shader);
	cout << "Initializing shadow shader... ";
	setupShadowShader(&scene.shadow_shader);
	cout << "Initializing bump mapping shader... ";
	setupBumpMapShader(&scene.bumpmap_shader);
	cout << "Initializing star shader... ";
	setupStarShader(&scene.star_shader);

	// Setup bumpmap textures.
	cout << "Reading bumpmap texture.\n";
	readTexture(&scene.bumpmap_texture, "earth.ppm");
	cout << "Loading bumpmap texture.\n";
	loadTexture(&scene.bumpmap_texture);
	cout << "Calculating gradient texture for bumpmap.\n";
	calculateGradientsTexture(&scene.bumpmap_texture, &scene.bumpmap_gradients_texture, 10);
	cout << "Loading gradient texture.\n";
	loadTexture(&scene.bumpmap_gradients_texture);

    wallCube = new Cube();
	wallCube->setUseTexture(1);
    wallCube->setLightTexture(0);

	trackballSphere = new Sphere();

    bumpedTorus = new Torus();
    bumpedTorus->setAmbient(0.1, 0.1, 0.1);
    bumpedTorus->setDiffuse(0.6, 0.6, 0.6);
    bumpedTorus->setSpecular(0.6, 0.6, 0.6);
    bumpedTorus->setSpecularPower(6);
	bumpedTorus->setUseTexture(1);
	bumpedTorus->setLightTexture(1);
	bumpedTorus->setReflectCubemap(0);
    
	sun = new Sphere();
	sun->setUseTexture(1);
	sun->setLightTexture(0);

	sun_corona = new Plane();
	sun_corona->setUseTexture(1);
	sun_corona->setLightTexture(0);

	sun_halo = new Plane();
	sun_halo->setUseTexture(1);
	sun_halo->setLightTexture(0);

    rotating_spheres.resize(30);
    for (int i = 0; i < rotating_spheres.size(); i++) {
        rotating_spheres[i] = new Sphere();
        rotating_spheres[i]->setUseTexture(1);
        rotating_spheres[i]->setReflectCubemap(1);
        rotating_spheres[i]->setLightTexture(1);
        rotating_spheres[i]->setAmbient(0.0, 0.0, 0.0);
        rotating_spheres[i]->setDiffuse(0.6, 0.6, 0.6);
        rotating_spheres[i]->setSpecular(0.4, 0.4, 0.4);
        rotating_spheres[i]->setSpecularPower(20);
    }
    
	// Load star textures.
	loadStarTextures();

	// Load CubeMap textures.
	cout << "Loading the 6 cubemap textures.\n";
	loadCubeMap();

	cout << "\n----------------------------------------\n";
	cout << "(q) to quit.\n";
    cout << "(w) to toggle wireframes.\n";
	cout << "(v) to draw the trackball.\n";
	cout << "(b) to disable bump mapping.\n";
	cout << "(g) to disable glowing effect.\n";
	cout << "(left mouse button) rotate the scene.";
	glutMainLoop();

	return 0;
}
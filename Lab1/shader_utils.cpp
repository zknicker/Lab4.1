#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <math.h>
#include <vector>

#include "platform.h"
#include "shader_utils.h"

using namespace std;
using namespace glm;

/* Prints useful information about the client's graphics card.
 *
 * Code courtesy of:
 *   - http://www.cse.ohio-state.edu/~hwshen/5542/Site/Slides_files/shaderSetup.C
 * -------------------------------------------------------------------------- */
void checkGraphics() 
{
  const GLubyte *renderer = glGetString(GL_RENDERER); 
  const GLubyte *vendor = glGetString(GL_VENDOR); 
  const GLubyte *version = glGetString(GL_VERSION); 
  const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION); 
  GLint major, minor; 
  glGetIntegerv(GL_MAJOR_VERSION, &major); 
  glGetIntegerv(GL_MINOR_VERSION, &minor); 
  
  printf("Checking graphics capability...\n"); 
  printf("GL Vendor: %s \n", vendor); 
  printf("GL Renderer: %s \n", renderer); 
  printf("GL version: %s\n", version); 
  printf("GL version: %d.%d\n", major, minor); 
  printf("GLSL version: %s\n", glslVersion); 

  // check for shader support
  if (glewGetExtension("GL_ARB_fragment_shader")      != GL_TRUE ||
      glewGetExtension("GL_ARB_vertex_shader")        != GL_TRUE ||
      glewGetExtension("GL_ARB_shader_objects")       != GL_TRUE ||
      glewGetExtension("GL_ARB_shading_language_100") != GL_TRUE)
    {
      fprintf(stderr, "Driver does not support OpenGL Shading Language.\n");
      exit(1);
    }
  else fprintf(stderr, "GLSL supported and ready to go.\n");
}



/* Retrieves the source code of a file.
 * -------------------------------------------------------------------------- */
char* getFileContents(char *file_name)
{
    FILE *fp;
	char *file_contents = NULL;

	fp = fopen(file_name,"rt");
	if (fp != NULL) {

		// Get the size of the shader file.
		fseek(fp, 0, SEEK_END);
		int count = ftell(fp);
		rewind(fp);

		// Read in the shader file's contents.
		file_contents = (char*)malloc(sizeof(char) * (count + 1));
		count = fread(file_contents, sizeof(char), count, fp);
		file_contents[count] = '\0';
		
		fclose(fp);
	} else {
		printf("Could not open the shader file %s.\n", file_name);
	}

	return file_contents;
}

/* Prints errors that result from shader compilation issues.
 *
 * Code courtesy of:
 *   - http://www.cse.ohio-state.edu/~hwshen/5542/Site/Slides_files/shaderSetup.C
 * -------------------------------------------------------------------------- */
void printShaderErrors(GLuint shader_object, char *shader_name) {
	GLint result; 
	glGetShaderiv(shader_object, GL_COMPILE_STATUS, &result); 
	if (result == GL_FALSE) {
		GLint logLen; 
		glGetShaderiv(shader_object, GL_INFO_LOG_LENGTH, &logLen); 
		if (logLen > 0) {
			char *log = (char*) malloc(logLen); 
			GLsizei written; 
			glGetShaderInfoLog(shader_object, logLen, &written, log); 
			printf("%s", log); 
			free(log); 
		}
	} else {
		printf("%s successfully compiled.\n", shader_name);
	}
}

/* Loads and compiles shaders into a program object which is then returned.
 * 
 * Note that the naming convention requires that your shaders use file names
 * <shader_name>.vert and <shader_name>.frag
 *
 * Inspiration from:
 *   - http://www.cse.ohio-state.edu/~hwshen/5542/Site/Slides_files/shaderSetup.C
 *   - http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/
 * -------------------------------------------------------------------------- */
GLuint setupShaders(char *shader_name) {

	GLuint vertex_shader_object = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_object = glCreateShader(GL_FRAGMENT_SHADER);

	// Load the vertex shader.
	char vert_shader_name[100];
    strcpy(vert_shader_name, shader_name);
	strcat(vert_shader_name, ".vert");
	char *vertext_shader = getFileContents(vert_shader_name);

	// Load the fragment shader.
	char frag_shader_name[100];
    strcpy(frag_shader_name, shader_name);
	strcat(frag_shader_name, ".frag");
	char *fragment_shader = getFileContents(frag_shader_name);

	glShaderSource(vertex_shader_object, 1, (const GLchar**)&vertext_shader, NULL);
	glShaderSource(fragment_shader_object, 1, (const GLchar**)&fragment_shader, NULL);

	free(vertext_shader); free(fragment_shader);

	glCompileShader(vertex_shader_object);
	printShaderErrors(vertex_shader_object, vert_shader_name);

	glCompileShader(fragment_shader_object);
	printShaderErrors(fragment_shader_object, frag_shader_name);

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader_object);
	glAttachShader(program, fragment_shader_object);

	glLinkProgram(program);

	return program;
}

/* Quick and easy method to initialize a phong shader. Takes care of 
 * binding the program and getting uniform locations.
 * -------------------------------------------------------------------------- */
void setupPhongShader(PhongShader* phong_shader) {
	phong_shader->program = setupShaders("phong_tex");

	phong_shader->viewId = glGetUniformLocation(phong_shader->program, "view");
    phong_shader->modelId = glGetUniformLocation(phong_shader->program, "model");
    phong_shader->projectionId = glGetUniformLocation(phong_shader->program, "projection");
    phong_shader->normalId = glGetUniformLocation(phong_shader->program, "normal");
    phong_shader->lightId = glGetUniformLocation(phong_shader->program, "light_world");
	phong_shader->lightMatId = glGetUniformLocation(phong_shader->program, "light_mat");
	phong_shader->ambientMatId = glGetUniformLocation(phong_shader->program, "ambient_mat");
	phong_shader->diffuseMatId = glGetUniformLocation(phong_shader->program, "diffuse_mat");
	phong_shader->specularMatId = glGetUniformLocation(phong_shader->program, "specular_mat");
	phong_shader->specularPowerId = glGetUniformLocation(phong_shader->program, "specular_power");
	phong_shader->use_texture_id = glGetUniformLocation(phong_shader->program, "use_texture");
	phong_shader->tex_sampler = glGetUniformLocation(phong_shader->program, "tex_sampler");
    
    // Default is 0, AKA no texture.
    glUniform1i(phong_shader->use_texture_id, 0);
}

/* Quick and easy method to initialize a shadow shader. Takes care of 
 * binding the program and getting uniform locations.
 * -------------------------------------------------------------------------- */
void setupShadowShader(ShadowShader* shadow_shader) {
	shadow_shader->program = setupShaders("shadow");

	shadow_shader->viewId = glGetUniformLocation(shadow_shader->program, "view");
    shadow_shader->modelId = glGetUniformLocation(shadow_shader->program, "model");
    shadow_shader->projectionId = glGetUniformLocation(shadow_shader->program, "projection");
    shadow_shader->normalId = glGetUniformLocation(shadow_shader->program, "normal");
    shadow_shader->planeNormalId = glGetUniformLocation(shadow_shader->program, "plane_normal");
    shadow_shader->lightId = glGetUniformLocation(shadow_shader->program, "light_world");
}

/* Quick and easy method to initialize a shadow shader. Takes care of 
 * binding the program and getting uniform locations.
 * -------------------------------------------------------------------------- */
void setupPickingShader(PickingShader* picking_shader) {
	picking_shader->program = setupShaders("picker");

	picking_shader->viewId = glGetUniformLocation(picking_shader->program, "view");
    picking_shader->modelId = glGetUniformLocation(picking_shader->program, "model");
    picking_shader->projectionId = glGetUniformLocation(picking_shader->program, "projection");
}

/* Updates simple uniforms in the phong shader.
 * -------------------------------------------------------------------------- */
void updatePhongShader(Scene *scene) {
	mat3 normal_matrix = mat3(transpose(inverse(scene->view * scene->model)));

    glUniformMatrix4fv(scene->phong_shader.modelId, 1, GL_FALSE, &scene->model[0][0]);
    glUniformMatrix4fv(scene->phong_shader.viewId, 1, GL_FALSE, &scene->view[0][0]);
    glUniformMatrix4fv(scene->phong_shader.projectionId, 1, GL_FALSE, &scene->projection[0][0]);
	glUniformMatrix3fv(scene->phong_shader.normalId, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniform3f(scene->phong_shader.lightId, scene->light_pos.x, scene->light_pos.y, scene->light_pos.z);
}

/* Updates simple uniforms in the shadow shader.
 * -------------------------------------------------------------------------- */
void updateShadowShader(Scene *scene) {
	mat3 normal_matrix = mat3(transpose(inverse(scene->view * scene->model)));

    glUniformMatrix4fv(scene->shadow_shader.modelId, 1, GL_FALSE, &scene->model[0][0]);
    glUniformMatrix4fv(scene->shadow_shader.viewId, 1, GL_FALSE, &scene->view[0][0]);
    glUniformMatrix4fv(scene->shadow_shader.projectionId, 1, GL_FALSE, &scene->projection[0][0]);
	glUniformMatrix3fv(scene->shadow_shader.normalId, 1, GL_FALSE, &normal_matrix[0][0]);
	glUniform3f(scene->shadow_shader.planeNormalId, scene->shadow_shader.plane_normal.x, scene->shadow_shader.plane_normal.y, scene->shadow_shader.plane_normal.z);
	glUniform3f(scene->shadow_shader.lightId, scene->light_pos.x, scene->light_pos.y, scene->light_pos.z);
}

/* Updates simple uniforms in the picking shader.
 * -------------------------------------------------------------------------- */
void updatePickingShader(Scene *scene) {
	mat3 normal_matrix = mat3(transpose(inverse(scene->view * scene->model)));

    glUniformMatrix4fv(scene->picking_shader.modelId, 1, GL_FALSE, &scene->model[0][0]);
    glUniformMatrix4fv(scene->picking_shader.viewId, 1, GL_FALSE, &scene->view[0][0]);
    glUniformMatrix4fv(scene->picking_shader.projectionId, 1, GL_FALSE, &scene->projection[0][0]);
}

/* Reads a texture into the scene struct.
 * -------------------------------------------------------------------------- */
void readTexture(Texture *texture, char* texture_name) {
	FILE* in = fopen(texture_name, "r"); 

	int height, width, ccv; 
	char header[100]; 
	fscanf(in, "%s %d %d %d", header, &width, &height, &ccv); 
	texture->height = height;
	texture->width = width;
	texture->image.resize(height * width);

	int r, g, b; 
	for (int i=height-1; i>=0; i--) {
		for (int j=0; j<width; j++)	{
			fscanf(in, "%d %d %d", &r, &g, &b);
			int index = i * width + j;
			texture->image[index].r = (GLubyte)r; 
			texture->image[index].g = (GLubyte)g; 
			texture->image[index].b = (GLubyte)b; 
			texture->image[index].a = 255; 
		}
	}

	fclose(in); 
}
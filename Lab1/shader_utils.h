#ifndef SHADER_UTILS_H
#define SHADER_UTILS_H

/* --------------------------------------------------------------------------
 *  Defines functions to make using shaders really easy.
 * -------------------------------------------------------------------------- */

#define PHONG_SHADER 0
#define SHADOW_SHADER 1
#define PICKER_SHADER 2
#define BUMPMAP_SHADER 3
#define STAR_SHADER 4

typedef struct {
	GLuint program;

	// Shader vars.
	GLuint lightId;
	GLuint cameraPosId;
	GLuint modelId;
	GLuint viewId;
	GLuint projectionId;
	GLuint normalId;

	GLuint ambientMatId;
	GLuint diffuseMatId;
	GLuint specularMatId;
	GLuint specularPowerId;
	GLuint lightMatId;
    GLuint textureGlowPowerId;
	
	GLuint cubemap_sampler;
	GLuint cubemap_texture;
	GLuint cubemap_textures[6];

	GLuint use_texture_id;
	GLuint light_texture_id;
	GLuint reflect_cubemap_id;
    GLuint glow_id;
	GLuint tex_sampler;
} PhongShader;

typedef struct {
	GLuint program;

	// Shader vars.
	GLuint lightId;
	GLuint cameraPosId;
	GLuint modelId;
	GLuint viewId;
	GLuint projectionId;
	GLuint normalId;

	GLuint spectral_lookup_id;
	GLuint time_id;
	GLuint shade_corona_id;
	GLuint shade_halo_id;

	float spectral_lookup;
	float time;
	GLuint shade_corona;
	GLuint shade_halo;

	GLuint tex_primary;
	GLuint tex_color;
	GLuint tex_spectral;
} StarShader;

typedef struct {
	GLuint program;

	// Shader vars.
	GLuint lightId;
	GLuint cameraPosId;
	GLuint modelId;
	GLuint viewId;
	GLuint projectionId;
	GLuint normalId;

	GLuint ambientMatId;
	GLuint diffuseMatId;
	GLuint specularMatId;
	GLuint specularPowerId;
	GLuint lightMatId;

	GLuint tex_sampler;
	GLuint tex_normals_sampler;
} BumpMapShader;

typedef struct {
	GLuint program;

	// Shader vars.
	GLuint lightId;
	GLuint modelId;
	GLuint viewId;
	GLuint projectionId;
	GLuint normalId;
	GLuint planeNormalId;

	// Plane data.
	glm::vec3 plane_normal;
} ShadowShader;

typedef struct {
	GLuint program;

	// Shader vars.
	GLuint modelId;
	GLuint viewId;
	GLuint projectionId;
	GLuint pickColorId;
} PickingShader;

typedef struct {
	GLubyte r;
	GLubyte g;
	GLubyte b;
	GLubyte a;
} Texel;

typedef struct {
	int id;
	glm::mat4 model;
	glm::mat4 trackball;
	GLuint array_buffer;
	GLuint element_array_buffer;
	float ambient_mat[3];
	float diffuse_mat[3];
	float specular_mat[3];
	float specular_pow;
	int use_texture;
	GLenum draw_elements_mode;
	GLsizei draw_elements_count;
	GLenum draw_elements_type;
} Object;

typedef struct {
	GLuint id;
	GLuint height;
	GLuint width;
	std::vector<Texel> image;
} Texture;

typedef struct {
	PhongShader phong_shader;
	StarShader star_shader;
	ShadowShader shadow_shader;
	BumpMapShader bumpmap_shader;
	PickingShader picking_shader;

	// Graphics pipeline matrices.
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 model;

	// Camera
	glm::vec3 camera_pos;

	// Lighting
	glm::vec3 light_pos;
	
	// Textures
	Texture bumpmap_texture;
	Texture bumpmap_gradients_texture;
	Texture cubemap_positive_x;
	Texture cubemap_negative_x;
	Texture cubemap_positive_y;
	Texture cubemap_negative_y;
	Texture cubemap_position_z;
	Texture cubemap_negative_z;

	// Star Texture
	Texture sun_surface;
	Texture sun_halo;
	Texture star_colorshift;
	Texture halo_colorshift;
	Texture star_colorgraph;
	Texture corona;

	// Mapping of object model transformations to id (i.e. index in vector).
	// The object's index maps to a color such that it can be "picked" by the
	// mouse, and the associated model matrix is then used to reposition the
	// trackball.
	std::vector<Object> objects;
} Scene;

/* Prints useful information about the client's graphics card.
 * -------------------------------------------------------------------------- */
void checkGraphics();

/* Loads and compiles shaders into a program object which is then returned.
 * 
 * Note that the naming convention requires that your shaders use file names
 * <shader_name>.vert and <shader_name>.frag
 * -------------------------------------------------------------------------- */
GLuint setupShaders(char *shader_name);

/* Quick and easy method to initialize a phong shader. Takes care of 
 * binding the program and getting uniform locations.
 * -------------------------------------------------------------------------- */
void setupPhongShader(PhongShader* phong_shader);

/* Quick and easy method to initialize a star shader. Takes care of 
 * binding the program and getting uniform locations.
 * -------------------------------------------------------------------------- */
void setupStarShader(StarShader* star_shader);

/* Quick and easy method to initialize a shadow shader. Takes care of 
 * binding the program and getting uniform locations.
 * -------------------------------------------------------------------------- */
void setupShadowShader(ShadowShader* shadow_shader);

/* Quick and easy method to initialize a bump mapping shader. Takes care of 
 * binding the program and getting uniform locations.
 * -------------------------------------------------------------------------- */
void setupBumpMapShader(BumpMapShader* bumpmap_shader);

/* Quick and easy method to initialize a picking shader. Takes care of 
 * binding the program and getting uniform locations.
 * -------------------------------------------------------------------------- */
void setupPickingShader(PickingShader* picking_shader);

/* Updates simple uniforms in the phong shader.
 * -------------------------------------------------------------------------- */
void updatePhongShader(Scene *scene);

/* Updates simple uniforms in the star shader.
 * -------------------------------------------------------------------------- */
void updateStarShader(Scene *scene);

/* Updates simple uniforms in the shadow shader.
 * -------------------------------------------------------------------------- */
void updateShadowShader(Scene *scene);

/* Updates simple uniforms in the bump mapping shader.
 * -------------------------------------------------------------------------- */
void updateBumpMapShader(Scene *scene);

/* Updates simple uniforms in the picking shader.
 * -------------------------------------------------------------------------- */
void updatePickingShader(Scene *scene);

/* Reads a texture into the scene struct.
 * -------------------------------------------------------------------------- */
void readTexture(Texture *texture, char *texture_name);

/* Creates a gradient texture (2nd parameter) for a given input texture.
 * -------------------------------------------------------------------------- */
void calculateGradientsTexture(Texture *texture, Texture *gradient, int normal);

/* Loads a texture.
 * -------------------------------------------------------------------------- */
void loadTexture(Texture *texture);

#endif

// Phong lighting vertex shader. Created using inspiration and code from:
//   * http://www.cse.ohio-state.edu/~hwshen/5542/Site/Slides_files/shading_glsl.pdf
//   * http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/
//
// Note: *_model refers to model space, *_eye refers to camera/eye space, etc.

attribute vec3 vertex_model;
attribute vec3 normal_model;
attribute vec2 tex_coord;

varying vec3 normal_eye;
varying vec3 eyedir_eye;
varying vec3 lightdir_eye;
varying vec2 frag_tex_coord;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform mat3 normal;
uniform vec3 light_world;
uniform int use_texture;

void main(){
	// Vertex position in clip space.
	gl_Position =  projection * view * model * vec4(vertex_model, 1);
    
	// Eye space vector from vertex to camera.
	vec3 vertex_eye = (view * model * vec4(vertex_model, 1)).xyz;
	eyedir_eye = vec3(0,0,0) - vertex_eye;
    
	// Eye space vector from vertex to light.
    vec4 vertex_world = model * vec4(vertex_model, 1.0);
    vec3 point_light_world = light_world - vertex_world.xyz;
	vec3 light_eye = (view * vec4(point_light_world, 1)).xyz;
	lightdir_eye = light_eye + eyedir_eye;
	
	// Eye space normal vector of the vertex.
    // Note: no adjustments made for non-uniform scaling.
	normal_eye = (normal * vec3(normal_model)).xyz;
    
    // Texturing
    if (use_texture == 1) {
        frag_tex_coord = tex_coord;
    }
}
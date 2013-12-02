
// Star shader.

attribute vec3 vertex_model;
attribute vec3 normal_model;
attribute vec2 tex_coord;

varying vec3 normal_eye;
varying vec2 frag_tex_coord;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform mat3 normal;

void main(){
	// Vertex position in clip space.
	gl_Position =  projection * view * model * vec4(vertex_model, 1);
	
	// Eye space normal vector of the vertex.
	normal_eye = normalize(normal * vec3(normal_model)).xyz;
    
    // Texturing
    frag_tex_coord = tex_coord;
}
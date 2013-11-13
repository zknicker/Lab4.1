
// Simply draws the object to its clip-space position.

attribute vec3 vertex_model;
attribute vec3 normal_model;
attribute vec2 tex_coord;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void main(){
	gl_Position =  projection * view * model * vec4(vertex_model, 1);
}
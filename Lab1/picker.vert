
// Simply draws the object to its clip-space position.

layout(location = 0) in vec3 vertex_model;
layout(location = 1) in vec3 normal_model;
layout(location = 2) in vec2 tex_coord;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

void main(){
	gl_Position =  projection * view * model * vec4(vertex_model, 1);
}
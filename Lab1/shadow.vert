
// Planar shadow vertex shader.

attribute vec3 vertex_model;
attribute vec3 normal_model;
attribute vec2 tex_coord;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform mat3 normal;
uniform vec3 light_world;
uniform vec3 plane_normal;

void main(){
    vec4 mvm = model * vec4(vertex_model, 1);
    vec3 point_light_world = normalize(mvm.xyz - light_world);
    vec3 p = mvm.xyz - (((dot(plane_normal, mvm.xyz)) - 0.0) / (dot(plane_normal, point_light_world)) * point_light_world);
    
	// Vertex position in clip space.
	gl_Position =  projection * view * vec4(p, 1.0);
}
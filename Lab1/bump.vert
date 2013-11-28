
// Bump Mapping Shader

attribute vec3 vertex_model;
attribute vec3 normal_model;
attribute vec2 tex_coord;
attribute vec4 tangent;

varying vec3 normal_eye;
varying vec3 eyedir_eye;
varying vec3 eyedir_tangent;
varying vec3 lightdir_eye;
varying vec3 lightdir_tangent;
varying vec2 frag_tex_coord;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform mat3 normal;
uniform vec3 light_world;
uniform vec3 camera_world;

void main(){
	// Vertex position in clip space.
	gl_Position =  projection * view * model * vec4(vertex_model, 1);
    
	// Eye space vector from vertex to camera (phong lighting).
	vec3 vertex_eye = (view * model * vec4(vertex_model, 1)).xyz;
	eyedir_eye = vec3(0,0,0) - vertex_eye;
    
	// Eye space vector from vertex to light (phong lighting).
    vec4 vertex_world = model * vec4(vertex_model, 1.0);
    vec3 point_light_world = light_world - vertex_world.xyz;
	vec3 light_eye = (view * vec4(point_light_world, 1)).xyz;
	lightdir_eye = light_eye + eyedir_eye;
	
	// Eye space normal vector of the vertex (phong lighting, bump mapping).
    // Note: no adjustments made for non-uniform scaling.
	normal_eye = (normal * vec3(normal_model)).xyz;
    
    // Tangent calculation (bump mapping).
    vec3 bm_tangent = normalize(normal * tangent.xyz);
    
    // Binormal calculation (bump mapping).
    vec3 bm_binormal = normalize(cross(normal_eye, bm_tangent));
    
    // Tangent space transformation matrix (bump mapping).
    mat3 to_object_local = mat3(
        bm_tangent.x, bm_binormal.x, normal_eye.x,
        bm_tangent.y, bm_binormal.y, normal_eye.y,
        bm_tangent.z, bm_binormal.z, normal_eye.z);
    
    // Bump mapping final calculations.
    lightdir_tangent = normalize(to_object_local * point_light_world);
    eyedir_tangent = normalize(to_object_local * eyedir_eye);
    
    // Texturing
    frag_tex_coord = tex_coord;
}
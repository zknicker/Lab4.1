
// Bump Mapping Shader

varying vec3 normal_eye;
varying vec3 eyedir_eye;
varying vec3 eyedir_tangent;
varying vec3 lightdir_eye;
varying vec3 lightdir_tangent;
varying vec2 frag_tex_coord;

uniform mat4 view;
uniform vec3 light_mat;
uniform vec3 ambient_mat;
uniform vec3 diffuse_mat;
uniform vec3 specular_mat;
uniform float specular_power;

uniform sampler2D tex_sampler;
uniform sampler2D tex_normals_sampler;

void main(){
    
    vec3 normal_from_map = normalize(texture2D(tex_normals_sampler, frag_tex_coord).rgb * 2.0 - 1.0);
    vec3 light = normalize(lightdir_tangent);
    float lambert_factor = max(dot(normal_from_map, light), 0.0);
    
    // Diffuse angle calculation.
	vec3 n = normalize(normal_from_map); // normal
	vec3 l = normalize(lightdir_tangent); // to light
    float diffuse_angle = clamp(dot(n, l), 0.0, 1.0);
	
    // Specular angle calculation
	vec3 e = normalize(eyedir_eye); // to eye
	vec3 r = reflect(-normalize(lightdir_eye), normalize(normal_eye)); // light reflection
    float specular_angle = clamp(dot(e, r), 0.0, 1.0);
	
    vec3 ambient = ambient_mat;
    vec3 diffuse = diffuse_mat * light_mat * diffuse_angle;
    vec3 specular = specular_mat * light_mat * pow(specular_angle, specular_power);
    
    vec4 tex_color = texture2D(tex_sampler, frag_tex_coord);
	gl_FragColor = vec4(ambient + light_mat * diffuse_angle * 7.0 * tex_color.xyz + specular, 1.0);
}
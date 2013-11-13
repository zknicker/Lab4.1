
// Phong lighting vertex shader. Created using inspiration and code from:
//   * http://www.cse.ohio-state.edu/~hwshen/5542/Site/Slides_files/shading_glsl.pdf
//   * http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/

varying vec3 normal_eye;
varying vec3 eyedir_eye;
varying vec3 lightdir_eye;
varying vec2 frag_tex_coord;

uniform vec3 light_mat;
uniform vec3 ambient_mat;
uniform vec3 diffuse_mat;
uniform vec3 specular_mat;
uniform float specular_power;
uniform sampler2D tex_one_sampler;
uniform int use_texture;

void main(){
    
    // Diffuse angle calculation.
	vec3 n = normalize(normal_eye); // normal
	vec3 l = normalize(lightdir_eye); // to light
    float diffuse_angle = clamp(dot(n, l), 0.0, 1.0);
	
    // Specular angle calculation
	vec3 e = normalize(eyedir_eye); // to eye
	vec3 r = reflect(-l, n); // light reflection
    float specular_angle = clamp(dot(e, r), 0.0, 1.0);
	
    vec3 ambient = ambient_mat;
    vec3 diffuse = diffuse_mat * light_mat * diffuse_angle;
    vec3 specular = specular_mat * light_mat * pow(specular_angle, specular_power);
    
	if (use_texture == 1) {
        vec4 tex_color = texture2D(tex_one_sampler, frag_tex_coord);
        gl_FragColor = vec4(ambient + (diffuse + tex_color.xyz) + specular, 1.0);
    } else {
        gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
    }

}
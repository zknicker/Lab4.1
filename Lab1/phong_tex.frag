
// Phong lighting vertex shader. Created using inspiration and code from:
//   * http://www.cse.ohio-state.edu/~hwshen/5542/Site/Slides_files/shading_glsl.pdf
//   * http://www.opengl-tutorial.org/beginners-tutorials/tutorial-8-basic-shading/

varying vec3 normal_eye;
varying vec3 eyedir_eye;
varying vec3 lightdir_eye;
varying vec2 frag_tex_coord;
varying vec3 reflection;

uniform mat4 view;
uniform vec3 light_mat;
uniform vec3 ambient_mat;
uniform vec3 diffuse_mat;
uniform vec3 specular_mat;
uniform float specular_power;
uniform float glow_power;
uniform int use_texture;
uniform int light_texture;
uniform int reflect_cubemap;
uniform int use_texture_glow;

uniform sampler2D tex_sampler;
uniform samplerCube cube_map_sampler;


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
    
    // Glow (attribution: http://myheroics.wordpress.com/2008/09/04/glsl-bloom-shader/)
    if (use_texture == 1 && use_texture_glow == 1) {
        vec4 sum = vec4(0);
        vec4 glow_color = vec4(0);
        vec4 tex_val;
        int j;
        int i;
        int k;
    
        if (use_texture == 1 && light_texture == 1 && reflect_cubemap == 1) {
            for(i= -4 ;i < 4; i++) {
                for (j = -3; j < 3; j++) {
                    for (k = -3; k < 3; k++) {
                        sum += textureCube(cube_map_sampler, reflection.xyz + vec3(k, j, i)*0.004) * 0.35;
                    }
                }
            }
            tex_val = textureCube(cube_map_sampler, reflection.xyz);
        } else {
            for(i= -4 ;i < 4; i++) {
                for (j = -3; j < 3; j++) {
                    sum += texture2D(tex_sampler, frag_tex_coord + vec2(j, i)*0.002) * 0.55;
                }
            }
            tex_val = texture2D(tex_sampler, frag_tex_coord);
        }
    
        if (tex_val.x < 0.3) {
            glow_color = sum*sum*0.012;
        } else {
            if (tex_val.x < 0.5) {
                glow_color = sum*sum*0.009;
            } else {
                glow_color = sum*sum*0.0075;
            }
        }
        gl_FragColor = vec4(glow_power * glow_color.xyz + tex_val.xyz * (1.0 - glow_power) + specular, 1.0);
    
    // Standard Texturing
    } else {
        if (use_texture == 1) {
            vec4 tex_color = vec4(1.0, 0.0, 0.0, 1.0);
            if (reflect_cubemap == 1) {
                tex_color = textureCube(cube_map_sampler, reflection.xyz);
                tex_color += tex_color;
            } else {
                tex_color = texture2D(tex_sampler, frag_tex_coord);
            }
            if (light_texture == 1) {
                gl_FragColor = vec4(ambient + tex_color.xyz * diffuse + tex_color.xyz + specular, 1.0);
            } else {
                gl_FragColor = tex_color;
            }
        } else {
            gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
        }
    }
}
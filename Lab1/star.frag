
// Star Shader
// Attribution: http://workshop.chromeexperiments.com/stars/

varying vec3 normal_eye;
varying vec2 frag_tex_coord;

uniform sampler2D tex_primary;
uniform sampler2D tex_color;
uniform sampler2D tex_spectral;
uniform float time;
uniform float spectral_lookup;
uniform int shade_corona;
uniform int shade_halo;

void main(){

    // Corona
    if (shade_corona == 1) {
        vec2 uv = frag_tex_coord;
	
        vec4 foundColor = texture2D( tex_primary, uv );
        foundColor.x *= 1.4;
        foundColor.y *= 1.2;
        foundColor.z *= 0.7;
        //foundColor.xyz *= 10.0;
        foundColor = clamp( foundColor, 0., 1. );	

        float spectralLookupClamped = clamp( spectral_lookup, 0., 1. );
        vec2 spectralLookupUV = vec2( 0., spectralLookupClamped );
        vec4 spectralColor = texture2D( tex_spectral, spectralLookupUV );	

        spectralColor.x = pow( spectralColor.x, 2. );
        spectralColor.y = pow( spectralColor.y, 2. );
        spectralColor.z = pow( spectralColor.z, 2. );

        spectralColor.xyz += 0.2;

        vec3 finalColor = clamp( foundColor.xyz * spectralColor.xyz * 1.4 , 0., 1.);

        gl_FragColor = vec4( finalColor, 1.0 );
    
    // Halo
    } else if (shade_halo == 1) {
        vec3 colorIndex = texture2D( tex_primary, frag_tex_coord ).xyz;
        float lookupColor = colorIndex.x;
        lookupColor = fract( lookupColor + time * 0.04 );
        lookupColor = clamp(lookupColor,0.2,0.98);
        vec2 lookupUV = vec2( lookupColor, 0. );
        vec3 foundColor = texture2D( tex_color, lookupUV ).xyz;

        foundColor.xyz += 0.4;
        foundColor *= 10.0;

        float spectralLookupClamped = clamp( spectral_lookup, 0., 1. );
        vec2 spectralLookupUV = vec2( 0., spectralLookupClamped );
        vec4 spectralColor = texture2D( tex_spectral, spectralLookupUV );	

        spectralColor.x = pow( spectralColor.x, 3. );
        spectralColor.y = pow( spectralColor.y, 3. );
        spectralColor.z = pow( spectralColor.z, 3. );

        gl_FragColor = vec4( foundColor * colorIndex * spectralColor.xyz , 1.0 );
    // Sun
    } else {
        float uvMag = 2.0;
        float paletteSpeed = 0.2;
        float minLookup = 0.2;
        float maxLookup = 0.98;

        //	let's double up on the texture to make the sun look more detailed
        vec2 uv = frag_tex_coord * uvMag;

        //	do a lookup for the texture now, but hold on to its gray value
        vec3 colorIndex = texture2D( tex_primary, uv ).xyz;
        float lookupColor = colorIndex.x;

        //	now cycle the value, and clamp it, we're going to use this for a second lookup
        lookupColor = fract( lookupColor - time * paletteSpeed );
        lookupColor = clamp(lookupColor, minLookup, maxLookup );

        //	use the value found and find what color to use in a palette texture
        vec2 lookupUV = vec2( lookupColor, 0. );
        vec3 foundColor = texture2D( tex_color, lookupUV ).xyz;

        //	now do some color grading
        foundColor.xyz *= 0.6;
        foundColor.x = pow(foundColor.x, 2.);
        foundColor.y = pow(foundColor.y, 2.);
        foundColor.z = pow(foundColor.z, 2.);

        foundColor.xyz += vec3( 0.6, 0.6, 0.6 ) * 1.4;
        //foundColor.xyz += vec3(0.6,0.35,0.21) * 2.2;

        float spectralLookupClamped = clamp( spectral_lookup, 0., 1. );
        vec2 spectralLookupUV = vec2( 0., spectralLookupClamped );
        vec4 spectralColor = texture2D( tex_spectral, spectralLookupUV );	

        spectralColor.x = pow( spectralColor.x, 2. );
        spectralColor.y = pow( spectralColor.y, 2. );
        spectralColor.z = pow( spectralColor.z, 2. );

        foundColor.xyz *= spectralColor.xyz;	
        

        //	apply a secondary, subtractive pass to give it more detail
        //	first we get the uv and apply some warping
        vec2 uv2 = vec2( frag_tex_coord.x + cos(time) * 0.001, frag_tex_coord.y + sin(time) * 0.001 );
        vec3 secondaryColor = texture2D( tex_primary, uv2 ).xyz;

        //	finally give it an outer rim to blow out the edges
        float intensity = 1.15 - dot( normal_eye, vec3( 0.0, 0.0, 0.3 ) );
        vec3 outerGlow = vec3( 1.0, 0.8, 0.6 ) * pow( intensity, 6.0 );

        vec3 desiredColor = foundColor + outerGlow - secondaryColor;
        float darkness = 1.0 - clamp( length( desiredColor ), 0., 1. );
        vec3 colorCorrection = vec3(0.7, 0.4, 0.01) * pow(darkness,2.0) * secondaryColor;
        desiredColor += colorCorrection;

        //	the final composite color
        gl_FragColor = vec4( desiredColor, 1.0 );
    }
}
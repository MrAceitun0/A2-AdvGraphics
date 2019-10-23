#ifdef GL_ES
precision mediump float;
#endif

varying vec3 v_wPos;
varying vec3 v_wNormal;
varying vec3 v_pos;

uniform vec3 u_camera_position;

uniform vec3 ambient_light;
uniform vec3 light_color;
uniform vec3 light_pos;

uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_gloss;


uniform vec2 u_resolution = vec2(100.0, 100.0);
uniform vec2 u_mouse;
uniform float u_time;

float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

// Based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

#define OCTAVES 6
float fbm (in vec2 st) {
    // Initial values
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    //
    // Loop of octaves
    for (int i = 0; i < OCTAVES; i++) {
        value += amplitude * noise(st);
        st *= 2.;
        amplitude *= .5;
    }
    return value;
}

void main() {
    vec2 st = gl_FragCoord.xy/u_resolution.xy;
    st.x *= u_resolution.x/u_resolution.y;

    vec3 color = vec3(0.0);
    color += fbm(st*3.0  + 0.1 * u_time);


    //here we set up the normal as a color to see them as a debug
	vec3 N = normalize(v_wNormal);
	//vec3 uv = v_coord;
	vec3 L = normalize( light_pos - v_wPos );
	vec3 E = normalize( u_camera_position - v_wPos );
	vec3 R = normalize( reflect(E,N) );
	float NdotL = max(0.0, dot(N,L));
	float RdotL = pow( max(0.0, dot(-R,L)), material_gloss );


    gl_FragColor = vec4( color * ambient_light * material_ambient + material_diffuse * light_color * NdotL + material_specular * light_color * RdotL, 1.0);
}
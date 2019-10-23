
//uniform vec4 u_color;


//this var comes from the vertex shader
//they are baricentric interpolated by pixel according to the distance to every vertex
varying vec3 v_wPos;
varying vec3 v_wNormal;
varying vec3 v_pos;
varying vec3 v_coord;

//here create uniforms for all the data we need here
uniform vec3 u_camera_position;

uniform vec3 ambient_light;
uniform vec3 light_color;
uniform vec3 light_pos;

uniform vec3 material_ambient;
uniform vec3 material_diffuse;
uniform vec3 material_specular;
uniform float material_gloss;

uniform sampler2D u_texture_beauty;

void main()
{
	//here we set up the normal as a color to see them as a debug
	vec3 N = normalize(v_wNormal);
	vec3 uv = v_coord;
	vec3 L = normalize( light_pos - v_wPos );
	vec3 E = normalize( u_camera_position - v_wPos );
	vec3 R = normalize( reflect(E,N) );
	float NdotL = max(0.0, dot(N,L));
	float RdotL = pow( max(0.0, dot(-R,L)), material_gloss );



	gl_FragColor = texture2D( u_texture_beauty, uv ) * vec4(ambient_light * material_ambient + material_diffuse * light_color * NdotL + material_specular * light_color * RdotL, 1.0);;
}
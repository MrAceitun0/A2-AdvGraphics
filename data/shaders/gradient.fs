varying vec3 v_position;
varying vec3 v_world_position;

//Texture space is [0, 1]
uniform sampler2D u_texture;

//Camera position
uniform vec3 u_camera_position;
uniform vec3 u_local_camera_position;   //You can use this now for the algorithm, in the assigment you will be responsible to compute it

//Optional to use
uniform float u_quality;
uniform float u_brightness;
uniform vec4 u_color;

void main()
{
    vec3 current_sample = v_position;   //initial position

    //Ray
    vec3 step_vector = normalize(v_position - u_local_camera_position) * u_quality; //normalize the ray vector

    //color accumulator
    vec4 color_acc = vec4(0.0, 0.0, 0.0, 0.0);

    //start loop
    for (int i = 0; i < 4096; i++)
    {
        current_sample += step_vector;     //for every iteration sum the step vector to the sample
        vec3 current_sample_norm = (current_sample + 1) / 2.0;  //norm of the current sample

        float d1 = texture2D(u_texture, vec3(current_sample_norm.x + u_quality, current_sample_norm.y, current_sample_norm.z)) 
        		- texture2D(u_texture, vec3(current_sample_norm.x - u_quality, current_sample_norm.y, current_sample_norm.z));

		float d2 = texture2D(u_texture, vec3(current_sample_norm.x, current_sample_norm.y + u_quality, current_sample_norm.z)) 
				- texture2D(u_texture, vec3(current_sample_norm.x, current_sample_norm.y - u_quality, current_sample_norm.z));
        
        float d3 = texture2D(u_texture, vec3(current_sample_norm.x, current_sample_norm.y, current_sample_norm.z + u_quality)) 
				- texture2D(u_texture, vec3(current_sample_norm.x, current_sample_norm.y, current_sample_norm.z - u_quality));

        vec3 gradient = (1.0 / (2.0 * u_quality)) * vec3(d1, d2, d3);
        vec4 gradient4 = vec4(gradient.x, gradient.y, gradient.z, 1.0);

        //operations for the color
        vec4 color_i = gradient4;   //color sample
        color_i = vec4(u_color.xyz, color_i.x);
        color_i.rgb = color_i.rgb * color_i.a;
        color_acc = length(step_vector) * color_i * (1.0 - color_acc.a) + color_acc;

        //break the loop if the color alpha value is huge
        if(color_acc.a > 0.99) 
            break;

        //break the loop if the ray is not inside the volume
        if (current_sample.x > 1 || current_sample.y > 1 || current_sample.z > 1 || current_sample.x < -1 || current_sample.y < -1 || current_sample.z < -1 ) 
            break;
    }

	//Brightness Options
	if(1.0 < u_color.x * u_brightness)
		u_color.x = 1.0;
	if(1.0 < color_acc.x * u_brightness)
		color_acc.x = 1.0;
	else
		u_color.x = u_color.x * u_brightness;
		color_acc.x = color_acc.x * u_brightness;

	if(1.0< u_color.y * u_brightness)
		u_color.y = 1.0;
	if(1.0< color_acc.y * u_brightness)
		color_acc.y = 1.0;
	else
		u_color.y = u_color.y * u_brightness;
		color_acc.y = color_acc.y * u_brightness;

	if(1.0 < u_color.z * u_brightness)
		u_color.z = 1.0;
	if(1.0 < color_acc.z * u_brightness)
		color_acc.z = 1.0;
	else
		u_color.z = u_color.z * u_brightness;
		color_acc.z = color_acc.z * u_brightness;

    //For Volume Rendering
    gl_FragColor = color_acc;
}
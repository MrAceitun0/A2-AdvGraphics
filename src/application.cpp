#include "application.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "volume.h"
#include "fbo.h"
#include "shader.h"
#include "input.h"
#include "animation.h"
#include "extra/hdre.h"
#include "includes.h"

#include <cmath>

Application* Application::instance = NULL;
Camera* Application::camera = nullptr;

Application::Application(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;
	render_debug = true;
	render_wireframe = false;
	render_jittering = false;
	render_gradient = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	// OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	// Create camera
	camera = new Camera();
	camera->lookAt(Vector3(100.f, 100.0f, 200.f), Vector3(0.f, 20.0f, 0.f), Vector3(0.f, 1.f, 0.f));
	camera->setPerspective(45.f, window_width / (float)window_height, 0.1f, 10000.f);

	// Create 3 scene nodes
	SceneNode * abdomen = new SceneNode("Rendered Abdomen");
	SceneNode * orange = new SceneNode("Rendered Orange");
	SceneNode * smoke = new SceneNode("Rendered Smoke");

	// Create meshes from cubes for the 3 nodes
	abdomen->mesh = new Mesh();
	abdomen->mesh->createCube();
	orange->mesh = new Mesh();
	orange->mesh->createCube();
	smoke->mesh = new Mesh();
	smoke->mesh->createCube();

	//This change is just to make an easier visualization of the nodes from the camera position
	abdomen->model.setScale(32, 32, 70);
	orange->model.setScale(32, 32, 32);
	smoke->model.setScale(32, 32, 32);

	// Create node material and manipulate it with different parameters for color and brightness
	VolumeMaterial * abdomen_material = new VolumeMaterial();
	abdomen_material->color = vec4(1.0, 1.0, 1.0, 1.0);
	abdomen_material->brightness = 0.5;
	abdomen->material = abdomen_material;

	VolumeMaterial * orange_material = new VolumeMaterial();
	orange_material->color = vec4(1.0, 0.0, 0.0, 1.0);
	orange_material->brightness = 1.5;
	orange->material = orange_material;

	VolumeMaterial * smoke_material = new VolumeMaterial();
	smoke_material->color = vec4(1.0, 0.0, 1.0, 1.0);
	smoke_material->brightness = 1;
	smoke->material = smoke_material;

	//Create volumes for each node
	Volume* v_abdomen = new Volume(32, 32, 32);
	v_abdomen->loadPVM("data/volumes/abdomen.pvm");
	Volume* v_orange = new Volume(32, 32, 32);
	v_orange->loadPVM("data/volumes/orange.pvm");
	Volume* v_smoke = new Volume(32,32,32);
	v_smoke->fillNoise(2, 4, 1);

	//Create textures from each previously created volume
	Texture* t_abdomen = new Texture();
	t_abdomen->create3D(v_abdomen->width, v_abdomen->height, v_abdomen->depth, GL_RED, GL_UNSIGNED_BYTE, false, v_abdomen->data, GL_RED);
	Texture* t_orange = new Texture();
	t_orange->create3D(v_orange->width, v_orange->height, v_orange->depth, GL_RED, GL_UNSIGNED_BYTE, false, v_orange->data, GL_RED);
	Texture* t_smoke = new Texture();
	t_smoke->create3D(v_smoke->width, v_smoke->height, v_smoke->depth, GL_RED, GL_UNSIGNED_BYTE, false, v_smoke->data, GL_RED);

	//Assign textures to nodes
	abdomen->material->texture = t_abdomen;
	orange->material->texture = t_orange;
	smoke->material->texture = t_smoke;

	//Add nodes to a list, to be iterated later in order to render each node
	root.push_back(abdomen);
	root.push_back(orange);
	root.push_back(smoke);


	//Full scene with map, clouds and light
	//Map
	SceneNode * map = new SceneNode("Rendered Menorca");
	root.push_back(map);
	Mesh * plane = new Mesh();
	plane->createSubdividedPlane(100.0, 512, true);
	map->mesh = plane;
	map->model.setScale(1, 1, 1);
	HeightMapMaterial * map_material = new HeightMapMaterial();
	//map_material->color = vec4(1.0, 0.0, 0.0, 1.0);
	map_material->texture = Texture::Get("data/textures/Menorca_gray.tga");
	map_material->beauty = Texture::Get("data/textures/Menorca_color.tga");
	map->material = map_material;

	//Clouds
	SceneNode * cloud = new SceneNode("Rendered Cloud");
	root.push_back(cloud);
	Mesh * mesh_cloud = new Mesh();
	mesh_cloud = Mesh::Get("data/meshes/cloud.obj");
	cloud->mesh = mesh_cloud;
	cloud->model.setScale2(0.005, 0.005, 0.005);
	cloud->model.setTranslation2(60.0, 8.5, 33.0);
	CloudMaterial * material_cloud = new CloudMaterial();
	cloud->material = material_cloud;
	cloud->material->time = 0.0f;
	material_cloud->color = vec4(1.0, 1.0, 1.0, 1.0);

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

//what to do when the image has to be draw
void Application::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.725, 0.886, 0.961, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	camera->enable();

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	//Iterate over all nodes
	for (int i = 0; i < size(root); i++) {
		root[4]->material->time = time;

		if (i == volume_index - 1) //When node index == keyboard number (between 1, 2, 3 and 4) render the node
		{
			root[i]->material->jittering = render_jittering;
			root[i]->material->gradient = render_gradient;

			root[i]->render(camera);

			if (render_wireframe)
				root[i]->renderWireframe(camera);

			//Render full scene
			if (i == 3)
				root[i + 1]->render(camera);
			

			if (render_wireframe && i == 3)
				root[i + 1]->renderWireframe(camera);
			
		}
	}

	//Draw the floor grid
	if(render_debug)
		drawGrid();
}

void Application::update(double seconds_elapsed)
{
	float speed = seconds_elapsed * 10; //the speed is defined by the seconds_elapsed so it goes constant
	float orbit_speed = seconds_elapsed * 0.5;
	
	//example
	float angle = (float)seconds_elapsed * 10.0f*DEG2RAD;
	for (int i = 0; i < root.size(); i++) {
		//root[i]->model.rotate(angle, Vector3(0,1,0));
	}

	//Change which node has to be rendered
	if (Input::isKeyPressed(SDL_SCANCODE_1))		volume_index = 1;
	else if (Input::isKeyPressed(SDL_SCANCODE_2))	volume_index = 2;
	else if (Input::isKeyPressed(SDL_SCANCODE_3))	volume_index = 3;
	else if (Input::isKeyPressed(SDL_SCANCODE_4))	volume_index = 4;

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT && !ImGui::IsAnyWindowHovered() 
		&& !ImGui::IsAnyItemHovered() && !ImGui::IsAnyItemActive())) //is left button pressed?
	{
		camera->orbit(-Input::mouse_delta.x * orbit_speed, Input::mouse_delta.y * orbit_speed);
	}

	//async input to move the camera around
	if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f,-1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_SPACE)) camera->moveGlobal(Vector3(0.0f, -1.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_LCTRL)) camera->moveGlobal(Vector3(0.0f,  1.0f, 0.0f) * speed);
	

	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();
}

//Keyboard event handler (sync input)
void Application::onKeyDown( SDL_KeyboardEvent event )
{
	switch(event.keysym.sym)
	{
		case SDLK_ESCAPE: must_exit = true; break; //ESC key, kill the app
		case SDLK_F1: render_debug = !render_debug; break;
		case SDLK_F5: Shader::ReloadAll(); break; 
	}
}

void Application::onKeyUp(SDL_KeyboardEvent event)
{
}

void Application::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Application::onGamepadButtonUp(SDL_JoyButtonEvent event)
{

}

void Application::onMouseButtonDown( SDL_MouseButtonEvent event )
{
	if (event.button == SDL_BUTTON_MIDDLE) //middle mouse
	{
		mouse_locked = !mouse_locked;
		SDL_ShowCursor(!mouse_locked);
	}
}

void Application::onMouseButtonUp(SDL_MouseButtonEvent event)
{
}

void Application::onMouseWheel(SDL_MouseWheelEvent event)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (event.type)
	{
		case SDL_MOUSEWHEEL:
		{
			if (event.x > 0) io.MouseWheelH += 1;
			if (event.x < 0) io.MouseWheelH -= 1;
			if (event.y > 0) io.MouseWheel += 1;
			if (event.y < 0) io.MouseWheel -= 1;
		}
	}

	if(!ImGui::IsAnyWindowHovered() && event.y)
		camera->changeDistance(event.y * 0.5);
}

void Application::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}


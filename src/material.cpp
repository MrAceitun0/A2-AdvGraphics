#include "material.h"
#include "texture.h"
#include "application.h"
#include "extra/hdre.h"

StandardMaterial::StandardMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
}

StandardMaterial::~StandardMaterial()
{

}

void StandardMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);

	shader->setUniform("u_color", color);

	if (texture)
		shader->setUniform("u_texture", texture);
}

void StandardMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void StandardMaterial::renderInMenu()
{
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
}

WireframeMaterial::WireframeMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
}

WireframeMaterial::~WireframeMaterial()
{

}

void WireframeMaterial::render(Mesh* mesh, Matrix44 model, Camera * camera)
{
	if (shader && mesh)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		//enable shader
		shader->enable();

		//upload material specific uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}


VolumeMaterial::VolumeMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/volume.fs");	//Load the volume shader
}

VolumeMaterial::~VolumeMaterial()
{

}

void VolumeMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);

	//Get the local camera position by multiplying the camera position by the inverse of the model and getting the homogeneous values
	model.inverse();
	Vector3 local_camera_position = vec3((model * vec4(camera->eye, 1.0)).x, (model * vec4(camera->eye, 1.0)).y, (model * vec4(camera->eye, 1.0)).z) * (1 / (model * vec4(camera->eye, 1.0)).w);

	//passing the local camerea position and the color to the shader
	shader->setUniform("u_local_camera_position", local_camera_position);
	shader->setUniform("u_color", color);

	//Extra uniforms
	shader->setUniform("u_quality", quality);
	shader->setUniform("u_brightness", brightness);

	if (texture)
	{
		shader->setUniform("u_texture", texture);	//texture
	}

	shader->setUniform("u_jittering", jittering);
	shader->setUniform("u_gradient", gradient);
}

void VolumeMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void VolumeMaterial::renderInMenu()
{
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
	ImGui::SliderFloat("Brightness", (float*)&brightness, 0.0, 2.0);	//Edit the brightness
	ImGui::SliderFloat("Step size", (float*)&quality, 0.001, 1.0);	//Edit the step size
}

CloudMaterial::CloudMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/phong.vs", "data/shaders/cloud.fs");

	//Light (Using a Phong shader)
	phong_light = new My_Light();

	ambient = { 1.0, 1.0, 1.0 };
	diffuse = { 1.0, 1.0, 1.0 };
	specular = { 1.0, 1.0, 1.0 };
	shininess = 50.0;

	phong_light->position = { 0.0, -1000.0, 35.0 };
	phong_light->specular_color = { 1.0, 0.0, 0.0 };
}

CloudMaterial::~CloudMaterial()
{

}

void CloudMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);

	shader->setUniform("u_color", color);

	if (texture)
	{
		shader->setUniform("u_texture", texture);	//texture
	}

	shader->setUniform("u_time", time);

	Matrix44 viewprojection = camera->viewprojection_matrix;

	Matrix44 model_matrix;
	model_matrix.setIdentity();
	model_matrix.translate(0, 0, 0); //example of translation

	shader->setMatrix44("model", model_matrix); //upload the transform matrix to the shader
	shader->setMatrix44("viewprojection", viewprojection); //upload viewprojection info to the shader
	shader->setVector3("u_camera_position", camera->eye);
	shader->setVector3("ambient_light", phong_light->diffuse_color);
	shader->setVector3("light_color", phong_light->specular_color);
	shader->setVector3("light_pos", phong_light->position);
	shader->setVector3("material_ambient", ambient);
	shader->setVector3("material_diffuse", diffuse);
	shader->setVector3("material_specular", specular);
	shader->setFloat("material_gloss", shininess);
}

void CloudMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void CloudMaterial::renderInMenu()
{
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
	
	ImGui::ColorEdit3("Material Ambient", (float*)&ambient); // Edit 3 floats representing a color
	ImGui::ColorEdit3("Material Diffuse", (float*)&diffuse); // Edit 3 floats representing a color
	ImGui::ColorEdit3("Material Specular", (float*)&specular); // Edit 3 floats representing a color

	ImGui::ColorEdit3("Light Diffuse", (float*)&phong_light->diffuse_color); // Edit 3 floats representing a color
	ImGui::ColorEdit3("Light Specular", (float*)&phong_light->specular_color); // Edit 3 floats representing a color
	ImGui::DragFloat3("Light Position", (float*)&phong_light->position); // Edit 3 floats representing a color
}


HeightMapMaterial::HeightMapMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/height.vs", "data/shaders/texture.fs");

	phong_light = new My_Light();

	ambient = { 1.0, 1.0, 1.0 };
	diffuse = { 1.0, 1.0, 1.0 };
	specular = { 1.0, 1.0, 1.0 };
	shininess = 50.0;

	phong_light->diffuse_color = { 1.0, 1.0, 1.0 };
}

HeightMapMaterial::~HeightMapMaterial()
{

}

void HeightMapMaterial::setUniforms(Camera* camera, Matrix44 model)
{
	//upload node uniforms
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_model", model);

	shader->setUniform("u_color", color);

	if (texture)
	{
		shader->setUniform("u_texture", texture);	//texture
	}

	if (beauty)
	{
		shader->setUniform("u_texture_beauty", beauty);	//texture
	}
}

void HeightMapMaterial::render(Mesh* mesh, Matrix44 model, Camera* camera)
{
	if (mesh && shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		setUniforms(camera, model);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
}

void HeightMapMaterial::renderInMenu()
{

}

My_Light::My_Light()
{
	position.set(50, 50, 0);
	diffuse_color.set(1.0f, 1.0f, 1.0f);
	specular_color.set(1.0f, 1.0f, 1.0f);
}
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
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/cloud.fs");
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

	shader->setUniform("u_time", time);
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
}


HeightMapMaterial::HeightMapMaterial()
{
	color = vec4(1.f, 1.f, 1.f, 1.f);
	shader = Shader::Get("data/shaders/height.vs", "data/shaders/flat.fs");
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
	ImGui::ColorEdit3("Color", (float*)&color); // Edit 3 floats representing a color
}

#ifndef MATERIAL_H
#define MATERIAL_H

#include "framework.h"
#include "shader.h"
#include "camera.h"
#include "mesh.h"
#include "extra/hdre.h"
#include "volume.h"

class My_Light;

class Material {
public:

	Shader* shader = NULL;
	Texture* texture = NULL;
	Texture* beauty = NULL;
	Volume* volume = NULL;
	bool jittering = false;
	bool gradient = false;
	vec4 color;

	float time = 0.0f;
	float brightness;
	float quality = 0.01;
	int index;

	My_Light* phong_light = NULL;

	Vector3 phong_light_color = { 1.0, 0.8, 0.0 };

	Vector3 ambient; //reflected ambient light
	Vector3 diffuse; //reflected diffuse light
	Vector3 specular; //reflected specular light
	float shininess; //glosiness coefficient (plasticity)

	virtual void setUniforms(Camera* camera, Matrix44 model) = 0;
	virtual void render(Mesh* mesh, Matrix44 model, Camera * camera) = 0;
	virtual void renderInMenu() = 0;
};

class StandardMaterial : public Material {
public:

	StandardMaterial();
	~StandardMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera * camera);
	void renderInMenu();
};

class WireframeMaterial : public StandardMaterial {
public:

	WireframeMaterial();
	~WireframeMaterial();

	void render(Mesh* mesh, Matrix44 model, Camera * camera);
};

class VolumeMaterial : public Material {
public:
	VolumeMaterial();
	~VolumeMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera * camera);
	void renderInMenu();
};

class CloudMaterial : public Material {
public:
	CloudMaterial();
	~CloudMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera * camera);
	void renderInMenu();
};

class HeightMapMaterial : public Material {
public:
	HeightMapMaterial();
	~HeightMapMaterial();

	void setUniforms(Camera* camera, Matrix44 model);
	void render(Mesh* mesh, Matrix44 model, Camera * camera);
	void renderInMenu();
};

class My_Light
{
public:

	Vector3 position;
	Vector3 diffuse_color;
	Vector3 specular_color;

	My_Light();
};

#endif
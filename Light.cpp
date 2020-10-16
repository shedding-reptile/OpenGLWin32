#include "Light.h"

Light::Light()
= default;

Light::~Light()
= default;

void Light::setDiffuseColor(float red, float green, float blue, float alpha)
{
	diffuseColor[0] = red;
	diffuseColor[1] = green;
	diffuseColor[2] = blue;
	diffuseColor[3] = alpha;
}

void Light::setDirection(float x, float y, float z)
{
	direction[0] = x;
	direction[1] = y;
	direction[2] = z;
}

void Light::setAmbientLight(float red, float green, float blue, float alpha)
{
	ambientLight[0] = red;
	ambientLight[1] = green;
	ambientLight[2] = blue;
	ambientLight[3] = alpha;
}

void Light::getDiffuseColor(float* color)
{
	color[0] = diffuseColor[0];
	color[1] = diffuseColor[1];
	color[2] = diffuseColor[2];
	color[3] = diffuseColor[3];
}

void Light::getDirection(float* dir)
{
	dir[0] = direction[0];
	dir[1] = direction[1];
	dir[2] = direction[2];
}

void Light::getAmbientLight(float* ambient)
{
	ambient[0] = ambientLight[0];
	ambient[1] = ambientLight[1];
	ambient[2] = ambientLight[2];
	ambient[3] = ambientLight[3];
}

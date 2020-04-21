#include "Light.h"

Light::Light()
= default;

Light::Light(const Light& other): m_diffuseColor{}, m_direction{}, m_ambientLight{}
{
}

Light::~Light()
= default;

void Light::setDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor[0] = red;
	m_diffuseColor[1] = green;
	m_diffuseColor[2] = blue;
	m_diffuseColor[3] = alpha;
}

void Light::setDirection(float x, float y, float z)
{
	m_direction[0] = x;
	m_direction[1] = y;
	m_direction[2] = z;
}

void Light::setAmbientLight(float red, float green, float blue, float alpha)
{
	m_ambientLight[0] = red;
	m_ambientLight[1] = green;
	m_ambientLight[2] = blue;
	m_ambientLight[3] = alpha;
}

void Light::getDiffuseColor(float* color)
{
	color[0] = m_diffuseColor[0];
	color[1] = m_diffuseColor[1];
	color[2] = m_diffuseColor[2];
	color[3] = m_diffuseColor[3];
}

void Light::getDirection(float* direction)
{
	direction[0] = m_direction[0];
	direction[1] = m_direction[1];
	direction[2] = m_direction[2];
}

void Light::getAmbientLight(float* ambient)
{
	ambient[0] = m_ambientLight[0];
	ambient[1] = m_ambientLight[1];
	ambient[2] = m_ambientLight[2];
	ambient[3] = m_ambientLight[3];
}

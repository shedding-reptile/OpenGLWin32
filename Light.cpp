#include "Light.h"

void Light::setDiffuseColour(glm::vec4 colour)
{
	diffuseColor = colour;
}

void Light::setDirection(glm::vec3 dir)
{
	direction = dir;
}

void Light::setAmbientLight(glm::vec4 colour)
{
	ambientLight = colour;
}

glm::vec4 Light::getDiffuseColor()
{
	return diffuseColor;
}

glm::vec3 Light::getDirection()
{
	return direction;
}

glm::vec4 Light::getAmbientLight()
{
	return ambientLight;
}

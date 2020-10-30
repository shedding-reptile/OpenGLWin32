#pragma once

#include <glm/glm.hpp>

class Light
{
public:

	Light() = default;
	~Light() = default;
	void setDiffuseColour(glm::vec4 colour);
	void setDirection(glm::vec3 dir);
	void setAmbientLight(glm::vec4 colour);
	glm::vec4 getDiffuseColor();
	glm::vec3 getDirection();
	glm::vec4 getAmbientLight();

private:
	glm::vec4 diffuseColor;
	glm::vec3 direction;
	glm::vec4 ambientLight;
};

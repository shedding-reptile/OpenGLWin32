#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera() = default;
	~Camera() = default;

	void setPosition(glm::vec3 pos);
	void render();
	glm::mat4 getViewMatrix();

private:
	
	glm::vec3 eye;	
	glm::mat4 viewMatrix;
};

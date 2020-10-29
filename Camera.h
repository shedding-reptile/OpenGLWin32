#pragma once

#include <glm/glm.hpp>

class Camera
{
private:
	struct VectorType 
	{ 
		float x, y, z;
	};

public:
	Camera() = default;
	~Camera() = default;

	void setPosition(float, float, float);
	void render();
	glm::mat4 getViewMatrix();

private:
	
	glm::vec3 eye;	
	glm::mat4 viewMatrix;
};

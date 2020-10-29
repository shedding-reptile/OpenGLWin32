#include <cmath>
#include <cstring>
#include <glm/ext/matrix_transform.hpp> 
#include "Camera.h"

void Camera::setPosition(float x, float y, float z)
{
	eye.x = x;
	eye.y = y;
	eye.z = z;
}

void Camera::render()
{	
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 3.0f);
	viewMatrix = glm::lookAt(eye, eye + center , up);
}

glm::mat4 Camera::getViewMatrix()
{
	return viewMatrix;
}

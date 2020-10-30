#pragma once

#include "OpenGL.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "Light.h"

enum class Direction { Left, Right, Up, Down };

class Graphics
{
public:
	Graphics(OpenGL* OpenGL);
	~Graphics();
	bool render() const;
	void move(Direction dir);
	bool load(const std::string &file);

private:
	bool initialize();

	OpenGL* context;
	Camera* camera;
	Model* model;
	Shader* shader;
	Light* light;
	glm::vec3 camPos;
};

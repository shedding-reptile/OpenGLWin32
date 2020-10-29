#pragma once

#include "OpenGL.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "Light.h"

const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

enum class Direction { Left, Right, Up, Down };

class Graphics
{
public:
	Graphics(OpenGL* OpenGL);
	~Graphics();
	bool render() const;
	void move(Direction dir);

private:
	bool initialize();

	OpenGL* context;
	Camera* camera;
	Model* model;
	Shader* shader;
	Light* light;
	float x;
	float y;
	float z;
};

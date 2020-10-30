#pragma once

#include "OpenGL.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "Light.h"
#include "PxPhysicsAPI.h"

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
	void createDynamic();
    bool load(const std::string &file);

private:
	bool initialize();
	void initPhysics();

	OpenGL* context;
	Camera* camera;
	Model* model;
	Shader* shader;
	glm::vec3 camPos;
	physx::PxMaterial* pxMat = nullptr;
};

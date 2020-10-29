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

private:
	bool initialize();
	void initPhysics();

	OpenGL* context;
	Camera* camera;
	Model* model;
	Shader* shader;
	Light* light;
	float x;
	float y;
	float z;

	physx::PxDefaultAllocator pxAllocator;
	physx::PxDefaultErrorCallback	pxErrorCallback;
	physx::PxFoundation* pxFoundation = nullptr;
	physx::PxPhysics* physics = nullptr;
	physx::PxDefaultCpuDispatcher* pxDispatcher = nullptr;
	physx::PxScene* scene = nullptr;
	physx::PxMaterial* pxMat = nullptr;
};

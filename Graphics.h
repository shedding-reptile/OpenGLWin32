#pragma once

#include "OpenGL.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "Light.h"
#include "PxPhysicsAPI.h"

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
	Light* light;
	glm::vec3 camPos;
	physx::PxDefaultAllocator pxAllocator;
	physx::PxDefaultErrorCallback	pxErrorCallback;
	physx::PxFoundation* pxFoundation = nullptr;
	physx::PxPhysics* physics = nullptr;
	physx::PxDefaultCpuDispatcher* pxDispatcher = nullptr;
	physx::PxScene* scene = nullptr;
	physx::PxMaterial* pxMat = nullptr;
};

void pxMatToGlmMat(const physx::PxMat44& shapePose, glm::mat4& modelMatrix);

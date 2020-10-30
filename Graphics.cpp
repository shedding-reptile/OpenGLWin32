#include "Graphics.h"
#include <exception>
#include <glm/gtc/matrix_transform.hpp>

Graphics::Graphics(OpenGL* OpenGL) :
	context(nullptr),
	camera(nullptr),
	model(nullptr),
	shader(nullptr),
	light(nullptr),
	pxFoundation(nullptr),
	physics(nullptr),
	pxDispatcher(nullptr),
	scene(nullptr),
	pxMat(nullptr)
{
	context = OpenGL;

	if (!initialize())
	{
		throw std::exception("Cannot initialize graphics!");
	}
}

Graphics::~Graphics()
{
	if (light)
	{
		delete light;
	}

	if (shader)
	{
		delete shader;
	}

	if (model)
	{
		delete model;
	}

	if (camera)
	{
		delete camera;
	}

	scene->release();
	pxDispatcher->release();
	physics->release();
	pxFoundation->release();
}

bool Graphics::initialize()
{
	camera = new Camera;
	camPos = { 0.0f, 0.0f, -10.0f };
	camera->setPosition(camPos);

	try
	{
		shader = new Shader;
	}
	catch (const std::exception&)
	{
		throw;
	}

	light = new Light;
	light->setDiffuseColour({ 1.0f, 1.0f, 1.0f, 1.0f });
	light->setDirection({ 1.0f, 0.0f, 0.0f });
	light->setAmbientLight({ 0.15f, 0.15f, 0.15f, 1.0f });

	initPhysics();

	return true;
}

void Graphics::initPhysics()
{
	pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, pxAllocator, pxErrorCallback);

	physics = PxCreatePhysics(PX_PHYSICS_VERSION, *pxFoundation, physx::PxTolerancesScale(), true);

	physx::PxSceneDesc sceneDesc(physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	pxDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = pxDispatcher;
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	scene = physics->createScene(sceneDesc);

	physx::PxPvdSceneClient* pvdClient = scene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	pxMat = physics->createMaterial(0.5f, 0.5f, 0.5f);

	physx::PxRigidStatic* groundPlane = PxCreatePlane(*physics, physx::PxPlane(0, 1, 0, 0), *pxMat);
	scene->addActor(*groundPlane);
}

void Graphics::move(Direction dir)
{
	switch (dir)
	{
	case Direction::Left:
		camPos.x += 0.1f;
		break;
	case Direction::Right:
		camPos.x -= 0.1f;
		break;
	case Direction::Up:
		camPos.z += 0.1f;
		break;
	case Direction::Down:
		camPos.z -= 0.1f;
		break;
	default:
		break;
	}
	camera->setPosition(camPos);
}

bool Graphics::load(const std::string& file)
{
	try
	{
		model = new Model(file);
	}
	catch (const std::exception&)
	{
		return false;
	}

	return true;
}

void Graphics::createDynamic()
{
	physx::PxTransform t = physx::PxTransform(physx::PxVec3(0, 10, 10));
	physx::PxRigidDynamic* dynamic = PxCreateDynamic(*physics, t, physx::PxSphereGeometry(1), *pxMat, 1.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(physx::PxVec3(0.3f, -10.0f, 0.5f));
	scene->addActor(*dynamic);
}

bool Graphics::render() const
{
	glm::mat4 modelMatrix = context->getModelMatrix();
	glm::mat4 viewMatrix = camera->getViewMatrix();
	glm::mat4 projectionMatrix = context->getProjectionMatrix();

	glm::vec3 lightDirection = light->getDirection();
	glm::vec4 diffuseLightColour = light->getDiffuseColor();
	glm::vec4 ambientLight = light->getAmbientLight();

	context->beginScene();

	camera->render();

	shader->setShader();

	if (!shader->setLightPosition({ 0.0f, 0.0f, -10.0f }))
	{
		return false;
	}

	if (!shader->setObjectColour({ 0.5f, 0.5f, 0.5f, 1.0f }))
	{
		return false;
	}

	if (model)
	{
		scene->simulate(1.0f / 60.0f);
		scene->fetchResults(true);

		physx::PxScene* scene;
		PxGetPhysics().getScenes(&scene, 1);
		physx::PxU32 nbActors = scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC);

		if (nbActors)
		{
			std::vector<physx::PxRigidActor*> actors(nbActors);
			scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC | physx::PxActorTypeFlag::eRIGID_STATIC, reinterpret_cast<physx::PxActor**>(&actors[0]), nbActors);

			physx::PxShape* shapes[128];
			for (physx::PxU32 i = 1; i < static_cast<physx::PxU32>(actors.size()); i++)
			{
				const physx::PxU32 nbShapes = actors[i]->getNbShapes();
				PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
				actors[i]->getShapes(shapes, nbShapes);
				const bool sleeping = actors[i]->is<physx::PxRigidDynamic>() ? actors[i]->is<physx::PxRigidDynamic>()->isSleeping() : false;

				for (physx::PxU32 j = 0; j < nbShapes; j++)
				{
					const physx::PxMat44 shapePose(physx::PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));
					const physx::PxGeometryHolder h = shapes[j]->getGeometry();

					pxMatToGlmMat(shapePose, modelMatrix);

					if (!shader->setMatrices(modelMatrix, viewMatrix, projectionMatrix))
					{
						return false;
					}

					model->render();
				}
			}
		}
	}
	context->endScene();

	return true;
}

void pxMatToGlmMat(const physx::PxMat44& shapePose, glm::mat4& modelMatrix)
{
	modelMatrix[0][0] = shapePose[0][0];
	modelMatrix[0][1] = shapePose[0][1];
	modelMatrix[0][2] = shapePose[0][2];
	modelMatrix[0][3] = shapePose[0][3];

	modelMatrix[1][0] = shapePose[1][0];
	modelMatrix[1][1] = shapePose[1][1];
	modelMatrix[1][2] = shapePose[1][2];
	modelMatrix[1][3] = shapePose[1][3];

	modelMatrix[2][0] = shapePose[2][0];
	modelMatrix[2][1] = shapePose[2][1];
	modelMatrix[2][2] = shapePose[2][2];
	modelMatrix[2][3] = shapePose[2][3];

	modelMatrix[3][0] = shapePose[3][0];
	modelMatrix[3][1] = shapePose[3][1];
	modelMatrix[3][2] = shapePose[3][2];
	modelMatrix[3][3] = shapePose[3][3];
}

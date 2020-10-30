#include "Graphics.h"
#include <exception>
#include <glm/gtc/matrix_transform.hpp>

Graphics::Graphics(OpenGL* OpenGL) :
	context(nullptr),
	camera(nullptr),
	model(nullptr),
	shader(nullptr),
	light(nullptr)
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
	light->setAmbientLight({0.15f, 0.15f, 0.15f, 1.0f});

	return true;
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
	if (!shader->setMatrices(modelMatrix, viewMatrix, projectionMatrix))
	{
		return false;
	}

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
		model->render();
	}

	context->endScene();

	return true;
}

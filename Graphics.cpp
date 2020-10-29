#include "Graphics.h"
#include <exception>
#include <glm/gtc/matrix_transform.hpp>

Graphics::Graphics(OpenGL* OpenGL) :
	context(nullptr),
	camera(nullptr),
	model(nullptr),
	shader(nullptr),
	light(nullptr),
	x(0.0f),
	y(0.0f),
	z(0.0f)
{
	// Store a pointer to the OpenGL class object.
	context = OpenGL;

	if (!initialize())
	{
		throw std::exception("Cannot initialize graphics!");
	}
}

Graphics::~Graphics()
{
	// Release the light object.
	if (light)
	{
		delete light;
	}

	// Release the light shader object.
	if (shader)
	{
		delete shader;
	}

	// Release the model object.
	if (model)
	{
		delete model;
	}

	// Release the camera object.
	if (camera)
	{
		delete camera;
	}
}

bool Graphics::initialize()
{
	// Create the camera object.
	camera = new Camera;

	// Set the initial position of the camera.
	z = -10.0f;

	camera->setPosition(x, y, z);

	// Create the model object.
	try
	{
		model = new Model("monkey.stl");
	}
	catch (const std::exception&)
	{
		throw;
	}

	// Create the light shader object.
	try
	{
		shader = new Shader;
	}
	catch (const std::exception&)
	{
		throw;
	}

	// Create the light object.
	light = new Light;

	// Initialize the light object.
	light->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(1.0f, 0.0f, 0.0f);
	light->setAmbientLight(0.15f, 0.15f, 0.15f, 1.0f);

	return true;
}

void Graphics::move(Direction dir)
{
	switch (dir)
	{
	case Direction::Left:
		x += 0.1f;
		break;
	case Direction::Right:
		x -= 0.1f;
		break;
	case Direction::Up:
		z += 0.1f;
		break;
	case Direction::Down:
		z -= 0.1f;
		break;
	default:
		break;
	}
	camera->setPosition(x, y, z);
}

bool Graphics::render() const
{
	static float rotation = 0.0f;

	// Update the rotation variable each frame.
	rotation += 0.0174532925f * 1.0f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Render the graphics scene.
	
	glm::mat4 projectionMatrix;
	float lightDirection[3];
	float diffuseLightColor[4];
	float ambientLight[4];

	// Clear the buffers to begin the scene.
	context->beginScene();

	// Generate the view matrix based on the camera's position.
	camera->render();

	// Get the world, view, and projection matrices from the opengl and camera objects.
	glm::mat4 modelMatrix = context->getModelMatrix();
	glm::mat4 viewMatrix = camera->getViewMatrix();
	projectionMatrix = context->getProjectionMatrix();

	// Get the light properties.
	light->getDirection(lightDirection);
	light->getDiffuseColor(diffuseLightColor);
	light->getAmbientLight(ambientLight);

	// Rotate the world matrix by the rotation value so that the triangle will spin.
	modelMatrix = glm::rotate(modelMatrix, rotation, glm::vec3(0.0f, 1.0f, 0.0f));

	// Set the light shader as the current shader program and set the matrices that it will use for rendering.
	shader->setShader();
	if (!shader->setMatrices(&modelMatrix[0][0], &viewMatrix[0][0], &projectionMatrix[0][0]))
	{
		return false;
	}

	// Position the light where the camera is.
	if (!shader->setLightPosition(0.0f, 0.0f, -10.0f))
	{
		return false;
	}

	if (!shader->setObjectColour(0.5f, 0.5f, 0.5f, 1.0f))
	{
		return false;
	}

	// Render the model using the light shader.
	model->render();

	// Present the rendered scene to the screen.
	context->endScene();

	return true;
}

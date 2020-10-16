#include "Graphics.h"
#include <exception>

Graphics::Graphics(OpenGL* OpenGL)
{
	context = nullptr;
	camera = nullptr;
	model = nullptr;
	shader = nullptr;
	light = nullptr;

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

	// Release the pointer to the OpenGL class object.
	context = nullptr;
}

bool Graphics::initialize()
{
	// Create the camera object.
	camera = new Camera;
	if (!camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	camera->setPosition(0.0f, 0.0f, -10.0f);

	// Create the model object.
	try
	{
		model = new Model("cube.txt", "opengl.tga", 0, true);
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
	if (!light)
	{
		return false;
	}

	// Initialize the light object.
	light->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(1.0f, 0.0f, 0.0f);
	light->setAmbientLight(0.15f, 0.15f, 0.15f, 1.0f);

	return true;
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
	float modelMatrix[16];
	float viewMatrix[16];
	float projectionMatrix[16];
	float lightDirection[3];
	float diffuseLightColor[4];
	float ambientLight[4];

	// Clear the buffers to begin the scene.
	context->beginScene();

	// Generate the view matrix based on the camera's position.
	camera->render();

	// Get the world, view, and projection matrices from the opengl and camera objects.
	context->getWorldMatrix(modelMatrix);
	camera->getViewMatrix(viewMatrix);
	context->getProjectionMatrix(projectionMatrix);

	// Get the light properties.
	light->getDirection(lightDirection);
	light->getDiffuseColor(diffuseLightColor);
	light->getAmbientLight(ambientLight);

	// Rotate the world matrix by the rotation value so that the triangle will spin.
	context->matrixRotationY(modelMatrix, rotation);

	// Set the light shader as the current shader program and set the matrices that it will use for rendering.
	shader->setShader();
	shader->setShaderParameters(modelMatrix, viewMatrix, projectionMatrix, 0, lightDirection, diffuseLightColor, ambientLight);

	// Render the model using the light shader.
	model->render();

	// Present the rendered scene to the screen.
	context->endScene();

	return true;
}

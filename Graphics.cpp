#include "Graphics.h"

Graphics::Graphics()
{
	m_OpenGL = nullptr;
	m_Camera = nullptr;
	m_Model = nullptr;
	m_LightShader = nullptr;
	m_Light = nullptr;
}


Graphics::Graphics(const Graphics& other): m_OpenGL(nullptr), m_Camera(nullptr), m_Model(nullptr),
                                                          m_LightShader(nullptr),
                                                          m_Light(nullptr)
{
}


Graphics::~Graphics()
{
}


bool Graphics::initialize(OpenGL* OpenGL, HWND hwnd)
{
	// Store a pointer to the OpenGL class object.
	m_OpenGL = OpenGL;

	// Create the camera object.
	m_Camera = new Camera;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->setPosition(0.0f, 0.0f, -10.0f);

	// Create the model object.
	m_Model = new Model;
	if(!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	bool result = m_Model->initialize(m_OpenGL, (char*)"cube.txt", (char*)"opengl.tga", 0,
	                                  true);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	
	// Create the light shader object.
	m_LightShader = new Shader;
	if(!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->initialize(m_OpenGL, hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new Light;
	if(!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->setDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->setDirection(1.0f, 0.0f, 0.0f);
	m_Light->setAmbientLight(0.15f, 0.15f, 0.15f, 1.0f);

	return true;
}


void Graphics::shutdown()
{
	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
	}

	// Release the light shader object.
	if(m_LightShader)
	{
		m_LightShader->shutdown(m_OpenGL);
		delete m_LightShader;
	}

	// Release the model object.
	if(m_Model)
	{
		m_Model->shutdown(m_OpenGL);
		delete m_Model;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
	}

	// Release the pointer to the OpenGL class object.
	m_OpenGL = nullptr;
}

bool Graphics::frame() const
{
	static float rotation = 0.0f;

	// Update the rotation variable each frame.
	rotation += 0.0174532925f * 1.0f;
	if(rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	// Render the graphics scene.
	return render(rotation);
}


bool Graphics::render(float rotation) const
{
	float worldMatrix[16];
	float viewMatrix[16];
	float projectionMatrix[16];
	float lightDirection[3];
	float diffuseLightColor[4];
	float ambientLight[4];


	// Clear the buffers to begin the scene.
	m_OpenGL->beginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->render();

	// Get the world, view, and projection matrices from the opengl and camera objects.
	m_OpenGL->getWorldMatrix(worldMatrix);
	m_Camera->getViewMatrix(viewMatrix);
	m_OpenGL->getProjectionMatrix(projectionMatrix);

	// Get the light properties.
	m_Light->getDirection(lightDirection);
	m_Light->getDiffuseColor(diffuseLightColor);
	m_Light->getAmbientLight(ambientLight);

	// Rotate the world matrix by the rotation value so that the triangle will spin.
	m_OpenGL->matrixRotationY(worldMatrix, rotation);

	// Set the light shader as the current shader program and set the matrices that it will use for rendering.
	m_LightShader->setShader(m_OpenGL);
	m_LightShader->setShaderParameters(m_OpenGL, worldMatrix, viewMatrix, projectionMatrix, 0, lightDirection, diffuseLightColor, ambientLight);

	// Render the model using the light shader.
	m_Model->render(m_OpenGL);
	
	// Present the rendered scene to the screen.
	m_OpenGL->endScene();

	return true;
}

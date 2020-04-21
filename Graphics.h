#pragma once

#include "OpenGL.h"
#include "Camera.h"
#include "Model.h"
#include "Shader.h"
#include "Light.h"

const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class Graphics
{
public:
	Graphics();
	Graphics(const Graphics&);
	~Graphics();

	bool initialize(OpenGL*, HWND);
	void shutdown();
	bool frame() const;

private:
	bool render(float) const;

	OpenGL* m_OpenGL;
	Camera* m_Camera;
	Model* m_Model;
	Shader* m_LightShader;
	Light* m_Light;
};

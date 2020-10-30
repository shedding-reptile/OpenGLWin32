#pragma once

#include <glm/glm.hpp>
#include <string>

class Shader
{
public:
	Shader();
	~Shader();
	void setShader() const;
	bool setMatrices(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) const;
	bool setLightPosition(glm::vec3 pos) const;
	bool setObjectColour(glm::vec4 colour) const;
	std::string getError();

private:
	bool initializeShader(const char*, const char*);
	void getShaderError(unsigned int);
	void getProgramError(unsigned int);
	
	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int program;
	std::string error;
};

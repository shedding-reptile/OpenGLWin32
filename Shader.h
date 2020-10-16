#pragma once

class Shader
{
public:
	Shader();
	~Shader();
	void setShader() const;
	bool setShaderParameters(float*, float*, float*, int, float*, float*, float*) const;

private:
	bool initializeShader(const char*, const char*);
	static void outputShaderErrorMessage(unsigned int);
	static void outputLinkerErrorMessage(unsigned int);
	
	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int shaderProgram;
};

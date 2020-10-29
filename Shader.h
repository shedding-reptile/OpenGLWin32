#pragma once

class Shader
{
public:
	Shader();
	~Shader();
	void setShader() const;
	bool setMatrices(float*, float*, float*) const;
	bool setLightPosition(float x, float y, float z) const;
	bool setObjectColour(float r, float g, float b, float a) const;

private:
	bool initializeShader(const char*, const char*);
	static void outputShaderErrorMessage(unsigned int);
	static void outputLinkerErrorMessage(unsigned int);
	
	unsigned int vertexShader;
	unsigned int fragmentShader;
	unsigned int program;
};

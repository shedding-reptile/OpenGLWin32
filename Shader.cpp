#include "Shader.h"
#include "resource.h"

#include <fstream>

Shader::Shader(): vertexShader(0), fragmentShader(0), shaderProgram(0)
{
	HRSRC hRes = FindResource(nullptr, MAKEINTRESOURCE(IDR_SHADER_V), L"SHADER");
	LPVOID vs = nullptr;
	if (hRes)
	{
		const HGLOBAL hData = LoadResource(nullptr, hRes);
		if (hData)
		{
			vs = LockResource(hData);
		}
	}

	LPVOID fs = nullptr;
	hRes = FindResource(nullptr, MAKEINTRESOURCE(IDR_SHADER_F), L"SHADER");
	if (hRes)
	{
		const HGLOBAL hData = LoadResource(nullptr, hRes);
		if (hData)
		{
			fs = LockResource(hData);
		}
	}

	if (!vs || !fs)
	{
		throw std::exception("Cannot read shader files!");
	}

	if (!initializeShader(static_cast<char*>(vs), static_cast<char*>(fs)))
	{
		throw std::exception("Cannot initialize shaders!");
	}
}

Shader::~Shader()
{
	// Detach the vertex and fragment shaders from the program.
	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);

	// Delete the vertex and fragment shaders.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Delete the shader program.
	glDeleteProgram(shaderProgram);
}

void Shader::setShader() const
{
	// Install the shader program as part of the current rendering state.
	glUseProgram(shaderProgram);
	
}

bool Shader::initializeShader(const char* vertexShaderBuffer, const char* fragmentShaderBuffer)
{
	int status;

	// Create a vertex and fragment shader object.
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	// Copy the shader source code strings into the vertex and fragment shader objects.
	glShaderSource(vertexShader, 1, &vertexShaderBuffer, nullptr);
	glShaderSource(fragmentShader, 1, &fragmentShaderBuffer, nullptr);

	// Compile the shaders.
	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	// Check to see if the vertex shader compiled successfully.
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if(status != 1)
	{
		// If it did not compile then write the syntax error message out to a text file for review.
		outputShaderErrorMessage(vertexShader);
		return false;
	}

	// Check to see if the fragment shader compiled successfully.
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if(status != 1)
	{
		// If it did not compile then write the syntax error message out to a text file for review.
		outputShaderErrorMessage(fragmentShader);
		return false;
	}

	// Create a shader program object.
	shaderProgram = glCreateProgram();

	// Attach the vertex and fragment shader to the program object.
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// Bind the shader input variables.
	glBindAttribLocation(shaderProgram, 0, "inputPosition");
	glBindAttribLocation(shaderProgram, 1, "inputTexCoord");
	glBindAttribLocation(shaderProgram, 2, "inputNormal");

	// Link the shader program.
	glLinkProgram(shaderProgram);

	// Check the status of the link.
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if(status != 1)
	{
		// If it did not link then write the syntax error message out to a text file for review.
		outputLinkerErrorMessage(shaderProgram);
		return false;
	}

	return true;
}

void Shader::outputShaderErrorMessage(unsigned int shaderId)
{
	int logSize;
	std::ofstream fout;


	// Get the size of the string containing the information log for the failed shader compilation message.
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);

	// Increment the size by one to handle also the null terminator.
	logSize++;

	// Create a char buffer to hold the info log.
	char* infoLog = new char[logSize];
	if(!infoLog)
	{
		return;
	}

	// Now retrieve the info log.
	glGetShaderInfoLog(shaderId, logSize, nullptr, infoLog);

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(int i = 0; i<logSize; i++)
	{
		fout << infoLog[i];
	}

	// Close the file.
	fout.close();
}

void Shader::outputLinkerErrorMessage(unsigned int programId)
{
	int logSize;
	std::ofstream fout;

	// Get the size of the string containing the information log for the failed shader compilation message.
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize);

	// Increment the size by one to handle also the null terminator.
	logSize++;

	// Create a char buffer to hold the info log.
	char* infoLog = new char[logSize];
	if(!infoLog)
	{
		return;
	}

	// Now retrieve the info log.
	glGetProgramInfoLog(programId, logSize, nullptr, infoLog);

	// Open a file to write the error message to.
	fout.open("linker-error.txt");

	// Write out the error message.
	for(int i = 0; i<logSize; i++)
	{
		fout << infoLog[i];
	}

	// Close the file.
	fout.close();
}

bool Shader::setShaderParameters(float* worldMatrix, float* viewMatrix, float* projectionMatrix, int textureUnit, float* lightDirection, float* diffuseLightColor, float* ambientLight) const
{
	// Set the world matrix in the vertex shader.
	unsigned int location = glGetUniformLocation(shaderProgram, "worldMatrix");
	if(location == -1)
	{
		return false;
	}
	glUniformMatrix4fv(location, 1, false, worldMatrix);

	// Set the view matrix in the vertex shader.
	location = glGetUniformLocation(shaderProgram, "viewMatrix");
	if(location == -1)
	{
		return false;
	}
	glUniformMatrix4fv(location, 1, false, viewMatrix);

	// Set the projection matrix in the vertex shader.
	location = glGetUniformLocation(shaderProgram, "projectionMatrix");
	if(location == -1)
	{
		return false;
	}
	glUniformMatrix4fv(location, 1, false, projectionMatrix);

	// Set the texture in the pixel shader to use the data from the first texture unit.
	location = glGetUniformLocation(shaderProgram, "shaderTexture");
	if(location == -1)
	{
		return false;
	}
	glUniform1i(location, textureUnit);

	// Set the light direction in the pixel shader.
	location = glGetUniformLocation(shaderProgram, "lightDirection");
	if(location == -1)
	{
		return false;
	}
	glUniform3fv(location, 1, lightDirection);

	// Set the light direction in the pixel shader.
	location = glGetUniformLocation(shaderProgram, "diffuseLightColor");
	if(location == -1)
	{
		return false;
	}
	glUniform4fv(location, 1, diffuseLightColor);

	// Set the ambient light in the pixel shader.
	location = glGetUniformLocation(shaderProgram, "ambientLight");
	if(location == -1)
	{
		return false;
	}
	glUniform4fv(location, 1, ambientLight);

	return true;
}

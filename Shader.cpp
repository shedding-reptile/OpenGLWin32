#include "Shader.h"
#include "resource.h"
#include "OpenGL.h"
#include <fstream>

Shader::Shader(): vertexShader(0), fragmentShader(0), program(0)
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
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	// Delete the vertex and fragment shaders.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Delete the shader program.
	glDeleteProgram(program);
}

void Shader::setShader() const
{
	// Install the shader program as part of the current rendering state.
	glUseProgram(program);
	
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
	program = glCreateProgram();

	// Attach the vertex and fragment shader to the program object.
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	// Bind the shader input variables.
	glBindAttribLocation(program, 0, "inputPosition");
	glBindAttribLocation(program, 1, "inputTexCoord");
	glBindAttribLocation(program, 2, "inputNormal");

	// Link the shader program.
	glLinkProgram(program);

	// Check the status of the link.
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if(status != 1)
	{
		// If it did not link then write the syntax error message out to a text file for review.
		outputLinkerErrorMessage(program);
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

bool Shader::setMatrices(const float * worldMatrix, const float * viewMatrix, const float * projectionMatrix) const
{
	// Set the world matrix in the vertex shader.
	unsigned int location = glGetUniformLocation(program, "modelMatrix");
	if(location == -1)
	{
		return false;
	}
	glUniformMatrix4fv(location, 1, false, worldMatrix);

	// Set the view matrix in the vertex shader.
	location = glGetUniformLocation(program, "viewMatrix");
	if(location == -1)
	{
		return false;
	}
	glUniformMatrix4fv(location, 1, false, viewMatrix);

	// Set the projection matrix in the vertex shader.
	location = glGetUniformLocation(program, "projectionMatrix");
	if(location == -1)
	{
		return false;
	}
	glUniformMatrix4fv(location, 1, false, projectionMatrix);

	return true;
}

bool Shader::setLightPosition(float x, float y, float z) const
{
	unsigned location = glGetUniformLocation(program, "lightPos");
	if (location == -1)
	{
		return false;
	}

	float lightPos[3] = { x, y, z };

	glUniform3fv(location, 1, lightPos);

	location = glGetUniformLocation(program, "viewPos");
	if (location == -1)
	{
		return false;
	}

	float viewPos[3] = { x, y, z };
	glUniform3fv(location, 1, viewPos);

	return true;
}

bool Shader::setObjectColour(float r, float g, float b, float a) const
{
	unsigned location = glGetUniformLocation(program, "objColour");
	if (location == -1)
	{
		return false;
	}

	float objColour[4] = { r, g, b, a};

	glUniform4fv(location, 1, objColour);

	return true;
}

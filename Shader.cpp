#include "Shader.h"
#include "resource.h"
#include "OpenGL.h"

Shader::Shader(): 
	vertexShader(0), 
	fragmentShader(0),
	program(0)
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
		throw std::exception(error.c_str());
	}
}

Shader::~Shader()
{
	glDetachShader(program, vertexShader);
	glDetachShader(program, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glDeleteProgram(program);
}

void Shader::setShader() const
{
	glUseProgram(program);	
}

bool Shader::initializeShader(const char* vertexShaderBuffer, const char* fragmentShaderBuffer)
{
	int status;

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, &vertexShaderBuffer, nullptr);
	glShaderSource(fragmentShader, 1, &fragmentShaderBuffer, nullptr);

	glCompileShader(vertexShader);
	glCompileShader(fragmentShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if(status != 1)
	{
		getShaderError(vertexShader);
		return false;
	}

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if(status != 1)
	{
		getShaderError(fragmentShader);
		return false;
	}

	program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glBindAttribLocation(program, 0, "inputPosition");
	glBindAttribLocation(program, 1, "inputTexCoord");
	glBindAttribLocation(program, 2, "inputNormal");

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if(status != 1)
	{
		getProgramError(program);
		return false;
	}

	return true;
}

void Shader::getShaderError(unsigned int shaderId)
{	
	int logSize;
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);
	logSize++;

	error.resize(logSize);
	glGetShaderInfoLog(shaderId, logSize, nullptr, &error[0]);
}

void Shader::getProgramError(unsigned int programId)
{
	int logSize;
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize);
	logSize++;

	error.resize(logSize);
	glGetProgramInfoLog(programId, logSize, nullptr, &error[0]);
}

bool Shader::setMatrices(const float * worldMatrix, const float * viewMatrix, const float * projectionMatrix) const
{
	unsigned int location = glGetUniformLocation(program, "modelMatrix");
	if(location == -1)
	{
		return false;
	}
	glUniformMatrix4fv(location, 1, false, &modelMatrix[0][0]);

	location = glGetUniformLocation(program, "viewMatrix");
	if(location == -1)
	{
		return false;
	}
	glUniformMatrix4fv(location, 1, false, &viewMatrix[0][0]);

	location = glGetUniformLocation(program, "projectionMatrix");
	if(location == -1)
	{
		return false;
	}
	glUniformMatrix4fv(location, 1, false, &projectionMatrix[0][0]);

	return true;
}

bool Shader::setLightPosition(glm::vec3 pos) const
{
	unsigned location = glGetUniformLocation(program, "lightPos");
	if (location == -1)
	{
		return false;
	}

	glUniform3fv(location, 1, &pos[0]);

	location = glGetUniformLocation(program, "viewPos");
	if (location == -1)
	{
		return false;
	}

	glUniform3fv(location, 1, &pos[0]);

	return true;
}

bool Shader::setObjectColour(glm::vec4 colour) const
{
	unsigned location = glGetUniformLocation(program, "objColour");
	if (location == -1)
	{
		return false;
	}

	glUniform4fv(location, 1, &colour[0]);

	return true;
}

std::string Shader::getError()
{
	return error;
}

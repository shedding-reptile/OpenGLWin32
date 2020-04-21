#include "Shader.h"
#include "resource.h"

#include <fstream>

Shader::Shader(): m_vertexShader(0), m_fragmentShader(0), m_shaderProgram(0)
{
}

Shader::Shader(const Shader& other): m_vertexShader(0), m_fragmentShader(0),
                                                                   m_shaderProgram(0)
{
}

Shader::~Shader()
{
}

bool Shader::initialize(OpenGL* OpenGL, HWND hwnd)
{
	HRSRC hRes = FindResource(nullptr, MAKEINTRESOURCE(IDR_SHADER_V), L"SHADER");
	LPVOID vs = nullptr;
	if(hRes)
	{
		const HGLOBAL hData = LoadResource(nullptr, hRes);
		if(hData)
		{
			vs = LockResource(hData);
		}		
	}

	LPVOID fs = nullptr;
	hRes = FindResource(nullptr, MAKEINTRESOURCE(IDR_SHADER_F), L"SHADER");
	if (hRes)
	{
		const HGLOBAL hData = LoadResource(nullptr, hRes);
		if(hData)
		{
			fs = LockResource(hData);
		}		
	}

	if(!vs || !fs)
	{
		return false;
	}

	return initializeShader(static_cast<char*>(vs), static_cast<char*>(fs), OpenGL, hwnd);
}

void Shader::shutdown(OpenGL* OpenGL) const
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	shutdownShader(OpenGL);

}

void Shader::setShader(OpenGL* OpenGL) const
{
	// Install the shader program as part of the current rendering state.
	OpenGL->glUseProgram(m_shaderProgram);
	
}

bool Shader::initializeShader(const char* vertexShaderBuffer, const char* fragmentShaderBuffer, OpenGL* OpenGL, HWND hwnd)
{
	int status;

	// Create a vertex and fragment shader object.
	m_vertexShader = OpenGL->glCreateShader(GL_VERTEX_SHADER);
	m_fragmentShader = OpenGL->glCreateShader(GL_FRAGMENT_SHADER);

	// Copy the shader source code strings into the vertex and fragment shader objects.
	OpenGL->glShaderSource(m_vertexShader, 1, &vertexShaderBuffer, nullptr);
	OpenGL->glShaderSource(m_fragmentShader, 1, &fragmentShaderBuffer, nullptr);

	// Compile the shaders.
	OpenGL->glCompileShader(m_vertexShader);
	OpenGL->glCompileShader(m_fragmentShader);

	// Check to see if the vertex shader compiled successfully.
	OpenGL->glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &status);
	if(status != 1)
	{
		// If it did not compile then write the syntax error message out to a text file for review.
		outputShaderErrorMessage(OpenGL, hwnd, m_vertexShader);
		return false;
	}

	// Check to see if the fragment shader compiled successfully.
	OpenGL->glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &status);
	if(status != 1)
	{
		// If it did not compile then write the syntax error message out to a text file for review.
		outputShaderErrorMessage(OpenGL, hwnd, m_fragmentShader);
		return false;
	}

	// Create a shader program object.
	m_shaderProgram = OpenGL->glCreateProgram();

	// Attach the vertex and fragment shader to the program object.
	OpenGL->glAttachShader(m_shaderProgram, m_vertexShader);
	OpenGL->glAttachShader(m_shaderProgram, m_fragmentShader);

	// Bind the shader input variables.
	OpenGL->glBindAttribLocation(m_shaderProgram, 0, "inputPosition");
	OpenGL->glBindAttribLocation(m_shaderProgram, 1, "inputTexCoord");
	OpenGL->glBindAttribLocation(m_shaderProgram, 2, "inputNormal");

	// Link the shader program.
	OpenGL->glLinkProgram(m_shaderProgram);

	// Check the status of the link.
	OpenGL->glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &status);
	if(status != 1)
	{
		// If it did not link then write the syntax error message out to a text file for review.
		outputLinkerErrorMessage(OpenGL, hwnd, m_shaderProgram);
		return false;
	}

	return true;
}

void Shader::outputShaderErrorMessage(OpenGL* OpenGL, HWND hwnd, unsigned int shaderId)
{
	int logSize;
	std::ofstream fout;


	// Get the size of the string containing the information log for the failed shader compilation message.
	OpenGL->glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logSize);

	// Increment the size by one to handle also the null terminator.
	logSize++;

	// Create a char buffer to hold the info log.
	char* infoLog = new char[logSize];
	if(!infoLog)
	{
		return;
	}

	// Now retrieve the info log.
	OpenGL->glGetShaderInfoLog(shaderId, logSize, nullptr, infoLog);

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(int i = 0; i<logSize; i++)
	{
		fout << infoLog[i];
	}

	// Close the file.
	fout.close();

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", L"Error", MB_OK);

}

void Shader::outputLinkerErrorMessage(OpenGL* OpenGL, HWND hwnd, unsigned int programId)
{
	int logSize;
	std::ofstream fout;

	// Get the size of the string containing the information log for the failed shader compilation message.
	OpenGL->glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &logSize);

	// Increment the size by one to handle also the null terminator.
	logSize++;

	// Create a char buffer to hold the info log.
	char* infoLog = new char[logSize];
	if(!infoLog)
	{
		return;
	}

	// Now retrieve the info log.
	OpenGL->glGetProgramInfoLog(programId, logSize, nullptr, infoLog);

	// Open a file to write the error message to.
	fout.open("linker-error.txt");

	// Write out the error message.
	for(int i = 0; i<logSize; i++)
	{
		fout << infoLog[i];
	}

	// Close the file.
	fout.close();

	// Pop a message up on the screen to notify the user to check the text file for linker errors.
	MessageBox(hwnd, L"Error compiling linker.  Check linker-error.txt for message.", L"Linker Error", MB_OK);

}

void Shader::shutdownShader(OpenGL* OpenGL) const
{
	// Detach the vertex and fragment shaders from the program.
	OpenGL->glDetachShader(m_shaderProgram, m_vertexShader);
	OpenGL->glDetachShader(m_shaderProgram, m_fragmentShader);

	// Delete the vertex and fragment shaders.
	OpenGL->glDeleteShader(m_vertexShader);
	OpenGL->glDeleteShader(m_fragmentShader);

	// Delete the shader program.
	OpenGL->glDeleteProgram(m_shaderProgram);

}

bool Shader::setShaderParameters(OpenGL* OpenGL, float* worldMatrix, float* viewMatrix, float* projectionMatrix, int textureUnit, float* lightDirection, float* diffuseLightColor, float* ambientLight) const
{
	// Set the world matrix in the vertex shader.
	unsigned int location = OpenGL->glGetUniformLocation(m_shaderProgram, "worldMatrix");
	if(location == -1)
	{
		return false;
	}
	OpenGL->glUniformMatrix4fv(location, 1, false, worldMatrix);

	// Set the view matrix in the vertex shader.
	location = OpenGL->glGetUniformLocation(m_shaderProgram, "viewMatrix");
	if(location == -1)
	{
		return false;
	}
	OpenGL->glUniformMatrix4fv(location, 1, false, viewMatrix);

	// Set the projection matrix in the vertex shader.
	location = OpenGL->glGetUniformLocation(m_shaderProgram, "projectionMatrix");
	if(location == -1)
	{
		return false;
	}
	OpenGL->glUniformMatrix4fv(location, 1, false, projectionMatrix);

	// Set the texture in the pixel shader to use the data from the first texture unit.
	location = OpenGL->glGetUniformLocation(m_shaderProgram, "shaderTexture");
	if(location == -1)
	{
		return false;
	}
	OpenGL->glUniform1i(location, textureUnit);

	// Set the light direction in the pixel shader.
	location = OpenGL->glGetUniformLocation(m_shaderProgram, "lightDirection");
	if(location == -1)
	{
		return false;
	}
	OpenGL->glUniform3fv(location, 1, lightDirection);

	// Set the light direction in the pixel shader.
	location = OpenGL->glGetUniformLocation(m_shaderProgram, "diffuseLightColor");
	if(location == -1)
	{
		return false;
	}
	OpenGL->glUniform4fv(location, 1, diffuseLightColor);

	// Set the ambient light in the pixel shader.
	location = OpenGL->glGetUniformLocation(m_shaderProgram, "ambientLight");
	if(location == -1)
	{
		return false;
	}
	OpenGL->glUniform4fv(location, 1, ambientLight);

	return true;
}

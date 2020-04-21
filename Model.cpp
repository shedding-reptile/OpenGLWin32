#include "Model.h"
#include <fstream>

Model::Model(): m_vertexCount(0), m_indexCount(0), m_vertexArrayId(0), m_vertexBufferId(0), m_indexBufferId(0)
{
	m_Texture = nullptr;
	m_model = nullptr;
}

Model::Model(const Model& other): m_vertexCount(0), m_indexCount(0), m_vertexArrayId(0),
                                                 m_vertexBufferId(0),
                                                 m_indexBufferId(0),
                                                 m_Texture(nullptr),
                                                 m_model(nullptr)
{
}

bool Model::initialize(OpenGL* OpenGL, char* modelFilename, char* textureFilename, unsigned int textureUnit, bool wrap)
{
	// Load in the model data.
	if(!loadModel(modelFilename))
	{
		return false;
	}

	// Initialize the vertex and index buffers that hold the geometry for the model.
	if(!initializeBuffers(OpenGL))
	{
		return false;
	}

	// Load the texture for this model.
	if(!loadTexture(OpenGL, textureFilename, textureUnit, wrap))
	{
		return false;
	}

	return true;
}

void Model::shutdown(OpenGL* OpenGL)
{
	// Release the texture used for this model.
	releaseTexture();

	// Release the vertex and index buffers.
	shutdownBuffers(OpenGL);

	// Release the model data.
	releaseModel();
}

void Model::render(OpenGL* OpenGL) const
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	renderBuffers(OpenGL);
}

bool Model::initializeBuffers(OpenGL* OpenGL)
{
	// Create the vertex array.
	VertexType* vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	unsigned* indices = new unsigned[m_indexCount];

	// Load the vertex array and index array with data.
	for(unsigned i = 0; i<m_vertexCount; i++)
	{
		vertices[i].x  = m_model[i].x;
		vertices[i].y  = m_model[i].y;
		vertices[i].z  = m_model[i].z;
		vertices[i].tu = m_model[i].tu;
		vertices[i].tv = 1.0f - m_model[i].tv;
		vertices[i].nx = m_model[i].nx;
		vertices[i].ny = m_model[i].ny;
		vertices[i].nz = m_model[i].nz;

		indices[i] = i;
	}

	// Allocate an OpenGL vertex array object.
	OpenGL->glGenVertexArrays(1, &m_vertexArrayId);

	// Bind the vertex array object to store all the buffers and vertex attributes we create here.
	OpenGL->glBindVertexArray(m_vertexArrayId);

	// Generate an ID for the vertex buffer.
	OpenGL->glGenBuffers(1, &m_vertexBufferId);

	// Bind the vertex buffer and load the vertex (position, texture, and normal) data into the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glBufferData(GL_ARRAY_BUFFER, m_vertexCount * sizeof(VertexType), vertices, GL_STATIC_DRAW);

	// Enable the three vertex array attributes.
	OpenGL->glEnableVertexAttribArray(0);  // Vertex position.
	OpenGL->glEnableVertexAttribArray(1);  // Texture coordinates.
	OpenGL->glEnableVertexAttribArray(2);  // Normals.

	// Specify the location and format of the position portion of the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexType), nullptr);

	// Specify the location and format of the texture coordinate portion of the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexType), (unsigned char*)nullptr + (3 * sizeof(float)));

	// Specify the location and format of the normal vector portion of the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
	OpenGL->glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(VertexType), (unsigned char*)nullptr + (5 * sizeof(float)));

	// Generate an ID for the index buffer.
	OpenGL->glGenBuffers(1, &m_indexBufferId);

	// Bind the index buffer and load the index data into it.
	OpenGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
	OpenGL->glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexCount* sizeof(unsigned int), indices, GL_STATIC_DRAW);
	
	// Now that the buffers have been loaded we can release the array data.
	delete [] vertices;

	delete [] indices;

	return true;
}

void Model::shutdownBuffers(OpenGL* OpenGL) const
{
	// Disable the two vertex array attributes.
	OpenGL->glDisableVertexAttribArray(0);
	OpenGL->glDisableVertexAttribArray(1);
	
	// Release the vertex buffer.
	OpenGL->glBindBuffer(GL_ARRAY_BUFFER, 0);
	OpenGL->glDeleteBuffers(1, &m_vertexBufferId);

	// Release the index buffer.
	OpenGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	OpenGL->glDeleteBuffers(1, &m_indexBufferId);

	// Release the vertex array object.
	OpenGL->glBindVertexArray(0);
	OpenGL->glDeleteVertexArrays(1, &m_vertexArrayId);

}

void Model::renderBuffers(OpenGL* OpenGL) const
{
	// Bind the vertex array object that stored all the information about the vertex and index buffers.
	OpenGL->glBindVertexArray(m_vertexArrayId);

	// Render the vertex buffer using the index buffer.
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);

}

bool Model::loadTexture(OpenGL* OpenGL, char* textureFilename, unsigned int textureUnit, bool wrap)
{
	// Create the texture object.
	m_Texture = new Texture;
	if(!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	bool result = m_Texture->initialize(OpenGL, textureFilename, textureUnit, wrap);
	if(!result)
	{
		return false;
	}

	return true;
}

void Model::releaseTexture()
{
	// Release the texture object.
	if(m_Texture)
	{
		m_Texture->shutdown();
		delete m_Texture;
		m_Texture = nullptr;
	}

}

bool Model::loadModel(char* filename)
{
	std::ifstream fin;
	char input;


	// Open the model file.
	fin.open(filename);
	
	// If it could not open the file then exit.
	if(fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_vertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_model = new ModelType[m_vertexCount];
	if(!m_model)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for(int i = 0; i<m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}

void Model::releaseModel()
{
	if(m_model)
	{
		delete [] m_model;
		m_model = nullptr;
	}

}

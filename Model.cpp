#include "Model.h"
#include <fstream>
#include <exception>

Model::Model(char* modelFilename, char* textureFilename, unsigned int textureUnit, bool wrap) :
	vertexCount(0),
	indexCount(0),
	vertexArrayId(0),
	vertexBufferId(0),
	indexBufferId(0)
{
	texture = nullptr;
	type = nullptr;

	// Load in the model data.
	if (!loadModel(modelFilename))
	{
		throw std::exception("Cannot load model file!");
	}

	// Initialize the vertex and index buffers that hold the geometry for the model.
	if (!initializeBuffers())
	{
		throw std::exception("Cannot initialize buffers!");
	}

	// Load the texture for this model.
	if (!loadTexture(textureFilename, textureUnit, wrap))
	{
		throw std::exception("Cannot load texture!");
	}
}

Model::~Model()
{
	// Release the texture used for this model.
	releaseTexture();

	// Release the vertex and index buffers.
	shutdownBuffers();

	// Release the model data.
	releaseModel();
}

void Model::render() const
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	renderBuffers();
}

bool Model::initializeBuffers()
{
	// Create the vertex array.
	VertexType* vertices = new VertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	unsigned* indices = new unsigned[indexCount];

	// Load the vertex array and index array with data.
	for (unsigned i = 0; i < vertexCount; i++)
	{
		vertices[i].x = type[i].x;
		vertices[i].y = type[i].y;
		vertices[i].z = type[i].z;
		vertices[i].tu = type[i].tu;
		vertices[i].tv = 1.0f - type[i].tv;
		vertices[i].nx = type[i].nx;
		vertices[i].ny = type[i].ny;
		vertices[i].nz = type[i].nz;

		indices[i] = i;
	}

	// Allocate an OpenGL vertex array object.
	glGenVertexArrays(1, &vertexArrayId);

	// Bind the vertex array object to store all the buffers and vertex attributes we create here.
	glBindVertexArray(vertexArrayId);

	// Generate an ID for the vertex buffer.
	glGenBuffers(1, &vertexBufferId);

	// Bind the vertex buffer and load the vertex (position, texture, and normal) data into the vertex buffer.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(VertexType), vertices, GL_STATIC_DRAW);

	// Enable the three vertex array attributes.
	glEnableVertexAttribArray(0);  // Vertex position.
	glEnableVertexAttribArray(1);  // Texture coordinates.
	glEnableVertexAttribArray(2);  // Normals.

	// Specify the location and format of the position portion of the vertex buffer.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(VertexType), nullptr);

	// Specify the location and format of the texture coordinate portion of the vertex buffer.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(VertexType), (unsigned char*)nullptr + (3 * sizeof(float)));

	// Specify the location and format of the normal vector portion of the vertex buffer.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(VertexType), (unsigned char*)nullptr + (5 * sizeof(float)));

	// Generate an ID for the index buffer.
	glGenBuffers(1, &indexBufferId);

	// Bind the index buffer and load the index data into it.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	// Now that the buffers have been loaded we can release the array data.
	delete[] vertices;

	delete[] indices;

	return true;
}

void Model::shutdownBuffers() const
{
	// Disable the two vertex array attributes.
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	// Release the vertex buffer.
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vertexBufferId);

	// Release the index buffer.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &indexBufferId);

	// Release the vertex array object.
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vertexArrayId);

}

void Model::renderBuffers() const
{
	// Bind the vertex array object that stored all the information about the vertex and index buffers.
	glBindVertexArray(vertexArrayId);

	// Render the vertex buffer using the index buffer.
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

}

bool Model::loadTexture(char* textureFilename, unsigned int textureUnit, bool wrap)
{
	// Create the texture object.
	try
	{
		texture = new Texture(textureFilename, textureUnit, wrap);
	}
	catch (const std::exception&)
	{
		return false;
	}

	return true;
}

void Model::releaseTexture()
{
	// Release the texture object.
	if (texture)
	{
		delete texture;
		texture = nullptr;
	}
}

bool Model::loadModel(char* filename)
{
	std::ifstream fin;
	char input;


	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> vertexCount;

	// Set the number of indices to be the same as the vertex count.
	indexCount = vertexCount;

	// Create the model using the vertex count that was read in.
	type = new ModelType[vertexCount];
	if (!type)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (unsigned i = 0; i < vertexCount; i++)
	{
		fin >> type[i].x >> type[i].y >> type[i].z;
		fin >> type[i].tu >> type[i].tv;
		fin >> type[i].nx >> type[i].ny >> type[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}

void Model::releaseModel()
{
	if (type)
	{
		delete[] type;
		type = nullptr;
	}

}

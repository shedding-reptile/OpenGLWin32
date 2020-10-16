#pragma once

#include "Texture.h"
#include "glad/glad.h"

class Model
{
	struct VertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	Model(const char* modelFilename, const char* textureFilename, unsigned int textureUnit, bool wrap);
	~Model();
	void render() const;

private:
	bool initializeBuffers();
	void shutdownBuffers() const;
	void renderBuffers() const;
	bool loadTexture(const char*, unsigned int, bool);
	void releaseTexture();
	bool loadModel(const char*);
	void releaseModel();

	unsigned vertexCount;
	unsigned indexCount;
	unsigned vertexArrayId;
	GLuint vertexBufferId;
	GLuint indexBufferId;
	Texture* texture;
	ModelType* type;
};

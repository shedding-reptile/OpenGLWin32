#pragma once

#include "Texture.h"

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
	Model();
	Model(const Model&);
	~Model() = default;
	bool initialize(OpenGL*, char*, char*, unsigned int, bool);
	void shutdown(OpenGL*);
	void render(OpenGL*) const;

private:
	bool initializeBuffers(OpenGL*);
	void shutdownBuffers(OpenGL*) const;
	void renderBuffers(OpenGL*) const;
	bool loadTexture(OpenGL*, char*, unsigned int, bool);
	void releaseTexture();
	bool loadModel(char*);
	void releaseModel();

	int m_vertexCount, m_indexCount;
	unsigned int m_vertexArrayId, m_vertexBufferId, m_indexBufferId;
	Texture* m_Texture;
	ModelType* m_model;
};

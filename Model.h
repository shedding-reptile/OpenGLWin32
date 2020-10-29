#pragma once

#include "glad/glad.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <glm/glm.hpp>
#include <vector>

class Model
{
public:
	Model(const char* modelFilename, int meshIndex = 0);
	~Model();
	void render() const;

private:
	void initializeBuffers();
	bool loadModel(const char*, int meshIndex);

	unsigned vao;
	unsigned vertVbo;
	unsigned normVbo;
	unsigned ebo;
	
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
};

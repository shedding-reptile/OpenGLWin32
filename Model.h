#pragma once

#include "glad/glad.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <vector>

class Model
{
public:
	Model(const std::string& modelFilename, int meshIndex = 0);
	~Model();
	void render() const;

private:
	void initializeBuffers();
	bool loadModel(const std::string & filename, int meshIndex);

	unsigned vao;
	unsigned vertVbo;
	unsigned normVbo;
	unsigned ebo;
	
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
};

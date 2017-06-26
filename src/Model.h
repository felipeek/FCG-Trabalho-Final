#pragma once

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "Texture.h"

#define DIRECTORY_MAX_SIZE 256

namespace raw
{
	class Model
	{
	public:
		Model(const std::vector<Mesh*>& meshes);
		Model(const char* path);
		~Model();
		void render(const Shader& shader, bool useNormalMap) const;
		std::vector<Mesh*> getMeshes() const;
		void setDiffuseMapOfAllMeshes(Texture* diffuseMap);
		void setSpecularMapOfAllMeshes(Texture* specularMap);
		void setNormalMapOfAllMeshes(Texture* normalMap);
		void setSpecularShinenessOfAllMeshes(float specularShineness);
	private:
		void loadModel(const char* path);
		int getPathDirectory(const char* path, char* buffer, unsigned int bufferSize) const;
		void processNode(aiNode* node, const aiScene* scene, char* directory);
		Mesh* processMesh(aiMesh* mesh, const aiScene* scene, char* directory);
		Texture* loadMaterialTexture(aiMaterial* material, aiTextureType type, char* directory);
		std::vector<Mesh*> meshes;
	};
}
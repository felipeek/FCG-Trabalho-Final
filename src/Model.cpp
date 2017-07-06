#include "Model.h"
#include <iostream>
#include <cstring>

using namespace raw;

// Creates a model using a vector of meshes.
Model::Model(const std::vector<Mesh*>& meshes)
{
	this->meshes = meshes;
}

// Creates a model receiving a path, which will be loaded with Assimp.
Model::Model(const char* path)
{
	this->loadModel(path);
}

Model::~Model()
{
	for (Mesh* m : this->meshes)
		delete m;
}

// Render the model using the shader received as parameter.
void Model::render(const Shader& shader, bool useNormalMap) const
{
	for (Mesh* m : this->meshes)
		m->render(shader, useNormalMap);
}

std::vector<Mesh*> Model::getMeshes() const
{
	return this->meshes;
}

void Model::setDiffuseMapOfAllMeshes(Texture* diffuseMap)
{
	for (unsigned int i = 0; i < this->meshes.size(); ++i)
		meshes[i]->setDiffuseMap(diffuseMap);
}

void Model::setSpecularMapOfAllMeshes(Texture* specularMap)
{
	for (unsigned int i = 0; i < this->meshes.size(); ++i)
		meshes[i]->setSpecularMap(specularMap);
}

void Model::setNormalMapOfAllMeshes(Texture* normalMap)
{
	for (unsigned int i = 0; i < this->meshes.size(); ++i)
		meshes[i]->setNormalMap(normalMap);
}

void Model::setSpecularShinenessOfAllMeshes(float specularShineness)
{
	for (unsigned int i = 0; i < this->meshes.size(); ++i)
		meshes[i]->setSpecularShineness(specularShineness);
}

// Load the model using Assimp. An array of meshes will be created for the model.
void Model::loadModel(const char* path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | /*aiProcess_FlipUVs |*/
		aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Error loading model with path " << path << ":";
		std::cout << "Error: " << importer.GetErrorString() << std::endl;
		return;
	}

	char directory[DIRECTORY_MAX_SIZE];
	this->getPathDirectory(path, directory, DIRECTORY_MAX_SIZE);
	this->processNode(scene->mRootNode, scene, directory);
}

// Receives a path and stores the directory of the path into the "buffer" received as parameter.
int Model::getPathDirectory(const char* path, char* buffer, unsigned int bufferSize) const
{
	unsigned int lastSlashIndex = 0;
	unsigned int pathSize = strlen(path);

	for (unsigned int i = 0; i < pathSize; ++i)
		if (path[i] == '/' || path[i] == '\\')
			lastSlashIndex = i;

	if ((lastSlashIndex + 1) > bufferSize)
		return -1;

	memcpy(buffer, path, lastSlashIndex);
	buffer[lastSlashIndex] = 0;
	return 0;
}

void Model::processNode(aiNode* node, const aiScene* scene, char* directory)
{
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(this->processMesh(mesh, scene, directory));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		this->processNode(node->mChildren[i], scene, directory);
}

// Generate the meshes based on the aiMesh received. aiMesh is an Assimp class and must be transformed to
// a Mesh class, used by the engine.
Mesh* Model::processMesh(aiMesh* mesh, const aiScene* scene, char* directory)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	Texture* diffuseMap = 0;
	Texture* specularMap = 0;
	Texture* normalMap = 0;

	// Fill Vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;
		vertex.position.w = 1.0f;

		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;
		vertex.normal.w = 0.0f;

		if (mesh->mTangents)
		{
			vertex.tangent.x = mesh->mTangents[i].x;
			vertex.tangent.y = mesh->mTangents[i].y;
			vertex.tangent.z = mesh->mTangents[i].z;
			vertex.tangent.w = 0.0f;
		}
		
		if (mesh->mTextureCoords[0])
		{
			vertex.textureCoordinates.x = mesh->mTextureCoords[0][i].x;
			vertex.textureCoordinates.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.textureCoordinates = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	// Fill Indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
			indices.push_back(mesh->mFaces[i].mIndices[j]);

	// Fill Material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		diffuseMap = this->loadMaterialTexture(material, aiTextureType_DIFFUSE, directory);
		specularMap = this->loadMaterialTexture(material, aiTextureType_SPECULAR, directory);
		normalMap = this->loadMaterialTexture(material, aiTextureType_HEIGHT, directory);
	}

	return new Mesh(vertices, indices, diffuseMap, specularMap, normalMap, 128.0f);
}

Texture* Model::loadMaterialTexture(aiMaterial* material, aiTextureType type, char* directory)
{
	aiString relativePath;

	if (material->GetTextureCount(type) <= 0)
		return 0;

	material->GetTexture(type, 0, &relativePath);
	// Generate Path
	char fullPath[256];
	strcpy(fullPath, directory);
	strcat(fullPath, "\\");
	strcat(fullPath, relativePath.C_Str());

	// Create and Return Texture
	Texture* texture = Texture::load(fullPath);
	return texture;
}
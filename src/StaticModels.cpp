#include "StaticModels.h"

using namespace raw;

Mesh* StaticModels::getCubeMesh(Texture* diffuseMap, Texture* specularMap, Texture* normalMap, float specularShineness)
{
	Vertex vertex[4];
	std::vector<Vertex> meshVertices;
	std::vector<unsigned int> meshIndices;

	glm::vec2 diffUV1 = glm::vec2(0.0f, 0.0f) - glm::vec2(1.0f, 0.0f);
	glm::vec2 diffUV2 = glm::vec2(1.0f, 0.0f) - glm::vec2(0.0f, 1.0f);
	glm::vec4 edge1, edge2, tangent;

	// FLOOR
	vertex[0].position = glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	vertex[0].normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertex[0].textureCoordinates = glm::vec2(0.0f, 0.0f);

	vertex[1].position = glm::vec4(1.0f, -1.0f, -1.0f, 1.0f);
	vertex[1].normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertex[1].textureCoordinates = glm::vec2(1.0f, 0.0f);

	vertex[2].position = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	vertex[2].normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertex[2].textureCoordinates = glm::vec2(0.0f, 1.0f);

	vertex[3].position = glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
	vertex[3].normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertex[3].textureCoordinates = glm::vec2(1.0f, 1.0f);

	edge1 = vertex[0].position - vertex[1].position;
	edge2 = vertex[1].position - vertex[2].position;
	tangent = Mesh::getTangentVector(diffUV1, diffUV2, edge1, edge2);

	for (unsigned int i = 0; i < 4; ++i)
	{
		vertex[i].tangent = tangent;
		meshVertices.push_back(vertex[i]);
	}

	meshIndices.push_back(0 + 0 * 4);
	meshIndices.push_back(1 + 0 * 4);
	meshIndices.push_back(2 + 0 * 4);
	meshIndices.push_back(1 + 0 * 4);
	meshIndices.push_back(3 + 0 * 4);
	meshIndices.push_back(2 + 0 * 4);

	// TOP
	vertex[0].position = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	vertex[0].normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertex[0].textureCoordinates = glm::vec2(0.0f, 0.0f);

	vertex[1].position = glm::vec4(1.0f, 1.0f, -1.0f, 1.0f);
	vertex[1].normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertex[1].textureCoordinates = glm::vec2(1.0f, 0.0f);

	vertex[2].position = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertex[2].textureCoordinates = glm::vec2(0.0f, 1.0f);

	vertex[3].position = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertex[3].textureCoordinates = glm::vec2(1.0f, 1.0f);

	edge1 = vertex[0].position - vertex[1].position;
	edge2 = vertex[1].position - vertex[2].position;
	tangent = Mesh::getTangentVector(diffUV1, diffUV2, edge1, edge2);

	for (unsigned int i = 0; i < 4; ++i)
	{
		vertex[i].tangent = tangent;
		meshVertices.push_back(vertex[i]);
	}

	meshIndices.push_back(0 + 1 * 4);
	meshIndices.push_back(1 + 1 * 4);
	meshIndices.push_back(2 + 1 * 4);
	meshIndices.push_back(1 + 1 * 4);
	meshIndices.push_back(3 + 1 * 4);
	meshIndices.push_back(2 + 1 * 4);

	// LEFT
	vertex[0].position = glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	vertex[0].normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertex[0].textureCoordinates = glm::vec2(0.0f, 0.0f);

	vertex[1].position = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	vertex[1].normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertex[1].textureCoordinates = glm::vec2(1.0f, 0.0f);

	vertex[2].position = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	vertex[2].normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertex[2].textureCoordinates = glm::vec2(0.0f, 1.0f);

	vertex[3].position = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertex[3].textureCoordinates = glm::vec2(1.0f, 1.0f);

	edge1 = vertex[0].position - vertex[1].position;
	edge2 = vertex[1].position - vertex[2].position;
	tangent = Mesh::getTangentVector(diffUV1, diffUV2, edge1, edge2);

	for (unsigned int i = 0; i < 4; ++i)
	{
		vertex[i].tangent = tangent;
		meshVertices.push_back(vertex[i]);
	}

	meshIndices.push_back(0 + 2 * 4);
	meshIndices.push_back(1 + 2 * 4);
	meshIndices.push_back(2 + 2 * 4);
	meshIndices.push_back(1 + 2 * 4);
	meshIndices.push_back(3 + 2 * 4);
	meshIndices.push_back(2 + 2 * 4);

	// RIGHT
	vertex[0].position = glm::vec4(1.0f, -1.0f, -1.0f, 1.0f);
	vertex[0].normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vertex[0].textureCoordinates = glm::vec2(0.0f, 0.0f);

	vertex[1].position = glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
	vertex[1].normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vertex[1].textureCoordinates = glm::vec2(1.0f, 0.0f);

	vertex[2].position = glm::vec4(1.0f, 1.0f, -1.0f, 1.0f);
	vertex[2].normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vertex[2].textureCoordinates = glm::vec2(0.0f, 1.0f);

	vertex[3].position = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vertex[3].textureCoordinates = glm::vec2(1.0f, 1.0f);

	edge1 = vertex[0].position - vertex[1].position;
	edge2 = vertex[1].position - vertex[2].position;
	tangent = Mesh::getTangentVector(diffUV1, diffUV2, edge1, edge2);

	for (unsigned int i = 0; i < 4; ++i)
	{
		vertex[i].tangent = tangent;
		meshVertices.push_back(vertex[i]);
	}

	meshIndices.push_back(0 + 3 * 4);
	meshIndices.push_back(1 + 3 * 4);
	meshIndices.push_back(2 + 3 * 4);
	meshIndices.push_back(1 + 3 * 4);
	meshIndices.push_back(3 + 3 * 4);
	meshIndices.push_back(2 + 3 * 4);

	// FRONT
	vertex[0].position = glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f);
	vertex[0].normal = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertex[0].textureCoordinates = glm::vec2(0.0f, 0.0f);

	vertex[1].position = glm::vec4(1.0f, -1.0f, -1.0f, 1.0f);
	vertex[1].normal = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertex[1].textureCoordinates = glm::vec2(1.0f, 0.0f);

	vertex[2].position = glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	vertex[2].normal = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertex[2].textureCoordinates = glm::vec2(0.0f, 1.0f);

	vertex[3].position = glm::vec4(1.0f, 1.0f, -1.0f, 1.0f);
	vertex[3].normal = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertex[3].textureCoordinates = glm::vec2(1.0f, 1.0f);

	edge1 = vertex[0].position - vertex[1].position;
	edge2 = vertex[1].position - vertex[2].position;
	tangent = Mesh::getTangentVector(diffUV1, diffUV2, edge1, edge2);

	for (unsigned int i = 0; i < 4; ++i)
	{
		vertex[i].tangent = tangent;
		meshVertices.push_back(vertex[i]);
	}

	meshIndices.push_back(0 + 4 * 4);
	meshIndices.push_back(1 + 4 * 4);
	meshIndices.push_back(2 + 4 * 4);
	meshIndices.push_back(1 + 4 * 4);
	meshIndices.push_back(3 + 4 * 4);
	meshIndices.push_back(2 + 4 * 4);

	// BACK
	vertex[0].position = glm::vec4(-1.0f, -1.0f, 1.0f, 1.0f);
	vertex[0].normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertex[0].textureCoordinates = glm::vec2(0.0f, 0.0f);

	vertex[1].position = glm::vec4(1.0f, -1.0f, 1.0f, 1.0f);
	vertex[1].normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertex[1].textureCoordinates = glm::vec2(1.0f, 0.0f);

	vertex[2].position = glm::vec4(-1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertex[2].textureCoordinates = glm::vec2(0.0f, 1.0f);

	vertex[3].position = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertex[3].textureCoordinates = glm::vec2(1.0f, 1.0f);

	edge1 = vertex[0].position - vertex[1].position;
	edge2 = vertex[1].position - vertex[2].position;
	tangent = Mesh::getTangentVector(diffUV1, diffUV2, edge1, edge2);

	for (unsigned int i = 0; i < 4; ++i)
	{
		vertex[i].tangent = tangent;
		meshVertices.push_back(vertex[i]);
	}

	meshIndices.push_back(0 + 5 * 4);
	meshIndices.push_back(1 + 5 * 4);
	meshIndices.push_back(2 + 5 * 4);
	meshIndices.push_back(1 + 5 * 4);
	meshIndices.push_back(3 + 5 * 4);
	meshIndices.push_back(2 + 5 * 4);

	return new Mesh(meshVertices, meshIndices, diffuseMap, specularMap, normalMap, specularShineness);
}
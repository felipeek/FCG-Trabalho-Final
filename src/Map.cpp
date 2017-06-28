#include "Map.h"
#include "stb_image.h"

static const int mapChannels = 4;

using namespace raw;

Map::Map(const char* mapPath)
{
	stbi_set_flip_vertically_on_load(1);

	this->mapBytes = stbi_load(mapPath, &this->mapWidth,
		&this->mapHeight, 0, mapChannels);

	this->mapXScalement = 1.0f;
	this->mapYScalement = 1.0f;
	this->mapZScalement = 1.0f;
}

Map::~Map()
{
	stbi_image_free(this->mapBytes);
}

Model* Map::generateMapModel() const
{
	std::vector<Mesh*> mapMeshes;

	for (int i = 0; i < this->mapHeight; ++i)
		for (int j = 0; j < this->mapWidth; ++j)
		{
			TerrainType terrainType = this->getTerrainType(glm::vec4(this->mapXScalement * j, 0.0f,
				this->mapZScalement * i, 1.0f));

			switch (terrainType)
			{
			case TerrainType::BLOCKED:
				mapMeshes.push_back(this->getMeshForBlockedTerrain(this->mapXScalement * j, this->mapZScalement * i));
				break;
			case TerrainType::FREE:
				mapMeshes.push_back(this->getMeshForFreeTerrain(this->mapXScalement * j, this->mapZScalement * i));
				break;
			case TerrainType::OUT:
			default:
				throw "Error generating Map Model: Out of Bounds";
				break;
			}
		}

	return new Model(mapMeshes);
}

std::vector<MapWallDescriptor> Map::generateMapWallDescriptors() const
{
	std::vector<MapWallDescriptor> mapWallDescriptors;
	std::vector<MapWallDescriptor> auxiliar;

	for (int i = 0; i < this->mapHeight; ++i)
		for (int j = 0; j < this->mapWidth; ++j)
		{
			TerrainType terrainType = this->getTerrainType(glm::vec4(this->mapXScalement * j, 0.0f,
				this->mapZScalement * i, 1.0f));

			switch (terrainType)
			{
			case TerrainType::BLOCKED:
				auxiliar = this->getMapWallDescriptorsForBlockedTerrain(this->mapXScalement * j,
					this->mapZScalement * i);
				// Append Vector
				mapWallDescriptors.insert(mapWallDescriptors.end(), auxiliar.begin(), auxiliar.end());
				break;
			case TerrainType::FREE:
				auxiliar = this->getMapWallDescriptorsForFreeTerrain(this->mapXScalement * j,
					this->mapZScalement * i);
				// Append Vector
				mapWallDescriptors.insert(mapWallDescriptors.end(), auxiliar.begin(), auxiliar.end());
				break;
			case TerrainType::OUT:
			default:
				throw "Error generating Map Wall Descriptor: Out of Bounds";
				break;
			}
		}

	return mapWallDescriptors;
}

TerrainType Map::getTerrainType(const glm::vec4& position) const
{
	glm::vec4 pixelColor;
	int xPos = (int)floorf(position.x);
	int zPos = (int)floorf(position.z);
	
	if (xPos < 0 || xPos > (this->mapWidth - 1) * this->mapXScalement)
		return TerrainType::OUT;
	if (zPos < 0 || zPos > (this->mapHeight - 1) * this->mapZScalement)
		return TerrainType::OUT;

	int z = (int)floorf((zPos / this->mapZScalement)) * mapChannels;
	int x = (int)floorf((xPos / this->mapXScalement)) * mapChannels;

	pixelColor.r = (float)*(this->mapBytes + (x * this->mapWidth + z));
	pixelColor.g = (float)*(this->mapBytes + (x * this->mapWidth + z + 1));
	pixelColor.b = (float)*(this->mapBytes + (x * this->mapWidth + z + 2));
	pixelColor.a = (float)*(this->mapBytes + (x * this->mapWidth + z + 3));

	if (pixelColor.r == 255)
		return TerrainType::FREE;
	else
		return TerrainType::BLOCKED;
}

TerrainType Map::getTerrainTypeForMovement(const glm::vec4& position) const
{
	const float collisionFactor = 0.2f;
	TerrainType terrainType;

	terrainType = this->getTerrainType(position);

	if (terrainType == TerrainType::BLOCKED || terrainType == TerrainType::OUT)
		return terrainType;

	glm::vec4 testPositions[6] = { position, position, position, position, position, position };

	testPositions[0].x += collisionFactor;
	testPositions[1].x -= collisionFactor;
	testPositions[2].y += collisionFactor;
	testPositions[3].y -= collisionFactor;
	testPositions[4].z += collisionFactor;
	testPositions[5].z -= collisionFactor;

	for (unsigned int i = 0; i < 6; ++i)
	{
		terrainType = this->getTerrainType(testPositions[i]);
		if (terrainType == TerrainType::BLOCKED || terrainType == TerrainType::OUT)
			return terrainType;
	}

	return TerrainType::FREE;
}

Mesh* Map::getMeshForBlockedTerrain(float xPos, float zPos) const
{
	Vertex vertex[4];
	std::vector<Vertex> meshVertices;
	std::vector<unsigned int> meshIndices;

	glm::vec2 diffUV1 = glm::vec2(0.0f, 0.0f) - glm::vec2(1.0f, 0.0f);
	glm::vec2 diffUV2 = glm::vec2(1.0f, 0.0f) - glm::vec2(0.0f, 1.0f);
	glm::vec4 edge1, edge2, tangent;

	// FLOOR
	vertex[0].position = glm::vec4(xPos, 0.0f, zPos, 1.0f);
	vertex[0].normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertex[0].textureCoordinates = glm::vec2(0.0f, 0.0f);

	vertex[1].position = glm::vec4(xPos + this->mapXScalement, 0.0f, zPos, 1.0f);
	vertex[1].normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertex[1].textureCoordinates = glm::vec2(1.0f, 0.0f);

	vertex[2].position = glm::vec4(xPos, 0.0f, zPos + this->mapZScalement, 1.0f);
	vertex[2].normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	vertex[2].textureCoordinates = glm::vec2(0.0f, 1.0f);

	vertex[3].position = glm::vec4(xPos + this->mapXScalement, 0.0f, zPos + this->mapZScalement, 1.0f);
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
	vertex[0].position = glm::vec4(xPos, this->mapYScalement, zPos, 1.0f);
	vertex[0].normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertex[0].textureCoordinates = glm::vec2(0.0f, 0.0f);

	vertex[1].position = glm::vec4(xPos + this->mapXScalement, this->mapYScalement, zPos, 1.0f);
	vertex[1].normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertex[1].textureCoordinates = glm::vec2(1.0f, 0.0f);

	vertex[2].position = glm::vec4(xPos, this->mapYScalement, zPos + this->mapZScalement, 1.0f);
	vertex[2].normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertex[2].textureCoordinates = glm::vec2(0.0f, 1.0f);

	vertex[3].position = glm::vec4(xPos + this->mapXScalement, this->mapYScalement,
		zPos + this->mapZScalement, 1.0f);
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
	vertex[0].position = glm::vec4(xPos, 0.0f, zPos, 1.0f);
	vertex[0].normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertex[0].textureCoordinates = glm::vec2(0.0f, 0.0f);

	vertex[1].position = glm::vec4(xPos, 0.0f, zPos + this->mapZScalement, 1.0f);
	vertex[1].normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertex[1].textureCoordinates = glm::vec2(1.0f, 0.0f);

	vertex[2].position = glm::vec4(xPos, this->mapYScalement, zPos, 1.0f);
	vertex[2].normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	vertex[2].textureCoordinates = glm::vec2(0.0f, 1.0f);

	vertex[3].position = glm::vec4(xPos, this->mapYScalement, zPos + this->mapZScalement, 1.0f);
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
	vertex[0].position = glm::vec4(xPos + this->mapXScalement, 0.0f, zPos, 1.0f);
	vertex[0].normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vertex[0].textureCoordinates = glm::vec2(0.0f, 0.0f);

	vertex[1].position = glm::vec4(xPos + this->mapXScalement, 0.0f, zPos + this->mapZScalement, 1.0f);
	vertex[1].normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vertex[1].textureCoordinates = glm::vec2(1.0f, 0.0f);

	vertex[2].position = glm::vec4(xPos + this->mapXScalement, this->mapYScalement, zPos, 1.0f);
	vertex[2].normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	vertex[2].textureCoordinates = glm::vec2(0.0f, 1.0f);

	vertex[3].position = glm::vec4(xPos + this->mapXScalement, this->mapYScalement,
		zPos + this->mapZScalement, 1.0f);
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
	vertex[0].position = glm::vec4(xPos, 0.0f, zPos, 1.0f);
	vertex[0].normal = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertex[0].textureCoordinates = glm::vec2(0.0f, 0.0f);

	vertex[1].position = glm::vec4(xPos + this->mapXScalement, 0.0f, zPos, 1.0f);
	vertex[1].normal = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertex[1].textureCoordinates = glm::vec2(1.0f, 0.0f);

	vertex[2].position = glm::vec4(xPos, this->mapYScalement, zPos, 1.0f);
	vertex[2].normal = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	vertex[2].textureCoordinates = glm::vec2(0.0f, 1.0f);

	vertex[3].position = glm::vec4(xPos + this->mapXScalement, this->mapYScalement, zPos, 1.0f);
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
	vertex[0].position = glm::vec4(xPos, 0.0f, zPos + this->mapZScalement, 1.0f);
	vertex[0].normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertex[0].textureCoordinates = glm::vec2(0.0f, 0.0f);

	vertex[1].position = glm::vec4(xPos + this->mapXScalement, 0.0f, zPos + this->mapZScalement, 1.0f);
	vertex[1].normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertex[1].textureCoordinates = glm::vec2(1.0f, 0.0f);

	vertex[2].position = glm::vec4(xPos, this->mapYScalement, zPos + this->mapZScalement, 1.0f);
	vertex[2].normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	vertex[2].textureCoordinates = glm::vec2(0.0f, 1.0f);

	vertex[3].position = glm::vec4(xPos + this->mapXScalement, this->mapYScalement,
		zPos + this->mapZScalement, 1.0f);
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

	//Texture* diffuseMap = Texture::load(".\\res\\art\\rust_diffuse.jpg");
	//Texture* specularMap = Texture::load(".\\res\\art\\rust_specular.jpg");
	//Texture* normalMap = Texture::load(".\\res\\art\\rust_normal.jpg");

	Texture* diffuseMap = Texture::load(".\\res\\art\\brickwall_diffuse.jpg");
	Texture* specularMap = Texture::load(".\\res\\art\\black.png");
	Texture* normalMap = Texture::load(".\\res\\art\\brickwall_normal.jpg");

//	Texture* diffuseMap = Texture::load(".\\res\\art\\tileable_stone_wall_diffuse.jpg");
//	Texture* specularMap = Texture::load(".\\res\\art\\tileable_stone_wall_specular.jpg");
//	Texture* normalMap = Texture::load(".\\res\\art\\tileable_stone_wall_normal.jpg");

	return new Mesh(meshVertices, meshIndices, diffuseMap, specularMap, normalMap, 32.0f);
}

Mesh* Map::getMeshForFreeTerrain(float xPos, float zPos) const
{
	Vertex vertex;
	std::vector<Vertex> meshVertices;
	std::vector<unsigned int> meshIndices;

	vertex.position = glm::vec4(xPos, 0.0f, zPos, 1.0f);
	vertex.normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertex.textureCoordinates = glm::vec2(0.0f, 0.0f);
	meshVertices.push_back(vertex);

	vertex.position = glm::vec4(xPos + this->mapXScalement, 0.0f, zPos, 1.0f);
	vertex.normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertex.textureCoordinates = glm::vec2(1.0f, 0.0f);
	meshVertices.push_back(vertex);

	vertex.position = glm::vec4(xPos, 0.0f, zPos + this->mapZScalement, 1.0f);
	vertex.normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertex.textureCoordinates = glm::vec2(0.0f, 1.0f);
	meshVertices.push_back(vertex);

	vertex.position = glm::vec4(xPos + this->mapXScalement, 0.0f, zPos + this->mapZScalement, 1.0f);
	vertex.normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	vertex.textureCoordinates = glm::vec2(1.0f, 1.0f);
	meshVertices.push_back(vertex);

	meshIndices.push_back(0 + 0 * 4);
	meshIndices.push_back(1 + 0 * 4);
	meshIndices.push_back(2 + 0 * 4);
	meshIndices.push_back(1 + 0 * 4);
	meshIndices.push_back(3 + 0 * 4);
	meshIndices.push_back(2 + 0 * 4);

	Texture* diffuseMap = Texture::load(".\\res\\art\\grass01.jpg");
	Texture* specularMap = Texture::load(".\\res\\art\\grass01_s.jpg");
	Texture* normalMap = Texture::load(".\\res\\art\\grass01_n.jpg");
	//Texture* specularMap = Texture::load(".\\res\\art\\black.png");
	//Texture* normalMap = 0;

	return new Mesh(meshVertices, meshIndices, diffuseMap, specularMap, normalMap, 32.0f);
}

std::vector<MapWallDescriptor> Map::getMapWallDescriptorsForBlockedTerrain(float xPos, float zPos) const
{
	MapWallDescriptor mapWallDescriptor;
	std::vector<MapWallDescriptor> mapWallDescriptors;

	// FLOOR
	mapWallDescriptor.centerPosition = glm::vec4(xPos + (this->mapXScalement / 2.0f),
		0.0f, zPos + (this->mapZScalement / 2.0f), 1.0f);
	mapWallDescriptor.normalVector = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	mapWallDescriptor.xLength = this->mapXScalement;
	mapWallDescriptor.yLength = 0.0f;
	mapWallDescriptor.zLength = this->mapZScalement;
	mapWallDescriptors.push_back(mapWallDescriptor);

	// TOP
	mapWallDescriptor.centerPosition = glm::vec4(xPos + (this->mapXScalement / 2.0f),
		this->mapYScalement, zPos + (this->mapZScalement / 2.0f), 1.0f);
	mapWallDescriptor.normalVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	mapWallDescriptor.xLength = this->mapXScalement;
	mapWallDescriptor.yLength = 0.0f;
	mapWallDescriptor.zLength = this->mapZScalement;
	mapWallDescriptors.push_back(mapWallDescriptor);

	// LEFT
	mapWallDescriptor.centerPosition = glm::vec4(xPos,
		this->mapYScalement / 2.0f, zPos + (this->mapZScalement / 2.0f), 1.0f);
	mapWallDescriptor.normalVector = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
	mapWallDescriptor.xLength = 0.0f;
	mapWallDescriptor.yLength = this->mapYScalement;
	mapWallDescriptor.zLength = this->mapZScalement;
	mapWallDescriptors.push_back(mapWallDescriptor);

	// RIGHT
	mapWallDescriptor.centerPosition = glm::vec4(xPos + this->mapXScalement,
		this->mapYScalement / 2.0f, zPos + (this->mapZScalement / 2.0f), 1.0f);
	mapWallDescriptor.normalVector = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	mapWallDescriptor.xLength = 0.0f;
	mapWallDescriptor.yLength = this->mapYScalement;
	mapWallDescriptor.zLength = this->mapZScalement;
	mapWallDescriptors.push_back(mapWallDescriptor);

	// FRONT
	mapWallDescriptor.centerPosition = glm::vec4(xPos + (this->mapXScalement / 2.0f),
		this->mapYScalement / 2.0f, zPos, 1.0f);
	mapWallDescriptor.normalVector = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	mapWallDescriptor.xLength = this->mapXScalement;
	mapWallDescriptor.yLength = this->mapYScalement;
	mapWallDescriptor.zLength = 0.0f;
	mapWallDescriptors.push_back(mapWallDescriptor);

	// BACK
	mapWallDescriptor.centerPosition = glm::vec4(xPos + (this->mapXScalement / 2.0f),
		this->mapYScalement / 2.0f, zPos + this->mapZScalement, 1.0f);
	mapWallDescriptor.normalVector = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	mapWallDescriptor.xLength = this->mapXScalement;
	mapWallDescriptor.yLength = this->mapYScalement;
	mapWallDescriptor.zLength = 0.0f;
	mapWallDescriptors.push_back(mapWallDescriptor);

	return mapWallDescriptors;
}

std::vector<MapWallDescriptor> Map::getMapWallDescriptorsForFreeTerrain(float xPos, float zPos) const
{
	MapWallDescriptor mapWallDescriptor;
	std::vector<MapWallDescriptor> mapWallDescriptors;

	// FLOOR
	mapWallDescriptor.centerPosition = glm::vec4(xPos + (this->mapXScalement / 2.0f),
		0.0f, zPos + (this->mapZScalement / 2.0f), 1.0f);
	mapWallDescriptor.normalVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	mapWallDescriptor.xLength = this->mapXScalement;
	mapWallDescriptor.yLength = 0.0f;
	mapWallDescriptor.zLength = this->mapZScalement;
	mapWallDescriptors.push_back(mapWallDescriptor);

	return mapWallDescriptors;
}
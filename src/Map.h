#pragma once

#include "Model.h"

namespace raw
{
	enum class TerrainType
	{
		BLOCKED,
		FREE,
		OUT
	};

	struct MapWallDescriptor
	{
		glm::vec4 normalVector;
		glm::vec4 centerPosition;
		float xLength;
		float yLength;
		float zLength;
	};

	struct TerrainMesh
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	class Map
	{
	public:
		Map(const char* mapPath);
		~Map();
		Model* generateMapModel() const;
		std::vector<MapWallDescriptor> generateMapWallDescriptors() const;
		TerrainType getTerrainType(const glm::vec4& position) const;
		TerrainType getTerrainTypeForMovement(const glm::vec4& position) const;
		float getMapXSize() const;
		float getMapYSize() const;
		float getMapZSize() const;
	private:
		TerrainMesh getBlockedTerrainVerticesAndIndices(float xPos, float zPos) const;
		TerrainMesh getFreeTerainVerticesAndIndices(float xPos, float zPos) const;
		std::vector<MapWallDescriptor> getMapWallDescriptorsForBlockedTerrain(float xPos, float zPos) const;
		std::vector<MapWallDescriptor> getMapWallDescriptorsForFreeTerrain(float xPos, float zPos) const;
		float mapXScalement;
		float mapYScalement;
		float mapZScalement;
		unsigned char* mapBytes;
		int mapWidth;
		int mapHeight;
	};
}
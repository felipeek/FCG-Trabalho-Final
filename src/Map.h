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

	class Map
	{
	public:
		Map(const char* mapPath);
		~Map();
		Model* generateMapModel() const;
		TerrainType getTerrainType(const glm::vec4& position) const;
		TerrainType getTerrainTypeForMovement(const glm::vec4& position) const;
	private:
		Mesh* getMeshForBlockedTerrain(float xPos, float zPos) const;
		Mesh* getMeshForFreeTerrain(float xPos, float zPos) const;
		float mapXScalement;
		float mapYScalement;
		float mapZScalement;
		unsigned char* mapBytes;
		int mapWidth;
		int mapHeight;
	};
}
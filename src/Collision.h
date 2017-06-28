#pragma once

#include "MathIncludes.h"
#include "Player.h"
#include "Map.h"

namespace raw
{
	struct CollisionDescriptor
	{
		bool collide;
		glm::vec4 worldPosition;
	};

	struct PlayerCollisionDescriptor
	{
		CollisionDescriptor collision;
		PlayerBodyPart bodyPart;
	};

	class Collision
	{
	public:
		static CollisionDescriptor getClosestWallRayIsColliding(glm::vec4 rayPosition, glm::vec4 rayDirection,
			const std::vector<MapWallDescriptor>& mapWallDescriptors);
		static CollisionDescriptor isRayCollidingWithWall(glm::vec4 rayPosition, glm::vec4 rayDirection,
			const MapWallDescriptor& mapWallDescriptor);
		static PlayerCollisionDescriptor isRayCollidingWithBoundingBox(glm::vec4 rayPosition,
			glm::vec4 rayDirection, glm::vec4 rayXAxis, glm::vec4 rayYAxis, std::vector<BoundingShape>& boundingBox);
	};
}
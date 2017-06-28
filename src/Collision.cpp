#include "Collision.h"
#include <vector>

using namespace raw;

// Test ray-wall collision using a line-plane collision algorithm!
CollisionDescriptor Collision::getClosestWallRayIsColliding(glm::vec4 rayPosition, glm::vec4 rayDirection,
	const std::vector<MapWallDescriptor>& mapWallDescriptors)
{
	std::vector<glm::vec4> wallCollisions;

	for (unsigned int i = 0; i < mapWallDescriptors.size(); ++i)
	{
		CollisionDescriptor collisionDescriptor = Collision::isRayCollidingWithWall(rayPosition, rayDirection,
			mapWallDescriptors[i]);

		if (collisionDescriptor.collide)
			wallCollisions.push_back(collisionDescriptor.worldPosition);
	}

	// Find nearest collide position
	CollisionDescriptor collisionDescriptor;

	if (wallCollisions.size() > 0)
	{
		glm::vec4 nearestPosition = wallCollisions[0];
		float nearestPositionDistance = glm::length(rayPosition - nearestPosition);

		for (unsigned int i = 1; i < wallCollisions.size(); ++i)
		{
			glm::vec4 currentPosition = wallCollisions[i];
			float currentPositionDistance = glm::length(rayPosition - currentPosition);

			if (currentPositionDistance < nearestPositionDistance)
			{
				nearestPosition = currentPosition;
				nearestPositionDistance = currentPositionDistance;
			}
		}

		collisionDescriptor.collide = true;
		collisionDescriptor.worldPosition = nearestPosition;
	}
	else
		collisionDescriptor.collide = false;

	return collisionDescriptor;
}

CollisionDescriptor Collision::isRayCollidingWithWall(glm::vec4 rayPosition, glm::vec4 rayDirection, const MapWallDescriptor& mapWallDescriptor)
{
	CollisionDescriptor collisionDescriptor;

	float rayWallDot = glm::dot(rayDirection, mapWallDescriptor.normalVector);

	// Ray parallel to wall
	if (rayWallDot == 0)
	{
		collisionDescriptor.collide = false;
		return collisionDescriptor;
	}

	float d = glm::dot(mapWallDescriptor.centerPosition - rayPosition, mapWallDescriptor.normalVector) / rayWallDot;

	glm::vec4 pointOfIntersection = d * rayDirection + rayPosition;

	// Ok, ray collides with plane. But is it inside the wall limit?

	float xLength = mapWallDescriptor.xLength / 2.0f;
	float yLength = mapWallDescriptor.yLength / 2.0f;
	float zLength = mapWallDescriptor.zLength / 2.0f;

	if (d > 0.0f)
		if (xLength == 0.0f || xLength >= fabs(pointOfIntersection.x - mapWallDescriptor.centerPosition.x))
			if (yLength == 0.0f || yLength >= fabs(pointOfIntersection.y - mapWallDescriptor.centerPosition.y))
				if (zLength == 0.0f || zLength >= fabs(pointOfIntersection.z - mapWallDescriptor.centerPosition.z))
				// Inside Limit!
				{
					collisionDescriptor.collide = true;
					collisionDescriptor.worldPosition = pointOfIntersection;
					return collisionDescriptor;
				}

	collisionDescriptor.collide = false;
	return collisionDescriptor;
}

PlayerCollisionDescriptor Collision::isRayCollidingWithBoundingBox(glm::vec4 rayPosition,
	glm::vec4 rayDirection, glm::vec4 rayXAxis, glm::vec4 rayYAxis, std::vector<BoundingShape>& boundingBox)
{
	// Normalize vectors
	rayDirection = glm::normalize(rayDirection);
	rayXAxis = glm::normalize(rayXAxis);
	rayYAxis = glm::normalize(rayYAxis);

	// Create View Ray Bounding Shape
	BoundingShape viewRayBoundingShape;

	vec3 viewRayVertices[8];

	float epsilon = 0.0001f;
	float rayLength = 30.0f;

	glm::vec4 prismVertices[8];

	prismVertices[0] = rayPosition + (0.0f * rayDirection - epsilon * rayXAxis - epsilon * rayYAxis);
	prismVertices[1] = rayPosition + (0.0f * rayDirection - epsilon * rayXAxis + epsilon * rayYAxis);
	prismVertices[2] = rayPosition + (0.0f * rayDirection + epsilon * rayXAxis - epsilon * rayYAxis);
	prismVertices[3] = rayPosition + (0.0f * rayDirection + epsilon * rayXAxis + epsilon * rayYAxis);
	prismVertices[4] = rayPosition + (rayLength * rayDirection - epsilon * rayXAxis - epsilon * rayYAxis);
	prismVertices[5] = rayPosition + (rayLength * rayDirection - epsilon * rayXAxis + epsilon * rayYAxis);
	prismVertices[6] = rayPosition + (rayLength * rayDirection + epsilon * rayXAxis - epsilon * rayYAxis);
	prismVertices[7] = rayPosition + (rayLength * rayDirection + epsilon * rayXAxis + epsilon * rayYAxis);

	viewRayVertices[0] = vec3(prismVertices[0].x, prismVertices[0].y, prismVertices[0].z);
	viewRayVertices[1] = vec3(prismVertices[1].x, prismVertices[1].y, prismVertices[1].z);
	viewRayVertices[2] = vec3(prismVertices[2].x, prismVertices[2].y, prismVertices[2].z);
	viewRayVertices[3] = vec3(prismVertices[3].x, prismVertices[3].y, prismVertices[3].z);
	viewRayVertices[4] = vec3(prismVertices[4].x, prismVertices[4].y, prismVertices[4].z);
	viewRayVertices[5] = vec3(prismVertices[5].x, prismVertices[5].y, prismVertices[5].z);
	viewRayVertices[6] = vec3(prismVertices[6].x, prismVertices[6].y, prismVertices[6].z);
	viewRayVertices[7] = vec3(prismVertices[7].x, prismVertices[7].y, prismVertices[7].z);

	viewRayBoundingShape.vertices = viewRayVertices;
	viewRayBoundingShape.num_vertices = 8;

	PlayerCollisionDescriptor playerCollisionDescriptor;

	// @TODO: check which part is closer to the fire to avoid bugs!!
	for (unsigned int i = 0; i < boundingBox.size(); ++i)
		if (gjk_collides(&boundingBox[i], &viewRayBoundingShape))
		{
			playerCollisionDescriptor.bodyPart = Player::getBoundingBoxBodyPart(i);
			playerCollisionDescriptor.collision.collide = true;
			// This is an approximation.
			playerCollisionDescriptor.collision.worldPosition = rayPosition;
			return playerCollisionDescriptor;
		}

	playerCollisionDescriptor.collision.collide = false;

	return playerCollisionDescriptor;
}
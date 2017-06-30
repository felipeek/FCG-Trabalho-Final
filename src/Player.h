#pragma once

#include "Entity.h"
#include "Camera.h"
#include "Map.h"
#include "PhysicsEngine\hphysics.h"
#include <queue>

namespace raw
{
	class Network;

	enum class PlayerBodyPart
	{
		HEAD,
		UPPERRIGHTARM,
		LOWERRIGHTARM,
		TORSO,
		UPPERLEFTARM,
		LOWERLEFTARM,
		RIGHTHAND,
		LEFTHAND,
		RIGHTTHIGH,
		LEFTTHIGH,
		RIGHTSHIN,
		LEFTSHIN,
		RIGHTFOOT,
		LEFTFOOT
	};

	struct ShotMark
	{
		Entity* entity;
		glm::vec4 color;
	};

	class Player : public Entity
	{
	public:
		Player(Model* model);
		~Player();

		int getHp() const;
		void setHp(int hp);
		void removeHp(int damage);
		int damage(PlayerBodyPart bodyPart);
		void setWallShotMarkColor(const glm::vec4& wallShotMarkColor);
		void createShotMark(glm::vec4 position, glm::vec4 color);
		void setRenderShotMarks(bool renderShotMarks);
		bool isRenderingShotMarks() const;
		void updateVelocityAndAccelerationBasedOnDirection(glm::vec4 direction);
		glm::vec4 getVelocity() const;
		glm::vec4 getAcceleration() const;
		Camera* getCamera();
		void renderGun(const Shader& shader, const Camera& camera, const std::vector<Light*>& lights, bool useNormalMap) const;
		void Player::renderShotMarks(const Shader& shader, const Camera& camera) const;
		void renderScreenImages(const Shader& shader) const;
		void setMovementInterpolationOn(bool movementInterpolationOn);
		void pushMovementInterpolation(const glm::vec4& finalPosition, const glm::vec4& velocity,
			const glm::vec4& acceleration);
		void interpolateMovement(float deltaTime);
		void update(Map* map, float deltaTime);
		void changeLookDirection(float xDifference, float yDifference, float speed);
		void changeLookDirection(const glm::vec4& lookDirection);
		glm::vec4 getLookDirection() const;
		glm::vec4 getPerpendicularDirection() const;
		void shoot(Player* secondPlayer, const std::vector<MapWallDescriptor>& mapWallDescriptors, Network* network);
		void shoot(Player* secondPlayer, const std::vector<MapWallDescriptor>& mapWallDescriptors);
		void shoot(const std::vector<MapWallDescriptor>& mapWallDescriptors);
		void startShootingAnimation();
		void startDamageAnimation();
		void jump();
		void setSlowMovement(bool slowMovement);

		// GJK & COLLISION
		const std::vector<BoundingShape>& getBoundingBoxInModelCoordinates() const;
		std::vector<BoundingShape>& getBoundingBoxInWorldCoordinates();
		static PlayerBodyPart getBoundingBoxBodyPart(unsigned int boundingBoxVectorIndex);
	private:
		void createCamera();
		void createBoundingBox();
		void createGun();
		void createAim();
		void setIsShootingAnimationOn(bool isShootingAnimationOn);
		void setIsDamageAnimationOn(bool isDamageAnimationOn);
		void updateMovement(Map* map, float deltaTime);
		glm::vec4 getNewPositionForMovement(Map* map, float deltaTime) const;
		void setMovementVelocity(const glm::vec4& movementVelocity);
		void setMovementAcceleration(const glm::vec4& movementAcceleration);

		const static int initialHp = 100;
		int hp;
		glm::vec4 movementVelocity;
		glm::vec4 movementAcceleration;

		Camera* camera;
		Entity* gun;					// 3D Gun
		Entity* aim;					// 2D Player Aim
		Entity* firstPersonGun;			// 2D Gun (Fixed in screen)
		Entity* firstPersonGunFiring;	// 2D Gun Firing (Fixed in screen)
		Entity* damageAnimationEntity;	// 2D damageAnimation

		// Movement Constants
		const static float playerMovementAccelerationLength;
		const static float maxVelocityLength;
		const static float maxSlowVelocityLength;
		const static float frictionStrength;

		// Shot Marks
		glm::vec4 wallShotMarkColor;
		std::vector<ShotMark> shotMarks;
		Model* shotMarkModel;
		bool bRenderShotMarks;

		// GJK & COLLISION
		Entity* boundingBoxEntity;
		Model* boundingBoxModel;
		std::vector<BoundingShape> boundingBoxInModelCoordinates;
		std::vector<BoundingShape> boundingBoxInWorldCoordinates;

		// Shooting Animation Related
		bool isShootingAnimationOn;
		const double shootingAnimationTotalTime = 0.05;
		double shootingAnimationTime;

		// Damage Animation Related
		bool isDamageAnimationOn;
		const double damageAnimationTotalTime = 0.7;
		double damageAnimationTime;

		// Movement Interpolation
		bool isMovementInterpolationOn;
		bool isInterpolationActive;
		glm::vec4 finalPosition;
		float currentVelocity;
		float nextVelocity;

		// Jump related
		bool isJumpOn;
		const glm::vec4 jumpGravityAcceleration = glm::vec4(0.0f, -9.8f, 0.0f, 0.0f);
		const glm::vec4 jumpInitialVelocity = glm::vec4(0.0f, 4.0f, 0.0f, 0.0f);
		glm::vec4 jumpVelocity;
		glm::vec4 jumpAcceleration;

		bool slowMovement;
	};
}
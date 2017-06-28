#pragma once

#include "Entity.h"
#include "Camera.h"
#include "Map.h"
#include "PhysicsEngine\hphysics.h"

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
		Camera* getCamera();
		void renderGun(const Shader& shader, const Camera& camera, const std::vector<Light*>& lights, bool useNormalMap) const;
		void Player::renderShotMarks(const Shader& shader, const Camera& camera) const;
		void renderScreenImages(const Shader& shader) const;
		void startMovementInterpolation(const glm::vec4& position);
		void interpolateMovement(float deltaTime);
		void update(float deltaTime);
		void changeLookDirection(float xDifference, float yDifference, float speed);
		void changeLookDirection(const glm::vec4& lookDirection);
		glm::vec4 getLookDirection() const;
		glm::vec4 getPerpendicularDirection() const;
		void shoot(Player* secondPlayer, const std::vector<MapWallDescriptor>& mapWallDescriptors, Network* network);
		void shoot(Player* secondPlayer, const std::vector<MapWallDescriptor>& mapWallDescriptors);
		void shoot(const std::vector<MapWallDescriptor>& mapWallDescriptors);
		void startShootingAnimation();
		void startDamageAnimation();

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
		void createShotMark(glm::vec4 position, glm::vec4 color);

		const static int initialHp = 100;
		int hp;

		Camera* camera;
		Entity* gun;					// 3D Gun
		Entity* aim;					// 2D Player Aim
		Entity* firstPersonGun;			// 2D Gun (Fixed in screen)
		Entity* firstPersonGunFiring;	// 2D Gun Firing (Fixed in screen)
		Entity* damageAnimationEntity;	// 2D damageAnimation

		// Shot Marks
		static glm::vec4 wallShotMarkColor;
		std::vector<ShotMark> shotMarks;
		Model* shotMarkModel;

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
		glm::vec4 movementInterpolationPosition;
	};
}
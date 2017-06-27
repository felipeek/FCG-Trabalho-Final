#pragma once

#include "Entity.h"
#include "Camera.h"
#include "PhysicsEngine\hphysics.h"

namespace raw
{
	enum class PlayerCollision
	{
		NONE,
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

	class Player : public Entity
	{
	public:
		Player(Model* model);
		Player(Model* model, Transform& transform);
		~Player();

		int getHp() const;
		void setHp(int hp);
		void removeHp(int damage);

		const Camera& getCamera() const;
		virtual void render(const Shader& shader, const Camera& camera, const std::vector<Light*>& lights, bool useNormalMap) const;
		void renderGun(const Shader& shader, const Camera& camera, const std::vector<Light*>& lights, bool useNormalMap) const;
		void renderAim(const Shader& shader) const;
		void update();
		void changeLookDirection(float xDifference, float yDifference);
		void changeLookDirection(const glm::vec4& lookDirection);
		glm::vec4 getLookDirection() const;
		glm::vec4 getPerpendicularDirection() const;
		void fire();

		// GJK & COLLISION
		PlayerCollision isViewRayCollidingWith(Player* player) const;
		std::vector<BoundingShape>& getBoundingBoxInModelCoordinates();
		std::vector<BoundingShape>& getBoundingBoxInWorldCoordinates();
	private:
		void initCamera();
		void initBoundingBox();
		void createGun();
		void createAim();
		void setIsFireAnimationOn(bool isFireAnimationOn);

		const static int initialHp = 100;
		int hp;

		Camera camera;
		Entity* gun;					// 3D Gun
		Entity* aim;					// 2D Player Aim
		Entity* firstPersonGun;			// 2D Gun (Fixed in screen)
		Entity* firstPersonGunFiring;	// 2D Gun Firing (Fixed in screen)

		// GJK & COLLISION
		PlayerCollision getCollisionBodyPart(unsigned int boundingBoxVectorIndex) const;
		Entity* boundingBoxEntity;
		Model* boundingBoxModel;
		std::vector<BoundingShape> boundingBoxInModelCoordinates;
		std::vector<BoundingShape> boundingBoxInWorldCoordinates;

		// Fire Related
		bool isFireAnimationOn;
		const double fireAnimationTime = 0.05;
		double fireTime;
	};
}
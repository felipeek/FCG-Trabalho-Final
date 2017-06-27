#pragma once

#include "Entity.h"
#include "Camera.h"
#include "PhysicsEngine\hphysics.h"

namespace raw
{
	class Player : public Entity
	{
	public:
		Player(Model* model);
		Player(Model* model, Transform& transform);
		~Player();
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
		bool isViewRayCollidingWith(Player* player) const;
		std::vector<BoundingShape>& getBoundingBoxInModelCoordinates();
		std::vector<BoundingShape>& getBoundingBoxInWorldCoordinates();
	private:
		void initCamera();
		void initBoundingBox();
		void createGun();
		void createAim();
		void setIsFireAnimationOn(bool isFireAnimationOn);

		Camera camera;
		Entity* gun;					// 3D Gun
		Entity* aim;					// 2D Player Aim
		Entity* firstPersonGun;			// 2D Gun (Fixed in screen)
		Entity* firstPersonGunFiring;	// 2D Gun Firing (Fixed in screen)

		// GJK & COLLISION
		Model* boundingBoxModel;
		std::vector<BoundingShape> boundingBoxInModelCoordinates;
		std::vector<BoundingShape> boundingBoxInWorldCoordinates;

		// Fire Related
		bool isFireAnimationOn;
		const double fireAnimationTime = 0.05;
		double fireTime;
	};
}
#pragma once

#include "Entity.h"
#include "Camera.h"

namespace raw
{
	class Player : public Entity
	{
	public:
		Player(Model* model);
		Player(Model* model, Transform& transform);
		~Player();
		const Camera& getCamera() const;
		void renderGun(const Shader& shader, const Camera& camera, const std::vector<Light*>& lights, bool useNormalMap) const;
		void renderAim(const Shader& shader) const;
		void update();
		void changeLookDirection(float xDifference, float yDifference);
		void changeLookDirection(const glm::vec4& lookDirection);
		glm::vec4 getLookDirection() const;
		glm::vec4 getPerpendicularDirection() const;
		void fire();
	private:
		void initCamera();
		void createGun();
		void createAim();
		void setIsFireAnimationOn(bool isFireAnimationOn);

		Camera camera;
		Entity* gun;
		Entity* aim;

		// Fire Related
		bool isFireAnimationOn;
		const double fireAnimationTime = 0.02;
		double fireTime;
	};
}
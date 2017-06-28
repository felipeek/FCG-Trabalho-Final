#pragma once

#include "PointLight.h"

namespace raw
{
	class StreetLamp : public PointLight
	{
	public:
		StreetLamp(glm::vec4 position, glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor);
		~StreetLamp();
		void render(const Shader& shaderForBase, const Shader& shaderForBulb, const Camera& camera,
			const std::vector<Light*>& lights, bool useNormalMap) const;
		Entity* getBaseEntity();
		Entity* getBulbEntity();
		glm::vec4 getWorldPosition() const;
		void setWorldPosition(const glm::vec4& position);
		glm::vec3 getWorldRotation() const;
		void setWorldRotation(const glm::vec3& rotation);
		glm::vec3 getWorldScale() const;
		void setWorldScale(const glm::vec3& scale);
	private:
		Entity* baseEntity;
		Entity* bulbEntity;

		static Model* getBaseModel();
		static Model* getBulbModel();
		static Model* baseModel;
		static Model* bulbModel;
	};
}
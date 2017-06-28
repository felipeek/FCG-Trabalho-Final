#pragma once

#include "Light.h"

namespace raw
{
	class Model;

	class SpotLight : public Light
	{
	public:
		SpotLight();
		SpotLight(glm::vec4 position, glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor);
		~SpotLight();
		void setPosition(const glm::vec4& position);
		glm::vec4 getPosition() const;
		void setAttenuation(LightAttenuation& attenuation);
		LightAttenuation getAttenuation() const;
		void setDirection(const glm::vec4& direction);
		glm::vec4 getDirection() const;
		void setInnerCutOffAngle(float cutOffAngle);
		float getInnerCutOffAngle() const;
		void setOuterCutOffAngle(float cutOffAngle);
		float getOuterCutOffAngle() const;
		virtual void updateUniforms(const Shader& shader, unsigned int arrayPosition) const;
		virtual LightType getType() const;
	private:
		glm::vec4 position;
		LightAttenuation attenuation;
		glm::vec4 direction;
		float innerCutOffAngle;
		float outerCutOffAngle;
	};

	class StreetSpotLight : public SpotLight
	{
	public:
		StreetSpotLight(glm::vec4 position, glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor);
		~StreetSpotLight();

		void render(const Shader& shaderForBase, const Shader& shaderForBulb, const Camera& camera,
			const std::vector<Light*>& lights, bool useNormalMap) const;
		Entity* getBodyEntity();
		Entity* getInsideEntity();
		Entity* getLampEntity();
		glm::vec4 getWorldPosition() const;
		void setWorldPosition(const glm::vec4& position);
		glm::vec3 getWorldRotation() const;
		void setWorldRotation(const glm::vec3& rotation);
		glm::vec3 getWorldScale() const;
		void setWorldScale(const glm::vec3& scale);
	private:
		Entity* bodyEntity;
		Entity* insideEntity;
		Entity* lampEntity;

		static Model* getBodyModel();
		static Model* getInsideModel();
		static Model* getLampModel();
		static Model* bodyModel;
		static Model* insideModel;
		static Model* lampModel;
	};
}
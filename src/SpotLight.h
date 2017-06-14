#pragma once

#include "Light.h"

namespace raw
{
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
}
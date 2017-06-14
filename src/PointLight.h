#pragma once

#include "Light.h"

namespace raw
{
	class PointLight : public Light
	{
	public:
		PointLight();
		PointLight(glm::vec4 position, glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor);
		~PointLight();
		void setPosition(const glm::vec4& position);
		glm::vec4 getPosition() const;
		void setAttenuation(LightAttenuation& attenuation);
		LightAttenuation getAttenuation() const;
		virtual void updateUniforms(const Shader& shader, unsigned int arrayPosition) const;
		virtual LightType getType() const;
	private:
		glm::vec4 position;
		LightAttenuation attenuation;
	};
}
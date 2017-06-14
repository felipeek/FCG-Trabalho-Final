#pragma once

#include "Light.h"

namespace raw
{
	class DirectionalLight : public Light
	{
	public:
		DirectionalLight();
		DirectionalLight(glm::vec4 direction, glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor);
		~DirectionalLight();
		void setDirection(const glm::vec4& direction);
		glm::vec4 getDirection() const;
		virtual void updateUniforms(const Shader& shader, unsigned int arrayPosition) const;
		virtual LightType getType() const;
	private:
		glm::vec4 direction;
	};
}
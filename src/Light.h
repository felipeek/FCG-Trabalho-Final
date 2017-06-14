#pragma once

#include "MathIncludes.h"
#include "Entity.h"

namespace raw
{
	enum LightType
	{
		LT_POINTLIGHT = 0,
		LT_SPOTLIGHT = 1,
		LT_DIRECTIONALLIGHT = 2
	};

	struct LightAttenuation
	{
		float constantTerm;
		float linearTerm;
		float quadraticTerm;
	};

	class Light
	{
	public:
		Light();
		Light(glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor);
		~Light();
		void setAmbientColor(const glm::vec4& ambientColor);
		glm::vec4 getAmbientColor() const;
		void setDiffuseColor(const glm::vec4& diffuseColor);
		glm::vec4 getDiffuseColor() const;
		void setSpecularColor(const glm::vec4& specularColor);
		glm::vec4 getSpecularColor() const;
		virtual void updateUniforms(const Shader& shader, unsigned int arrayPosition) const;
		virtual LightType getType() const = 0;
	protected:
		void getShaderLocationString(char* attribute, char* buffer, unsigned int arrayPosition) const;
	private:
		glm::vec4 position;
		glm::vec4 ambientColor;
		glm::vec4 diffuseColor;
		glm::vec4 specularColor;
		LightType type;
	};
}
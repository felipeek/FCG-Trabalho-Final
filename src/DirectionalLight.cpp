#include "DirectionalLight.h"

using namespace raw;

DirectionalLight::DirectionalLight() : Light()
{

}

DirectionalLight::DirectionalLight(glm::vec4 direction, glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor) :
	Light(ambientColor, diffuseColor, specularColor)
{
	this->direction = direction;
}

DirectionalLight::~DirectionalLight()
{
}

// Set the DirectionalLight direction, a vector which points to the direction the light is being emitted.
void DirectionalLight::setDirection(const glm::vec4& direction)
{
	this->direction = direction;
}

glm::vec4 DirectionalLight::getDirection() const
{
	return this->direction;
}

// Send all information about the light to the shader, so it can be used when rendering.
void DirectionalLight::updateUniforms(const Shader& shader, unsigned int arrayPosition) const
{
	Light::updateUniforms(shader, arrayPosition);

	char locationBuffer[1024];

	glm::vec4 directionalLightDirection = this->direction;

	this->getShaderLocationString("direction", locationBuffer, arrayPosition);
	GLuint directionalLightDirectionLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);

	glUniform4f(directionalLightDirectionLocation, directionalLightDirection.x,
		directionalLightDirection.y, directionalLightDirection.z, directionalLightDirection.w);
}

LightType DirectionalLight::getType() const
{
	return LT_DIRECTIONALLIGHT;
}
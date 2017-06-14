#include "PointLight.h"

using namespace raw;

PointLight::PointLight() : Light()
{

}

PointLight::PointLight(glm::vec4 position, glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor) :
	Light(ambientColor, diffuseColor, specularColor)
{
	this->position = position;
}

PointLight::~PointLight()
{
}

void PointLight::setPosition(const glm::vec4& position)
{
	this->position = position;
}

glm::vec4 PointLight::getPosition() const
{
	return this->position;
}

void PointLight::setAttenuation(LightAttenuation& attenuation)
{
	this->attenuation = attenuation;
}

LightAttenuation PointLight::getAttenuation() const
{
	return this->attenuation;
}

// Send all information about the light to the shader, so it can be used when rendering.
void PointLight::updateUniforms(const Shader& shader, unsigned int arrayPosition) const
{
	Light::updateUniforms(shader, arrayPosition);

	char locationBuffer[1024];

	glm::vec4 lightPosition = this->getPosition();
	float lightConstantTerm = this->attenuation.constantTerm;
	float lightLinearTerm = this->attenuation.linearTerm;
	float lightQuadraticTerm = this->attenuation.quadraticTerm;

	this->getShaderLocationString("position", locationBuffer, arrayPosition);
	GLuint lightPositionLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);
	this->getShaderLocationString("constantTerm", locationBuffer, arrayPosition);
	GLuint lightConstantTermLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);
	this->getShaderLocationString("linearTerm", locationBuffer, arrayPosition);
	GLuint lightLinearTermLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);
	this->getShaderLocationString("quadraticTerm", locationBuffer, arrayPosition);
	GLuint lightQuadraticTermLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);

	glUniform4f(lightPositionLocation, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);
	glUniform1f(lightConstantTermLocation, lightConstantTerm);
	glUniform1f(lightLinearTermLocation, lightLinearTerm);
	glUniform1f(lightQuadraticTermLocation, lightQuadraticTerm);
}

LightType PointLight::getType() const
{
	return LT_POINTLIGHT;
}
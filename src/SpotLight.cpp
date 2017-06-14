#include "SpotLight.h"

using namespace raw;

SpotLight::SpotLight() : Light()
{

}

SpotLight::SpotLight(glm::vec4 position, glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor) :
	Light(ambientColor, diffuseColor, specularColor)
{
	this->position = position;
}

SpotLight::~SpotLight()
{
}

void SpotLight::setPosition(const glm::vec4& position)
{
	this->position = position;
}

glm::vec4 SpotLight::getPosition() const
{
	return this->position;
}

void SpotLight::setAttenuation(LightAttenuation& attenuation)
{
	this->attenuation = attenuation;
}

LightAttenuation SpotLight::getAttenuation() const
{
	return this->attenuation;
}

void SpotLight::setDirection(const glm::vec4& direction)
{
	this->direction = direction;
}

glm::vec4 SpotLight::getDirection() const
{
	return this->direction;
}

void SpotLight::setInnerCutOffAngle(float innerCutOffAngle)
{
	this->innerCutOffAngle = innerCutOffAngle;
}

float SpotLight::getInnerCutOffAngle() const
{
	return this->innerCutOffAngle;
}

void SpotLight::setOuterCutOffAngle(float outerCutOffAngle)
{
	this->outerCutOffAngle = outerCutOffAngle;
}

float SpotLight::getOuterCutOffAngle() const
{
	return this->outerCutOffAngle;
}

// Send all information about the light to the shader, so it can be used when rendering.
void SpotLight::updateUniforms(const Shader& shader, unsigned int arrayPosition) const
{
	Light::updateUniforms(shader, arrayPosition);

	char locationBuffer[1024];

	glm::vec4 lightPosition = this->getPosition();
	float lightConstantTerm = this->attenuation.constantTerm;
	float lightLinearTerm = this->attenuation.linearTerm;
	float lightQuadraticTerm = this->attenuation.quadraticTerm;
	glm::vec4 spotLightDirection = this->direction;
	float spotLightInnerCutOffAngleCos = cosf(this->innerCutOffAngle);
	float spotLightOuterCutOffAngleCos = cosf(this->outerCutOffAngle);

	this->getShaderLocationString("position", locationBuffer, arrayPosition);
	GLuint lightPositionLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);
	this->getShaderLocationString("constantTerm", locationBuffer, arrayPosition);
	GLuint lightConstantTermLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);
	this->getShaderLocationString("linearTerm", locationBuffer, arrayPosition);
	GLuint lightLinearTermLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);
	this->getShaderLocationString("quadraticTerm", locationBuffer, arrayPosition);
	GLuint lightQuadraticTermLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);
	this->getShaderLocationString("direction", locationBuffer, arrayPosition);
	GLuint spotLightDirectionLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);
	this->getShaderLocationString("innerCutOffAngleCos", locationBuffer, arrayPosition);
	GLuint spotLightInnerCutOffAngleCosTermLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);
	this->getShaderLocationString("outerCutOffAngleCos", locationBuffer, arrayPosition);
	GLuint spotLightOuterCutOffAngleCosTermLocation = glGetUniformLocation(shader.getProgram(), locationBuffer);

	glUniform4f(lightPositionLocation, lightPosition.x, lightPosition.y, lightPosition.z, lightPosition.w);
	glUniform1f(lightConstantTermLocation, lightConstantTerm);
	glUniform1f(lightLinearTermLocation, lightLinearTerm);
	glUniform1f(lightQuadraticTermLocation, lightQuadraticTerm);
	glUniform4f(spotLightDirectionLocation, spotLightDirection.x, spotLightDirection.y, spotLightDirection.z, spotLightDirection.w);
	glUniform1f(spotLightInnerCutOffAngleCosTermLocation, spotLightInnerCutOffAngleCos);
	glUniform1f(spotLightOuterCutOffAngleCosTermLocation, spotLightOuterCutOffAngleCos);
}

LightType SpotLight::getType() const
{
	return LT_SPOTLIGHT;
}
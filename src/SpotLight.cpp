#include "SpotLight.h"
#include "Model.h"

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

/* StreetSpotLight methods */

StreetSpotLight::StreetSpotLight(glm::vec4 position, glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor) :
	SpotLight(position, ambientColor, diffuseColor, specularColor)
{
	Model* bodyModel = StreetSpotLight::getBodyModel();
	this->bodyEntity = new Entity(bodyModel);
	this->bodyEntity->getTransform().setWorldPosition(position);
	this->bodyEntity->getTransform().setWorldScale(glm::vec3(0.01f, 0.01f, 0.01f));

	Model* insideModel = StreetSpotLight::getInsideModel();
	this->insideEntity = new Entity(insideModel);
	this->insideEntity->getTransform().setWorldPosition(glm::vec4(position));
	this->insideEntity->getTransform().setWorldScale(glm::vec3(0.01f, 0.01f, 0.01f));

	Model* lampModel = StreetSpotLight::getLampModel();
	this->lampEntity = new Entity(lampModel);
	this->lampEntity->getTransform().setWorldPosition(glm::vec4(position));
	this->lampEntity->getTransform().setWorldScale(glm::vec3(0.01f, 0.01f, 0.01f));

}

StreetSpotLight::~StreetSpotLight()
{
	delete this->bodyEntity;
	delete this->insideEntity;
	delete this->lampEntity;
}

void StreetSpotLight::render(const Shader& shaderForBase, const Shader& shaderForBulb, const Camera& camera,
	const std::vector<Light*>& lights, bool useNormalMap) const
{
	const static glm::vec4 bulbColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	this->bodyEntity->render(shaderForBase, camera, lights, useNormalMap);

	if (this->isOn())
	{
		this->insideEntity->render(shaderForBulb, camera, bulbColor);
		this->lampEntity->render(shaderForBulb, camera, bulbColor);
	}
	else
	{
		this->insideEntity->render(shaderForBase, camera, lights, useNormalMap);
		this->lampEntity->render(shaderForBase, camera, lights, useNormalMap);
	}
}

Entity* StreetSpotLight::getBodyEntity()
{
	return this->bodyEntity;
}

Entity* StreetSpotLight::getInsideEntity()
{
	return this->insideEntity;
}

Entity* StreetSpotLight::getLampEntity()
{
	return this->lampEntity;
}

glm::vec4 StreetSpotLight::getWorldPosition() const
{
	return this->getPosition();
}

void StreetSpotLight::setWorldPosition(const glm::vec4& position)
{
	this->setPosition(position);
	this->bodyEntity->getTransform().setWorldPosition(position);
	this->insideEntity->getTransform().setWorldPosition(position);
	this->lampEntity->getTransform().setWorldPosition(position);
}

glm::vec3 StreetSpotLight::getWorldRotation() const
{
	return this->bodyEntity->getTransform().getWorldRotation();
}

void StreetSpotLight::setWorldRotation(const glm::vec3& rotation)
{
	this->bodyEntity->getTransform().setWorldRotation(rotation);
	this->insideEntity->getTransform().setWorldRotation(rotation);
	this->lampEntity->getTransform().setWorldRotation(rotation);
}

glm::vec3 StreetSpotLight::getWorldScale() const
{
	return this->bodyEntity->getTransform().getWorldScale();
}

void StreetSpotLight::setWorldScale(const glm::vec3& scale)
{
	this->bodyEntity->getTransform().setWorldScale(scale);
	this->insideEntity->getTransform().setWorldScale(scale);
	this->lampEntity->getTransform().setWorldScale(scale);
}

Model* StreetSpotLight::bodyModel;
Model* StreetSpotLight::insideModel;
Model* StreetSpotLight::lampModel;

Model* StreetSpotLight::getBodyModel()
{
	if (StreetSpotLight::bodyModel == 0)
	{
		StreetSpotLight::bodyModel = new Model(".\\res\\art\\spotlight\\body.obj");
		StreetSpotLight::bodyModel->setDiffuseMapOfAllMeshes(Texture::load(".\\res\\art\\spotlight\\spotlightbody.png"));
		StreetSpotLight::bodyModel->setSpecularMapOfAllMeshes(Texture::load(".\\res\\art\\black.png"));
	}

	return StreetSpotLight::bodyModel;
}

Model* StreetSpotLight::getInsideModel()
{
	if (StreetSpotLight::insideModel == 0)
	{
		StreetSpotLight::insideModel = new Model(".\\res\\art\\spotlight\\inside.obj");
		StreetSpotLight::insideModel->setDiffuseMapOfAllMeshes(Texture::load(".\\res\\art\\spotlight\\spotlightinside.png"));
		StreetSpotLight::insideModel->setSpecularMapOfAllMeshes(Texture::load(".\\res\\art\\black.png"));
	}

	return StreetSpotLight::insideModel;
}

Model* StreetSpotLight::getLampModel()
{
	if (StreetSpotLight::lampModel == 0)
	{
		StreetSpotLight::lampModel = new Model(".\\res\\art\\spotlight\\lamp.obj");
		StreetSpotLight::lampModel->setDiffuseMapOfAllMeshes(Texture::load(".\\res\\art\\spotlight\\spotlightlamp.png"));
		StreetSpotLight::lampModel->setSpecularMapOfAllMeshes(Texture::load(".\\res\\art\\black.png"));
	}

	return StreetSpotLight::lampModel;
}
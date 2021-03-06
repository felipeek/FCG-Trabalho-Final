#include "StreetLamp.h"
#include "Model.h"

using namespace raw;

StreetLamp::StreetLamp(glm::vec4 position, glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor) :
	PointLight(position, ambientColor, diffuseColor, specularColor)
{
	this->baseEntity = new Entity(StreetLamp::getBaseModel());
	this->baseEntity->getTransform().setWorldPosition(position);
	this->baseEntity->getTransform().setWorldScale(glm::vec3(0.01f, 0.01f, 0.01f));

	this->bulbEntity = new Entity(StreetLamp::getBulbModel());
	this->bulbEntity->getTransform().setWorldPosition(glm::vec4(position));
	this->bulbEntity->getTransform().setWorldScale(glm::vec3(0.01f, 0.01f, 0.01f));
}

StreetLamp::~StreetLamp()
{
	delete this->baseEntity;
	delete this->bulbEntity;
}

void StreetLamp::render(const Shader& shaderForBase, const Shader& shaderForBulb, const Camera& camera,
	const std::vector<Light*>& lights, bool useNormalMap) const
{
	glm::vec4 bulbColor = this->isOn() ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.1f, 0.1f, 0.1f, 0.1f);
	this->baseEntity->render(shaderForBase, camera, lights, useNormalMap);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	this->bulbEntity->render(shaderForBulb, camera, bulbColor);
	glDisable(GL_BLEND);
}

Entity* StreetLamp::getBaseEntity()
{
	return this->baseEntity;
}

Entity* StreetLamp::getBulbEntity()
{
	return this->bulbEntity;
}

glm::vec4 StreetLamp::getWorldPosition() const
{
	return this->getPosition();
}

void StreetLamp::setWorldPosition(const glm::vec4& position)
{
	this->setPosition(position);
	this->baseEntity->getTransform().setWorldPosition(position);
	this->bulbEntity->getTransform().setWorldPosition(position);
}

glm::vec3 StreetLamp::getWorldRotation() const
{
	return this->baseEntity->getTransform().getWorldRotation();
}

void StreetLamp::setWorldRotation(const glm::vec3& rotation)
{
	this->baseEntity->getTransform().setWorldRotation(rotation);
	this->bulbEntity->getTransform().setWorldRotation(rotation);
}

glm::vec3 StreetLamp::getWorldScale() const
{
	return this->baseEntity->getTransform().getWorldScale();
}

void StreetLamp::setWorldScale(const glm::vec3& scale)
{
	this->baseEntity->getTransform().setWorldScale(scale);
	this->bulbEntity->getTransform().setWorldScale(scale);
}

Model* StreetLamp::baseModel;
Model* StreetLamp::bulbModel;

Model* StreetLamp::getBaseModel()
{
	if (StreetLamp::baseModel == 0)
	{
		StreetLamp::baseModel = new Model(".\\res\\art\\base_lamp.obj");
		StreetLamp::baseModel->setDiffuseMapOfAllMeshes(Texture::load(".\\res\\art\\metal_diffuse.jpg"));
		StreetLamp::baseModel->setSpecularMapOfAllMeshes(Texture::load(".\\res\\art\\meta_specular.jpg"));
		StreetLamp::baseModel->setNormalMapOfAllMeshes(Texture::load(".\\res\\art\\metal_normal.jpg"));
	}

	return StreetLamp::baseModel;
}

Model* StreetLamp::getBulbModel()
{
	if (StreetLamp::bulbModel == 0)
	{
		StreetLamp::bulbModel = new Model(".\\res\\art\\bulb_lamp.obj");
	}

	return StreetLamp::bulbModel;
}
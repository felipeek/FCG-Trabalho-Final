#include "StreetLamp.h"
#include "Model.h"

using namespace raw;

StreetLamp::StreetLamp(glm::vec4 position, glm::vec4 ambientColor, glm::vec4 diffuseColor, glm::vec4 specularColor) :
	PointLight(position, ambientColor, diffuseColor, specularColor)
{
	Model* baseModel = new Model(".\\res\\art\\base_lamp.obj");
	this->baseEntity = new Entity(baseModel);
	this->baseEntity->getTransform().setWorldPosition(position);
	this->baseEntity->getTransform().setWorldScale(glm::vec3(0.01f, 0.01f, 0.01f));
	this->baseEntity->getModel()->setDiffuseMapOfAllMeshes(Texture::load(".\\res\\art\\metal_diffuse.jpg"));
	this->baseEntity->getModel()->setSpecularMapOfAllMeshes(Texture::load(".\\res\\art\\meta_specular.jpg"));
	this->baseEntity->getModel()->setNormalMapOfAllMeshes(Texture::load(".\\res\\art\\metal_normal.jpg"));

	Model* bulbModel = new Model(".\\res\\art\\bulb_lamp.obj");
	this->bulbEntity = new Entity(bulbModel);
	this->bulbEntity->getTransform().setWorldPosition(glm::vec4(position));
	this->bulbEntity->getTransform().setWorldScale(glm::vec3(0.01f, 0.01f, 0.01f));
}

StreetLamp::~StreetLamp()
{
	delete this->baseEntity->getModel();
	delete this->bulbEntity->getModel();
	delete this->baseEntity;
	delete this->bulbEntity;
}

void StreetLamp::render(const Shader& shaderForBase, const Shader& shaderForBulb, const Camera& camera,
	const std::vector<Light*>& lights, bool useNormalMap) const
{
	const static glm::vec4 bulbColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->baseEntity->render(shaderForBase, camera, lights, useNormalMap);
	this->bulbEntity->render(shaderForBulb, camera, bulbColor);
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
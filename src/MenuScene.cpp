#include "MenuScene.h"

#include "Skybox.h"
#include "Camera.h"
#include "Light.h"
#include "DirectionalLight.h"

using namespace raw;

MenuScene::MenuScene()
{
	// Create sky
	this->skybox = new Skybox();

	// Create Map
	this->map = new Map(".\\res\\map\\map.png");
	Model* mapModel = this->map->generateMapModel();
	this->models.push_back(mapModel);
	Entity* mapEntity = new Entity(mapModel);
	this->entities.push_back(mapEntity);

	// Create Shaders
	this->phongShader = new Shader(ShaderType::PHONG);
	this->skyboxShader = new Shader(ShaderType::SKYBOX);

	// Create Cameras
	glm::vec4 lookAtCameraInitialPosition = glm::vec4(0.46f, 4.08f, 15.65f, 1.0f);
	glm::vec4 lookAtCameraInitialUpVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 lookAtCameraInitialViewVector = glm::vec4(1.0039f, -0.5f, 1.0f, 0.0f);
	this->lookAtCamera = new PerspectiveCamera(lookAtCameraInitialPosition, lookAtCameraInitialUpVector,
		lookAtCameraInitialViewVector);

	// Create Lights
	glm::vec4 ambientLight = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 diffuseLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 specularLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	LightAttenuation attenuation = {
		1.0f,		// Constant Term
		0.0014f,		// Linear Term
		0.000007f		// Quadratic Term
	};

	// DIRECTIONAL LIGHT
	glm::vec4 dlDirection = glm::vec4(-0.2f, -1.0f, -0.3f, 0.0f);
	glm::vec4 dAmbientLight = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	glm::vec4 dDiffuseLight = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	glm::vec4 dSpecularLight = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	DirectionalLight* directionalLight = new DirectionalLight(dlDirection, dAmbientLight, dDiffuseLight,
		dSpecularLight);
	this->lights.push_back(directionalLight);
}

MenuScene::~MenuScene()
{
	// Delete map
	delete this->map;

	// Delete Shaders
	delete this->phongShader;
	delete this->skyboxShader;

	// Destroy Lights
	for (unsigned int i = 0; i < this->lights.size(); ++i)
		delete this->lights[i];

	// Destroy Models
	for (unsigned int i = 0; i < this->models.size(); ++i)
		delete this->models[i];

	// Destroy Entities
	for (unsigned int i = 0; i < this->entities.size(); ++i)
		delete this->entities[i];
}

void MenuScene::render() const
{
	// Render skybox
	this->skybox->render(*skyboxShader, *lookAtCamera);

	// Render all entities
	for (unsigned int i = 0; i < this->entities.size(); ++i)
		this->entities[i]->render(*phongShader, *lookAtCamera, this->lights, false);
}

void MenuScene::update(float deltaTime)
{
	// Update LookAt Camera
	static const glm::vec4 lookPosition = glm::vec4(10.0f, 0.0f, 10.0f, 1.0f);
	glm::vec4 newLookAtPosition = this->lookAtCamera->getPosition();
	
	// Camera Movement Function
	static const float cameraSpeed = 0.02f;
	static const float delta = 40.0f;
	static float t = 0;
	static bool incrementingT = true;

	static float curveHeight = 5.0f;

	glm::vec4 controlPoints[5] = {
		{ 0.0f, curveHeight, 0.0f, 1.0f },
		{ this->map->getMapXSize() + delta, curveHeight + 20.0f, -delta, 1.0f },
		{ this->map->getMapXSize() + delta, curveHeight + 20.0f, this->map->getMapZSize() + delta, 1.0f },
		{ -delta, curveHeight, this->map->getMapZSize() + delta, 1.0f },
		{ 0.0f, curveHeight, 0.0f, 1.0f }
	};

	float c0 = (1 - t) * (1 - t) * (1 - t) * (1 - t);
	float c1 = 4 * t * (1 - t) * (1 - t) * (1 - t);
	float c2 = 6 * t * t * (1 - t) * (1 - t);
	float c3 = 4 * t * t * t * (1 - t);
	float c4 = t * t * t * t;

	newLookAtPosition = c0 * controlPoints[0] + c1 * controlPoints[1] + c2 * controlPoints[2]
		+ c3 * controlPoints[3] + c4 * controlPoints[4];

	t += cameraSpeed * deltaTime;
	if (t >= 1.0f)
		t = 0.0f;

	this->lookAtCamera->setPosition(newLookAtPosition);
	this->lookAtCamera->setViewVector(lookPosition - newLookAtPosition);
}
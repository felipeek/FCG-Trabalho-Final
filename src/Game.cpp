#include "Game.h"
#include "Model.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "Map.h"
#include "GLFW\glfw3.h"

using namespace raw;

Game::Game()
{
	
}

Game::~Game()
{

}

void Game::init()
{
	// Create Map
	this->map = new Map(".\\res\\map\\map.png");

	// Create Shaders
	this->createShaders();

	// Create Camera
	glm::vec4 initialPosition = glm::vec4(1.2f, 0.3f, 1.2f, 1.0f);
	this->camera = new Camera(initialPosition, glm::vec4(0.0f, 1.0f, 0.0f, 0.0f), glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));

	// Create Lights
	this->createLights();

	// Create Entities
	this->createEntities();
}

void Game::render() const
{
	switch (this->shaderType)
	{
		case ShaderType::BASIC:
			for (unsigned int i = 0; i < this->entities.size(); ++i)
				this->entities[i]->render(*this->basicShader, *this->camera, this->lights);
			break;
		case ShaderType::FLAT:
			for (unsigned int i = 0; i < this->entities.size(); ++i)
				this->entities[i]->render(*this->flatShader, *this->camera, this->lights);
			break;
		case ShaderType::GOURAD:
			for (unsigned int i = 0; i < this->entities.size(); ++i)
				this->entities[i]->render(*this->gouradShader, *this->camera, this->lights);
			break;
		case ShaderType::PHONG:
			for (unsigned int i = 0; i < this->entities.size(); ++i)
				this->entities[i]->render(*this->phongShader, *this->camera, this->lights);
			break;
	}
}

void Game::update(float deltaTime)
{
	if (this->boundSpotLight)
	{
		this->boundSpotLight->setPosition(camera->getPosition());
		this->boundSpotLight->setDirection(camera->getViewVector());
	}
}

void Game::destroy()
{
	// Destroy map
	delete this->map;
	
	// Destroy Shaders
	delete this->basicShader;
	delete this->phongShader;
	delete this->gouradShader;
	delete this->flatShader;

	// Destroy Lights
	for (unsigned int i = 0; i < this->lights.size(); ++i)
		delete this->lights[i];

	// Destroy Models
	for (unsigned int i = 0; i < this->entities.size(); ++i)
	{
		delete this->entities[i]->getModel();	// Delete Model
		delete this->entities[i];				// Delete Entity
	}

	this->map = 0;
	this->basicShader = 0;
	this->phongShader = 0;
	this->gouradShader = 0;
	this->flatShader = 0;
	this->lights.clear();
	this->entities.clear();

	// Destroy Textures
	// @TODO: destroyAll can't destroy default diffuseMap and default specularMap,
	// because it should be possible to destroy and re-create Game class.
	// Texture::destroyAll();
}

void Game::processMouseChange(double xPos, double yPos)
{
	static double xPosOld, yPosOld;
	static bool firstTime = true;
	static const float cameraMouseSpeed = 0.005f;

	if (!firstTime)
	{
		double xDifference = xPos - xPosOld;
		double yDifference = yPos - yPosOld;

		this->camera->incPitch(-cameraMouseSpeed * (float)xDifference);
		this->camera->incYaw(cameraMouseSpeed * (float)yDifference);
	}
	else
		firstTime = false;

	xPosOld = xPos;
	yPosOld = yPos;
}

void Game::processInput(bool* keyState, float deltaTime)
{
	static const float cameraSpeed = 5.0f;
	static const float rotationSpeed = 5.0f;

	if (this->camera)
	{
		if (keyState[GLFW_KEY_W])
		{
			glm::vec4 cameraPos = this->camera->getPosition();
			glm::vec4 movementDirection = glm::normalize(this->camera->getViewVector());
			this->camera->setPosition(this->getNewPositionForMovement(cameraPos, movementDirection));
		}
		if (keyState[GLFW_KEY_S])
		{
			glm::vec4 cameraPos = this->camera->getPosition();
			glm::vec4 movementDirection = -glm::normalize(this->camera->getViewVector());
			this->camera->setPosition(this->getNewPositionForMovement(cameraPos, movementDirection));
		}
		if (keyState[GLFW_KEY_A])
		{
			glm::vec4 cameraPos = this->camera->getPosition();
			glm::vec4 movementDirection = -glm::normalize(this->camera->getXAxis());
			this->camera->setPosition(this->getNewPositionForMovement(cameraPos, movementDirection));
		}
		if (keyState[GLFW_KEY_D])
		{
			glm::vec4 cameraPos = this->camera->getPosition();
			glm::vec4 movementDirection = glm::normalize(this->camera->getXAxis());
			this->camera->setPosition(this->getNewPositionForMovement(cameraPos, movementDirection));
		}
	}

	if (this->boundEntity)
	{
		if (keyState[GLFW_KEY_X])
			boundEntity->getTransform().incRotX(rotationSpeed * deltaTime);
		if (keyState[GLFW_KEY_Y])
			boundEntity->getTransform().incRotY(rotationSpeed * deltaTime);
		if (keyState[GLFW_KEY_Z])
			boundEntity->getTransform().incRotZ(rotationSpeed * deltaTime);
	}

	if (this->boundPointLight)
	{
		if (keyState[GLFW_KEY_UP])
			boundPointLight->setPosition(boundPointLight->getPosition() + deltaTime * 5.0f * camera->getViewVector());
	}

	if (keyState[GLFW_KEY_B])
	{
		shaderType = raw::ShaderType::BASIC;
		keyState[GLFW_KEY_B] = false;					// Force false to only compute one time.
	}
	if (keyState[GLFW_KEY_F])
	{
		shaderType = raw::ShaderType::FLAT;
		keyState[GLFW_KEY_F] = false;					// Force false to only compute one time.
	}
	if (keyState[GLFW_KEY_G])
	{
		shaderType = raw::ShaderType::GOURAD;
		keyState[GLFW_KEY_G] = false;					// Force false to only compute one time.
	}
	if (keyState[GLFW_KEY_P])
	{
		shaderType = raw::ShaderType::PHONG;
		keyState[GLFW_KEY_P] = false;					// Force false to only compute one time.
	}
}

void Game::createShaders()
{
	this->basicShader = new Shader(ShaderType::BASIC);
	this->phongShader = new Shader(ShaderType::PHONG);
	this->gouradShader = new Shader(ShaderType::GOURAD);
	this->flatShader = new Shader(ShaderType::FLAT);
	this->shaderType = ShaderType::PHONG;
}

void Game::createLights()
{
	glm::vec4 ambientLight = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	glm::vec4 diffuseLight = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	glm::vec4 specularLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	LightAttenuation attenuation = {
		1.0f,		// Constant Term
		0.14f,		// Linear Term
		0.07f		// Quadratic Term
	};

	// POINT LIGHT
	glm::vec4 plPosition = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
	PointLight* pointLight = new PointLight(plPosition, ambientLight, diffuseLight, specularLight);
	pointLight->setAttenuation(attenuation);
	this->boundPointLight = pointLight;	// @TEMPORARY

	// SPOT LIGHT
	glm::vec4 slPosition = glm::vec4(0, 0, 1, 1);
	SpotLight* spotLight = new SpotLight(slPosition, ambientLight, diffuseLight, specularLight);
	spotLight->setAttenuation(attenuation);
	spotLight->setInnerCutOffAngle(glm::radians(12.5f));
	spotLight->setOuterCutOffAngle(glm::radians(17.5f));
	spotLight->setDirection(glm::vec4(0, 0, -1, 0));
	this->boundSpotLight = spotLight;	// @TEMPORARY

	// DIRECTIONAL LIGHT
	glm::vec4 dlDirection = glm::vec4(-0.2f, -1.0f, -0.3f, 0.0f);
	DirectionalLight* directionalLight = new DirectionalLight(dlDirection, ambientLight, diffuseLight,
		specularLight);

	// VECTOR CREATION
	//this->lights.push_back(pointLight);
	this->lights.push_back(spotLight);
	this->lights.push_back(directionalLight);
}

Entity* e;

void Game::createEntities()
{
	// CREATE A CUBE (cube.obj)
	Texture* diffuseMap = Texture::load(".\\res\\art\\green.png");
	Model* cubeModel = new Model(".\\res\\art\\cube.obj");
	Entity* cubeEntity = new Entity(cubeModel);
	cubeModel->setDiffuseMapOfAllMeshes(diffuseMap);
	cubeEntity->getTransform().setWorldPosition(glm::vec4(-8.0f, 0.0f, 9.0f, 1.0f));
	cubeEntity->getTransform().setWorldRotation(glm::vec3(0.35f, 0.77f, 0.12f));

	// CREATE COMPLEX MODEL (nanosuit.obj)
	//Model* complexModel = new Model(".\\res\\art\\nanosuit\\nanosuit.obj");
	//Entity* complexEntity = new Entity(complexModel);
	//complexEntity->getTransform().setWorldPosition(glm::vec4(-5.0f, 0.0f, 0.0f, 1.0f));
	//
	//// CREATE BUNNY MODEL (bunny.obj)
	//Texture* bunnyDiffuseMap = Texture::load(".\\res\\art\\blue2.png");
	//Texture* bunnySpecularMap = Texture::load(".\\res\\art\\specBunny.png");
	//Model* bunnyModel = new Model(".\\res\\art\\bunny.obj");
	//bunnyModel->setDiffuseMapOfAllMeshes(bunnyDiffuseMap);
	//bunnyModel->setSpecularMapOfAllMeshes(bunnySpecularMap);
	//bunnyModel->setSpecularShinenessOfAllMeshes(32.0f);
	//Entity* bunnyEntity = new Entity(bunnyModel);
	//bunnyEntity->getTransform().setWorldPosition(glm::vec4(0.0f, 0.0f, -10.0f, 1.0f));
	//this->boundEntity = bunnyEntity;		// @TEMPORARY
	//
	//// CREATE TREE MODEL (arvore.obj)
	//Texture* arvoreTexture = Texture::load(".\\res\\art\\arvore_tex.png");
	//Model* arvoreModel = new Model(".\\res\\art\\arvore.obj");
	//Entity* arvoreEntity = new Entity(arvoreModel);
	//arvoreModel->setDiffuseMapOfAllMeshes(arvoreTexture);
	//arvoreEntity->getTransform().setWorldPosition(glm::vec4(0.0f, 0.0f, 6.0f, 1.0f));
	//
	//// CREATE SPHERE MODEL (sphere.obj)
	//Texture* sphereTexture = Texture::load(".\\res\\art\\blue2.png");
	//Model* sphereModel = new Model(".\\res\\art\\sphere.obj");
	//Entity* sphereEntity = new Entity(sphereModel);
	//sphereModel->getMeshes()[0]->setDiffuseMap(sphereTexture);
	//arvoreEntity->getTransform().setWorldPosition(glm::vec4(1.0f, 0.0f, 10.0f, 1.0f));
	//
	//// CREATE QUAD
	//Texture* quadTexture = Texture::load(".\\res\\art\\smile.png");
	//Quad* quad = new Quad(quadTexture);
	//std::vector<Mesh*> quadMeshes = std::vector<Mesh*>({ quad });
	//Model* quadModel = new Model(quadMeshes);
	//Entity* quadEntity = new Entity(quadModel);
	//quadEntity->getTransform().setWorldPosition(glm::vec4(10.5f, 0.0f, 10.0f, 1.0f));

	// temporary
	Model* mapModel = this->map->generateMapModel();
	Entity* mapEntity = new Entity(mapModel);

	// VECTOR CREATION
	this->entities.push_back(cubeEntity);
	//this->entities.push_back(complexEntity);
	//this->entities.push_back(bunnyEntity);
	//this->entities.push_back(arvoreEntity);
	//this->entities.push_back(sphereEntity);
	//this->entities.push_back(quadEntity);
	this->entities.push_back(mapEntity);
}

// Returns a new position based on an original position and a movement direction.
// If there are no collisions, the new position will just be the original position + movementSpeed * direction
// If there is a collision, instead of just returning the original position (no movement), try to return
// the best position based on the map. (Try to slide when colliding with walls).
// Method will only work for map where walls are parallel to X and Z axes.z
glm::vec4 Game::getNewPositionForMovement(glm::vec4 position, glm::vec4 direction)
{
	static const float movementSpeed = 0.05f;
	glm::vec4 newPos = position + movementSpeed * direction;
	newPos.y = position.y;

	glm::vec4 auxiliarVector;

	auxiliarVector = position;
	auxiliarVector.x = newPos.x;
	TerrainType terrainType = this->map->getTerrainTypeForMovement(auxiliarVector);
	if (terrainType != TerrainType::FREE)
		newPos.x = position.x;

	auxiliarVector = position;
	auxiliarVector.z = newPos.z;
	terrainType = this->map->getTerrainTypeForMovement(auxiliarVector);
	if (terrainType != TerrainType::FREE)
		newPos.z = position.z;

	return newPos;
}
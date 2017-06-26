#include "Game.h"
#include "Model.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "Map.h"
#include "StaticModels.h"
#include "GLFW\glfw3.h"
#include "Network.h"
#include "StreetLamp.h"
#include "Skybox.h"
#include <iostream>

using namespace raw;

// @TEMPORARY
float x, y, z, xx, yy, zz;

Game::Game()
{
	
}

Game::~Game()
{
}


void Game::init(bool singlePlayer)
{
	this->singlePlayer = singlePlayer;

	// Init Network
	if (!this->singlePlayer)
		this->network = new Network(this, "186.215.50.40", 8888);

	// Create sky
	this->skybox = new Skybox();

	// Create Map
	this->map = new Map(".\\res\\map\\map.png");

	// Create Shaders
	this->createShaders();

	// Create Cameras
	this->createCameras();

	// CREATE A CUBE (cube.obj)
	Texture* diffuseMap = Texture::load(".\\res\\art\\green.png");
	Model* cubeModel = new Model(std::vector<Mesh*>({StaticModels::getCubeMesh(1.0f, diffuseMap, 0, 0, 32.0f)}));

	// Create Player
	this->player = new Player(cubeModel);
	this->player->getTransform().setWorldScale(glm::vec3(0.5f, 0.5f, 0.5f));

	// Create Second Player
	if (!this->singlePlayer)
	{
		this->secondPlayer = new Player(cubeModel);
		this->secondPlayer->getTransform().setWorldScale(glm::vec3(0.5f, 0.5f, 0.5f));
	}

	// Create Lights
	this->createLights();

	// Create Entities
	this->createEntities();

	this->useNormalMap = true;
}

void Game::render() const
{
	const Camera* selectedCamera = this->getSelectedCamera();

	// First Pass
	Shader* shaderToUse;

	switch (this->shaderType)
	{
		case ShaderType::FIXED:
			shaderToUse = this->fixedShader;
			break;
		case ShaderType::BASIC:
			shaderToUse = this->basicShader;
			break;
		case ShaderType::TEXTURE:
			shaderToUse = this->textureShader;
			break;
		case ShaderType::FLAT:
			shaderToUse = this->flatShader;
			break;
		case ShaderType::GOURAD:
			shaderToUse = this->gouradShader;
			break;
		case ShaderType::PHONG:
			shaderToUse = this->phongShader;
			break;
		case ShaderType::SKYBOX:
			shaderToUse = this->skyboxShader;
			break;
		default:
			shaderToUse = this->phongShader;
			break;
	}

	this->skybox->render(*skyboxShader, *selectedCamera);

	for (unsigned int i = 0; i < this->entities.size(); ++i)
		this->entities[i]->render(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);

	for (unsigned int i = 0; i < this->streetLamps.size(); ++i)
		this->streetLamps[i]->render(*shaderToUse, *basicShader, *selectedCamera, this->lights, this->useNormalMap);

	// Avoid rendering the player when the player camera is being used to not block the camera.
	if (this->selectedCamera != CameraType::PLAYER)
		this->player->render(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);

	if (!this->singlePlayer)
	{
		this->secondPlayer->render(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);
		this->secondPlayer->renderGun(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);
	}

	// If player camera is being used, clear the depth buffer to render the gun. This way, the gun will always appear
	// on the screen, regardless if it is behind the wall, for example.
	if (this->selectedCamera == CameraType::PLAYER)
		glClear(GL_DEPTH_BUFFER_BIT);

	// Second Pass
	this->player->renderGun(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);

	// Should only render AIM when Player Camera is being used!
	if (this->selectedCamera == CameraType::PLAYER)
		this->player->renderAim(*fixedShader);
}

void Game::update(float deltaTime)
{
	static const int networkUpdatesPerSecond = 10;
	static double lastTime;
	const Camera& playerCamera = this->player->getCamera();

	if (this->boundSpotLight)
	{
		this->boundSpotLight->setPosition(playerCamera.getPosition());
		this->boundSpotLight->setDirection(playerCamera.getViewVector());
	}

	// Update LookAt Camera
	glm::vec4 playerPosition = this->player->getTransform().getWorldPosition();
	glm::vec4 distanceVector = -5.0f * glm::normalize(this->lookAtCamera.getViewVector());
	glm::vec4 lookAtNewPosition = playerPosition + distanceVector;
	this->lookAtCamera.setPosition(lookAtNewPosition);

	this->player->update();

	if (!this->singlePlayer)
	{
		this->secondPlayer->update();

		// If two players mode, update network.
		double currentTime = glfwGetTime();
		if (currentTime > lastTime + 1.0f/networkUpdatesPerSecond)
		{
			this->network->sendPlayerInformation(*this->player);
			this->network->receiveAndProcessPackets();
			lastTime = currentTime;
		}
	}
	//glm::vec4 p = this->freeCamera.getPosition();
	//std::cout << "<" << p.x << ", " << p.y << ", " << p.z << ">" << std::endl;

	//this->entities[0]->getTransform().setWorldRotation(glm::vec3(x, 0.0f, 0.0f));
	//this->sky->getTransform().setWorldScale(glm::vec3(10.0f + x, 0.0f + y, 10.0f + z));
}

void Game::destroy()
{
	// Destroy network
	if (!this->singlePlayer)
		delete this->network;

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

	// Destroy Street Lamps
	// We can just clear the vector, since all street lamps were inside the lights array.
	this->streetLamps.clear();

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

		// If Free Camera is Selected, mouse change should only change free camera direction.
		if (this->selectedCamera == CameraType::FREE)
		{
			freeCamera.incPitch(-cameraMouseSpeed * (float)xDifference);
			freeCamera.incYaw(cameraMouseSpeed * (float)yDifference);
		}
		// If LookAt Camera is selected, mouse change should change look at camera direction
		else if (this->selectedCamera == CameraType::LOOKAT)
		{
			lookAtCamera.incPitch(-cameraMouseSpeed * (float)xDifference);
			lookAtCamera.incYaw(cameraMouseSpeed * (float)yDifference);
		}
		// If Player camera is selected, mouse change should change player look direction
		else if (this->selectedCamera == CameraType::PLAYER)
		{
			this->player->changeLookDirection(xDifference, yDifference);
		}
	}
	else
		firstTime = false;

	xPosOld = xPos;
	yPosOld = yPos;
}

void Game::processMouseClick(int button, int action)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (!this->singlePlayer)
			this->network->sendPlayerFireAnimation();
		this->player->fire();
	}
}

Player* Game::getSecondPlayer()
{
	if (!this->singlePlayer)
		return this->secondPlayer;

	return 0;
}

void Game::createShaders()
{
	this->fixedShader = new Shader(ShaderType::FIXED);
	this->basicShader = new Shader(ShaderType::BASIC);
	this->textureShader = new Shader(ShaderType::TEXTURE);
	this->phongShader = new Shader(ShaderType::PHONG);
	this->gouradShader = new Shader(ShaderType::GOURAD);
	this->flatShader = new Shader(ShaderType::FLAT);
	this->skyboxShader = new Shader(ShaderType::SKYBOX);
	this->shaderType = ShaderType::PHONG;
}

void Game::createCameras()
{
	glm::vec4 freeCameraInitialPosition = glm::vec4(0.46f, 4.08f, 15.65f, 1.0f);
	glm::vec4 freeCameraInitialUpVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 freeCameraInitialViewVector = glm::vec4(0.0039f, -0.0007967f, -1.0f, 0.0f);
	this->freeCamera = Camera(freeCameraInitialPosition, freeCameraInitialUpVector, freeCameraInitialViewVector);

	glm::vec4 lookAtCameraInitialPosition = glm::vec4(0.46f, 4.08f, 15.65f, 1.0f);
	glm::vec4 lookAtCameraInitialUpVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 lookAtCameraInitialViewVector = glm::vec4(0.0039f, -0.0007967f, -1.0f, 0.0f);
	this->lookAtCamera = Camera(freeCameraInitialPosition, freeCameraInitialUpVector, freeCameraInitialViewVector);

	this->selectedCamera = CameraType::PLAYER;
}

void Game::createLights()
{
	glm::vec4 ambientLight = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 diffuseLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 specularLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//LightAttenuation attenuation = {
	//	1.0f,		// Constant Term
	//	0.14f,		// Linear Term
	//	0.07f		// Quadratic Term
	//};

	LightAttenuation attenuation = {
		1.0f,		// Constant Term
		0.22f,		// Linear Term
		0.20f		// Quadratic Term
	};

	// POINT LIGHT
	//glm::vec4 plPosition = glm::vec4(5.6923f, 0.56f, 1.0f + 0.1f, 1.0f);
	//PointLight* pointLight = new PointLight(plPosition, ambientLight, diffuseLight, specularLight);
	//pointLight->setAttenuation(attenuation);
	//this->lights.push_back(pointLight);

	// SPOT LIGHT
	//glm::vec4 slPosition = glm::vec4(0, 0, 1, 1);
	//SpotLight* spotLight = new SpotLight(slPosition, ambientLight, diffuseLight, specularLight);
	//spotLight->setAttenuation(attenuation);
	//spotLight->setInnerCutOffAngle(glm::radians(12.5f));
	//spotLight->setOuterCutOffAngle(glm::radians(17.5f));
	//spotLight->setDirection(glm::vec4(0, 0, -1, 0));
	//this->boundSpotLight = spotLight;	// @TEMPORARY
	//this->lights.push_back(spotLight);

	// DIRECTIONAL LIGHT
	//glm::vec4 dlDirection = glm::vec4(-0.2f, -1.0f, -0.3f, 0.0f);
	//DirectionalLight* directionalLight = new DirectionalLight(dlDirection, ambientLight, diffuseLight,
	//	specularLight);
	//this->lights.push_back(directionalLight);

	// CREATE ALL STREET LAMPS
	glm::vec4 streetLampPosition = glm::vec4(5.69f, 0.56f, 1.165f, 1.0f);
	StreetLamp* streetLamp = this->createStreetLamp(streetLampPosition, 0.0f);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(10.69f, 0.56f, 1.165f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, 0.0f);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(10.838f, 0.56f, 4.3f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, -PI_F/2.0f);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(3.842f, 0.56f, 8.157f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, -PI_F/2.0f);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(1.164f, 0.56f, 19.1359f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, PI_F/2.0f);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(11.686f, 0.56f, 22.84f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, PI_F);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(18.836f, 0.56f, 12.93f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, -PI_F/2.0f);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(7.0f, 0.56f, 22.84f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, PI_F);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(20.766f, 0.56f, 22.84f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, PI_F);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(16.1714f, 0.56f, 21.47f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, PI_F / 2);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(19.405f, 0.56f, 1.8406f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, PI_F);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(11.415f, 0.56f, 13.836f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, PI_F);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(16.164f, 0.56f, 7.348f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, PI_F / 2.0f);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(6.836f, 0.56f, 16.53f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, -PI_F / 2.0f);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(22.838f, 0.56f, 18.31f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, -PI_F / 2.0f);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(22.838f, 0.56f, 5.31f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, -PI_F / 2.0f);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);

	streetLampPosition = glm::vec4(6.836f, 0.56f, 10.48f, 1.0f);
	streetLamp = this->createStreetLamp(streetLampPosition, -PI_F / 2.0f);
	this->lights.push_back(streetLamp);
	this->streetLamps.push_back(streetLamp);
}

StreetLamp* Game::createStreetLamp(const glm::vec4& position, float rotY)
{
	glm::vec4 ambientLight = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 diffuseLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 specularLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	LightAttenuation attenuation = {
		1.0f,		// Constant Term
		0.35f,		// Linear Term
		0.44f		// Quadratic Term
	};

	StreetLamp* streetLamp = new StreetLamp(position, ambientLight, diffuseLight, specularLight);
	streetLamp->setAttenuation(attenuation);
	streetLamp->setWorldRotation(glm::vec3(0.0f, rotY, 0.0f));

	return streetLamp;
}

void Game::createEntities()
{
	// CREATE A CUBE (cube.obj)
	//Texture* diffuseMap = Texture::load(".\\res\\art\\green.png");
	//Model* cubeModel = new Model(".\\res\\art\\cube.obj");
	//Entity* cubeEntity = new Entity(cubeModel);
	//cubeModel->setDiffuseMapOfAllMeshes(diffuseMap);
	//cubeEntity->getTransform().setWorldPosition(glm::vec4(-8.0f, 0.0f, 9.0f, 1.0f));
	//cubeEntity->getTransform().setWorldRotation(glm::vec3(0.35f, 0.77f, 0.12f));

	// CREATE COMPLEX MODEL (nanosuit.obj)
	//Model* complexModel = new Model(".\\res\\art\\obeseMale\\obese_male.obj");
	//Entity* complexEntity = new Entity(complexModel);
	//complexEntity->getTransform().setWorldPosition(glm::vec4(10.0f, 2.0f, 10.0f, 1.0f));
	//complexEntity->getTransform().setWorldScale(glm::vec3(0.05f, 0.05f, 0.05f));
	
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

	// CREATE MAP
	Model* mapModel = this->map->generateMapModel();
	Entity* mapEntity = new Entity(mapModel);

	// VECTOR CREATION
	//this->entities.push_back(cubeEntity);
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
glm::vec4 Game::getNewPositionForMovement(const glm::vec4& position, const glm::vec4& direction, float deltaTime) const
{
	static const float movementSpeed = 5.0f;
	glm::vec4 newPos = position + movementSpeed * deltaTime * direction;
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

const Camera* Game::getSelectedCamera() const
{
	switch (this->selectedCamera)
	{
	case CameraType::FREE:
		return &this->freeCamera;
		break;
	case CameraType::LOOKAT:
		return &this->lookAtCamera;
	case CameraType::PLAYER:
	default:
		return &this->player->getCamera();
	}
}

void Game::processInput(bool* keyState, float deltaTime)
{
	static const float cameraSpeed = 5.0f;

	if (this->selectedCamera == CameraType::FREE)
	{
		glm::vec4 movementDirection = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
		glm::vec4 lookDirection = freeCamera.getViewVector();
		glm::vec4 perpendicularDirection = freeCamera.getXAxis();

		if (keyState[GLFW_KEY_W])
			movementDirection += glm::normalize(lookDirection);
		if (keyState[GLFW_KEY_S])
			movementDirection -= glm::normalize(lookDirection);
		if (keyState[GLFW_KEY_A])
			movementDirection -= glm::normalize(perpendicularDirection);
		if (keyState[GLFW_KEY_D])
			movementDirection += glm::normalize(perpendicularDirection);

		if (movementDirection != glm::vec4(0.0f, 0.0f, 0.0f, 0.0f))
		{
			glm::vec4 playerPos = this->player->getTransform().getWorldPosition();
			movementDirection = glm::normalize(movementDirection);
			freeCamera.setPosition(freeCamera.getPosition() + deltaTime * cameraSpeed * movementDirection);
		}
	}
	else if (this->selectedCamera == CameraType::PLAYER || this->selectedCamera == CameraType::LOOKAT)
	{
		if (this->player)
		{
			glm::vec4 movementDirection = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec4 lookDirection = this->player->getLookDirection();
			glm::vec4 perpendicularDirection = this->player->getPerpendicularDirection();

			// Must ignore Y
			lookDirection.y = 0;
			perpendicularDirection.y = 0;

			if (keyState[GLFW_KEY_W])
				movementDirection += glm::normalize(lookDirection);
			if (keyState[GLFW_KEY_S])
				movementDirection -= glm::normalize(lookDirection);
			if (keyState[GLFW_KEY_A])
				movementDirection -= glm::normalize(perpendicularDirection);
			if (keyState[GLFW_KEY_D])
				movementDirection += glm::normalize(perpendicularDirection);

			if (movementDirection != glm::vec4(0.0f, 0.0f, 0.0f, 0.0f))
			{
				glm::vec4 playerPos = this->player->getTransform().getWorldPosition();
				movementDirection = glm::normalize(movementDirection);
				this->player->getTransform().setWorldPosition(this->getNewPositionForMovement(playerPos, movementDirection, deltaTime));
			}
		}
	}

	if (keyState[GLFW_KEY_X] && !keyState[GLFW_KEY_Q])
		x += 0.01f;
	if (keyState[GLFW_KEY_Y] && !keyState[GLFW_KEY_Q])
		y += 0.01f;
	if (keyState[GLFW_KEY_Z] && !keyState[GLFW_KEY_Q])
		z += 0.01f;
	if (keyState[GLFW_KEY_X] && keyState[GLFW_KEY_Q])
		x -= 0.01f;
	if (keyState[GLFW_KEY_Y] && keyState[GLFW_KEY_Q])
		y -= 0.01f;
	if (keyState[GLFW_KEY_Z] && keyState[GLFW_KEY_Q])
		z -= 0.01f;

	if (keyState[GLFW_KEY_1] && !keyState[GLFW_KEY_Q])
		xx += 0.01f;
	if (keyState[GLFW_KEY_2] && !keyState[GLFW_KEY_Q])
		yy += 0.01f;
	if (keyState[GLFW_KEY_3] && !keyState[GLFW_KEY_Q])
		zz += 0.01f;
	if (keyState[GLFW_KEY_1] && keyState[GLFW_KEY_Q])
		xx -= 0.01f;
	if (keyState[GLFW_KEY_2] && keyState[GLFW_KEY_Q])
		yy -= 0.01f;
	if (keyState[GLFW_KEY_3] && keyState[GLFW_KEY_Q])
		zz -= 0.01f;

	if (keyState[GLFW_KEY_N])
	{
		this->useNormalMap = !this->useNormalMap;
		keyState[GLFW_KEY_N] = false;					// Force false to only compute one time.
	}

	if (keyState[GLFW_KEY_P])
	{
		//char buffer[2048];
		//int size = udpReceiver->receiveMessage(buffer, 2048);
		//if (size > 0)
		//{
		//	buffer[size] = 0;
		//	std::cout << "BUFFER: " << buffer << std::endl;
		//	//udpServer->sendMessage("hello");
		//	keyState[GLFW_KEY_P] = false;					// Force false to only compute one time.
		//}
	}

	if (keyState[GLFW_KEY_C])
	{
		switch (this->selectedCamera)
		{
		case CameraType::FREE:
			this->selectedCamera = CameraType::LOOKAT; break;
		case CameraType::LOOKAT:
			this->selectedCamera = CameraType::PLAYER; break;
		case CameraType::PLAYER:
		default:
			this->selectedCamera = CameraType::FREE; break;
		}
		keyState[GLFW_KEY_C] = false;					// Force false to only compute one time.
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
	if (keyState[GLFW_KEY_T])
	{
		shaderType = raw::ShaderType::TEXTURE;
		keyState[GLFW_KEY_T] = false;					// Force false to only compute one time.
	}
}
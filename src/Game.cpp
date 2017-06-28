#include "Game.h"
#include "Model.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "StaticModels.h"
#include "Network.h"
#include "StreetLamp.h"
#include "Skybox.h"

#include <GLFW\glfw3.h>

using namespace raw;

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
		this->network = new Network(this, "127.0.0.1", 8888);

	// Create sky
	this->skybox = new Skybox();

	// Create Map
	this->createMap();

	// Create Shaders
	this->createShaders();

	// Create Cameras
	this->createCameras();

	// Create Player
	Model* playerModel = new Model(".\\res\\art\\carinhaloko\\carinhaloko_stance.obj");
	this->models.push_back(playerModel);
	this->player = new Player(playerModel);
	this->player->getTransform().setWorldScale(glm::vec3(0.12f, 0.12f, 0.12f));

	// Create Second Player
	if (!this->singlePlayer)
	{
		this->secondPlayer = new Player(playerModel);
		this->secondPlayer->getTransform().setWorldScale(glm::vec3(0.12f, 0.12f, 0.12f));
	}

	// Create Lights
	this->createLights();

	// Create Entities
	this->createEntities();

	// Set game settings
	this->useNormalMap = true;
	this->useOrthoCamera = false;
}

void Game::render() const
{
	const Camera* selectedCamera = this->getSelectedCamera();
	Shader* shaderToUse;

	// Get selected shader
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

	// Render skybox
	this->skybox->render(*skyboxShader, *selectedCamera);

	// Render all entities
	for (unsigned int i = 0; i < this->entities.size(); ++i)
		this->entities[i]->render(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);

	// Render all street lamps
	for (unsigned int i = 0; i < this->streetLamps.size(); ++i)
		this->streetLamps[i]->render(*shaderToUse, *basicShader, *selectedCamera, this->lights, this->useNormalMap);

	// Avoid rendering the player when the player camera is being used to not block the camera.
	if (this->selectedCamera != CameraType::PLAYER)
	{
		this->player->render(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);
		this->player->renderGun(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);
	}

	// Render first player shotmarks
	this->player->renderShotMarks(*basicShader, *selectedCamera);

	// Render second player only if game is being played multiplayer
	if (!this->singlePlayer)
	{
		this->secondPlayer->render(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);
		this->secondPlayer->renderGun(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);
		this->secondPlayer->renderShotMarks(*basicShader, *selectedCamera);
	}

	// Render aim only if player Camera is being used
	if (this->selectedCamera == CameraType::PLAYER)
		this->player->renderScreenImages(*fixedShader);
}

// Update game
void Game::update(float deltaTime)
{
	static double lastTime;
	const Camera* playerCamera = this->player->getCamera();

	// How many times, per second, information about the player should be sent to the second player (multiplayer only)
	static const int networkUpdatesPerSecond = 10;

	// Update LookAt Camera
	static const float lookAtCameraDistance = 2.2f;
	glm::vec4 playerPosition = this->player->getTransform().getWorldPosition();
	glm::vec4 distanceVector = -lookAtCameraDistance * glm::normalize(this->lookAtCamera->getViewVector());
	glm::vec4 lookAtNewPosition = playerPosition + distanceVector;
	this->lookAtCamera->setPosition(lookAtNewPosition);

	// Update player
	this->player->update();

	// If multiplayer
	if (!this->singlePlayer)
	{
		// Update second player
		this->secondPlayer->update();

		// Check if new packets arrived from the second player.
		this->network->receiveAndProcessPackets();

		// Send information to second player periodically.
		double currentTime = glfwGetTime();
		if (currentTime > lastTime + 1.0f / networkUpdatesPerSecond)
		{
			this->network->sendPlayerInformation(*this->player);
			lastTime = currentTime;
		}
	}

	// Update Ortho Free Camera (to match Perspective Free Camera)
	this->orthoFreeCamera->setPosition(freeCamera->getPosition());
	this->orthoFreeCamera->setNearPlane(freeCamera->getNearPlane());
	this->orthoFreeCamera->setFarPlane(freeCamera->getFarPlane());
	this->orthoFreeCamera->setUpVector(freeCamera->getUpVector());
	this->orthoFreeCamera->setViewVector(freeCamera->getViewVector());
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
	delete this->fixedShader;
	delete this->textureShader;
	delete this->skyboxShader;

	// Destroy Cameras
	delete this->freeCamera;
	delete this->lookAtCamera;

	// Destroy Lights
	for (unsigned int i = 0; i < this->lights.size(); ++i)
		delete this->lights[i];

	// Destroy Street Lamps
	// We can just clear the vector, since all street lamps were inside the lights array anyway.
	this->streetLamps.clear();

	// Destroy Models
	for (unsigned int i = 0; i < this->models.size(); ++i)
		delete this->models[i];

	// Destroy Entities
	for (unsigned int i = 0; i < this->entities.size(); ++i)
		delete this->entities[i];

	// Destroy players
	delete this->player;
	if (!this->singlePlayer)
		delete this->secondPlayer;

	// Clear vectors, so game can be restarted by calling init() again.
	this->lights.clear();
	this->entities.clear();
	this->models.clear();

	// Destroy Textures
	// @TODO: destroyAll can't destroy default diffuseMap and default specularMap,
	// because it should be possible to destroy and re-create Game class.
	// Texture::destroyAll();
}

// If mouse is moved, this function is called as a callback.
void Game::processMouseChange(double xPos, double yPos)
{
	static double xPosOld, yPosOld;
	static bool firstTime = true;
	// This constant is basically the mouse sensibility.
	// @TODO: Allow mouse sensibility to be configurable.
	static const float cameraMouseSpeed = 0.01f;
	
	if (!firstTime)
	{
		double xDifference = xPos - xPosOld;
		double yDifference = yPos - yPosOld;

		// If Free Camera is Selected, mouse change should only change free camera direction.
		if (this->selectedCamera == CameraType::FREE)
		{
			freeCamera->incPitch(-cameraMouseSpeed * (float)xDifference);
			freeCamera->incYaw(cameraMouseSpeed * (float)yDifference);
		}
		// If LookAt Camera is selected, mouse change should change look at camera direction
		else if (this->selectedCamera == CameraType::LOOKAT)
		{
			lookAtCamera->incPitch(-cameraMouseSpeed * (float)xDifference);
			lookAtCamera->incYaw(cameraMouseSpeed * (float)yDifference);
		}
		// If Player camera is selected, mouse change should change player look direction
		else if (this->selectedCamera == CameraType::PLAYER)
		{
			this->player->changeLookDirection((float)xDifference, (float)yDifference, cameraMouseSpeed);
		}
	}
	else
		firstTime = false;

	xPosOld = xPos;
	yPosOld = yPos;
}

// If scroll was used, this function is called as a callback.
void Game::processScrollChange(double xOffset, double yOffset)
{
	static const float orthoZoomSpeed = 0.1f;

	// Check if ortho camera is being used
	if (this->selectedCamera == CameraType::FREE && this->useOrthoCamera)
	{
		float orthoRange = this->orthoFreeCamera->getOrthoRange();
		this->orthoFreeCamera->setOrthoRange(orthoRange - orthoZoomSpeed * (float)yOffset);
	}
}

// If mouse is clicked, this function is called as a callback.
void Game::processMouseClick(int button, int action)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		// Start player firing animation.
		this->player->startShootingAnimation();

		// If multiplayer
		if (!this->singlePlayer)
		{
			// Shoot, testing collisions with map walls and second player
			this->player->shoot(this->secondPlayer, this->mapWallDescriptors, this->network);
		}
		else
		{
			// Shoot, testing collisions with map walls only.
			this->player->shoot(this->mapWallDescriptors);
		}
	}
}

void Game::processWindowResize(int width, int height)
{
	this->freeCamera->setWindowHeight(height);
	this->freeCamera->setWindowWidth(width);
	this->orthoFreeCamera->setWindowHeight(height);
	this->orthoFreeCamera->setWindowWidth(width);
	this->lookAtCamera->setWindowHeight(height);
	this->lookAtCamera->setWindowWidth(width);
	this->player->getCamera()->setWindowHeight(height);
	this->player->getCamera()->setWindowWidth(width);
}

// Returns the local player.
Player* Game::getLocalPlayer()
{
	return this->player;
}

// Returns the second player or null if not multiplayer.
Player* Game::getSecondPlayer()
{
	if (!this->singlePlayer)
		return this->secondPlayer;

	return 0;
}

// Create game map
void Game::createMap()
{
	this->map = new Map(".\\res\\map\\map.png");
	// Generate map wall descriptors. They are used to calculate collisions with map walls.
	this->mapWallDescriptors = this->map->generateMapWallDescriptors();
	Model* mapModel = this->map->generateMapModel();
	this->models.push_back(mapModel);
	Entity* mapEntity = new Entity(mapModel);
	this->entities.push_back(mapEntity);
}

// Create all shaders
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

// Create all cameras
void Game::createCameras()
{
	glm::vec4 freeCameraInitialPosition = glm::vec4(0.46f, 4.08f, 15.65f, 1.0f);
	glm::vec4 freeCameraInitialUpVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 freeCameraInitialViewVector = glm::vec4(0.0039f, -0.0007967f, -1.0f, 0.0f);
	this->freeCamera = new PerspectiveCamera(freeCameraInitialPosition, freeCameraInitialUpVector, freeCameraInitialViewVector);
	this->orthoFreeCamera = new OrthographicCamera(freeCameraInitialPosition, freeCameraInitialUpVector, freeCameraInitialViewVector);

	glm::vec4 lookAtCameraInitialPosition = glm::vec4(0.46f, 4.08f, 15.65f, 1.0f);
	glm::vec4 lookAtCameraInitialUpVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 lookAtCameraInitialViewVector = glm::vec4(0.0039f, -0.0007967f, -1.0f, 0.0f);
	this->lookAtCamera = new PerspectiveCamera(freeCameraInitialPosition, freeCameraInitialUpVector, freeCameraInitialViewVector);

	this->selectedCamera = CameraType::PLAYER;
}

// Create all lights
void Game::createLights()
{
	glm::vec4 ambientLight = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 diffuseLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 specularLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

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
	//this->lights.push_back(spotLight);

	// DIRECTIONAL LIGHT
	glm::vec4 dlDirection = glm::vec4(-0.2f, -1.0f, -0.3f, 0.0f);
	glm::vec4 dAmbientLight = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	glm::vec4 dDiffuseLight = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	glm::vec4 dSpecularLight = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	DirectionalLight* directionalLight = new DirectionalLight(dlDirection, dAmbientLight, dDiffuseLight,
		dSpecularLight);
	this->lights.push_back(directionalLight);

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

// Create a street lamp given a position and a rotation.
StreetLamp* Game::createStreetLamp(const glm::vec4& position, float rotY)
{
	glm::vec4 ambientLight = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 diffuseLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 specularLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Street lamp light attenuation
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
	// Create Wood Billet
	Model* woodBilletModel = new Model(".\\res\\art\\woodbillet\\simple_wood.obj");
	// Hide underisable plane that came with the model
	woodBilletModel->getMeshes()[0]->setVisible(false);
	// Set black specular map - wood should probably have no specular.
	woodBilletModel->setSpecularMapOfAllMeshes(Texture::load(".\\res\\art\\black.png"));
	// Set diffuse maps in a loop based on the mesh order.
	for (int i = 1; i < 19; i++)
		if (i % 2)
			woodBilletModel->getMeshes()[i]->setDiffuseMap(Texture::load(".\\res\\art\\woodbillet\\BarkDecidious0194_1_S.jpg"));
		else
			woodBilletModel->getMeshes()[i]->setDiffuseMap(Texture::load(".\\res\\art\\woodbillet\\wood2.png"));
	this->models.push_back(woodBilletModel);
	// Create wood billet entity and transform
	Entity* woodBilletEntity = new Entity(woodBilletModel);
	woodBilletEntity->getTransform().setWorldPosition(glm::vec4(5.0114f, 0.05f, 1.39f, 1.0f));
	woodBilletEntity->getTransform().setWorldScale(glm::vec3(0.05f, 0.05f, 0.05f));
	this->entities.push_back(woodBilletEntity);
}

// Returns a new position based on an original position and a movement direction.
// If there are no collisions, the new position will just be the original position + movementSpeed * direction
// If there is a collision, instead of just returning the original position (no movement), try to return
// the best position based on the map. (Try to slide when colliding with walls).
// Method will only work for map where walls are parallel to X and Z axes.
glm::vec4 Game::getNewPositionForMovement(const glm::vec4& position, const glm::vec4& direction,
	float deltaTime, float movementSpeed) const
{
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

// Get game's selected camera
Camera* Game::getSelectedCamera()
{
	switch (this->selectedCamera)
	{
	case CameraType::FREE:
		return this->freeCamera;
	case CameraType::LOOKAT:
		return this->lookAtCamera;
	case CameraType::PLAYER:
	default:
		return this->player->getCamera();
	}
}

// Get game's selected camera (const version)
const Camera* Game::getSelectedCamera() const
{
	switch (this->selectedCamera)
	{
	case CameraType::FREE:
		if (this->useOrthoCamera)
			return this->orthoFreeCamera;
		else
			return this->freeCamera;
	case CameraType::LOOKAT:
		return this->lookAtCamera;
	case CameraType::PLAYER:
	default:
		return this->player->getCamera();
	}
}

// This function will check which keys are pressed and do stuff based on it.
void Game::processInput(bool* keyState, float deltaTime)
{
	// Move Players and Cameras based on Input
	this->movePlayerAndCamerasBasedOnInput(keyState, deltaTime);

	// Toggle Normal Map
	if (keyState[GLFW_KEY_N])
	{
		this->useNormalMap = !this->useNormalMap;
		keyState[GLFW_KEY_N] = false;					// Force false to only compute one time.
	}

	// Change camera
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

	// Change Shader
	if (keyState[GLFW_KEY_P])
	{
		switch (shaderType)
		{
		case ShaderType::BASIC:
			this->shaderType = ShaderType::FIXED; break;
		case ShaderType::FIXED:
			this->shaderType = ShaderType::FLAT; break;
		case ShaderType::FLAT:
			this->shaderType = ShaderType::GOURAD; break;
		case ShaderType::GOURAD:
			this->shaderType = ShaderType::PHONG; break;
		case ShaderType::PHONG:
			this->shaderType = ShaderType::SKYBOX; break;
		case ShaderType::SKYBOX:
			this->shaderType = ShaderType::TEXTURE; break;
		case ShaderType::TEXTURE:
			this->shaderType = ShaderType::BASIC; break;
		}

		keyState[GLFW_KEY_P] = false;					// Force false to only compute one time.
	}

	// Change Projection Type
	if (keyState[GLFW_KEY_O])
	{
		this->useOrthoCamera = !this->useOrthoCamera;
		keyState[GLFW_KEY_O] = false;					// Force false to only compute one time.
	}
}

// This function will check which keys are pressed and which camera is selected and will move cameras and players.
void Game::movePlayerAndCamerasBasedOnInput(bool* keyState, float deltaTime)
{
	static const float freeCameraSpeed = 5.0f;
	static const float playerSpeed = 3.5f;

	glm::vec4 lookDirection;
	glm::vec4 perpendicularDirection;

	if (this->selectedCamera == CameraType::FREE)
	{
		lookDirection = freeCamera->getViewVector();
		perpendicularDirection = freeCamera->getXAxis();
	}
	else
	{
		lookDirection = player->getLookDirection();
		perpendicularDirection = player->getPerpendicularDirection();

		// Y coordinate must be ignored, since player can't fly.
		lookDirection.y = 0;
		perpendicularDirection.y = 0;
	}

	glm::vec4 movementDirection = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

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

		if (this->selectedCamera == CameraType::FREE)
		{
			freeCamera->setPosition(freeCamera->getPosition() + deltaTime * freeCameraSpeed * movementDirection);
		}
		else
		{
			glm::vec4 playerNewPos = this->getNewPositionForMovement(playerPos, movementDirection,
				deltaTime, playerSpeed);

			this->player->getTransform().setWorldPosition(playerNewPos);
		}
	}
}
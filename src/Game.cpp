#include "Game.h"
#include "Model.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "StaticModels.h"
#include "Network.h"
#include "StreetLamp.h"
#include "Skybox.h"
#include "Scoreboard.h"

#include <GLFW\glfw3.h>
#include <Windows.h>

using namespace raw;

Game::Game()
{
	this->bExit = false;
}

Game::~Game()
{
}

void Game::init(const GameSettings& gameSettings)
{
	this->bExit = false;
	this->singlePlayer = gameSettings.singlePlayer;

	// Init Network
	if (!this->singlePlayer)
		this->network = new Network(this, gameSettings.ip, gameSettings.port);

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
	this->player->setWallShotMarkColor(glm::vec4(0.4f, 0.4f, 1.0f, 1.0f));
	this->lights.push_back(this->player->getShootLight());						// Push Shoot Light

	// Create Second Player
	if (!this->singlePlayer)
	{
		this->secondPlayer = new Player(playerModel);
		this->secondPlayer->getTransform().setWorldScale(glm::vec3(0.12f, 0.12f, 0.12f));
		this->secondPlayer->setMovementInterpolationOn(true);
		this->lights.push_back(this->secondPlayer->getShootLight());						// Push Shoot Light
	}

	// Create Lights
	this->createLights();

	// Create Entities
	this->createEntities();

	// Create scoreboard
	this->scoreboard = new Scoreboard();

	// Set game settings
	this->useNormalMap = true;
	this->useOrthoCamera = false;
	this->useFog = true;
	this->useCullFace = false;

	if (!this->singlePlayer)
	{
		this->network->handshake();

		const glm::vec4 client0Position(1.7f, 0.0f, 1.7f, 1.0f);
		const glm::vec4 client1Position(22.22f, 0.0f, 21.98f, 1.0f);
		const glm::vec4 client0WallShotMarkColor(0.4f, 0.4f, 1.0f, 1.0f);
		const glm::vec4 client1WallShotMarkColor(0.4f, 1.0f, 0.4f, 1.0f);

		Player* client0 = this->getPlayerByClientLevel(ClientLevel::CLIENT0);
		Player* client1 = this->getPlayerByClientLevel(ClientLevel::CLIENT1);

		client0->getTransform().setWorldPosition(client0Position);
		client0->setSpawnPosition(client0Position);
		client0->setWallShotMarkColor(client0WallShotMarkColor);
		client1->getTransform().setWorldPosition(glm::vec4(client1Position));
		client1->setWallShotMarkColor(client1WallShotMarkColor);
		client1->setSpawnPosition(client1Position);
	}
}

Player* Game::getPlayerByClientLevel(ClientLevel clientLevel)
{
	if (this->singlePlayer)
	{
		if (clientLevel == ClientLevel::CLIENT0)
			return this->player;
		else
			return 0;
	}

	if (this->network->getClientLevel() == clientLevel)
		return this->player;
	else
		return this->secondPlayer;
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

	// Enable Cullface if activated
	if (this->useCullFace)
	{
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
	}

	// Render all street lamps
	for (unsigned int i = 0; i < this->streetLamps.size(); ++i)
		this->streetLamps[i]->render(*shaderToUse, *basicShader, *selectedCamera, this->lights, this->useNormalMap);

	// Render street spot light
	this->streetSpotLight->render(*shaderToUse, *basicShader, *selectedCamera, this->lights, this->useNormalMap);

	// Avoid rendering the player when the player camera is being used to not block the camera.
	if (this->selectedCamera != CameraType::PLAYER)
	{
		this->player->render(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);
		this->player->renderGun(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);
	}

	// Render second player only if game is being played multiplayer
	if (!this->singlePlayer)
	{
		this->secondPlayer->render(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);
		this->secondPlayer->renderGun(*shaderToUse, *selectedCamera, this->lights, this->useNormalMap);
	}

	// Disable cullface if activated
	if (this->useCullFace)
		glDisable(GL_CULL_FACE);

	// Render first player shotmarks
	this->player->renderShotMarks(*basicShader, *selectedCamera);

	// Render second player shotmarks
	if (!this->singlePlayer)
		this->secondPlayer->renderShotMarks(*basicShader, *selectedCamera);

	// Render aim only if player Camera is being used
	if (this->selectedCamera == CameraType::PLAYER)
		this->player->renderScreenImages(*fixedShader, *hpBarShader);

	this->scoreboard->render(*fixedShader, (float)this->freeCamera->getWindowHeight() / (float)this->freeCamera->getWindowWidth());
}

// Update game
void Game::update(float deltaTime)
{
	static double lastTime;
	const Camera* playerCamera = this->player->getCamera();

	// How many times, per second, information about the player should be sent to the second player (multiplayer only)
	static const float networkUpdatesPerSecond = 20.0f;

	// Update player
	this->player->update(map, deltaTime);

	// Update cameras
	this->updateCameras(deltaTime);

	// If multiplayer
	if (!this->singlePlayer)
	{
		// Update second player
		this->secondPlayer->update(map, deltaTime);

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

	// Update scoreboard
	Player* c0Player = this->getPlayerByClientLevel(ClientLevel::CLIENT0);
	Player* c1Player = this->getPlayerByClientLevel(ClientLevel::CLIENT1);
	float leftScore = c1Player ? c1Player->getKillCount() : 0.0f;
	float rightScore = c0Player ? c0Player->getKillCount() : 0.0f;
	this->scoreboard->changeLeftScore(leftScore);
	this->scoreboard->changeRightScore(rightScore);
	this->scoreboard->update(deltaTime);

	// Test if game ended
	this->endGameIfNecessary(leftScore, rightScore);
}

void Game::updateCameras(float deltaTime)
{
	// Update LookAt Camera
	static const float lookAtCameraDistance = 2.2f;
	glm::vec4 playerPosition = this->player->getCamera()->getPosition();
	glm::vec4 distanceVector = -lookAtCameraDistance * glm::normalize(this->lookAtCamera->getViewVector());
	glm::vec4 lookAtNewPosition = playerPosition + distanceVector;
	this->lookAtCamera->setPosition(lookAtNewPosition);
	// If lookAt camera is selected, update player look direction to match camera direction.
	if (this->selectedCamera == CameraType::LOOKAT)
		this->player->changeLookDirection(glm::normalize(-distanceVector));

	// Update Perspective Free Camera Position
	glm::vec4 freeCameraPosition = this->freeCamera->getPosition();
	this->freeCamera->setPosition(freeCameraPosition + deltaTime * this->freeCameraVelocity);

	// Update Ortho Free Camera (to match Perspective Free Camera)
	this->orthoFreeCamera->setPosition(freeCamera->getPosition());
	this->orthoFreeCamera->setNearPlane(freeCamera->getNearPlane());
	this->orthoFreeCamera->setFarPlane(freeCamera->getFarPlane());
	this->orthoFreeCamera->setUpVector(freeCamera->getUpVector());
	this->orthoFreeCamera->setViewVector(freeCamera->getViewVector());

	// Update fog of all cameras
	if (this->useFog)
	{
		FogDescriptor fogDescriptor;
		fogDescriptor.density = 0.16f;
		fogDescriptor.gradient = 1.5f;
		fogDescriptor.skyColor = this->skybox->getSkyColor();

		this->freeCamera->setUseFog(true);
		this->freeCamera->setFogDescriptor(fogDescriptor);
		this->orthoFreeCamera->setUseFog(true);
		this->orthoFreeCamera->setFogDescriptor(fogDescriptor);
		this->lookAtCamera->setUseFog(true);
		this->lookAtCamera->setFogDescriptor(fogDescriptor);
		this->player->getCamera()->setUseFog(true);
		this->player->getCamera()->setFogDescriptor(fogDescriptor);
	}
	else
	{
		this->freeCamera->setUseFog(false);
		this->orthoFreeCamera->setUseFog(false);
		this->lookAtCamera->setUseFog(false);
		this->player->getCamera()->setUseFog(false);
	}
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
	delete this->hpBarShader;

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
	static const float cameraMouseSpeed = 0.001f;
	
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
	this->hpBarShader = new Shader(ShaderType::HPBAR);
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
	this->freeCameraVelocity = glm::vec4(0.0f);
}

// Create all lights
void Game::createLights()
{
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

	// STREET SPOTLIGHT
	this->streetSpotLight = new StreetSpotLight(glm::vec4(4.9f, 0.81f, 3.88f, 1.0f), dAmbientLight,
		dDiffuseLight, dSpecularLight);
	this->streetSpotLight->setWorldRotation(glm::vec3(PI_F / 2.0f, 0.0f, -PI_F/2.0f));
	this->streetSpotLight->setAttenuation(attenuation);
	this->streetSpotLight->setInnerCutOffAngle(glm::radians(12.5f));
	this->streetSpotLight->setOuterCutOffAngle(glm::radians(17.5f));
	this->streetSpotLight->setDirection(glm::vec4(0, -1.0f, -1.0f, 0));
	this->lights.push_back(streetSpotLight);

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
		{
			woodBilletModel->getMeshes()[i]->setDiffuseMap(Texture::load(".\\res\\art\\w_diffuse.jpg"));
			woodBilletModel->getMeshes()[i]->setNormalMap(Texture::load(".\\res\\art\\w_normal.jpg"));
		//a	woodBilletModel->getMeshes()[i]->setDiffuseMap(Texture::load(".\\res\\art\\woodbillet\\BarkDecidious0194_1_S.jpg"));
		}
		else
			woodBilletModel->getMeshes()[i]->setDiffuseMap(Texture::load(".\\res\\art\\woodbillet\\wood2.png"));
	this->models.push_back(woodBilletModel);
	// Create wood billet entity and transform
	Entity* woodBilletEntity = new Entity(woodBilletModel);
	woodBilletEntity->getTransform().setWorldPosition(glm::vec4(5.0114f, 0.05f, 1.39f, 1.0f));
	woodBilletEntity->getTransform().setWorldScale(glm::vec3(0.05f, 0.05f, 0.05f));
	this->entities.push_back(woodBilletEntity);

	//Model* chaoModel = new Model(".\\res\\art\\chao.obj");
	//Entity* chaoEntity = new Entity(chaoModel);
	//chaoEntity->getTransform().setWorldPosition(glm::vec4(2.0f, 0.2f, 2.0f, 1.0f));
	//chaoEntity->getTransform().setWorldScale(glm::vec3(0.1f, 0.1f, 0.1f));
	//this->entities.push_back(chaoEntity);
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

	// Toggle Use Fog
	if (keyState[GLFW_KEY_F])
	{
		this->useFog = !this->useFog;
		keyState[GLFW_KEY_F] = false;					// Force false to only compute one time.
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

	if (keyState[GLFW_KEY_1])
	{
		this->streetSpotLight->setOn(!this->streetSpotLight->isOn());
		keyState[GLFW_KEY_1] = false;					// Force false to only compute one time.
	}

	// Player Jump
	if (keyState[GLFW_KEY_SPACE])
	{
		this->player->jump();
		keyState[GLFW_KEY_SPACE] = false;				// Force false to only compute one time.
	}

	// Player Shift (slow walk)
	if (keyState[GLFW_KEY_LEFT_SHIFT] || keyState[GLFW_KEY_RIGHT_SHIFT])
		this->player->setSlowMovement(true);
	else
		this->player->setSlowMovement(false);

	// Toggle Cull face
	if (keyState[GLFW_KEY_M])
	{
		this->useCullFace = !this->useCullFace;
		keyState[GLFW_KEY_M] = false;				// Force false to only compute one time.
	}

	// Toggle render shot marks
	if (keyState[GLFW_KEY_R])
	{
		this->player->setRenderShotMarks(!this->player->isRenderingShotMarks());
		if (!this->singlePlayer)
			this->secondPlayer->setRenderShotMarks(!this->secondPlayer->isRenderingShotMarks());
		keyState[GLFW_KEY_R] = false;				// Force false to only compute one time.
	}

	// Turn on/off some lights to improve FPS
	if (keyState[GLFW_KEY_L])
	{
		for (unsigned int i = 0; i < this->streetLamps.size(); ++i)
		{
			Light* light = this->streetLamps[i];
			
			if (i % 2)
				light->setOn(!light->isOn());
		}

		keyState[GLFW_KEY_L] = false;				// Force false to only compute one time.
	}

	// Check if game should end
	if (keyState[GLFW_KEY_ESCAPE])
	{
		this->bExit = true;
		this->exitInfo.forcedExit = true;
		keyState[GLFW_KEY_ESCAPE] = false;				// Force false to only compute one time.
	}

	// Temporary
	if (keyState[GLFW_KEY_9])
	{
		this->player->damage(PlayerBodyPart::UPPERRIGHTARM);
		this->player->startDamageAnimation();
		keyState[GLFW_KEY_9] = false;				// Force false to only compute one time.
	}

	// Toggle skybox day/night
	if (keyState[GLFW_KEY_Z])
	{
		this->skybox->setNight(!this->skybox->isNight());
		keyState[GLFW_KEY_Z] = false;				// Force false to only compute one time.
	}

	//if (keyState[GLFW_KEY_X] && !keyState[GLFW_KEY_Q])
	//	x += 0.01f;
	//if (keyState[GLFW_KEY_X] && keyState[GLFW_KEY_Q])
	//	x -= 0.01f;
	//if (keyState[GLFW_KEY_Y] && !keyState[GLFW_KEY_Q])
	//	y += 0.01f;
	//if (keyState[GLFW_KEY_Y] && keyState[GLFW_KEY_Q])
	//	y -= 0.01f;
	//if (keyState[GLFW_KEY_Z] && !keyState[GLFW_KEY_Q])
	//	z += 0.01f;
	//if (keyState[GLFW_KEY_Z] && keyState[GLFW_KEY_Q])
	//	z -= 0.01f;
}

// This function will check which keys are pressed and which camera is selected and will
// move cameras and players.
void Game::movePlayerAndCamerasBasedOnInput(bool* keyState, float deltaTime)
{
	// Camera Velocity
	const static float freeCameraVelocityLength = 5.0f;
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

	if (this->selectedCamera == CameraType::FREE)
	{
		if (movementDirection != glm::vec4(0.0f))
		{
			movementDirection = glm::normalize(movementDirection);
			this->freeCameraVelocity = freeCameraVelocityLength * glm::normalize(movementDirection);
		}
		else
			this->freeCameraVelocity = glm::vec4(0.0f);
	}
	else
		this->player->updateVelocityAndAccelerationBasedOnDirection(movementDirection);
}

bool Game::shouldExit() const
{
	return this->bExit;
}

GameExitInfo Game::getExitInfo() const
{
	return this->exitInfo;
}

void Game::endGameIfNecessary(float leftScore, float rightScore)
{
	const int MAX_SCORE = 5;

	if (leftScore >= MAX_SCORE || rightScore >= MAX_SCORE)
	{
		this->exitInfo.forcedExit = false;
		this->exitInfo.leftScore = leftScore;
		this->exitInfo.rightScore = rightScore;

		if (this->getPlayerByClientLevel(ClientLevel::CLIENT0) == this->player)
		{
			if (leftScore > rightScore)
				this->exitInfo.win = true;
			else
				this->exitInfo.win = false;
		}
		else
		{
			if (leftScore > rightScore)
				this->exitInfo.win = false;
			else
				this->exitInfo.win = true;
		}

		this->bExit = true;
	}
}
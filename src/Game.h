#pragma once

#include "Shader.h"
#include "Light.h"
#include "Player.h"
#include "Map.h"
#include <vector>

namespace raw
{
	class StreetLamp;
	class Network;
	class Skybox;
	class OrthographicCamera;
	class StreetSpotLight;

	enum class CameraType
	{
		PLAYER,
		LOOKAT,
		FREE
	};

	struct GameSettings
	{
		bool singlePlayer;
		char ip[256];
		int port;
	};

	class Game
	{
	public:
		Game();
		~Game();
		void init(const GameSettings& gameSettings);
		bool shouldExit() const;
		void render() const;
		void update(float deltaTime);
		void destroy();
		void processInput(bool* keyState, float deltaTime);
		void processMouseChange(double xPos, double yPos);
		void processScrollChange(double xOffset, double yOffset);
		void processMouseClick(int button, int action);
		void processWindowResize(int width, int height);
		Player* getLocalPlayer();
		Player* getSecondPlayer();
	private:
		// Initialization Function
		void createMap();
		void createShaders();
		void createCameras();
		void createLights();
		void createEntities();

		// Auxiliar Functions
		Camera* getSelectedCamera();
		const Camera* getSelectedCamera() const;
		StreetLamp* createStreetLamp(const glm::vec4& position, float rotY);
		void movePlayerAndCamerasBasedOnInput(bool* keyState, float deltaTime);
		void updateCameras(float deltaTime);

		// Network
		Network* network;

		// Shaders
		ShaderType shaderType;
		Shader* fixedShader;
		Shader* basicShader;
		Shader* textureShader;
		Shader* flatShader;
		Shader* gouradShader;
		Shader* phongShader;
		Shader* skyboxShader;

		// Cameras
		Camera* freeCamera;
		OrthographicCamera* orthoFreeCamera;
		Camera* lookAtCamera;
		CameraType selectedCamera;
		glm::vec4 freeCameraVelocity;

		// Players
		Player* player;
		Player* secondPlayer;

		// Map
		Map* map;
		std::vector<MapWallDescriptor> mapWallDescriptors;

		// Skybox
		Skybox* skybox;
		
		// Entities and Light
		std::vector<Light*> lights;
		StreetSpotLight* streetSpotLight;
		std::vector<StreetLamp*> streetLamps;
		std::vector<Model*> models;
		std::vector<Entity*> entities;

		// SpotLight
		Entity* spotLightEntity;
		Model* spotLightModel;

		// Game settings
		bool singlePlayer;
		bool useNormalMap;
		bool useOrthoCamera;
		bool useFog;
		bool useCullFace;
		bool bExit;
	};
}
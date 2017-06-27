#pragma once

#include "Shader.h"
#include "Light.h"
#include "Player.h"
#include <vector>

namespace raw
{
	class PointLight;
	class SpotLight;
	class Map;
	class StreetLamp;
	class Network;
	class Skybox;

	enum class CameraType
	{
		PLAYER,
		LOOKAT,
		FREE
	};

	class Game
	{
	public:
		Game();
		~Game();
		void init(bool singlePlayer);
		void render() const;
		void update(float deltaTime);
		void destroy();
		void processInput(bool* keyState, float deltaTime);
		void processMouseChange(double xPos, double yPos);
		void processMouseClick(int button, int action);
		Player* getSecondPlayer();
	private:
		void createShaders();
		void createCameras();
		void createLights();
		void createEntities();
		glm::vec4 getNewPositionForMovement(const glm::vec4& position, const glm::vec4& direction, float deltaTime,
			float movementSpeed) const;
		const Camera* getSelectedCamera() const;
		StreetLamp* createStreetLamp(const glm::vec4& position, float rotY);
		void damagePlayer(Player* player, PlayerCollision fireCollision);

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
		Camera freeCamera;
		Camera lookAtCamera;
		CameraType selectedCamera;

		Skybox* skybox;
		Player* player;
		Player* secondPlayer;
		Map* map;
		std::vector<Light*> lights;
		std::vector<StreetLamp*> streetLamps;
		std::vector<Entity*> entities;

		bool singlePlayer;
		bool useNormalMap;

		// Temporary for tests
		SpotLight* boundSpotLight;
		Entity* boundEntity;
	};
}
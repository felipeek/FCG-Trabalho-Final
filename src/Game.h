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
		void init();
		void render() const;
		void update(float deltaTime);
		void destroy();
		void processInput(bool* keyState, float deltaTime);
		void processMouseChange(double xPos, double yPos);
		void processMouseClick(int button, int action);
		void updateSecondPlayer(glm::vec4 newPosition, glm::vec4 newLookDirection);
	private:
		void createShaders();
		void createCameras();
		void createLights();
		void createEntities();
		glm::vec4 getNewPositionForMovement(glm::vec4 position, glm::vec4 direction, float deltaTime);
		const Camera* getSelectedCamera() const;
		
		// Shaders
		ShaderType shaderType;
		Shader* fixedShader;
		Shader* basicShader;
		Shader* flatShader;
		Shader* gouradShader;
		Shader* phongShader;

		// Cameras
		Camera freeCamera;
		Camera lookAtCamera;
		CameraType selectedCamera;

		Player* player;
		Player* secondPlayer;
		Map* map;
		std::vector<Light*> lights;
		std::vector<Entity*> entities;

		bool useNormalMap;

		// Temporary for tests
		SpotLight* boundSpotLight;
		Entity* boundEntity;
	};
}
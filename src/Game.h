#pragma once

#include "Shader.h"
#include "Light.h"
#include <vector>

namespace raw
{
	class PointLight;
	class SpotLight;
	class Map;

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
	private:
		void createShaders();
		void createLights();
		void createEntities();
		
		ShaderType shaderType;
		Shader* basicShader;
		Shader* flatShader;
		Shader* gouradShader;
		Shader* phongShader;
		Camera* camera;
		Map* map;
		std::vector<Light*> lights;
		std::vector<Entity*> entities;

		// Temporary for tests
		SpotLight* boundSpotLight;
		PointLight* boundPointLight;
		Entity* boundEntity;
	};
}
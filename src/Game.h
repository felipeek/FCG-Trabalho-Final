#pragma once

#include "Shader.h"
#include "Light.h"
#include <vector>

namespace raw
{
	class SpotLight;

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
		std::vector<Light*> lights;
		std::vector<Entity*> entities;

		// Temporary for tests
		SpotLight* boundSpotLight;
		Entity* boundEntity;
	};
}
#pragma once

#include "Map.h"

namespace raw
{
	class Camera;
	class Light;
	class Shader;
	class Skybox;
	class Model;
	class Entity;

	class MenuScene
	{
	public:
		MenuScene();
		~MenuScene();
		void render() const;
		void update(float deltaTime);
	private:
		Map* map;
		Shader* phongShader;
		Shader* skyboxShader;
		Camera* lookAtCamera;
		Skybox* skybox;

		std::vector<Light*> lights;
		std::vector<Model*> models;
		std::vector<Entity*> entities;
	};
}
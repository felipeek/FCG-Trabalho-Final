#pragma once

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Entity.h"

namespace raw
{
	class Skybox
	{
	public:
		Skybox();
		~Skybox();
		void render(const Shader& shader, const Camera& camera) const;
		glm::vec4 getSkyColor() const;
		bool isNight() const;
		void setNight(bool night);
	private:
		CubeMapTexture* skyboxDayTexture;
		CubeMapTexture* skyboxNightTexture;
		Entity* cube;
		Model* cubeModel;
		glm::vec4 daySkyColor;
		glm::vec4 nightSkyColor;
		bool bNight;
	};
}
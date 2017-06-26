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
	private:
		CubeMapTexture* skyboxTexture;
		Entity* cube;
	};
}
#pragma once

#include "GL\glew.h"
#include "Mesh.h"

namespace raw
{
	class StaticModels
	{
	public:
		static Mesh* getCubeMesh(Texture* diffuseMap, Texture* specularMap, Texture* normalMap, float specularShineness);
	};
}
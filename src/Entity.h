#pragma once
#include <GL\glew.h>
#include "Shader.h"
#include "Transform.h"
#include "Camera.h"
#include "Texture.h"
#include <vector>

namespace raw
{
	class Light;
	class Model;

	class Entity
	{
	public:
		Entity(Model* model);
		Entity(Model* model, Transform& transform);
		~Entity();

		virtual void render(const Shader& shader, const Camera& camera, const std::vector<Light*>& lights, bool useNormalMap) const;
		virtual void render(const Shader& shader) const;
		Transform& getTransform();
		const Transform& getTransform() const;
		Model* getModel();
		const Model* getModel() const;
	private:
		Transform transform;
		Model* model;
	};
}
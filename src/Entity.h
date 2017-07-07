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
		virtual void render(const Shader& shader, const Camera& camera, glm::vec4 solidColor) const;
		virtual void render(const Shader& shader, const Camera& camera) const;
		virtual void render(const Shader& shader, float windowRatio) const;
		virtual void render(const Shader& shader, float windowRatio, float playerHp, float playerMaxHp) const;
		Transform& getTransform();
		const Transform& getTransform() const;
		Model* getModel();
		const Model* getModel() const;
		void setModel(Model* model);
	private:
		Transform transform;
		Model* model;
	};
}
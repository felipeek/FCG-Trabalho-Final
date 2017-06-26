#pragma once

#include <GL\glew.h>
#include "MathIncludes.h"

namespace raw
{
	class Transform
	{
	public:
		Transform();
		Transform(glm::vec4 worldPosition, glm::vec3 worldRotation, glm::vec3 worldScale);
		~Transform();
		glm::vec4 getWorldPosition() const;
		void setWorldPosition(const glm::vec4& worldPosition);
		glm::vec3 getWorldRotation() const;
		void setWorldRotation(const glm::vec3& worldRotation);
		glm::vec3 getWorldScale() const;
		void setWorldScale(const glm::vec3& worldScale);
		Transform* getPreTransform();
		void setPreTransform(Transform* preTransform);
		void incRotX(float increment);
		void incRotY(float increment);
		void incRotZ(float increment);
		const glm::mat4 getModelMatrix() const;
	private:
		void updateModelMatrix();
		glm::vec4 worldPosition;
		glm::vec3 worldRotation;
		glm::vec3 worldScale;
		glm::mat4 modelMatrix;
		Transform* preTransform;
	};
}
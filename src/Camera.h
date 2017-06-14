#pragma once

#include "MathIncludes.h"

namespace raw
{
	class Camera
	{
	public:
		Camera(glm::vec4 position, glm::vec4 up, glm::vec4 view);
		Camera();
		~Camera();
		void setPosition(const glm::vec4& position);
		glm::vec4 getPosition() const;
		void setUpVector(const glm::vec4& up);
		glm::vec4 getUpVector() const;
		void setViewVector(const glm::vec4& view);
		glm::vec4 getViewVector() const;
		void incPitch(float angle);
		float getPitch() const;
		void incYaw(float angle);
		float getYaw() const;
		void setWindowHeight(unsigned int windowHeight);
		void setWindowWidth(unsigned int windowWidth);
		void setFieldOfView(float fov);
		void setNearPlane(float nearPlane);
		void setFarPlane(float farPlane);
		glm::vec4 getXAxis() const;
		glm::vec4 getYAxis() const;
		glm::vec4 getZAxis() const;
		const glm::mat4& getViewMatrix() const;
		const glm::mat4& getProjectionMatrix() const;
	private:
		void recalculateView();
		void recalculateAngles();
		void truncateAngles();
		void recalculateViewMatrix();
		void recalculateProjectionMatrix();
		glm::vec4 position;
		glm::vec4 up;
		glm::vec4 view;
		float pitch;
		float yaw;
		unsigned int windowHeight;
		unsigned int windowWidth;
		float fov;
		float nearPlane;
		float farPlane;
		glm::vec4 xAxis;
		glm::vec4 yAxis;
		glm::vec4 zAxis;
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
	};
}
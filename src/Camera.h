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
		void setNearPlane(float nearPlane);
		float getNearPlane() const;
		void setFarPlane(float farPlane);
		float getFarPlane() const;
		glm::vec4 getXAxis() const;
		glm::vec4 getYAxis() const;
		glm::vec4 getZAxis() const;
		const glm::mat4& getViewMatrix() const;
		virtual const glm::mat4& getProjectionMatrix() const = 0;
	protected:
		void recalculateView();
		void recalculateAngles();
		void truncateAngles();
		void recalculateViewMatrix();
		virtual void recalculateProjectionMatrix() = 0;
		glm::vec4 position;
		glm::vec4 up;
		glm::vec4 view;
		float pitch;
		float yaw;
		unsigned int windowHeight;
		unsigned int windowWidth;
		float nearPlane;
		float farPlane;
		glm::vec4 xAxis;
		glm::vec4 yAxis;
		glm::vec4 zAxis;
		glm::mat4 viewMatrix;
	};

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(glm::vec4 position, glm::vec4 up, glm::vec4 view);
		PerspectiveCamera();
		~PerspectiveCamera();
		void setFieldOfView(float fov);
		virtual const glm::mat4& getProjectionMatrix() const;
	protected:
		virtual void recalculateProjectionMatrix();
		float fov;
		glm::mat4 projectionMatrix;
	};

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(glm::vec4 position, glm::vec4 up, glm::vec4 view);
		OrthographicCamera();
		~OrthographicCamera();
		void setOrthoRange(float range);
		float getOrthoRange();
		virtual const glm::mat4& getProjectionMatrix() const;
	protected:
		virtual void recalculateProjectionMatrix();
		float orthoRange;
		glm::mat4 projectionMatrix;
	};
}
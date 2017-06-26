#include "Camera.h"

using namespace raw;

// Init camera with standard values.
Camera::Camera(glm::vec4 position, glm::vec4 up, glm::vec4 view)
{
	this->position = position;
	this->up = up;
	this->view = view;
	this->fov = 45.0f;
	this->nearPlane = -0.1f;
	this->farPlane = -1000.0f;
	this->windowWidth = 800;
	this->windowHeight = 600;
	this->recalculateAngles();
	this->recalculateViewMatrix();
	this->recalculateProjectionMatrix();
}

// Init camera with standard values.
Camera::Camera()
{
	this->position = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	this->view = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	this->up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	this->fov = 45.0f;
	this->nearPlane = -0.1f;
	this->farPlane = -100.0f;
	this->windowWidth = 800;
	this->windowHeight = 600;
	this->recalculateAngles();
	this->recalculateViewMatrix();
	this->recalculateProjectionMatrix();
}

Camera::~Camera()
{

}

// Change camera's world position and recalculate the View Matrix.
void Camera::setPosition(const glm::vec4& position)
{
	this->position = position;
	this->recalculateViewMatrix();
}

glm::vec4 Camera::getPosition() const
{
	return this->position;
}

// Change camera's up vector and recalculate the View Matrix.
void Camera::setUpVector(const glm::vec4& up)
{
	this->up = up;
	this->recalculateViewMatrix();
}

glm::vec4 Camera::getUpVector() const
{
	return this->up;
}

// Change camera's view vector and recalculate the View Matrix.
// Also, recalculate the View Vector angles (pitch and yaw).
// The view vector can be changed manually using this function or by changing the yaw or pitch.
void Camera::setViewVector(const glm::vec4& view)
{
	this->view = view;
	this->recalculateAngles();
	this->recalculateViewMatrix();
}

glm::vec4 Camera::getViewVector() const
{
	return this->view;
}

// Increase camera's pitch angle. The pitch and yaw angles are the two angles which define the view vector.
// The view vector can be changed manually using setViewVector() or by changing the yaw or pitch.
void Camera::incPitch(float angle)
{
	this->pitch += angle;
	this->truncateAngles();
	this->recalculateView();
	this->recalculateViewMatrix();
}

float Camera::getPitch() const
{
	return this->pitch;
}

// Increase camera's yaw angle. The pitch and yaw angles are the two angles which define the view vector.
// The view vector can be changed manually using setViewVector() or by changing the yaw or pitch.
void Camera::incYaw(float angle)
{
	this->yaw += angle;
	this->truncateAngles();
	this->recalculateView();
	this->recalculateViewMatrix();
}

float Camera::getYaw() const
{
	return this->yaw;
}

// Change the window height. This value is used to create the projection matrix.
void Camera::setWindowHeight(unsigned int windowHeight)
{
	this->windowHeight = windowHeight;
	this->recalculateProjectionMatrix();
}

// Change the window width. This value is used to create the projection matrix.
void Camera::setWindowWidth(unsigned int windowWidth)
{
	this->windowWidth = windowWidth;
	this->recalculateProjectionMatrix();
}

// Change the field of view. This value is used to create the projection matrix.
void Camera::setFieldOfView(float fov)
{
	this->fov = fov;
	this->recalculateProjectionMatrix();
}

// Change the near plane. This value is used to create the projection matrix.
void Camera::setNearPlane(float nearPlane)
{
	this->nearPlane = nearPlane;
	this->recalculateProjectionMatrix();
}

// Change the far plane. This value is used to create the projection matrix.
void Camera::setFarPlane(float farPlane)
{
	this->farPlane = farPlane;
	this->recalculateProjectionMatrix();
}

// This function will recalculate the view vector based on pitch and yaw values.
// Must be called everytime pitch or yaw are changed.
void Camera::recalculateView()
{
	this->view.x = sinf(this->pitch) * sinf(this->yaw);
	this->view.y = cosf(this->yaw);
	this->view.z = cosf(this->pitch) * sinf(this->yaw);
	this->view.w = 0.0f;
}

// This function will recalculate the pitch and yaw values based on the view vector.
// Must be called everytime the view vector is changed manually.
void Camera::recalculateAngles()
{
	this->pitch = atan2f(this->view.x, this->view.z);
	this->yaw = atan2f(sqrtf(this->view.x * this->view.x + this->view.z * this->view.z), this->view.y);
	this->truncateAngles();
}

// This function will truncate yaw and pitch angles to avoid errors when changing the view vector.
void Camera::truncateAngles()
{
	const float yawTruncateThreshold = 0.1f;
	const float yawBottomLimit = PI_F - yawTruncateThreshold;
	const float yawTopLimit = yawTruncateThreshold;

	if (this->yaw > yawBottomLimit)
		this->yaw = yawBottomLimit;
	else if (this->yaw < yawTopLimit)
		this->yaw = yawTopLimit;
}

const glm::mat4& Camera::getViewMatrix() const
{
	return this->viewMatrix;
}

const glm::mat4& Camera::getProjectionMatrix() const
{
	return this->projectionMatrix;
}

glm::vec4 Camera::getXAxis() const
{
	return this->xAxis;
}

glm::vec4 Camera::getYAxis() const
{
	return this->yAxis;
}

glm::vec4 Camera::getZAxis() const
{
	return this->zAxis;
}

// This function will recalculate the View Matrix based on the view vector, up vector and camera position.
// Must be called everytime one of them are changed. The result is stored in this->viewMatrix.
void Camera::recalculateViewMatrix()
{
	glm::vec4 w = -glm::normalize(this->view);
	glm::vec4 u = glm::normalize(glm::vec4(glm::cross(glm::vec3(this->up), glm::vec3(w)), 0.0f));
	glm::vec4 v = glm::vec4(glm::cross(glm::vec3(w), glm::vec3(u)), 0.0f);
	// Useless, but conceptually correct.
	glm::vec4 worldToCameraVec = this->position - glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	this->xAxis = u;
	this->yAxis = v;
	this->zAxis = w;

	this->viewMatrix = glm::transpose(glm::mat4({
		u.x, u.y, u.z, -glm::dot(u, worldToCameraVec),
		v.x, v.y, v.z, -glm::dot(v, worldToCameraVec),
		w.x, w.y, w.z, -glm::dot(w, worldToCameraVec),
		0.0f, 0.0f, 0.0f, 1.0f
	}));
}

// This function will recalculate the Projection Matrix based on the near plane, the far plane, the field of
// view, the window width and the window height.
// Must be called everytime one of them are changed. The result is stored in this->projectionMatrix.
void Camera::recalculateProjectionMatrix()
{
	float near = this->nearPlane;
	float far = this->farPlane;
	float top = fabs(near) * atanf(glm::radians(this->fov) / 2.0f);
	float bottom = -top;
	float right = top * ((float)this->windowWidth / (float)this->windowHeight);
	float left = -right;

	glm::mat4 P = glm::transpose(glm::mat4({
		near, 0, 0, 0,
		0, near, 0, 0,
		0, 0, near + far, - near * far,
		0, 0, 1, 0
	}));

	glm::mat4 M = glm::transpose(glm::mat4({
		2.0f / (right - left), 0, 0, - (right + left)/(right - left),
		0, 2.0f / (top - bottom), 0, - (top + bottom)/(top - bottom),
		0, 0, 2.0f / (far - near), - (far + near) / (far - near),
		0, 0, 0, 1
	}));

	this->projectionMatrix = -M * P;
}
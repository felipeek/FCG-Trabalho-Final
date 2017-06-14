#include "Transform.h"

using namespace raw;

// Creates a transform with default values.
Transform::Transform()
{
	this->worldPosition = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	this->worldRotation = glm::vec3(0.0f, 0.0f, 0.0f);
	this->worldScale = glm::vec3(1.0f, 1.0f, 1.0f);
}

// Creates a transform receiving the world position, the world rotation and the world scale.
Transform::Transform(glm::vec4 worldPosition, glm::vec3 worldRotation, glm::vec3 worldScale)
{
	this->worldPosition = worldPosition;
	this->worldRotation = worldRotation;
	this->worldScale = worldScale;
}

Transform::~Transform()
{

}

glm::vec4 Transform::getWorldPosition() const
{
	return this->worldPosition;
}

// Set the world position of the transform and update the Model Matrix.
void Transform::setWorldPosition(glm::vec4& worldPosition)
{
	this->worldPosition = worldPosition;
	this->updateModelMatrix();
}

glm::vec3 Transform::getWorldRotation() const
{
	return this->worldRotation;
}

// Set the world rotation of the transform and update the Model Matrix.
void Transform::setWorldRotation(glm::vec3& worldRotation)
{
	this->worldRotation = worldRotation;
	this->updateModelMatrix();
}

glm::vec3 Transform::getWorldScale() const
{
	return this->worldScale;
}

// Set the world scale of the transform and update the Model Matrix.
void Transform::setWorldScale(glm::vec3& worldScale)
{
	this->worldScale = worldScale;
	this->updateModelMatrix();
}

const glm::mat4& Transform::getModelMatrix() const
{
	return this->modelMatrix;
}

// Increments the rotation in the x axis of the transform and update the Model Matrix.
void Transform::incRotX(float increment)
{
	this->worldRotation.x += increment;
	this->updateModelMatrix();
}

// Increments the rotation in the y axis of the transform and update the Model Matrix.
void Transform::incRotY(float increment)
{
	this->worldRotation.y += increment;
	this->updateModelMatrix();
}

// Increments the rotation in the z axis of the transform and update the Model Matrix.
void Transform::incRotZ(float increment)
{
	this->worldRotation.z += increment;
	this->updateModelMatrix();
}

// Generates a new Model Matrix based on the translation, the rotation and the scale of the transform.
// This function must be called everytime one of the attributes above is changed.
// The result is stored in this->modelMatrix.
void Transform::updateModelMatrix()
{
	float s, c;

	glm::mat4 scaleMatrix = glm::transpose(glm::mat4({
		this->worldScale.x, 0.0f, 0.0f, 0.0f,
		0.0f, this->worldScale.y, 0.0f, 0.0f,
		0.0f, 0.0f, this->worldScale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	}));
	
	s = sinf(this->worldRotation.x);
	c = cosf(this->worldRotation.x);
	glm::mat4 rotXMatrix = glm::transpose(glm::mat4({
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, c, -s, 0.0f,
		0.0f, s, c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	}));

	s = sinf(this->worldRotation.y);
	c = cosf(this->worldRotation.y);
	glm::mat4 rotYMatrix = glm::transpose(glm::mat4({
		c, 0.0f, s, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-s, 0.0f, c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	}));

	s = sinf(this->worldRotation.z);
	c = cosf(this->worldRotation.z);
	glm::mat4 rotZMatrix = glm::transpose(glm::mat4({
		c, -s, 0.0f, 0.0f,
		s, c, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	}));

	glm::mat4 rotationMatrix = rotZMatrix * rotYMatrix * rotXMatrix;

	glm::mat4 translationMatrix = glm::transpose(glm::mat4({
		1.0f, 0.0f, 0.0f, worldPosition.x,
		0.0f, 1.0f, 0.0f, worldPosition.y,
		0.0f, 0.0f, 1.0f, worldPosition.z,
		0.0f, 0.0f, 0.0f, 1.0f
	}));

	this->modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
}
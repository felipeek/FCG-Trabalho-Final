#include "Entity.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Model.h"

using namespace raw;

Entity::Entity(Model* model)
{
	this->model = model;
}

Entity::Entity(Model* model, Transform& transform)
{
	this->model = model;
	this->transform = transform;
}

Entity::~Entity()
{

}

Model* Entity::getModel()
{
	return this->model;
}

const Model* Entity::getModel() const
{
	return this->model;
}

// Get the entity's transform. The transform is used to apply geometric transformations to the entity.
Transform& Entity::getTransform()
{
	return this->transform;
}

const Transform& Entity::getTransform() const
{
	return this->transform;
}

// Render the entity, using the shader, the camera and the vector of lights provided.
void Entity::render(const Shader& shader, const Camera& camera, const std::vector<Light*>& lights, bool useNormalMap) const
{
	shader.useProgram();
	GLuint modelMatrixLocation = glGetUniformLocation(shader.getProgram(), "modelMatrix");
	GLuint viewMatrixLocation = glGetUniformLocation(shader.getProgram(), "viewMatrix");
	GLuint projectionMatrixLocation = glGetUniformLocation(shader.getProgram(), "projectionMatrix");
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(this->transform.getModelMatrix()));
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix()));

	if (shader.getType() == ShaderType::PHONG || shader.getType() == ShaderType::GOURAD
		|| shader.getType() == ShaderType::FLAT)
	{
		if (camera.isUsingFog())
		{
			glm::vec4 skyColor = camera.getFogDescriptor().skyColor;
			GLuint fogDensityLocation = glGetUniformLocation(shader.getProgram(), "fogDescriptor.density");
			GLuint fogGradientLocation = glGetUniformLocation(shader.getProgram(), "fogDescriptor.gradient");
			GLuint fogSkyColorLocation = glGetUniformLocation(shader.getProgram(), "fogDescriptor.skyColor");
			GLuint fogOnLocation = glGetUniformLocation(shader.getProgram(), "fogDescriptor.on");
			glUniform1f(fogDensityLocation, camera.getFogDescriptor().density);
			glUniform1f(fogGradientLocation, camera.getFogDescriptor().gradient);
			glUniform4f(fogSkyColorLocation, skyColor.x, skyColor.y, skyColor.z, skyColor.w);
			glUniform1i(fogOnLocation, true);
		}
		else
		{
			GLuint fogOnLocation = glGetUniformLocation(shader.getProgram(), "fogDescriptor.on");
			glUniform1i(fogOnLocation, false);
		}

		glm::vec4 cameraPosition = camera.getPosition();
		GLuint cameraPositionLocation = glGetUniformLocation(shader.getProgram(), "cameraPosition");
		GLuint lightQuantityLocation = glGetUniformLocation(shader.getProgram(), "lightQuantity");
		glUniform4f(cameraPositionLocation, cameraPosition.x, cameraPosition.y, cameraPosition.z, cameraPosition.w);
		glUniform1i(lightQuantityLocation, lights.size());

		for (unsigned int i = 0; i < lights.size(); ++i)
			lights[i]->updateUniforms(shader, i);
	}

	this->model->render(shader, useNormalMap);
}

void Entity::render(const Shader& shader, const Camera& camera, glm::vec4 solidColor) const
{
	shader.useProgram();
	GLuint modelMatrixLocation = glGetUniformLocation(shader.getProgram(), "modelMatrix");
	GLuint viewMatrixLocation = glGetUniformLocation(shader.getProgram(), "viewMatrix");
	GLuint projectionMatrixLocation = glGetUniformLocation(shader.getProgram(), "projectionMatrix");
	GLuint solidColorLocation = glGetUniformLocation(shader.getProgram(), "solidColor");
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(this->transform.getModelMatrix()));
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix()));
	glUniform4f(solidColorLocation, solidColor.x, solidColor.y, solidColor.z, solidColor.w);

	this->model->render(shader, false);
}

void Entity::render(const Shader& shader, const Camera& camera) const
{
	shader.useProgram();
	GLuint modelMatrixLocation = glGetUniformLocation(shader.getProgram(), "modelMatrix");
	GLuint viewMatrixLocation = glGetUniformLocation(shader.getProgram(), "viewMatrix");
	GLuint projectionMatrixLocation = glGetUniformLocation(shader.getProgram(), "projectionMatrix");
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(this->transform.getModelMatrix()));
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.getViewMatrix()));
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(camera.getProjectionMatrix()));

	this->model->render(shader, false);
}

void Entity::render(const Shader& shader, float windowRatio) const
{
	glm::mat4 scaleMatrix = glm::transpose(glm::mat4({
		windowRatio, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	}));

	shader.useProgram();
	GLuint modelMatrixLocation = glGetUniformLocation(shader.getProgram(), "modelMatrix");
	GLuint scaleMatrixLocation = glGetUniformLocation(shader.getProgram(), "scaleMatrix");
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(this->transform.getModelMatrix()));
	glUniformMatrix4fv(scaleMatrixLocation, 1, GL_FALSE, glm::value_ptr(scaleMatrix));

	this->model->render(shader, false);
}

void Entity::render(const Shader& shader, float windowRatio, float playerHp, float playerMaxHp) const
{
	shader.useProgram();
	GLuint playerHpLocation = glGetUniformLocation(shader.getProgram(), "playerHp");
	GLuint playerMaximumHpLocation = glGetUniformLocation(shader.getProgram(), "playerMaximumHp");
	glUniform1i(playerHpLocation, playerHp);
	glUniform1i(playerMaximumHpLocation, playerMaxHp);

	Entity::render(shader, windowRatio);
}
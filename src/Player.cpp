#include "Player.h"
#include "Model.h"
#include "StaticModels.h"
#include "GLFW\glfw3.h"
#include "Entity.h"

using namespace raw;

extern int windowWidth;
extern int windowHeight;

//void render_vector(vec3 vec, vec3 position, const Shader& shader)
//{
//	mat4 ident;
//	mat4::identity(ident);
//	glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "modelMatrix"), 1, GL_TRUE, (float*)ident.data);
//	glBegin(GL_LINES);
//	glVertex3f(position.x, position.y, position.z);
//	glVertex3f(vec.x + position.x, vec.y + position.y, vec.z + position.z);
//	glEnd();
//}

Player::Player(Model* model) : Entity(model)
{
	glm::vec4 initialPosition = glm::vec4(1.2f, 0.0f, 1.2f, 1.0f);
	this->getTransform().setWorldPosition(initialPosition);

	this->initCamera();
	this->initBoundingBox();
	this->createGun();
	this->createAim();
	this->update();
}

Player::Player(Model* model, Transform& transform) : Entity(model, transform)
{
	glm::vec4 initialPosition = glm::vec4(1.2f, 0.0f, 1.2f, 1.0f);
	this->getTransform().setWorldPosition(initialPosition);

	this->initCamera();
	this->initBoundingBox();
	this->createGun();
	this->createAim();
	this->update();
}

Player::~Player()
{

}

void Player::initBoundingBox()
{
	this->boundingBoxModel = new Model(".\\res\\art\\carinhaloko\\carinhaloko_bb.obj");
	std::vector<Mesh*> boundingBoxMeshes = this->boundingBoxModel->getMeshes();

	for (unsigned int i = 0; i < boundingBoxMeshes.size(); ++i)
	{
		const std::vector<Vertex>& vertices = boundingBoxMeshes[i]->getVertices();

		BoundingShape boundingBoxModelCoords;
		boundingBoxModelCoords.vertices = (vec3*)malloc(vertices.size() * sizeof(vec3));
		boundingBoxModelCoords.num_vertices = vertices.size();

		BoundingShape boundingBoxWorldCoords;
		boundingBoxWorldCoords.vertices = (vec3*)malloc(vertices.size() * sizeof(vec3));
		boundingBoxWorldCoords.num_vertices = vertices.size();

		for (unsigned int j = 0; j < vertices.size(); ++j)
		{
			vec3 aux;
			aux.x = vertices[j].position.x;
			aux.y = vertices[j].position.y;
			aux.z = vertices[j].position.z;
			boundingBoxModelCoords.vertices[j] = aux;
		}

		this->boundingBoxInModelCoordinates.push_back(boundingBoxModelCoords);
		// ! bounding box in world coordinates are not being initialized.
		this->boundingBoxInWorldCoordinates.push_back(boundingBoxWorldCoords);
	}
}

void Player::initCamera()
{
	glm::vec4 position = this->getTransform().getWorldPosition();
	glm::vec4 upVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 viewVector = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	this->camera = Camera(position, upVector, viewVector);
}

void Player::createGun()
{
	Model* gunModel = new Model(".\\res\\art\\gun\\Handgun_obj.obj");
	this->gun = new Entity(gunModel);
	this->gun->getTransform().setWorldPosition(this->getTransform().getWorldPosition());

	// Remove underisable meshes
	this->gun->getModel()->getMeshes()[0]->setVisible(false);
	this->gun->getModel()->getMeshes()[2]->setVisible(false);
	this->gun->getModel()->getMeshes()[5]->setVisible(false);

	// Set gun constant position with relation to the camera.
	const static float gunScalement = 0.05f;
	const static float gunXTranslation = 0.007f;
	const static float gunYTranslation = 0.529f;
	const static float gunZTranslation = -0.235f;

	Transform* preTransform = new Transform();
	preTransform->setWorldRotation(glm::vec3(0.0f, PI_F / 2.0f, 0.0f));
	preTransform->setWorldPosition(glm::vec4(gunXTranslation, gunYTranslation, gunZTranslation, 1.0f));
	preTransform->setWorldScale(glm::vec3(gunScalement, gunScalement, gunScalement));
	this->gun->getTransform().setPreTransform(preTransform);
}

void Player::createAim()
{
	Mesh* aimMesh = new Quad(Texture::load(".\\res\\art\\aim4.png"));
	Model* aimModel = new Model(std::vector<Mesh*>({ aimMesh }));
	this->aim = new Entity(aimModel);

	this->setIsFireAnimationOn(false);
	this->fireTime = 0;

	Mesh* firstPersonGunMesh = new Quad(Texture::load(".\\res\\art\\gun_tex.png"));
	Model* firstPersonGunModel = new Model(std::vector<Mesh*>({ firstPersonGunMesh }));
	this->firstPersonGun = new Entity(firstPersonGunModel);

	Mesh* firstPersonGunFiringMesh = new Quad(Texture::load(".\\res\\art\\gun_tex_f.png"));
	Model* firstPersonGunFiringModel = new Model(std::vector<Mesh*>({ firstPersonGunFiringMesh }));
	this->firstPersonGunFiring = new Entity(firstPersonGunFiringModel);
}

const Camera& Player::getCamera() const
{
	return this->camera;
}

void Player::render(const Shader& shader, const Camera& camera, const std::vector<Light*>& lights, bool useNormalMap) const
{
	Entity::render(shader, camera, lights, useNormalMap);

	//vec3 viewRayVertices[8];
	//
	//glm::vec4 rayPosition = this->camera.getPosition();
	//glm::vec4 rayDirection = glm::normalize(this->camera.getViewVector());
	//float epsilon = 0.0001f;
	//float rayLength = 30.0f;
	//
	//glm::vec4 xAxis = glm::normalize(this->camera.getXAxis());
	//glm::vec4 yAxis = glm::normalize(this->camera.getYAxis());
	//
	//std::vector<Vertex> prismVertices;
	//
	//Vertex v;
	//
	//v.position = rayPosition + (0.0f * rayDirection - epsilon * xAxis - epsilon * yAxis);
	//prismVertices.push_back(v);
	//v.position = rayPosition + (0.0f * rayDirection - epsilon * xAxis + epsilon * yAxis);
	//prismVertices.push_back(v);
	//v.position = rayPosition + (0.0f * rayDirection + epsilon * xAxis - epsilon * yAxis);
	//prismVertices.push_back(v);
	//v.position = rayPosition + (0.0f * rayDirection + epsilon * xAxis + epsilon * yAxis);
	//prismVertices.push_back(v);
	//v.position = rayPosition + (rayLength * rayDirection - epsilon * xAxis - epsilon * yAxis);
	//prismVertices.push_back(v);
	//v.position = rayPosition + (rayLength * rayDirection - epsilon * xAxis + epsilon * yAxis);
	//prismVertices.push_back(v);
	//v.position = rayPosition + (rayLength * rayDirection + epsilon * xAxis - epsilon * yAxis);
	//prismVertices.push_back(v);
	//v.position = rayPosition + (rayLength * rayDirection + epsilon * xAxis + epsilon * yAxis);
	//prismVertices.push_back(v);
	//
	//std::vector<unsigned int> indices;
	//indices.push_back(0);
	//indices.push_back(1);
	//indices.push_back(2);
	//indices.push_back(1);
	//indices.push_back(2);
	//indices.push_back(3);
	//indices.push_back(2);
	//indices.push_back(3);
	//indices.push_back(4);
	//indices.push_back(3);
	//indices.push_back(4);
	//indices.push_back(5);
	//indices.push_back(4);
	//indices.push_back(5);
	//indices.push_back(6);
	//indices.push_back(5);
	//indices.push_back(6);
	//indices.push_back(7);
	//indices.push_back(6);
	//indices.push_back(7);
	//indices.push_back(0);
	//indices.push_back(7);
	//indices.push_back(0);
	//indices.push_back(1);
	//
	//Mesh* m = new Mesh(prismVertices, indices, 0, 0, 0, 32.0f);
	//
	//Model* testModel = new Model(std::vector<Mesh*>({ m }));
	//Entity* testEntity = new Entity(testModel);
	//
	//testEntity->render(shader, camera, lights, false);
}

void Player::renderGun(const Shader& shader, const Camera& camera, const std::vector<Light*>& lights, bool useNormalMap) const
{
	this->gun->render(shader, camera, lights, useNormalMap);
}

void Player::renderAim(const Shader& shader) const
{
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	this->aim->render(shader);
	if (this->isFireAnimationOn)
		this->firstPersonGunFiring->render(shader);
	else
		this->firstPersonGun->render(shader);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void Player::update()
{
	glm::vec4 playerPosition = this->getTransform().getWorldPosition();

	// Refresh camera's position
	const float cameraHeight = 0.6f;
	glm::vec4 cameraPosition = playerPosition;
	playerPosition.y = cameraHeight;
	this->camera.setPosition(playerPosition);

	// Refresh model rotation
	glm::vec4 lookDirection = glm::normalize(this->camera.getViewVector());
	const static glm::vec4 initialLookDirection = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	glm::vec2 vecA = glm::normalize(glm::vec2(lookDirection.x, lookDirection.z));
	glm::vec2 vecB = glm::normalize(glm::vec2(initialLookDirection.x, initialLookDirection.z));
	float modelRotation = atan2f(vecB.x, vecB.y) - atan2f(vecA.x, vecA.y);
	this->getTransform().setWorldRotation(glm::vec3(0.0f, -modelRotation - PI_F, 0.0f));

	// Refresh gun position
	this->gun->getTransform().setWorldPosition(this->getTransform().getWorldPosition());
	//this->gun->getTransform().setWorldRotation(glm::vec3(this->camera.getYaw(), this->camera.getPitch(), 0));
	this->gun->getTransform().setWorldRotation(glm::vec3(0.0f, -modelRotation, 0.0f));

	// Refresh aim scalement based on window's size
	const static float aimScalement = 0.1f;
	float windowRatio = (float)windowHeight / (float)windowWidth;
	this->aim->getTransform().setWorldScale(glm::vec3(aimScalement * windowRatio, aimScalement, 0.0f));

	// Refresh fire animation
	if (this->isFireAnimationOn)
	{
		double currentTime = glfwGetTime();

		if (currentTime > this->fireTime + fireAnimationTime)
			this->setIsFireAnimationOn(false);
	}
}

void Player::fire()
{
	this->setIsFireAnimationOn(true);
	this->fireTime = glfwGetTime();
}

void Player::changeLookDirection(float xDifference, float yDifference)
{
	static const float cameraMouseSpeed = 0.01f;

	this->camera.incPitch(-cameraMouseSpeed * (float)xDifference);
	this->camera.incYaw(cameraMouseSpeed * (float)yDifference);
}

void Player::changeLookDirection(const glm::vec4& lookDirection)
{
	this->camera.setViewVector(lookDirection);
}

glm::vec4 Player::getLookDirection() const
{
	return this->camera.getViewVector();
}

glm::vec4 Player::getPerpendicularDirection() const
{
	return this->camera.getXAxis();
}

void Player::setIsFireAnimationOn(bool isFireAnimationOn)
{
	this->isFireAnimationOn = isFireAnimationOn;
	this->gun->getModel()->getMeshes()[5]->setVisible(isFireAnimationOn);
}

bool Player::isViewRayCollidingWith(Player* player) const
{
	std::vector<BoundingShape>& playerBoundingBox = player->getBoundingBoxInWorldCoordinates();

	// Create View Ray Bounding Shape
	BoundingShape viewRayBoundingShape;

	vec3 viewRayVertices[8];

	glm::vec4 rayPosition = this->camera.getPosition();
	glm::vec4 rayDirection = glm::normalize(this->camera.getViewVector());
	float epsilon = 0.0001f;
	float rayLength = 30.0f;

	glm::vec4 xAxis = glm::normalize(this->camera.getXAxis());
	glm::vec4 yAxis = glm::normalize(this->camera.getYAxis());

	glm::vec4 prismVertices[8];

	prismVertices[0] = rayPosition + (0.0f * rayDirection - epsilon * xAxis - epsilon * yAxis);
	prismVertices[1] = rayPosition + (0.0f * rayDirection - epsilon * xAxis + epsilon * yAxis);
	prismVertices[2] = rayPosition + (0.0f * rayDirection + epsilon * xAxis - epsilon * yAxis);
	prismVertices[3] = rayPosition + (0.0f * rayDirection + epsilon * xAxis + epsilon * yAxis);
	prismVertices[4] = rayPosition + (rayLength * rayDirection - epsilon * xAxis - epsilon * yAxis);
	prismVertices[5] = rayPosition + (rayLength * rayDirection - epsilon * xAxis + epsilon * yAxis);
	prismVertices[6] = rayPosition + (rayLength * rayDirection + epsilon * xAxis - epsilon * yAxis);
	prismVertices[7] = rayPosition + (rayLength * rayDirection + epsilon * xAxis + epsilon * yAxis);

	viewRayVertices[0] = vec3(prismVertices[0].x, prismVertices[0].y, prismVertices[0].z);
	viewRayVertices[1] = vec3(prismVertices[1].x, prismVertices[1].y, prismVertices[1].z);
	viewRayVertices[2] = vec3(prismVertices[2].x, prismVertices[2].y, prismVertices[2].z);
	viewRayVertices[3] = vec3(prismVertices[3].x, prismVertices[3].y, prismVertices[3].z);
	viewRayVertices[4] = vec3(prismVertices[4].x, prismVertices[4].y, prismVertices[4].z);
	viewRayVertices[5] = vec3(prismVertices[5].x, prismVertices[5].y, prismVertices[5].z);
	viewRayVertices[6] = vec3(prismVertices[6].x, prismVertices[6].y, prismVertices[6].z);
	viewRayVertices[7] = vec3(prismVertices[7].x, prismVertices[7].y, prismVertices[7].z);

	viewRayBoundingShape.vertices = viewRayVertices;
	viewRayBoundingShape.num_vertices = 8;

	for (unsigned int i = 0; i < playerBoundingBox.size(); ++i)
		if (gjk_collides(&playerBoundingBox[i], &viewRayBoundingShape))
			return true;

	return false;
}

std::vector<BoundingShape>& Player::getBoundingBoxInModelCoordinates()
{
	return this->boundingBoxInModelCoordinates;
}

std::vector<BoundingShape>& Player::getBoundingBoxInWorldCoordinates()
{
	mat4 modelMatrix;
	glm::mat4 m;

	for (unsigned int i = 0; i < this->boundingBoxInModelCoordinates.size(); ++i)
	{
		m = glm::transpose(this->getTransform().getModelMatrix());
		memcpy(modelMatrix.data, glm::value_ptr(m), 16 * sizeof(float));

		transform_shape(&this->boundingBoxInModelCoordinates[i], &this->boundingBoxInWorldCoordinates[i], modelMatrix);
	}

	return this->boundingBoxInWorldCoordinates;
}
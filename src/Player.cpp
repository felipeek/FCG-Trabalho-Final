#include "Player.h"
#include "Model.h"
#include "StaticModels.h"
#include "GLFW\glfw3.h"

using namespace raw;

extern int windowWidth;
extern int windowHeight;

Player::Player(Model* model) : Entity(model)
{
	this->initCamera();
	this->createGun();
	this->createAim();
	this->update();
}

Player::Player(Model* model, Transform& transform) : Entity(model, transform)
{
	this->initCamera();
	this->createGun();
	this->createAim();
	this->update();
}

Player::~Player()
{

}

void Player::initCamera()
{
	glm::vec4 initialPosition = glm::vec4(1.2f, 0.3f, 1.2f, 1.0f);
	glm::vec4 upVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 viewVector = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	this->camera = Camera(initialPosition, upVector, viewVector);
	this->getTransform().setWorldPosition(initialPosition);
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
	// Hardcoded to be on the bottom right of the screen.
	const static float gunXRotation = 4.68f;
	const static float gunYRotation = 6.27f;
	const static float gunZRotation = 7.61f;
	const static float gunXTranslation = -0.31f;
	const static float gunYTranslation = 0.0f;
	const static float gunZTranslation = 0.22;
	const static float gunScalement = 0.3f;

	Transform* preTransform = new Transform();
	preTransform->setWorldRotation(glm::vec3(gunXRotation, gunYRotation, gunZRotation));
	preTransform->setWorldScale(glm::vec3(gunScalement, gunScalement, gunScalement));
	preTransform->setWorldPosition(glm::vec4(gunXTranslation, gunYTranslation, gunZTranslation, 1.0f));
	this->gun->getTransform().setPreTransform(preTransform);
}

void Player::createAim()
{
	Mesh* aimMesh = new Quad(Texture::load(".\\res\\art\\aim4.png"));
	Model* aimModel = new Model(std::vector<Mesh*>({ aimMesh }));
	this->aim = new Entity(aimModel);

	this->setIsFireAnimationOn(false);
	this->fireTime = 0;
}

const Camera& Player::getCamera() const
{
	return this->camera;
}

void Player::renderGun(const Shader& shader, const Camera& camera, const std::vector<Light*>& lights, bool useNormalMap) const
{
	this->gun->render(shader, camera, lights, useNormalMap);
}

void Player::renderAim(const Shader& shader) const
{
	this->aim->render(shader);
}

void Player::update()
{
	glm::vec4 playerPosition = this->getTransform().getWorldPosition();

	// Refresh camera's position
	this->camera.setPosition(playerPosition);

	// Refresh model rotation
	glm::vec4 lookDirection = glm::normalize(this->camera.getViewVector());
	const static glm::vec4 initialLookDirection = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	glm::vec2 vecA = glm::normalize(glm::vec2(lookDirection.x, lookDirection.z));
	glm::vec2 vecB = glm::normalize(glm::vec2(initialLookDirection.x, initialLookDirection.z));
	float modelRotation = atan2f(vecB.x, vecB.y) - atan2f(vecA.x, vecA.y);
	this->getTransform().setWorldRotation(glm::vec3(0.0f, -modelRotation, 0.0f));

	// Refresh gun position
	const static float gunDistance = 0.9f;
	this->gun->getTransform().setWorldPosition(gunDistance * lookDirection + playerPosition);
	this->gun->getTransform().setWorldRotation(glm::vec3(this->camera.getYaw(), this->camera.getPitch(), 0));

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
	static const float cameraMouseSpeed = 0.005f;

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
#include "Player.h"
#include "Model.h"
#include "StaticModels.h"
#include "Collision.h"
#include "Entity.h"
#include "Network.h"

#include <GLFW\glfw3.h>

using namespace raw;

glm::vec4 Player::wallShotMarkColor = glm::vec4(0.5f, 0.5f, 1.0f, 1.0f);

// Create player
Player::Player(Model* model) : Entity(model)
{
	// Set initial HP and initial position
	this->hp = initialHp;
	glm::vec4 initialPosition = glm::vec4(1.7f, 0.0f, 1.7f, 1.0f);
	this->getTransform().setWorldPosition(initialPosition);

	// Create player camera
	this->createCamera();

	// Create player bounding box
	this->createBoundingBox();

	// Create player gun
	this->createGun();

	// Create player aim
	this->createAim();

	// Create shot mark model
	std::vector<Mesh*> shotMarkModelMeshes;
	shotMarkModelMeshes.push_back(StaticModels::getCubeMesh(0.02f, 0, 0, 0, 0));
	this->shotMarkModel = new Model(shotMarkModelMeshes);

	// Create damage animation
	const float damageAnimationScale = 2.0f;
	Texture* damageAnimationTexture = Texture::load(".\\res\\art\\damage.png");
	this->damageAnimationEntity = new Entity(new Model(std::vector<Mesh*>({new Quad(damageAnimationTexture)})));
	this->damageAnimationEntity->getTransform().setWorldScale(glm::vec3(damageAnimationScale,
		damageAnimationScale, damageAnimationScale));

	// Movement Interpolation
	this->isMovementInterpolationOn = false;
	this->movementInterpolationPosition = glm::vec4(0.0f);

	// Update player
	// this->update();
}

// This function will start moving the player towards "position"
// Used in multiplayer mode to simulate the enemy movement
void Player::startMovementInterpolation(const glm::vec4& position)
{
	if (this->getTransform().getWorldPosition() != position)
	{
		this->isMovementInterpolationOn = true;
		this->movementInterpolationPosition = position;
	}
}

// Interpolate player movement if interpolation is on
void Player::interpolateMovement(float deltaTime)
{
	static const float playerSpeed = 3.5f;

	if (this->isMovementInterpolationOn)
	{
		// Calculate new position
		glm::vec4 currentPosition = this->getTransform().getWorldPosition();
		glm::vec4 direction = glm::normalize(this->movementInterpolationPosition - currentPosition);
		glm::vec4 newPosition = currentPosition + playerSpeed * deltaTime * direction;
		newPosition.y = currentPosition.y;		// Keep y

		float playerToFinalPositionDistance = glm::length(currentPosition - this->movementInterpolationPosition);
		float playerToNewPositionDistance = glm::length(currentPosition - newPosition);

		// If the distance from player to the new position is greater than the distance from
		// player to the final interpolation position, than interpolation is over and we can
		// just set player position to be the final interpolation position
		if (playerToFinalPositionDistance <= playerToNewPositionDistance)
		{
			newPosition = this->movementInterpolationPosition;
			this->isMovementInterpolationOn = false;
		}

		this->getTransform().setWorldPosition(newPosition);
	}
}

// Delete player
Player::~Player()
{
	// Delete camera
	delete this->camera;

	// Delete bounding box entity and model
	delete this->boundingBoxEntity;
	delete this->boundingBoxModel;

	// Free bounding box vertices that are allocated in BoundingShapes.
	for (unsigned int i = 0; i < this->boundingBoxInModelCoordinates.size(); ++i)
		free(this->boundingBoxInModelCoordinates[i].vertices);
	for (unsigned int i = 0; i < this->boundingBoxInModelCoordinates.size(); ++i)
		free(this->boundingBoxInWorldCoordinates[i].vertices);

	// Delete gun model and entity
	delete this->gun->getModel();
	delete this->gun;
	
	// Delete aim model and entity
	delete this->aim->getModel();
	delete this->aim;

	// Delete first person gun model and entity
	delete this->firstPersonGun->getModel();
	delete this->firstPersonGun;
	delete this->firstPersonGunFiring->getModel();
	delete this->firstPersonGunFiring;

	// Delete shot mark model
	delete this->shotMarkModel;

	// Destroy Shot Marks
	for (unsigned int i = 0; i < this->shotMarks.size(); ++i)
		delete this->shotMarks[i].entity;

	// Delete damage animation model and entity
	delete this->damageAnimationEntity->getModel();
	delete this->damageAnimationEntity;
}

// Create player bounding box
// This function will create two vectors of BoundingShapes. These vectors of bounding shapes will contain
// all vertices of the bounding box and they will be used when calling GJK collisions functions. The space for the
// vertices must be allocated. The first vector will contain the vertices in model coordinates, while the second one
// will contain them in world coordinates. However, this function will only fill the model coordinates vector, and will
// just allocate the world coordinates vector. This is done this way because the world coordinate vertices might change
// so they will be update whenever necessary (ie, whenever they are accessed by the program later).
// This function will also create a Bounding Box Entity, based on the loaded model. This is just to make it possible
// to render the bounding box with the player whenever necessary.
void Player::createBoundingBox()
{
	// Load player bounding box model
	this->boundingBoxModel = new Model(".\\res\\art\\carinhaloko\\carinhaloko_bb.obj");

	// Get bounding box meshes
	std::vector<Mesh*> boundingBoxMeshes = this->boundingBoxModel->getMeshes();

	// Create bounding box entity and transform
	this->boundingBoxEntity = new Entity(this->boundingBoxModel);
	this->boundingBoxEntity->getTransform().setWorldPosition(this->getTransform().getWorldPosition());
	this->boundingBoxEntity->getTransform().setWorldRotation(this->getTransform().getWorldRotation());
	this->boundingBoxEntity->getTransform().setWorldScale(this->getTransform().getWorldScale());

	// Iterate over bounding box meshes
	for (unsigned int i = 0; i < boundingBoxMeshes.size(); ++i)
	{
		// Get mesh vertices
		const std::vector<Vertex>& vertices = boundingBoxMeshes[i]->getVertices();

		// Allocate bounding shape vertices space (model coords)
		BoundingShape boundingBoxModelCoords;
		boundingBoxModelCoords.vertices = (vec3*)malloc(vertices.size() * sizeof(vec3));
		boundingBoxModelCoords.num_vertices = vertices.size();

		// Allocate bounding shape vertices space (world coords)
		BoundingShape boundingBoxWorldCoords;
		boundingBoxWorldCoords.vertices = (vec3*)malloc(vertices.size() * sizeof(vec3));
		boundingBoxWorldCoords.num_vertices = vertices.size();

		// Fill bounding shape with vertices (model coords)
		for (unsigned int j = 0; j < vertices.size(); ++j)
		{
			vec3 aux;
			aux.x = vertices[j].position.x;
			aux.y = vertices[j].position.y;
			aux.z = vertices[j].position.z;
			boundingBoxModelCoords.vertices[j] = aux;
		}

		// Push bounding boxes (world coordinates just allocaed, not initialized).
		this->boundingBoxInModelCoordinates.push_back(boundingBoxModelCoords);
		this->boundingBoxInWorldCoordinates.push_back(boundingBoxWorldCoords);
	}
}

// Create player camera
void Player::createCamera()
{
	glm::vec4 position = this->getTransform().getWorldPosition();
	glm::vec4 upVector = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 viewVector = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	this->camera = new PerspectiveCamera(position, upVector, viewVector);
}

// Create player gun model and entity
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

// Create player aim
void Player::createAim()
{
	// Create aim model and entity
	const static float aimScalement = 0.1f;
	Mesh* aimMesh = new Quad(Texture::load(".\\res\\art\\aim4.png"));
	Model* aimModel = new Model(std::vector<Mesh*>({ aimMesh }));
	this->aim = new Entity(aimModel);
	this->aim->getTransform().setWorldScale(glm::vec3(aimScalement, aimScalement, aimScalement));

	// Set fire animation off
	this->setIsShootingAnimationOn(false);
	this->shootingAnimationTime = 0;

	// Create first person gun model and entity
	Mesh* firstPersonGunMesh = new Quad(Texture::load(".\\res\\art\\gun_tex.png"));
	Model* firstPersonGunModel = new Model(std::vector<Mesh*>({ firstPersonGunMesh }));
	this->firstPersonGun = new Entity(firstPersonGunModel);

	// Create first person gun (firing) model and entity
	Mesh* firstPersonGunFiringMesh = new Quad(Texture::load(".\\res\\art\\gun_tex_f.png"));
	Model* firstPersonGunFiringModel = new Model(std::vector<Mesh*>({ firstPersonGunFiringMesh }));
	this->firstPersonGunFiring = new Entity(firstPersonGunFiringModel);
}

Camera* Player::getCamera()
{
	return this->camera;
}

// Render all shotmarks
void Player::renderShotMarks(const Shader& shader, const Camera& camera) const
{
	for (unsigned int i = 0; i < this->shotMarks.size(); ++i)
		this->shotMarks[i].entity->render(shader, camera, this->shotMarks[i].color);
}

// Render gun
void Player::renderGun(const Shader& shader, const Camera& camera, const std::vector<Light*>& lights, bool useNormalMap) const
{
	this->gun->render(shader, camera, lights, useNormalMap);
}

// Render aim and first person gun
// These are just textures that will be rendered in the middle of the screen by a special shader.
// Blend should be activated and depth test disabled, otherwise they may conflict.
void Player::renderScreenImages(const Shader& shader) const
{
	float windowRatio = (float)this->camera->getWindowHeight() / (float)this->camera->getWindowWidth();

	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	this->aim->render(shader, windowRatio);

	if (this->isShootingAnimationOn)
		this->firstPersonGunFiring->render(shader, windowRatio);
	else
		this->firstPersonGun->render(shader, windowRatio);

	if (this->isDamageAnimationOn)
		this->damageAnimationEntity->render(shader, windowRatio);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

// Update player
void Player::update(float deltaTime)
{
	glm::vec4 playerPosition = this->getTransform().getWorldPosition();

	// Refresh camera's position
	const float cameraHeight = 0.6f;
	glm::vec4 cameraPosition = playerPosition;
	cameraPosition.y = cameraHeight;
	this->camera->setPosition(cameraPosition);

	// Refresh model rotation
	glm::vec4 lookDirection = glm::normalize(this->camera->getViewVector());
	const static glm::vec4 initialLookDirection = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	glm::vec2 vecA = glm::normalize(glm::vec2(lookDirection.x, lookDirection.z));
	glm::vec2 vecB = glm::normalize(glm::vec2(initialLookDirection.x, initialLookDirection.z));
	float modelRotation = atan2f(vecB.x, vecB.y) - atan2f(vecA.x, vecA.y);
	this->getTransform().setWorldRotation(glm::vec3(0.0f, -modelRotation - PI_F, 0.0f));

	// Refresh gun position
	this->gun->getTransform().setWorldPosition(this->getTransform().getWorldPosition());
	//this->gun->getTransform().setWorldRotation(glm::vec3(this->camera.getYaw(), this->camera.getPitch(), 0));
	this->gun->getTransform().setWorldRotation(glm::vec3(0.0f, -modelRotation, 0.0f));

	// Refresh fire animation
	if (this->isShootingAnimationOn)
	{
		double currentTime = glfwGetTime();

		if (currentTime > this->shootingAnimationTime + shootingAnimationTotalTime)
			this->setIsShootingAnimationOn(false);
	}

	// Refresh damage animation
	if (this->isDamageAnimationOn)
	{
		double currentTime = glfwGetTime();

		if (currentTime > this->damageAnimationTime + damageAnimationTotalTime)
			this->setIsDamageAnimationOn(false);
	}

	// Refresh bouding box entity
	this->boundingBoxEntity->getTransform().setWorldPosition(this->getTransform().getWorldPosition());
	this->boundingBoxEntity->getTransform().setWorldRotation(this->getTransform().getWorldRotation());
	this->boundingBoxEntity->getTransform().setWorldScale(this->getTransform().getWorldScale());

	// Check if player is dead
	if (this->hp <= 0)
	{
		// Player is dead. @TODO
		// @TEMPORARY?
		this->getTransform().setWorldPosition(glm::vec4(1.7f, 0.0f, 1.7f, 1.0f));
		this->setHp(this->initialHp);
	}

	// Update player position interpolation, if activated
	if (this->isMovementInterpolationOn)
		this->interpolateMovement(deltaTime);
}

// Shoot, test collisions with second player and map walls.
// If network is not null, this function will also send collision information to the second player (multiplayer mode)
void Player::shoot(Player* secondPlayer, const std::vector<MapWallDescriptor>& mapWallDescriptors, Network* network)
{
	// Ray attributes
	glm::vec4 rayPosition = this->camera->getPosition();
	glm::vec4 rayDirection = this->camera->getViewVector();
	glm::vec4 rayXAxis = this->camera->getXAxis();
	glm::vec4 rayYAxis = this->camera->getYAxis();

	// Test collision with second Player
	PlayerCollisionDescriptor playerCollisionDescriptor = Collision::isRayCollidingWithBoundingBox(rayPosition,
		rayDirection, rayXAxis, rayYAxis, secondPlayer->getBoundingBoxInWorldCoordinates());

	// Test collision with map walls
	CollisionDescriptor mapWallsCollisionDescriptor = Collision::getClosestWallRayIsColliding(rayPosition, 
		rayDirection, mapWallDescriptors);

	// If there was a collision with second player AND a wall
	if (playerCollisionDescriptor.collision.collide && mapWallsCollisionDescriptor.collide)
	{
		// Check closer collision - wall or player?
		glm::vec4 secondPlayerCollisionPosition = secondPlayer->getTransform().getWorldPosition();	// Approximation
		glm::vec4 wallCollisionPosition = mapWallsCollisionDescriptor.worldPosition;

		float distancePlayerToPlayer = glm::length(rayPosition - secondPlayerCollisionPosition);
		float distancePlayerToWall = glm::length(rayPosition - wallCollisionPosition);

		if (distancePlayerToPlayer < distancePlayerToWall)
		{
			// Player is closer
			int damage = secondPlayer->damage(playerCollisionDescriptor.bodyPart);
			secondPlayer->startDamageAnimation();

			// If a network was received, send the collision information to the second player
			// Note: This packet will also send a fire animation! There is no need to send a fire animation packet
			if (network)
				network->sendPlayerFireHitAndAnimation(damage);
		}
		else
		{
			// Wall is Closer
			this->createShotMark(mapWallsCollisionDescriptor.worldPosition, wallShotMarkColor);

			// If a network was received, send just a fire animation to second player
			if (network)
				network->sendPlayerFireAnimation();
		}
	}
	// If there was a collision with second player only
	if (playerCollisionDescriptor.collision.collide && !mapWallsCollisionDescriptor.collide)
	{
		int damage = secondPlayer->damage(playerCollisionDescriptor.bodyPart);

		// If a network was received, send the collision information to the second player
		// Note: This packet will also send a fire animation! There is no need to send a fire animation packet
		if (network)
			network->sendPlayerFireHitAndAnimation(damage);
	}
	// If there was a collision with wall only
	if (!playerCollisionDescriptor.collision.collide && mapWallsCollisionDescriptor.collide)
	{
		this->createShotMark(mapWallsCollisionDescriptor.worldPosition, wallShotMarkColor);

		// If a network was received, send just a fire animation to second player
		if (network)
			network->sendPlayerFireAnimation();
	}
	// If there was no collision
	else
	{
		// If a network was received, send just a fire animation to second player
		if (network)
			network->sendPlayerFireAnimation();
	}
}

// Shoot, test collisions with second player and map walls.
// This is offline mode, secondPlayer should probably be a bot or something
void Player::shoot(Player* secondPlayer, const std::vector<MapWallDescriptor>& mapWallDescriptors)
{
	this->shoot(secondPlayer, mapWallDescriptors, 0);
}

// Shoot, test collisions with map walls only.
void Player::shoot(const std::vector<MapWallDescriptor>& mapWallDescriptors)
{
	// Ray attributes
	glm::vec4 rayPosition = this->camera->getPosition();
	glm::vec4 rayDirection = this->camera->getViewVector();

	// Test collision with map walls
	CollisionDescriptor mapWallsCollisionDescriptor = Collision::getClosestWallRayIsColliding(rayPosition,
		rayDirection, mapWallDescriptors);

	// If there was a collision
	if (mapWallsCollisionDescriptor.collide)
		this->createShotMark(mapWallsCollisionDescriptor.worldPosition, wallShotMarkColor);
}

// Start the shooting animation
void Player::startShootingAnimation()
{
	this->setIsShootingAnimationOn(true);
	this->shootingAnimationTime = glfwGetTime();
}

// Start the damage animation
void Player::startDamageAnimation()
{
	this->setIsDamageAnimationOn(true);
	this->damageAnimationTime = glfwGetTime();
}

// Change player look direction
void Player::changeLookDirection(float xDifference, float yDifference, float speed)
{
	this->camera->incPitch(-speed * (float)xDifference);
	this->camera->incYaw(speed * (float)yDifference);
}

// Change player look direction
void Player::changeLookDirection(const glm::vec4& lookDirection)
{
	this->camera->setViewVector(lookDirection);
}

glm::vec4 Player::getLookDirection() const
{
	return this->camera->getViewVector();
}

glm::vec4 Player::getPerpendicularDirection() const
{
	return this->camera->getXAxis();
}

// Set if shooting animation is on. Hide/unhide animation mesh accordingly.
void Player::setIsShootingAnimationOn(bool isShootingAnimationOn)
{
	this->isShootingAnimationOn = isShootingAnimationOn;
	this->gun->getModel()->getMeshes()[5]->setVisible(isShootingAnimationOn);
}

void Player::setIsDamageAnimationOn(bool isDamageAnimationOn)
{
	this->isDamageAnimationOn = isDamageAnimationOn;
}

// Get player bounding box in model coordinates. Always static.
const std::vector<BoundingShape>& Player::getBoundingBoxInModelCoordinates() const
{
	return this->boundingBoxInModelCoordinates;
}

// Get player bounding box in world coordinates. Dynamic, might change as player moves.
std::vector<BoundingShape>& Player::getBoundingBoxInWorldCoordinates()
{
	mat4 modelMatrix;
	glm::mat4 m;

	// For each boundingShape in model coordinates, multiply vertices by the model matrix by calling
	// transform_shape and passing the matrix. Store the result in boundingBoxInWorldCoordinates vector.
	for (unsigned int i = 0; i < this->boundingBoxInModelCoordinates.size(); ++i)
	{
		m = glm::transpose(this->boundingBoxEntity->getTransform().getModelMatrix());
		memcpy(modelMatrix.data, glm::value_ptr(m), 16 * sizeof(float));

		transform_shape(&this->boundingBoxInModelCoordinates[i], &this->boundingBoxInWorldCoordinates[i], modelMatrix);
	}

	// Return the bounding box in world coords.
	return this->boundingBoxInWorldCoordinates;
}

// This function receives an index identifying a bounding box mesh and returns the body part related.
PlayerBodyPart Player::getBoundingBoxBodyPart(unsigned int boundingBoxVectorIndex)
{
	// HARDCODED!
	switch (boundingBoxVectorIndex)
	{
	default:
	case 0: return PlayerBodyPart::TORSO;
	case 1: return PlayerBodyPart::HEAD;
	case 2: return PlayerBodyPart::RIGHTTHIGH;
	case 3: return PlayerBodyPart::LEFTTHIGH;
	case 4: return PlayerBodyPart::RIGHTSHIN;
	case 5: return PlayerBodyPart::LEFTSHIN;
	case 6: return PlayerBodyPart::LEFTFOOT;
	case 7: return PlayerBodyPart::RIGHTFOOT;
	case 8: return PlayerBodyPart::UPPERLEFTARM;
	case 9: return PlayerBodyPart::LOWERLEFTARM;
	case 10: return PlayerBodyPart::UPPERRIGHTARM;
	case 11: return PlayerBodyPart::LOWERRIGHTARM;
	case 12: return PlayerBodyPart::RIGHTHAND;
	case 13: return PlayerBodyPart::LEFTHAND;
	}
}

int Player::getHp() const
{
	return this->hp;
}

void Player::setHp(int hp)
{
	this->hp = hp;
}

void Player::removeHp(int damage)
{
	this->hp -= damage;

	if (this->hp < 0)
		this->hp = 0;
}

// Damage player. Value is decided based on the hit body part.
// Also, returns the damage.
int Player::damage(PlayerBodyPart bodyPart)
{
	switch (bodyPart)
	{
	case PlayerBodyPart::HEAD:
		this->removeHp(60);
		return 60;
	case PlayerBodyPart::TORSO:
		this->removeHp(30);
		return 30;
	case PlayerBodyPart::UPPERRIGHTARM:
	case PlayerBodyPart::LOWERRIGHTARM:
	case PlayerBodyPart::UPPERLEFTARM:
	case PlayerBodyPart::LOWERLEFTARM:
	case PlayerBodyPart::RIGHTHAND:
	case PlayerBodyPart::LEFTHAND:
		this->removeHp(16);
		return 16;
	case PlayerBodyPart::RIGHTTHIGH:
	case PlayerBodyPart::LEFTTHIGH:
	case PlayerBodyPart::RIGHTSHIN:
	case PlayerBodyPart::LEFTSHIN:
		this->removeHp(16);
		return 16;
	case PlayerBodyPart::RIGHTFOOT:
	case PlayerBodyPart::LEFTFOOT:
		this->removeHp(7);
		return 7;
	default:
		return 0;
	}
}

void Player::createShotMark(glm::vec4 position, glm::vec4 color)
{
	ShotMark shotMark;
	Entity* shotMarkEntity = new Entity(this->shotMarkModel);
	shotMarkEntity->getTransform().setWorldPosition(position);
	shotMark.entity = shotMarkEntity;
	shotMark.color = color;
	this->shotMarks.push_back(shotMark);
}
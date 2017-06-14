#include "Game.h"
#include "Model.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "DirectionalLight.h"
#include "GLFW\glfw3.h"

#define internal static

using namespace raw;

Game::Game()
{
	
}

Game::~Game()
{

}

void Game::init()
{
	// Create Shaders
	this->createShaders();

	// Create Camera
	this->camera = new Camera();

	// Create Lights
	this->createLights();

	// Create Entities
	this->createEntities();
}

void Game::render() const
{
	if (this->boundSpotLight)
	{
		this->boundSpotLight->setPosition(camera->getPosition());
		this->boundSpotLight->setDirection(camera->getViewVector());
	}

	switch (this->shaderType)
	{
		case ShaderType::BASIC:
			for (unsigned int i = 0; i < this->entities.size(); ++i)
				this->entities[i]->render(*this->basicShader, *this->camera, this->lights);
			break;
		case ShaderType::FLAT:
			for (unsigned int i = 0; i < this->entities.size(); ++i)
				this->entities[i]->render(*this->flatShader, *this->camera, this->lights);
			break;
		case ShaderType::GOURAD:
			for (unsigned int i = 0; i < this->entities.size(); ++i)
				this->entities[i]->render(*this->gouradShader, *this->camera, this->lights);
			break;
		case ShaderType::PHONG:
			for (unsigned int i = 0; i < this->entities.size(); ++i)
				this->entities[i]->render(*this->phongShader, *this->camera, this->lights);
			break;
	}
}

void Game::update(float deltaTime)
{
	// @TODO
}

void Game::destroy()
{
	// Destroy Shaders
	delete this->basicShader;
	delete this->phongShader;
	delete this->gouradShader;
	delete this->flatShader;

	// Destroy Lights
	for (unsigned int i = 0; i < this->lights.size(); ++i)
		delete this->lights[i];

	// Destroy Models
	for (unsigned int i = 0; i < this->entities.size(); ++i)
	{
		delete this->entities[i]->getModel();	// Delete Model
		delete this->entities[i];				// Delete Entity
	}

	this->lights.clear();
	this->entities.clear();
	this->basicShader = 0;
	this->phongShader = 0;
	this->gouradShader = 0;
	this->flatShader = 0;

	// Destroy Textures
	// @TODO: destroyAll can't destroy default diffuseMap and default specularMap,
	// because it should be possible to destroy and re-create Game class.
	// Texture::destroyAll();
}

void Game::processMouseChange(double xPos, double yPos)
{
	static double xPosOld, yPosOld;
	static bool firstTime = true;
	static const float cameraMouseSpeed = 0.005f;

	if (!firstTime)
	{
		double xDifference = xPos - xPosOld;
		double yDifference = yPos - yPosOld;

		this->camera->incPitch(-cameraMouseSpeed * (float)xDifference);
		this->camera->incYaw(cameraMouseSpeed * (float)yDifference);
	}
	else
		firstTime = false;

	xPosOld = xPos;
	yPosOld = yPos;
}

void Game::processInput(bool* keyState, float deltaTime)
{
	static const float cameraSpeed = 5.0f;
	static const float rotationSpeed = 5.0f;

	if (this->camera)
	{
		if (keyState[GLFW_KEY_W])
		{
			glm::vec4 cameraPos = this->camera->getPosition();
			this->camera->setPosition(cameraPos + cameraSpeed * deltaTime * this->camera->getViewVector());
		}
		if (keyState[GLFW_KEY_S])
		{
			glm::vec4 cameraPos = this->camera->getPosition();
			this->camera->setPosition(cameraPos - cameraSpeed * deltaTime * this->camera->getViewVector());
		}
		if (keyState[GLFW_KEY_A])
		{
			glm::vec4 cameraPos = this->camera->getPosition();
			this->camera->setPosition(cameraPos - cameraSpeed * deltaTime * this->camera->getXAxis());
		}
		if (keyState[GLFW_KEY_D])
		{
			glm::vec4 cameraPos = this->camera->getPosition();
			this->camera->setPosition(cameraPos + cameraSpeed * deltaTime * this->camera->getXAxis());
		}
	}

	if (this->boundEntity)
	{
		if (keyState[GLFW_KEY_X])
			boundEntity->getTransform().incRotX(rotationSpeed * deltaTime);
		if (keyState[GLFW_KEY_Y])
			boundEntity->getTransform().incRotY(rotationSpeed * deltaTime);
		if (keyState[GLFW_KEY_Z])
			boundEntity->getTransform().incRotZ(rotationSpeed * deltaTime);
	}

	if (keyState[GLFW_KEY_B])
	{
		shaderType = raw::ShaderType::BASIC;
		keyState[GLFW_KEY_B] = false;					// Force false to only compute one time.
	}
	if (keyState[GLFW_KEY_F])
	{
		shaderType = raw::ShaderType::FLAT;
		keyState[GLFW_KEY_F] = false;					// Force false to only compute one time.
	}
	if (keyState[GLFW_KEY_G])
	{
		shaderType = raw::ShaderType::GOURAD;
		keyState[GLFW_KEY_G] = false;					// Force false to only compute one time.
	}
	if (keyState[GLFW_KEY_P])
	{
		shaderType = raw::ShaderType::PHONG;
		keyState[GLFW_KEY_P] = false;					// Force false to only compute one time.
	}
}

void Game::createShaders()
{
	this->basicShader = new Shader(ShaderType::BASIC);
	this->phongShader = new Shader(ShaderType::PHONG);
	this->gouradShader = new Shader(ShaderType::GOURAD);
	this->flatShader = new Shader(ShaderType::FLAT);
	this->shaderType = ShaderType::PHONG;
}

void Game::createLights()
{
	glm::vec4 ambientLight = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	glm::vec4 diffuseLight = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	glm::vec4 specularLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	LightAttenuation attenuation = {
		1.0f,		// Constant Term
		0.14f,		// Linear Term
		0.07f		// Quadratic Term
	};

	// POINT LIGHT
	glm::vec4 plPosition = glm::vec4(-1.5f, 0.5f, 0.0f, 1.0f);
	PointLight* pointLight = new PointLight(plPosition, ambientLight, diffuseLight, specularLight);
	pointLight->setAttenuation(attenuation);

	// SPOT LIGHT
	glm::vec4 slPosition = glm::vec4(0, 0, 1, 1);
	SpotLight* spotLight = new SpotLight(slPosition, ambientLight, diffuseLight, specularLight);
	spotLight->setAttenuation(attenuation);
	spotLight->setInnerCutOffAngle(glm::radians(12.5f));
	spotLight->setOuterCutOffAngle(glm::radians(17.5f));
	spotLight->setDirection(glm::vec4(0, 0, -1, 0));
	this->boundSpotLight = spotLight;	// @TEMPORARY

	// DIRECTIONAL LIGHT
	glm::vec4 dlDirection = glm::vec4(-0.2f, -1.0f, -0.3f, 0.0f);
	DirectionalLight* directionalLight = new DirectionalLight(dlDirection, ambientLight, diffuseLight,
		specularLight);

	// VECTOR CREATION
	this->lights.push_back(pointLight);
	this->lights.push_back(spotLight);
	this->lights.push_back(directionalLight);
}

void Game::createEntities()
{
	// CREATE A CUBE (cube.obj)
	Texture* diffuseMap = Texture::load(".\\res\\green.png");
	Model* cubeModel = new Model(".\\res\\cube.obj");
	Entity* cubeEntity = new Entity(cubeModel);
	cubeModel->setDiffuseMapOfAllMeshes(diffuseMap);
	cubeEntity->getTransform().setWorldPosition(glm::vec4(-8.0f, 0.0f, 9.0f, 1.0f));
	cubeEntity->getTransform().setWorldRotation(glm::vec3(0.35f, 0.77f, 0.12f));

	// CREATE COMPLEX MODEL (nanosuit.obj)
	Model* complexModel = new Model(".\\res\\nanosuit\\nanosuit.obj");
	Entity* complexEntity = new Entity(complexModel);
	complexEntity->getTransform().setWorldPosition(glm::vec4(-5.0f, 0.0f, 0.0f, 1.0f));

	// CREATE BUNNY MODEL (bunny.obj)
	Texture* bunnyDiffuseMap = Texture::load(".\\res\\blue2.png");
	Texture* bunnySpecularMap = Texture::load(".\\res\\specBunny.png");
	Model* bunnyModel = new Model(".\\res\\bunny.obj");
	bunnyModel->setDiffuseMapOfAllMeshes(bunnyDiffuseMap);
	bunnyModel->setSpecularMapOfAllMeshes(bunnySpecularMap);
	bunnyModel->setSpecularShinenessOfAllMeshes(32.0f);
	Entity* bunnyEntity = new Entity(bunnyModel);
	bunnyEntity->getTransform().setWorldPosition(glm::vec4(0.0f, 0.0f, -10.0f, 1.0f));
	this->boundEntity = bunnyEntity;		// @TEMPORARY

	// CREATE TREE MODEL (arvore.obj)
	Texture* arvoreTexture = Texture::load(".\\res\\arvore_tex.png");
	Model* arvoreModel = new Model(".\\res\\arvore.obj");
	Entity* arvoreEntity = new Entity(arvoreModel);
	arvoreModel->setDiffuseMapOfAllMeshes(arvoreTexture);
	arvoreEntity->getTransform().setWorldPosition(glm::vec4(0.0f, 0.0f, 6.0f, 1.0f));

	// CREATE SPHERE MODEL (sphere.obj)
	Texture* sphereTexture = Texture::load(".\\res\\blue2.png");
	Model* sphereModel = new Model(".\\res\\sphere.obj");
	Entity* sphereEntity = new Entity(sphereModel);
	sphereModel->getMeshes()[0]->setDiffuseMap(sphereTexture);
	arvoreEntity->getTransform().setWorldPosition(glm::vec4(1.0f, 0.0f, 10.0f, 1.0f));

	// CREATE QUAD
	Texture* quadTexture = Texture::load(".\\res\\smile.png");
	Quad* quad = new Quad(quadTexture);
	std::vector<Mesh*> quadMeshes = std::vector<Mesh*>({ quad });
	Model* quadModel = new Model(quadMeshes);
	Entity* quadEntity = new Entity(quadModel);
	quadEntity->getTransform().setWorldPosition(glm::vec4(10.5f, 0.0f, 10.0f, 1.0f));

	// VECTOR CREATION
	this->entities.push_back(cubeEntity);
	this->entities.push_back(complexEntity);
	this->entities.push_back(bunnyEntity);
	this->entities.push_back(arvoreEntity);
	this->entities.push_back(sphereEntity);
	this->entities.push_back(quadEntity);
}
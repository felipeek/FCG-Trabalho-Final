#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Entity.h"
#include "StaticModels.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Result.exe"

#define internal static

internal bool keyState[1024];	// @TODO: Check range.

internal raw::Entity* boundEntity;
internal raw::Entity* boundMovement;
internal raw::SpotLight* boundFlashlight;
internal const float rotationSpeed = 5.0f;
internal raw::Camera* boundCamera;
internal const float cameraSpeed = 5.0f;
internal raw::ShaderType shaderType = raw::ShaderType::PHONG;

internal float deltaTime;

void glfwKeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (action == GLFW_PRESS)
		keyState[key] = true;
	if (action == GLFW_RELEASE)
		keyState[key] = false;

	if (key == GLFW_KEY_B && action == GLFW_PRESS)
		shaderType = raw::ShaderType::BASIC;
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
		shaderType = raw::ShaderType::FLAT;
	if (key == GLFW_KEY_G && action == GLFW_PRESS)
		shaderType = raw::ShaderType::GOURAD;
	if (key == GLFW_KEY_P && action == GLFW_PRESS)
		shaderType = raw::ShaderType::PHONG;
}

void glfwCursorCallback(GLFWwindow* window, double xPos, double yPos)
{
	static double xPosOld, yPosOld;
	static bool firstTime = true;
	static const float cameraMouseSpeed = 0.005f;
		
	if (!firstTime && boundCamera)
	{
		double xDifference = xPos - xPosOld;
		double yDifference = yPos - yPosOld;
		
		boundCamera->incPitch(-cameraMouseSpeed * (float)xDifference);
		boundCamera->incYaw(cameraMouseSpeed * (float)yDifference);
	}
	else
		firstTime = false;
		
	xPosOld = xPos;
	yPosOld = yPos;
}

GLFWwindow* initGlfw()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, 0, 0);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, glfwKeyCallback);
	glfwSetCursorPosCallback(window, glfwCursorCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return window;
}

void initGlew()
{
	glewExperimental = true;
	glewInit();
}

void refreshKeys()
{
	if (keyState[GLFW_KEY_W] && boundCamera)
	{
		glm::vec4 cameraPos = boundCamera->getPosition();
		boundCamera->setPosition(cameraPos + cameraSpeed * deltaTime * boundCamera->getViewVector());
	}
	if (keyState[GLFW_KEY_S] && boundCamera)
	{
		glm::vec4 cameraPos = boundCamera->getPosition();
		boundCamera->setPosition(cameraPos - cameraSpeed * deltaTime * boundCamera->getViewVector());
	}
	if (keyState[GLFW_KEY_A] && boundCamera)
	{
		glm::vec4 cameraPos = boundCamera->getPosition();
		boundCamera->setPosition(cameraPos - cameraSpeed * deltaTime * boundCamera->getXAxis());
	}
	if (keyState[GLFW_KEY_D] && boundCamera)
	{
		glm::vec4 cameraPos = boundCamera->getPosition();
		boundCamera->setPosition(cameraPos + cameraSpeed * deltaTime * boundCamera->getXAxis());
	}
	if (keyState[GLFW_KEY_X] && boundEntity)
	{
		boundEntity->getTransform().incRotX(rotationSpeed * deltaTime);
	}
	if (keyState[GLFW_KEY_Y] && boundEntity)
	{
		boundEntity->getTransform().incRotY(rotationSpeed * deltaTime);
	}
	if (keyState[GLFW_KEY_Z] && boundEntity)
	{
		boundEntity->getTransform().incRotZ(rotationSpeed * deltaTime);
	}
	if (keyState[GLFW_KEY_UP] && boundMovement)
	{
		glm::vec4 newPos = boundMovement->getTransform().getWorldPosition();
		newPos.z += deltaTime;
		boundMovement->getTransform().setWorldPosition(newPos);
	}
	if (keyState[GLFW_KEY_DOWN] && boundMovement)
	{
		glm::vec4 newPos = boundMovement->getTransform().getWorldPosition();
		newPos.z -= deltaTime;
		boundMovement->getTransform().setWorldPosition(newPos);
	}
	if (keyState[GLFW_KEY_RIGHT] && boundMovement)
	{
		glm::vec4 newPos = boundMovement->getTransform().getWorldPosition();
		newPos.x += deltaTime;
		boundMovement->getTransform().setWorldPosition(newPos);
	}
	if (keyState[GLFW_KEY_LEFT] && boundMovement)
	{
		glm::vec4 newPos = boundMovement->getTransform().getWorldPosition();
		newPos.x -= deltaTime;
		boundMovement->getTransform().setWorldPosition(newPos);
	}
}

std::vector<raw::Light*> createLights()
{
	glm::vec4 ambientLight = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	glm::vec4 diffuseLight = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	glm::vec4 specularLight = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	raw::LightAttenuation attenuation = {
		1.0f,		// Constant Term
		0.14f,		// Linear Term
		0.07f		// Quadratic Term
	};

	// POINT LIGHT
	glm::vec4 plPosition = glm::vec4(-1.5f, 0.5f, 0.0f, 1.0f);
	raw::PointLight* pointLight = new raw::PointLight(plPosition, ambientLight, diffuseLight, specularLight);
	pointLight->setAttenuation(attenuation);

	// SPOT LIGHT
	glm::vec4 slPosition = glm::vec4(0, 0, 1, 1);
	raw::SpotLight* spotLight = new raw::SpotLight(slPosition, ambientLight, diffuseLight, specularLight);
	spotLight->setAttenuation(attenuation);
	spotLight->setInnerCutOffAngle(glm::radians(12.5f));
	spotLight->setOuterCutOffAngle(glm::radians(17.5f));
	spotLight->setDirection(glm::vec4(0, 0, -1, 0));
	boundFlashlight = spotLight;

	// DIRECTIONAL LIGHT
	glm::vec4 dlDirection = glm::vec4(-0.2f, -1.0f, -0.3f, 0.0f);
	raw::DirectionalLight* directionalLight = new raw::DirectionalLight(dlDirection, ambientLight, diffuseLight,
		specularLight);

	// VECTOR CREATION
	std::vector<raw::Light*> lights = std::vector<raw::Light*>();
	lights.push_back(pointLight);
	lights.push_back(spotLight);
	lights.push_back(directionalLight);
	return lights;
}

void freeLights(std::vector<raw::Light*>& lights)
{
	for (unsigned int i = 0; i < lights.size(); ++i)
		delete lights[i];

	lights.clear();
}

int main()
{
	GLFWwindow* mainWindow = initGlfw();
	initGlew();

	raw::Shader* basicShader = new raw::Shader(raw::ShaderType::BASIC);
	raw::Shader* phongShader = new raw::Shader(raw::ShaderType::PHONG);
	raw::Shader* gouradShader = new raw::Shader(raw::ShaderType::GOURAD);
	raw::Shader* flatShader = new raw::Shader(raw::ShaderType::FLAT);

	raw::Camera* camera = new raw::Camera();
	boundCamera = camera;

	std::vector<raw::Light*> lights = createLights();

	// CREATE A CUBE (cube.obj)
	raw::Texture* diffuseMap = raw::Texture::load(".\\res\\green.png");
	raw::Model* cubeModel = new raw::Model(".\\res\\cube.obj");
	raw::Entity* cubeEntity = new raw::Entity(cubeModel);
	cubeModel->setDiffuseMapOfAllMeshes(diffuseMap);
	cubeEntity->getTransform().setWorldPosition(glm::vec4(-8.0f, 0.0f, 9.0f, 1.0f));
	cubeEntity->getTransform().setWorldRotation(glm::vec3(0.35f, 0.77f, 0.12f));

	// CREATE COMPLEX MODEL (nanosuit.obj)
	raw::Model* complexModel = new raw::Model(".\\res\\nanosuit\\nanosuit.obj");
	raw::Entity* complexEntity = new raw::Entity(complexModel);
	complexEntity->getTransform().setWorldPosition(glm::vec4(-5.0f, 0.0f, 0.0f, 1.0f));

	// CREATE BUNNY MODEL (bunny.obj)
	raw::Texture* bunnyDiffuseMap = raw::Texture::load(".\\res\\blue2.png");
	raw::Texture* bunnySpecularMap = raw::Texture::load(".\\res\\specBunny.png");
	raw::Model* bunnyModel = new raw::Model(".\\res\\bunny.obj");
	bunnyModel->setDiffuseMapOfAllMeshes(bunnyDiffuseMap);
	bunnyModel->setSpecularMapOfAllMeshes(bunnySpecularMap);
	bunnyModel->setSpecularShinenessOfAllMeshes(32.0f);
	raw::Entity* bunnyEntity = new raw::Entity(bunnyModel);
	bunnyEntity->getTransform().setWorldPosition(glm::vec4(0.0f, 0.0f, -10.0f, 1.0f));

	// CREATE TREE MODEL (arvore.obj)
	raw::Texture* arvoreTexture = raw::Texture::load(".\\res\\arvore_tex.png");
	raw::Model* arvoreModel = new raw::Model(".\\res\\arvore.obj");
	raw::Entity* arvoreEntity = new raw::Entity(arvoreModel);
	arvoreModel->setDiffuseMapOfAllMeshes(arvoreTexture);
	arvoreEntity->getTransform().setWorldPosition(glm::vec4(0.0f, 0.0f, 6.0f, 1.0f));

	// CREATE SPHERE MODEL (sphere.obj)
	raw::Texture* sphereTexture = raw::Texture::load(".\\res\\blue2.png");
	raw::Model* sphereModel = new raw::Model(".\\res\\sphere.obj");
	raw::Entity* sphereEntity = new raw::Entity(sphereModel);
	sphereModel->getMeshes()[0]->setDiffuseMap(sphereTexture);
	arvoreEntity->getTransform().setWorldPosition(glm::vec4(1.0f, 0.0f, 10.0f, 1.0f));

	// CREATE QUAD
	raw::Texture* quadTexture = raw::Texture::load(".\\res\\smile.png");
	raw::Quad* quad = new raw::Quad(quadTexture);
	std::vector<raw::Mesh*> quadMeshes = std::vector<raw::Mesh*>({ quad });
	raw::Model* quadModel = new raw::Model(quadMeshes);
	raw::Entity* quadEntity = new raw::Entity(quadModel);
	quadEntity->getTransform().setWorldPosition(glm::vec4(10.5f, 0.0f, 10.0f, 1.0f));

	glEnable(GL_DEPTH_TEST);
	glLineWidth(10);

	double lastFrame = glfwGetTime();
	int frameNumber = (int)lastFrame;
	unsigned int fps = 0;

	while (!glfwWindowShouldClose(mainWindow))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		if (boundFlashlight)
		{
			boundFlashlight->setPosition(camera->getPosition());
			boundFlashlight->setDirection(camera->getViewVector());
		}

		if (shaderType == raw::ShaderType::BASIC)
		{
			cubeEntity->render(*basicShader, *camera, lights);
			complexEntity->render(*basicShader, *camera, lights);
			arvoreEntity->render(*basicShader, *camera, lights);
			quadEntity->render(*basicShader, *camera, lights);
			bunnyEntity->render(*basicShader, *camera, lights);
			sphereEntity->render(*basicShader, *camera, lights);
		}
		else if (shaderType == raw::ShaderType::FLAT)
		{
			cubeEntity->render(*flatShader, *camera, lights);
			complexEntity->render(*flatShader, *camera, lights);
			arvoreEntity->render(*flatShader, *camera, lights);
			quadEntity->render(*flatShader, *camera, lights);
			bunnyEntity->render(*flatShader, *camera, lights);
			sphereEntity->render(*flatShader, *camera, lights);
		}
		else if (shaderType == raw::ShaderType::GOURAD)
		{
			cubeEntity->render(*gouradShader, *camera, lights);
			complexEntity->render(*gouradShader, *camera, lights);
			arvoreEntity->render(*gouradShader, *camera, lights);
			quadEntity->render(*gouradShader, *camera, lights);
			bunnyEntity->render(*gouradShader, *camera, lights);
			sphereEntity->render(*gouradShader, *camera, lights);
		}
		else if (shaderType == raw::ShaderType::PHONG)
		{
			cubeEntity->render(*phongShader, *camera, lights);
			complexEntity->render(*phongShader, *camera, lights);
			arvoreEntity->render(*phongShader, *camera, lights);
			quadEntity->render(*phongShader, *camera, lights);
			bunnyEntity->render(*phongShader, *camera, lights);
			sphereEntity->render(*phongShader, *camera, lights);
		}

		refreshKeys();
		glfwPollEvents();
		glfwSwapBuffers(mainWindow);

		double currentFrame = glfwGetTime();
		if ((int)currentFrame > frameNumber)
		{
			std::cout << "FPS: " << fps << std::endl;
			fps = 0;
			frameNumber++;
		}
		else
			++fps;

		deltaTime = (float)(currentFrame - lastFrame);
		lastFrame = currentFrame;
	}

	delete basicShader;
	delete phongShader;
	delete gouradShader;
	delete camera;

	raw::Texture::destroyAll();
	freeLights(lights);
	glfwTerminate();
}
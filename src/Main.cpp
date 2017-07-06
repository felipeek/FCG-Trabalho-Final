#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <Windows.h>
#include <time.h>
#include "Game.h"
#include "Application.h"

#define WINDOW_TITLE "Result.exe"

int windowWidth = 1366;
int windowHeight = 768;

static bool keyState[1024];	// @TODO: Check range.
static float deltaTime;
static raw::Application* application;

void glfwKeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	if (action == GLFW_PRESS)
		keyState[key] = true;
	if (action == GLFW_RELEASE)
		keyState[key] = false;
}

void glfwCursorCallback(GLFWwindow* window, double xPos, double yPos)
{
	application->processMouseChange(xPos, yPos);
}

void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	application->processMouseClick(button, action);	
}

void glfwScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	application->processScrollChange(xOffset, yOffset);
}

void glfwResizeCallback(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, width, height);
	application->processWindowResize(width, height);
}

GLFWwindow* initGlfw()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, WINDOW_TITLE, 0, 0);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, glfwKeyCallback);
	glfwSetCursorPosCallback(window, glfwCursorCallback);
	glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
	glfwSetScrollCallback(window, glfwScrollCallback);
	glfwSetWindowSizeCallback(window, glfwResizeCallback);
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
}

int main()
{
	srand(time(NULL));	// init random seed

	GLFWwindow* mainWindow = initGlfw();
	initGlew();
	application = new raw::Application(windowWidth, windowHeight);
	application->processWindowResize(windowWidth, windowHeight);	// Force application to process window size

	glEnable(GL_DEPTH_TEST);
	glLineWidth(10);

	double lastFrame = glfwGetTime();
	int frameNumber = (int)lastFrame;
	unsigned int fps = 0;

	while (!glfwWindowShouldClose(mainWindow))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClearColor(0x7E/255.0f, 0xC0/255.0f, 0xEE/255.0f, 1.0f);
		glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

		application->update(deltaTime);
		application->render();
		application->processInput(keyState, deltaTime);
	
		if (application->shouldExit())
			glfwSetWindowShouldClose(mainWindow, GLFW_TRUE);

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

	delete application;
	glfwTerminate();
}
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <Windows.h>
#include "Game.h"

#define WINDOW_TITLE "Result.exe"

int windowWidth = 800;
int windowHeight = 600;

static bool keyState[1024];	// @TODO: Check range.
static float deltaTime;
static raw::Game* game;

void glfwKeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (action == GLFW_PRESS)
		keyState[key] = true;
	if (action == GLFW_RELEASE)
		keyState[key] = false;
}

void glfwCursorCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (game)
		game->processMouseChange(xPos, yPos);
}

void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	game->processMouseClick(button, action);	
}

GLFWwindow* initGlfw()
{
	glfwInit();
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, WINDOW_TITLE, 0, 0);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, glfwKeyCallback);
	glfwSetCursorPosCallback(window, glfwCursorCallback);
	glfwSetMouseButtonCallback(window, glfwMouseButtonCallback);
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
	GLFWwindow* mainWindow = initGlfw();
	initGlew();

	game = new raw::Game();

	game->init(true);

	glEnable(GL_DEPTH_TEST);
	glLineWidth(10);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	double lastFrame = glfwGetTime();
	int frameNumber = (int)lastFrame;
	unsigned int fps = 0;

	while (!glfwWindowShouldClose(mainWindow))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClearColor(0x7E/255.0f, 0xC0/255.0f, 0xEE/255.0f, 1.0f);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		game->update(deltaTime);
		game->render();
		game->processInput(keyState, deltaTime);

		refreshKeys();
		glfwPollEvents();
		glfwSwapBuffers(mainWindow);

		double currentFrame = glfwGetTime();
		if ((int)currentFrame > frameNumber)
		{
		//	std::cout << "FPS: " << fps << std::endl;
			fps = 0;
			frameNumber++;
		}
		else
			++fps;

		deltaTime = (float)(currentFrame - lastFrame);

		lastFrame = currentFrame;
	}

	game->destroy();
	glfwTerminate();
}
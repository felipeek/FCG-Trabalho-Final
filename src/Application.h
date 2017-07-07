#pragma once

#include "Game.h"
#include "MenuScene.h"
#include <Windows.h>

namespace raw
{
	enum class ApplicationState
	{
		INITIALMENU,
		GAMERUNNING,
		GAMERESULTS
	};

	enum class InitialMenuSelection
	{
		SINGLEPLAYER,
		TWOPLAYERS
	};

	class Application
	{
	public:
		Application(int windowWidth, int windowHeight);
		~Application();
		void processInput(bool* keyState, float deltaTime);
		bool shouldExit() const;
		void render();
		void update(float deltaTime);
		void processMouseChange(double xPos, double yPos);
		void processMouseClick(int button, int action);
		void processScrollChange(int xOffset, int yOffset);
		void processWindowResize(int windowWidth, int windowHeight);
	private:
		void createAndRunGame();
		ApplicationState applicationState;
		MenuScene* menuScene;
		Game* activeGame;
		Shader* fixedShader;
		int windowHeight;
		int windowWidth;
		bool bExit;

		// Initial Menu
		Entity* initialMenuEntity;
		InitialMenuSelection initialMenuSelection;

		// Game results
		Entity* gameResultsEntity;
	};
}
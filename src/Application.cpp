#include "Application.h"
#include <GLFW\glfw3.h>
#include "resource.h"

using namespace raw;

const int udpValidPortRangeBegin = 1058;
const int udpValidPortRangeEnd = 47808;
char connectionDialogIp[256];
int connectionDialogPort;

BOOL CALLBACK processConnectionDialogEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool isStringNumber(char* string);

Application::Application(int windowWidth, int windowHeight)
{
	this->bExit = false;
	this->activeGame = new Game();
	this->applicationState = ApplicationState::INITIALMENU;
	this->initialMenuSelection = InitialMenuSelection::SINGLEPLAYER;

	// Create shader
	this->fixedShader = new Shader(ShaderType::FIXED);

	// Create initial menu screen
	Mesh* initialMenuMesh = new Quad(Texture::load(".\\res\\menu\\single.png"));
	Model* initialMenuModel = new Model(std::vector<Mesh*>({ initialMenuMesh }));
	this->initialMenuEntity = new Entity(initialMenuModel);

	// Create game results screen
	Mesh* gameResultsMesh = new Quad(Texture::load(".\\res\\menu\\win.png"));
	Model* gameResultsModel = new Model(std::vector<Mesh*>({ gameResultsMesh }));
	this->gameResultsEntity = new Entity(gameResultsModel);

	// Update window height and width
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
	
	// Create menu scene
	this->menuScene = new MenuScene();
}

Application::~Application()
{
	delete this->activeGame;
	delete this->fixedShader;
	delete this->initialMenuEntity->getModel();
	delete this->initialMenuEntity;
	delete this->menuScene;
}

bool Application::shouldExit() const
{
	return this->bExit;
}

void Application::render()
{
	float windowRatio = (float)this->windowHeight / (float)this->windowWidth;

	switch (this->applicationState)
	{
	case ApplicationState::INITIALMENU:
		this->menuScene->render();
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		this->initialMenuEntity->render(*this->fixedShader, windowRatio);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		break;
	case ApplicationState::GAMERUNNING:
		this->activeGame->render();
		break;
	case ApplicationState::GAMERESULTS:
		this->menuScene->render();
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		this->gameResultsEntity->render(*this->fixedShader, windowRatio);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	default:
		break;
	}
}

void Application::update(float deltaTime)
{
	switch (this->applicationState)
	{
	case ApplicationState::INITIALMENU:
		this->menuScene->update(deltaTime);
		break;
	case ApplicationState::GAMERUNNING:
		if (!this->activeGame->shouldExit())
			this->activeGame->update(deltaTime);
		else
		{
			GameExitInfo exitInfo = this->activeGame->getExitInfo();
			this->activeGame->destroy();

			if (exitInfo.forcedExit)
				this->applicationState = ApplicationState::INITIALMENU;
			else
			{
				if (exitInfo.win)
				{
					Texture* winTex = Texture::load(".\\res\\menu\\win.png");
					this->gameResultsEntity->getModel()->setDiffuseMapOfAllMeshes(winTex);
				}
				else
				{
					Texture* loseTex = Texture::load(".\\res\\menu\\lose.png");
					this->gameResultsEntity->getModel()->setDiffuseMapOfAllMeshes(loseTex);
				}

				this->applicationState = ApplicationState::GAMERESULTS;
			}
		}
		break;
	case ApplicationState::GAMERESULTS:
		this->menuScene->update(deltaTime);
		break;
	default:
		break;
	}
}

void Application::processMouseChange(double xPos, double yPos)
{
	if (this->applicationState == ApplicationState::GAMERUNNING)
		this->activeGame->processMouseChange(xPos, yPos);
}

void Application::processMouseClick(int button, int action)
{
	if (this->applicationState == ApplicationState::GAMERUNNING)
		this->activeGame->processMouseClick(button, action);
}

void Application::processScrollChange(int xOffset, int yOffset)
{
	if (this->applicationState == ApplicationState::GAMERUNNING)
		this->activeGame->processScrollChange(xOffset, yOffset);
}

void Application::processWindowResize(int windowWidth, int windowHeight)
{
	if (this->applicationState == ApplicationState::GAMERUNNING)
		this->activeGame->processWindowResize(windowWidth, windowHeight);

	// Update window height and width
	this->windowWidth = windowWidth;
	this->windowHeight = windowHeight;
}

void Application::processInput(bool* keyState, float deltaTime)
{
	switch (applicationState)
	{
	case ApplicationState::INITIALMENU:
		if (keyState[GLFW_KEY_ENTER])
		{
			this->createAndRunGame();
			keyState[GLFW_KEY_ENTER] = false;
		}
		if (keyState[GLFW_KEY_ESCAPE])
		{
			this->bExit = true;
			keyState[GLFW_KEY_ENTER] = false;
		}
		if (keyState[GLFW_KEY_UP] || keyState[GLFW_KEY_DOWN])
		{
			if (initialMenuSelection == InitialMenuSelection::SINGLEPLAYER)
			{
				initialMenuSelection = InitialMenuSelection::TWOPLAYERS;
				Texture* twoPlayersTex = Texture::load(".\\res\\menu\\two.png");
				this->initialMenuEntity->getModel()->setDiffuseMapOfAllMeshes(twoPlayersTex);
			}
			else
			{
				initialMenuSelection = InitialMenuSelection::SINGLEPLAYER;
				Texture* singlePlayerTex = Texture::load(".\\res\\menu\\single.png");
				this->initialMenuEntity->getModel()->setDiffuseMapOfAllMeshes(singlePlayerTex);
			}

			keyState[GLFW_KEY_UP] = false;
			keyState[GLFW_KEY_DOWN] = false;
		}
		break;
	case ApplicationState::GAMERUNNING:
		this->activeGame->processInput(keyState, deltaTime);
		break;
	case ApplicationState::GAMERESULTS:
		if (keyState[GLFW_KEY_ENTER])
		{
			this->applicationState = ApplicationState::INITIALMENU;
			keyState[GLFW_KEY_ENTER] = false;
		}
		break;
	}
}

void Application::createAndRunGame()
{
	GameSettings gameSettings;

	if (this->initialMenuSelection == InitialMenuSelection::SINGLEPLAYER)
	{
		gameSettings.singlePlayer = true;
		this->activeGame->init(gameSettings);
		this->activeGame->processWindowResize(this->windowWidth, this->windowHeight);
		this->applicationState = ApplicationState::GAMERUNNING;
	}
	else
	{
		if (DialogBox(0, MAKEINTRESOURCE(IDD_DIALOG1), 0, (DLGPROC)processConnectionDialogEvent) == OK_BUTTON)
		{
			strcpy(gameSettings.ip, connectionDialogIp);
			gameSettings.port = connectionDialogPort;
			gameSettings.singlePlayer = false;

			this->activeGame->init(gameSettings);
			this->activeGame->processWindowResize(this->windowWidth, this->windowHeight);
			this->applicationState = ApplicationState::GAMERUNNING;
		}
	}
}

BOOL CALLBACK processConnectionDialogEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case OK_BUTTON:
				{
					char receivedIp[256];
					char receivedPort[256];

					if (GetDlgItemText(hWnd, ID_FIELD, receivedIp, 256) &&
						GetDlgItemText(hWnd, PORT_FIELD, receivedPort, 256))
					{
						if (isStringNumber(receivedPort))
						{
							int portInteger = std::atoi(receivedPort);

							if (portInteger >= udpValidPortRangeBegin && portInteger <= udpValidPortRangeEnd)
							{
								// Copy values to global variables
								strcpy(connectionDialogIp, receivedIp);
								connectionDialogPort = portInteger;

								EndDialog(hWnd, wParam);
								return TRUE;
							}
						}
					}

					MessageBox(hWnd, "Invalid Format.", "Error", MB_OK);
					return FALSE;
				} break;
				case CANCEL_BUTTON:
				{
					EndDialog(hWnd, wParam);
					return TRUE;
				} break;
			}
		} break;
	}

	return FALSE;
}

bool isStringNumber(char* string)
{
	int i = 0;

	while (string[i] != 0)
	{
		if (string[i] < '0' || string[i] > '9')
			return false;
		++i;
	}

	if (i != 0)
		return true;
}
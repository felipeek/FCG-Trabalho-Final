#include "Network.h"
#include "Game.h"

using namespace raw;

#define DEBUG
//#define SHOW_PERIODIC_PACKETS

#ifdef DEBUG
#include <iostream>
#endif

enum PacketType
{
	PLAYER_INFORMATION = 1,
	PLAYER_FIRE_ANIMATION = 2,
	PLAYER_FIRE_ANIMATION_WITH_WALL_SHOT_MARK = 3,
	PLAYER_FIRE_HIT = 4,
};

Network::Network(Game* game, char* peerIp, unsigned int peerPort)
{
	this->peerPort = peerPort;
	this->udpReceiver = new UDPReceiver(this->peerPort);
	this->udpSender = new UDPSender(peerIp, this->peerPort);
	this->boundGame = game;
}

Network::~Network()
{
	delete this->udpReceiver;
	delete this->udpSender;
}

void Network::sendPlayerInformation(const Player& player)
{
	const unsigned int packetId = PLAYER_INFORMATION;
	glm::vec4 playerPosition = player.getTransform().getWorldPosition();
	glm::vec4 playerLookDirection = player.getLookDirection();
	glm::vec4 playerVelocity = player.getVelocity();
	glm::vec4 playerAcceleration = player.getAcceleration();

	const unsigned int bufferSize = sizeof(packetId) + sizeof(playerPosition) + sizeof(playerLookDirection) +
		sizeof(playerVelocity) + sizeof(playerAcceleration);
	char buffer[bufferSize];
	memcpy(buffer, &packetId, sizeof(packetId));
	memcpy(buffer + sizeof(packetId), &playerPosition, sizeof(playerPosition));
	memcpy(buffer + sizeof(packetId) + sizeof(playerPosition), &playerLookDirection, sizeof(playerLookDirection));
	memcpy(buffer + sizeof(packetId) + sizeof(playerPosition) + sizeof(playerLookDirection),
		&playerVelocity, sizeof(playerVelocity));
	memcpy(buffer + sizeof(packetId) + sizeof(playerPosition) + sizeof(playerLookDirection)
		+ sizeof(playerVelocity), &playerAcceleration, sizeof(playerAcceleration));
	this->udpSender->sendMessage(buffer, bufferSize);
}

void Network::sendPlayerFireAnimation()
{
	const unsigned int packetId = PLAYER_FIRE_ANIMATION;

	const unsigned int bufferSize = sizeof(packetId);
	char buffer[bufferSize];
	memcpy(buffer, &packetId, sizeof(packetId));
	this->udpSender->sendMessage(buffer, bufferSize);
}

// Send fire animation + wall shot mark position
void Network::sendPlayerFireAnimation(const glm::vec4& wallShotMarkPosition)
{
	if (wallShotMarkPosition == glm::vec4(0.0f))
	{
		int i = 0;
		++i;
	}
	const unsigned int packetId = PLAYER_FIRE_ANIMATION_WITH_WALL_SHOT_MARK;

	const unsigned int bufferSize = sizeof(packetId) + sizeof(wallShotMarkPosition);
	char buffer[bufferSize];
	memcpy(buffer, &packetId, sizeof(packetId));
	memcpy(buffer + sizeof(packetId), &wallShotMarkPosition, sizeof(wallShotMarkPosition));
	this->udpSender->sendMessage(buffer, bufferSize);
}

void Network::sendPlayerFireHitAndAnimation(int damage)
{
	const unsigned int packetId = PLAYER_FIRE_HIT;

	const unsigned int bufferSize = sizeof(packetId) + sizeof(damage);
	char buffer[bufferSize];
	memcpy(buffer, &packetId, sizeof(packetId));
	memcpy(buffer + sizeof(packetId), &damage, sizeof(damage));
	this->udpSender->sendMessage(buffer, bufferSize);
}

void Network::receiveAndProcessPackets()
{
	const unsigned int bufferSize = 2048;
	char buffer[bufferSize];

	// Try to receive packet. If there was a packet, process it accordingly!
	while (this->udpReceiver->receiveMessage(buffer, bufferSize) > 0)
	{
		unsigned int packetId = *(int*)buffer;

		switch (packetId)
		{
			case PLAYER_INFORMATION:
				this->processPlayerInformationPacket(buffer + sizeof(packetId));
				break;
			case PLAYER_FIRE_ANIMATION:
				this->processPlayerFireAnimationPacket(buffer + sizeof(packetId));
				break;
			case PLAYER_FIRE_ANIMATION_WITH_WALL_SHOT_MARK:
				this->processPlayerFireAnimationWithWallMarksPacket(buffer + sizeof(packetId));
				break;
			case PLAYER_FIRE_HIT:
				this->processPlayerFireHitPacket(buffer + sizeof(packetId));
				break;
		}
	}
}

void Network::processPlayerInformationPacket(char* buffer)
{
	glm::vec4 playerPosition = *(glm::vec4*)(buffer);
	glm::vec4 playerLookDirection = *(glm::vec4*)(buffer + sizeof(playerPosition));
	glm::vec4 playerVelocity = *(glm::vec4*)(buffer + sizeof(playerPosition) + sizeof(playerLookDirection));
	glm::vec4 playerAcceleration = *(glm::vec4*)(buffer + sizeof(playerPosition) + sizeof(playerLookDirection)
		+ sizeof(playerVelocity));

#if defined (DEBUG) && defined (SHOW_PERIODIC_PACKETS)
	std::cout << "Packet Received:" << std::endl;
	std::cout << "ID: " << PLAYER_INFORMATION << std::endl;
	std::cout << "Player Position: " << "<" << playerPosition.x << ", " << playerPosition.y << ", " <<
		playerPosition.z << ", " << playerPosition.w << ">" << std::endl;
	std::cout << "Player Look Direction: " << "<" << playerLookDirection.x << ", " << playerLookDirection.y << ", " <<
		playerLookDirection.z << ", " << playerLookDirection.w << ">" << std::endl;
	std::cout << "Player Velocity: " << "<" << playerVelocity.x << ", " << playerVelocity.y << ", " <<
		playerVelocity.z << ", " << playerVelocity.w << ">" << std::endl;
	std::cout << "Player Acceleration: " << "<" << playerAcceleration.x << ", " << playerAcceleration.y << ", " <<
		playerAcceleration.z << ", " << playerAcceleration.w << ">" << std::endl;
#endif

	// Process Packet
	Player* secondPlayer = this->boundGame->getSecondPlayer();

	if (secondPlayer)
	{
		secondPlayer->pushMovementInterpolation(playerPosition, playerVelocity, playerAcceleration);
		secondPlayer->changeLookDirection(playerLookDirection);
	}
}

void Network::processPlayerFireAnimationPacket(char* buffer)
{
	Player* secondPlayer = this->boundGame->getSecondPlayer();

#ifdef DEBUG
	std::cout << "Packet Received:" << std::endl;
	std::cout << "ID: " << PLAYER_FIRE_ANIMATION << std::endl;
#endif

	// Process Packet
	secondPlayer->startShootingAnimation();
}

void Network::processPlayerFireAnimationWithWallMarksPacket(char* buffer)
{
	Player* secondPlayer = this->boundGame->getSecondPlayer();
	glm::vec4 wallShotMarkPosition = *(glm::vec4*)(buffer);

#ifdef DEBUG
	std::cout << "Packet Received:" << std::endl;
	std::cout << "ID: " << PLAYER_FIRE_ANIMATION_WITH_WALL_SHOT_MARK << std::endl;
	std::cout << "Wall Shot Mark Position: <" << wallShotMarkPosition.x << ", " << wallShotMarkPosition.y
		<< ", " << wallShotMarkPosition.z << ", " << wallShotMarkPosition.w << ">" << std::endl;
#endif

	// Process Packet
	secondPlayer->startShootingAnimation();
	secondPlayer->createShotMark(wallShotMarkPosition, glm::vec4(1.0f, 165/255.0f, 0.0f, 1.0f));
}

void Network::processPlayerFireHitPacket(char* buffer)
{
	Player* localPlayer = this->boundGame->getLocalPlayer();
	Player* secondPlayer = this->boundGame->getSecondPlayer();
	int damage = *(int*)(buffer);

#ifdef DEBUG
	std::cout << "Packet Received:" << std::endl;
	std::cout << "ID: " << PLAYER_FIRE_HIT << std::endl;
	std::cout << "Damage: " << damage << std::endl;
#endif

	// Process Packet
	localPlayer->removeHp(damage);
	localPlayer->startDamageAnimation();
	secondPlayer->startShootingAnimation();
}
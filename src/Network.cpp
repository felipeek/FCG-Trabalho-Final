#include "Network.h"
#include "Game.h"

using namespace raw;

//#define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

const unsigned int Network::peerPort = 8888;
UDPSender* Network::udpSender;
UDPReceiver* Network::udpReceiver;
Game* Network::boundGame;

enum PacketType
{
	PLAYER_INFORMATION = 1,
	PLAYER_FIRE_HIT = 2
};

void Network::initNetwork(Game* game, char* peerIp)
{
	Network::udpReceiver = new UDPReceiver(Network::peerPort);
	Network::udpSender = new UDPSender(peerIp, Network::peerPort);
	Network::boundGame = game;
}

void Network::destroyNetwork()
{
	delete Network::udpReceiver;
	delete Network::udpSender;

	Network::udpReceiver = 0;
	Network::udpSender = 0;
	Network::boundGame = 0;
}

void Network::sendPlayerInformation(const Player& player)
{
	const unsigned int packetId = PLAYER_INFORMATION;
	glm::vec4 playerPosition = player.getTransform().getWorldPosition();
	glm::vec4 playerLookDirection = player.getLookDirection();

	unsigned int bufferSize = sizeof(packetId) + sizeof(playerPosition) + sizeof(playerLookDirection);
	char* buffer = (char*)malloc(bufferSize);
	memcpy(buffer, &packetId, sizeof(packetId));
	memcpy(buffer + sizeof(packetId), &playerPosition, sizeof(playerPosition));
	memcpy(buffer + sizeof(packetId) + sizeof(playerPosition), &playerLookDirection, sizeof(playerLookDirection));
	Network::udpSender->sendMessage(buffer, bufferSize);
	free(buffer);
}

void Network::sendPlayerFireHit()
{

}

void Network::receiveAndProcessPackets()
{
	const unsigned int bufferSize = 2048;
	char buffer[bufferSize];

	// Try to receive packet. If there was a packet, process it accordingly!
	if (Network::udpReceiver->receiveMessage(buffer, bufferSize) > 0)
	{
		unsigned int packetId = *(int*)buffer;

		switch (packetId)
		{
			case PLAYER_INFORMATION:
				Network::processPlayerInformationPacket(buffer + sizeof(packetId));
				break;
			case PLAYER_FIRE_HIT:
				Network::processPlayerFireHitPacket(buffer + sizeof(packetId));
				break;
		}
	}
}

void Network::processPlayerInformationPacket(char* buffer)
{
	glm::vec4 playerPosition = *(glm::vec4*)(buffer);
	glm::vec4 playerLookDirection = *(glm::vec4*)(buffer + sizeof(playerPosition));

#ifdef DEBUG
	std::cout << "Packet Received:" << std::endl;
	std::cout << "ID: " << PLAYER_INFORMATION << std::endl;
	std::cout << "Player Position: " << "<" << playerPosition.x << ", " << playerPosition.y << ", " <<
		playerPosition.z << ", " << playerPosition.w << ">" << std::endl;
	std::cout << "Player Look Direction: " << "<" << playerLookDirection.x << ", " << playerLookDirection.y << ", " <<
		playerLookDirection.z << ", " << playerLookDirection.w << ">" << std::endl;
#endif

	// Process Packet
	Network::boundGame->updateSecondPlayer(playerPosition, playerLookDirection);
}

void Network::processPlayerFireHitPacket(char* buffer)
{
	return;
}
#pragma once

#include "Player.h"
#include "UDPSender.h"
#include "UDPReceiver.h"

namespace raw
{
	class Game;

	class Network
	{
	public:
		Network(Game* game, char* peerIp, unsigned int peerPort);
		~Network();
		void sendPlayerInformation(const Player& player);
		void sendPlayerFireAnimation();
		void sendPlayerFireAnimation(const glm::vec4& wallShotMarkPosition);
		void sendPlayerFireHitAndAnimation(int damage);
		void receiveAndProcessPackets();
	private:
		void processPlayerInformationPacket(char* buffer);
		void processPlayerFireAnimationPacket(char* buffer);
		void processPlayerFireAnimationWithWallMarksPacket(char* buffer);
		void processPlayerFireHitPacket(char* buffer);
		unsigned int peerPort = 8888;
		UDPSender* udpSender;
		UDPReceiver* udpReceiver;
		Game* boundGame;
	};
}
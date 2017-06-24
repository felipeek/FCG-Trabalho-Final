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
		static void initNetwork(Game* game, char* peerIp);
		static void destroyNetwork();
		static void sendPlayerInformation(const Player& player);
		static void sendPlayerFireHit();
		static void receiveAndProcessPackets();
	private:
		static void processPlayerInformationPacket(char* buffer);
		static void processPlayerFireHitPacket(char* buffer);
		const static unsigned int peerPort;
		static UDPSender* udpSender;
		static UDPReceiver* udpReceiver;
		static Game* boundGame;
	};
}
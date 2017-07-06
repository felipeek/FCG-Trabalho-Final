#pragma once

namespace raw
{
	class UDPSender
	{
	public:
		UDPSender(const char* destinationIp, unsigned int destinationPort);
		~UDPSender();
		int sendMessage(char* message, unsigned int messageSize) const;
	private:
		char* destinationIp;
		unsigned int destinationPort;
		int socketDescriptor;
	};
}
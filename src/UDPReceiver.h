#pragma once

namespace raw
{
	class UDPReceiver
	{
	public:
		UDPReceiver(unsigned int senderPort);
		~UDPReceiver();
		int receiveMessage(char* buffer, int buffer_length) const;
	private:
		unsigned int senderPort;
		int socketDescriptor;
	};
}
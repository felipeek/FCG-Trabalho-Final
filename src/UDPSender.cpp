#include "UDPSender.h"
#include <iostream>
#include <winsock2.h>

using namespace raw;

#define BUFLEN 2048	// Max length of buffer
//#define DEBUG

UDPSender::UDPSender(char* destinationIp, unsigned int destinationPort)
{
	WSADATA wsa;

	// Stores destination IP
	this->destinationIp = (char*)malloc(sizeof(char) * (strlen(destinationIp) + 1));
	strcpy(this->destinationIp, destinationIp);

	// Stores destination Port
	this->destinationPort = destinationPort;

	// Initialise winsock
#ifdef DEBUG
	std::cout << "Initialising Winsock..." << std::endl;
#endif

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
#ifdef DEBUG
		std::cout << "Failed. Error Code: " << WSAGetLastError() << std::endl;
#endif
		return;
	}

#ifdef DEBUG
	std::cout << "Initialised.\n" << std::endl;
#endif

	// Create Socket
	if ((this->socketDescriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
#ifdef DEBUG
		std::cout << "socket() failed with error code: " << WSAGetLastError() << std::endl;
#endif
		return;
	}
}

UDPSender::~UDPSender()
{
	closesocket(this->socketDescriptor);
	WSACleanup();
	free(this->destinationIp);
}

int UDPSender::sendMessage(char* message, unsigned int messageSize) const
{
	// Setup address structure
	struct sockaddr_in socketAddress;
	memset((char *)&socketAddress, 0, sizeof(socketAddress));
	socketAddress.sin_family = AF_INET;
	socketAddress.sin_port = htons(this->destinationPort);
	socketAddress.sin_addr.S_un.S_addr = inet_addr(this->destinationIp);
	int socketAddressLength = sizeof(socketAddress);

	// Send Data
	if (sendto(this->socketDescriptor, message, messageSize, 0, (struct sockaddr *) &socketAddress,
		socketAddressLength) == SOCKET_ERROR)
	{
#ifdef DEBUG
		std::cout << "sendto() failed with error code: " << WSAGetLastError() << std::endl;
#endif
		return -1;
	}

	return 0;
}
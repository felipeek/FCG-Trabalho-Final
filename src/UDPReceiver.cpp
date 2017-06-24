#include "UDPReceiver.h"
#include <iostream>
#include <winsock2.h>

using namespace raw;

#define BUFLEN 2048	// Max length of buffer
//#define DEBUG

UDPReceiver::UDPReceiver(unsigned int senderPort)
{
	WSADATA wsa;

	// Stores sender Port
	this->senderPort = senderPort;

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

	u_long iMode = 1;	// Async Socket
	//u_long iMode = 0;	// Sync Socket

	ioctlsocket(this->socketDescriptor, FIONBIO, &iMode);

	// Prepare the sockaddr_in structure
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(this->senderPort);

	// Bind socket
	if (bind(this->socketDescriptor, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
#ifdef DEBUG
		std::cout << "Bind failed with error code:" << WSAGetLastError() << std::endl;
#endif
		return;
	}
}

UDPReceiver::~UDPReceiver()
{
	closesocket(this->socketDescriptor);
	WSACleanup();
}

int UDPReceiver::receiveMessage(char* buffer, int buffer_length) const
{
	int recv_len;
	struct sockaddr_in senderAddressInfo;
	int slen = sizeof(senderAddressInfo);

	// Receive Data
	if ((recv_len = recvfrom(this->socketDescriptor, buffer, buffer_length, 0, (struct sockaddr *) &senderAddressInfo,
		&slen)) == SOCKET_ERROR)
	{
#ifdef DEBUG
		std::cout << "recvfrom() failed with error code: " << WSAGetLastError() << std::endl;
#endif
		return -1;
	}

	// Print details of the client/peer and the data received
#ifdef DEBUG
	printf("Received packet from %s:%d\n", inet_ntoa(senderAddressInfo.sin_addr), ntohs(senderAddressInfo.sin_port));
#endif

	return recv_len;
}
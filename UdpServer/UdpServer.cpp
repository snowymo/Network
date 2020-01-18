#include "UDPServer.h"



UDPServer::UDPServer()
{
    ackToClient = false; 

    WSAStartup(MAKEWORD(2, 2), &wsaData);
    fromlen = sizeof(from);
    local.sin_family = AF_INET;
    local.sin_port = htons(DEFAULT_PORT);
    local.sin_addr.s_addr = INADDR_ANY;

    socketS = socket(AF_INET, SOCK_DGRAM, 0);
    bind(socketS, (sockaddr*)&local, sizeof(local));
}


UDPServer::~UDPServer()
{
}

bool UDPServer::recv()
{
    char buffer[1024];
    ZeroMemory(buffer, sizeof(buffer));
    printf("Waiting...\n");
    if (recvfrom(socketS, buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromlen) != SOCKET_ERROR)
    {
        char addr_buffer[16];
        const char* addr_char = inet_ntop(AF_INET, &from.sin_addr, addr_buffer, sizeof(addr_buffer));
        printf("Received message from %s: %s\n", addr_char, buffer);
        // optionally
        if(ackToClient)
            sendto(socketS, buffer, sizeof(buffer), 0, (sockaddr*)&from, fromlen);
    }
    return true;
}

void UDPServer::closeConnection()
{
    closesocket(socketS);
}

#include "UDPServer.h"

#include <iostream>

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

    //
    nRows = 48; nCols = 80;
    // debug
    nCols = 5; nRows = 4;
    int len = sizeof(long long) + sizeof(int) + sizeof(int) + sizeof(int)*nCols*nRows;
    buffer = new char[len];
    forces = new int[nRows*nCols];
}


UDPServer::~UDPServer()
{
}

bool UDPServer::recv()
{
    // the pressure data is 80*48 right now and will be 32*12 later I guess
    // the code is not necessarily to be smart so I predefine the size
    ZeroMemory(buffer, sizeof(buffer));
    int len = sizeof(long long) + sizeof(int) + sizeof(int) + sizeof(int)*nCols*nRows;
    printf("Waiting...\n");
    if (recvfrom(socketS, buffer, len, 0, (sockaddr*)&from, &fromlen) != SOCKET_ERROR)
    {
        char addr_buffer[16];
        const char* addr_char = inet_ntop(AF_INET, &from.sin_addr, addr_buffer, sizeof(addr_buffer));
        // parse it
        long long ts; int tempCols, tempRows;
        memcpy_s(&ts, sizeof(long long), buffer, sizeof(long long));
        memcpy_s(&tempCols, sizeof(int), buffer+ sizeof(long long), sizeof(int));
        memcpy_s(&tempRows, sizeof(int), buffer+ sizeof(long long)+ sizeof(int), sizeof(int));
        memcpy_s(forces, sizeof(forces), buffer + sizeof(long long) + sizeof(int)*2, sizeof(forces));
        std::cout << "Received message from " << addr_char << " ts:" << ts << " " << tempCols << " " << tempRows << "\n";
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

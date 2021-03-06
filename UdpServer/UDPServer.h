#pragma once

#include <ws2tcpip.h>
#include <winsock2.h>
#include <stdio.h>

#define DEFAULT_PORT 27015

class UDPServer
{
public:
    UDPServer();
    ~UDPServer();

    bool recvShoeInsoles();
    bool recvIMU();
    void closeConnection();
    void setAckToClient(bool b) { ackToClient = b; }

private:
    WSADATA wsaData;
    SOCKET socketS;
    struct sockaddr_in local;
    struct sockaddr_in from;
    int fromlen;
    bool ackToClient;
    int nRows, nCols;
    char* buffer;
    int* forces;
};


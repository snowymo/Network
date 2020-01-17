// UdpServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#include <ws2tcpip.h>
#include <winsock2.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT 27015

void InitWinsock()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}


int main(int argc, unsigned char* argv[])
{
    SOCKET socketS;

    InitWinsock();
    struct sockaddr_in local;
    struct sockaddr_in from;
    int fromlen = sizeof(from);
    local.sin_family = AF_INET;
    local.sin_port = htons(DEFAULT_PORT);
    local.sin_addr.s_addr = INADDR_ANY;

    socketS = socket(AF_INET, SOCK_DGRAM, 0);
    bind(socketS, (sockaddr*)&local, sizeof(local));
    while (1)
    {
        char buffer[1024];
        ZeroMemory(buffer, sizeof(buffer));
        printf("Waiting...\n");
        if (recvfrom(socketS, buffer, sizeof(buffer), 0, (sockaddr*)&from, &fromlen) != SOCKET_ERROR)
        {
            char addr_buffer[16];
            const char* addr_char = inet_ntop(AF_INET, &from.sin_addr, addr_buffer, sizeof(addr_buffer));
            printf("Received message from %s: %s\n", addr_char, buffer);
            sendto(socketS, buffer, sizeof(buffer), 0, (sockaddr*)&from, fromlen);
        }
        Sleep(500);
    }
    closesocket(socketS);

    return 0;
}

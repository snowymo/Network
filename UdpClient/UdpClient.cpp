// UdpClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
#include <Ws2tcpip.h>
#include <winsock2.h>
#include <stdio.h>
#include <string>
#include <iostream>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_PORT 27015

void InitWinsock()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
}

int main(int argc, char* argv[])
{
    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    SOCKET socketC;

    InitWinsock();
    struct sockaddr_in serverInfo;
    int len = sizeof(serverInfo);
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, argv[1], &(serverInfo.sin_addr.s_addr));

    socketC = socket(AF_INET, SOCK_DGRAM, 0);
    while (1)
    {
        //char buffer[1024];
        //ZeroMemory(buffer, sizeof(buffer));
        printf("Please input your message: ");
        std::string strMsg;
        std::cin >> strMsg;
        //scanf_s("%s", buffer);
        if (strcmp(strMsg.c_str(), "exit") == 0)
            break;
        if (sendto(socketC, strMsg.c_str(), strMsg.length(), 0, (sockaddr*)&serverInfo, len) != SOCKET_ERROR)
        {
            char buffer[1024];
            ZeroMemory(buffer, sizeof(buffer));
            if (recvfrom(socketC, buffer, sizeof(buffer), 0, (sockaddr*)&serverInfo, &len) != SOCKET_ERROR)
            {
                printf("Receive response from server: %s\n", buffer);
            }
        }
    }
    closesocket(socketC);

    return 0;
}
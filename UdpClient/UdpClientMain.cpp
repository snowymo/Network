// UdpClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
#include "UDPClient.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#include <iostream>


int main(int argc, char* argv[])
{
    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    UDPClient* udpClient = new UDPClient(argv[1]);
    bool TBC = true;
    while (TBC) {
        std::string msg;
        std::cin >> msg;
        TBC = udpClient->sendMsg(msg);
    }
    udpClient->closeConnection();

    return 0;
}
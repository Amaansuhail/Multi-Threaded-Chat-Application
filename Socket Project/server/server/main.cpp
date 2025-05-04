#include <iostream>
#include <WS2tcpip.h>
#include <winsock2.h>
#include<thread>
#include<vector>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

/*
   // inistialize winsock library

   // ceate the socket

   //get ip and port

   // bind the ip/pory with the socket

   //listen

   //accept

   //recv and send

   //close and socket

   //close winsocket


*/

bool InitializeWinsock() {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void IntrectWithClient(SOCKET clientSocket , vector<SOCKET> &clients) {

    cout << " client connected " << endl;
    char buffer[4096];
    ZeroMemory(buffer, 4096);

    while (1){
        // Receive message
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived > 0) {
        string message(buffer, bytesReceived);
        cout << "Message from client: " << message << endl;

        // Optionally send a reply
        send(clientSocket, buffer, bytesReceived, 0);
    }
    else {
        cout << "Receive failed or client disconnected." << endl;
        break;
    }
    string message(buffer, bytesReceived);
    for (auto client : clients) {

        if (client != clientSocket)
        {
            send(client, message.c_str(), message.length(), 0);
        }
    }
}
    // Cleanup
    auto it = find(clients.begin(), clients.end(), clientSocket);

    if (it != clients.end()) {
        clients.erase(it);
    }
    closesocket(clientSocket);
}

int main() {
    if (!InitializeWinsock()) {
        cout << "Winsock initialization failed." << endl;
        return 1;
    }

    cout << "Starting server..." << endl;

    // Create the listening socket
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed." << endl;
        WSACleanup();
        return 1;
    }

    // Set up the sockaddr_in structure
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(12345);  // Port number


    // Replace with your actual IP address (e.g., from ipconfig)
    const wchar_t* specificIP = L"127.0.0.1"; // Note the L prefix for wide string


    // Convert IP address from text to binary
    if (InetPton(AF_INET, specificIP, &serveraddr.sin_addr) != 1) {
        cout << "Invalid IP address format or conversion failed." << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Bind the socket to the IP address and port
    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
        cout << "Bind failed with error: " << WSAGetLastError() << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // Start listening
    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Listen failed." << endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server is listening on " << specificIP << ":12345" << endl;
    vector<SOCKET> clients;
    while (1) {
        // Accept a client
        SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            cout << "Client accept failed." << endl;
            closesocket(listenSocket);
            WSACleanup();
            return 1;
        }
        clients.push_back(clientSocket);
        thread t1(IntrectWithClient, clientSocket , std::ref(clients));
        t1.detach();
    }

    
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}

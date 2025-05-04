#include<iostream>
#include<winSock2.h>
#include<WS2tcpip.h>
#include<thread>
#include<string>

using namespace std;

#pragma  comment(lib , "ws2_32.lib")

/*
  // initialize winsock

  //  create socket

  // connect to server

  // send / recv
*/
bool  InitializeWinsock() {
    WSADATA data;
    return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void SendMessagToServer(SOCKET s) {
    cout << " enter your chat name : " << endl;
    string name;
    std::getline(cin, name);

    string message;

    while (1) {
        std::getline(cin, message);
        string msg = name + " : " + message;
        int bytesent = send(s, msg.c_str(), msg.length(), 0);

        if (bytesent == SOCKET_ERROR) {
            cout << "error sending message" << endl;
            break;
        }

        if (message == "quit") {
            cout << " stopping application" << endl;
            break;
        }
    }
    closesocket(s);
    WSACleanup();
}

void RecieveMessage(SOCKET s) {

    char buffer[4096];

    int recvlength;

    string msg = "";
    while (1) {
        recvlength = recv(s, buffer, sizeof(buffer), 0);
        if (recvlength <= 0)
        {
            cout << " discontinue from server" << endl;
            break;
        }
        else
        {
            msg = string(buffer, recvlength);
            cout << msg << endl;
        }
    }

    closesocket(s);
    WSACleanup();
}
int main() {
    if (!InitializeWinsock()) {
        cout << "Winsock initialization failed." << endl;
        return 1;
    }

    SOCKET s;
    s = socket(AF_INET, SOCK_STREAM, 0);

    if (s == INVALID_SOCKET) {
        cout << " invalid socket " << endl;
        return 1;
    }

    // Set up the sockaddr_in structure
    sockaddr_in serveraddr;
    string serveraddress = "127.0.0.1";
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(12345);  // Port number

    inet_pton(AF_INET, serveraddress.c_str(), &serveraddr.sin_addr);

    if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
        cout << "not able to connect to server: " << WSAGetLastError() << endl;
        closesocket(s);
        WSACleanup();
        return 1;
    }
    cout << " client program started " << endl;

    thread senderthread(SendMessagToServer, s);
    thread recievethread(RecieveMessage, s);

    senderthread.join();
    recievethread.join();

    ////send / recv
    //string message = " hello there";
    //int bytesent = send(s, message.c_str(), message.length() , 0);

    //if (bytesent == SOCKET_ERROR) {
    //    cout << " send failed" << endl;
    //}

  /*  closesocket(s);

    WSACleanup();
}*/
}
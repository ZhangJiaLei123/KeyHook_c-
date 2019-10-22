#include "stdafx.h"
#include "winsock2.h"  
#include <WS2tcpip.h>
#include <iostream>  
#include "UdpTool.h"
#pragma comment(lib, "ws2_32.lib")  

using namespace std;
//BOOL RecvLine(SOCKET s, char* buf); //Read the data of a line  

SOCKET sHost = NULL; //Server socket
const int BUF_SIZE = 64;

int CloseUdp() {
	//Exit  
	if(sHost != NULL){
		closesocket(sHost);
	}
	WSACleanup();
	return 0;
}

int SendUdp(char buf[], char bufRecv[]) {

	int retVal; //return value  
	//Sending data to the Server 
	ZeroMemory(buf, BUF_SIZE);
	cout << " Sending data to the Server:  ";
	cin >> buf;
	retVal = send(sHost, buf, strlen(buf), 0);
	if (SOCKET_ERROR == retVal)
	{
		cout << "send failed!" << endl;
		closesocket(sHost);
		WSACleanup();
		return -1;
	}
	if (bufRecv != NULL) {
	//	RecvLine(sHost, bufRecv);  
		ZeroMemory(bufRecv, BUF_SIZE);
		recv(sHost, bufRecv, BUF_SIZE, 0); // Accepting data from Server,only receiving 5 characters 
	}
	return 0;
}


int CreateUdp()
{

	WSADATA wsd; //WSADATA Variable  

	SOCKADDR_IN servAddr; //Server Address  
	int retVal; //return value  
	//Initial socket dynamic lib
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return -1;
	}
	//Create socket  
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sHost)
	{
		cout << "socket failed!" << endl;
		WSACleanup();//Release socket resource  
		return  -1;
	}
	//Set server address 
	servAddr.sin_family = AF_INET;
//	servAddr.sin_addr.s_addr 
	inet_pton(AF_INET, "127.0.0.1", &(servAddr.sin_addr.s_addr));
	servAddr.sin_port = htons((short)4999);
	int nServAddlen = sizeof(servAddr);

	//Connect Server 
	retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
	if (SOCKET_ERROR == retVal)
	{
		cout << "connect failed!" << endl;
		closesocket(sHost); //close socket 
		WSACleanup(); //Release socket resource   
		return -1;
	}
	return 0;
}

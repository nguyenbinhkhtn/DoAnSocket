#include<iostream>
#include<ws2tcpip.h>
#include<string>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

const int ACCEPT_REQUEST = 1;
const int LOGIN_REQUEST = 1;
const int CREATE_NEW_ACCOUNT_REQUEST = 2;
const int UPLOAD_REQUEST = 3;
const int DOWLOAD_REQUEST = 4;
const int DENIED = -1;

int convertStringToInt(string s) {
	int l1 = s.length();
	int num1 = 0;
	for (int i = l1 - 1; i >= 0; --i) {
		num1 += (int)(s[i] - '0') * pow(10, l1 - i - 1);
	}
	return num1;
}

void login_Request(SOCKET sock, char buf[4096], bool &isLogin) {
	string sendRequestLogin = "1";
	if (sendRequestLogin.size() > 0) {
		int sendResult = send(sock, sendRequestLogin.c_str(), sendRequestLogin.size() + 1, 0);
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(sock, buf, 4096, 0);
		int result = convertStringToInt(string(buf, 0, bytesReceived));
		if (result == ACCEPT_REQUEST) {
			string id;
			string password;
			fflush(stdin);
			cout << "ID > ";
			getline(cin, id);
			if (id.size() > 0) {
				int sendResult = send(sock, id.c_str(), id.size() + 1, 0);
			}
			cout << "PASSWORD > ";
			getline(cin, password);
			if (password.size() > 0) {
				int sendResult = send(sock, password.c_str(), password.size() + 1, 0);
			}
			int bytesReceived = recv(sock, buf, 4096, 0);
			int result = convertStringToInt(string(buf, 0, bytesReceived));
			if (result == ACCEPT_REQUEST) {
				isLogin = true;
				cout << "Login access!" << endl;
			}
		}
	}
}

void create_New_Account_Request(SOCKET sock, char buf[4096]) {
	string sendRequestRegister = "2";
	if (sendRequestRegister.size() > 0) {
		int sendResult = send(sock, sendRequestRegister.c_str(), sendRequestRegister.size() + 1, 0);
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(sock, buf, 4096, 0);
		int result = convertStringToInt(string(buf, 0, bytesReceived));
		if (result == ACCEPT_REQUEST) {
			string id;
			string password;

			fflush(stdin);
			cout << "ID > ";
			getline(cin, id);

			if (id.size() > 0) {
				int sendResult = send(sock, id.c_str(), id.size() + 1, 0);
			}


			cout << "PASSWORD > ";
			getline(cin, password);

			if (password.size() > 0) {
				int sendResult = send(sock, password.c_str(), password.size() + 1, 0);
			}

			int bytesReceived = recv(sock, buf, 4096, 0);
			int result = convertStringToInt(string(buf, 0, bytesReceived));

			cout << result;

			if (result == ACCEPT_REQUEST) {
				cout << "Regist is success" << endl;
			}
			else if (result == DENIED) {
				cout << "Regist is fail" << endl;
			}
		}
	}
}

void main()
{
	string ipAdress = "127.0.0.1";
	int port = 54000;

	//init winsock
	WSADATA data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0) {
		cerr << "can't start winsock, Err #" << wsResult << endl;
		return;
	}

	//create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		cerr << "can't create winsock, Err #" << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	//Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAdress.c_str(), &hint.sin_addr);

	//Connect to server
	cout << "Connecting to server..." << endl;
	Sleep(3000);
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR) {
		cerr << "Can't connect to server, Er #" << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}
	char buf[4096];
	ZeroMemory(buf, 4096);
	int bytesReceived = recv(sock, buf, 4096, 0);
	if (bytesReceived == SOCKET_ERROR) {
		cerr << "ERROR in recv(). Quitting" << endl;
	}
	cout << string(buf, 0, bytesReceived) << endl << endl;
	
	string userInput;
	bool isLogin = false;
	int tt = 1;
	do {
		int request;
		cout << "Enter your request send to server: " << endl;
		cout << "1. Login" << endl << "2. Create new account" << endl << "> ";
		fflush(stdin);
		cin >> request;
		string debug1;
		getline(cin, debug1);
		cout << debug1;
		fflush(stdin);
		if (request == LOGIN_REQUEST) {
			login_Request(sock, buf, isLogin);
		}
		else if(request == CREATE_NEW_ACCOUNT_REQUEST){
			create_New_Account_Request(sock, buf);
		}
	} while (tt == 1);
	
	closesocket(sock);
	WSACleanup();
}
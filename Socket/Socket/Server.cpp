#include <iostream>
#include <ws2tcpip.h>
#include <string>
#include <sstream>
#include <vector>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

const int CHECK_LOGIN = 1;
const int CREATE_NEW_ACCOUNT = 2;

int convertStringToInt(string s) {
	int l1 = s.length();
	int num1 = 0;
	for (int i = l1 - 1; i >= 0; --i) {
		num1 += (int)(s[i] - '0') * pow(10, l1 - i - 1);
	}
	return num1;
}

struct User {
	string id;
	string password;
};

vector<User> listUser ;

User user1 = { "binci","binci" };
User user2 = { "admin", "admin" };
User user3 = { "mentor", "mentor" };


int checkCreateNewAccount(string id, string password, vector<User> &listUser) {
	vector<User>::iterator item;
	User temp;
	for (item = listUser.begin(); item != listUser.end(); item++) {
		temp = *item;
		if (temp.id == id) {
			return -1;
		}
	}
	return 1;
}

int CheckLogin(string id, string password, vector<User> &listUser) {
	vector<User>::iterator item;
	User temp;
	for (item = listUser.begin(); item != listUser.end(); item++) {
		temp = *item;
		if (temp.id == id) {
			if (temp.password == password) {
				return 1;
			}
		}
	}
	return -1;
}

void s_Check_Login(SOCKET sock, vector<User> &listUser) {
	ostringstream ss;
	ss << "Client #" << sock << " want to login" << "\r\n";
	string strOut = ss.str();
	cout << strOut << endl;
	string accept = "1";
	send(sock, accept.c_str(), accept.size() + 1, 0);
	Sleep(10);
	char buf[4096];
	ZeroMemory(buf, 4096);
	int id = recv(sock, buf, 4096, 0);
	string userId = string(buf, 0, id);
	Sleep(10);
	ZeroMemory(buf, 4096);
	int password = recv(sock, buf, 4096, 0);
	string userPassword = string(buf, 0, id);
	int result = CheckLogin(userId, userPassword, listUser);
	if (result == 1) {
		send(sock, accept.c_str(), accept.size() + 1, 0);
	}
	ss << "Client #" << sock << " is login" << "\r\n";
	strOut = ss.str();
	cout << strOut << endl;
}

void s_Check_Create_New_User(SOCKET sock, vector<User> &listUser) {
	ostringstream ss;
	ss << "Client #" << sock << " want to create new account" << "\r\n";
	string strOut = ss.str();
	cout << strOut << endl;
	string accept = "1";
	string non_accept = "-1";
	send(sock, accept.c_str(), accept.size() + 1, 0);
	Sleep(10);
	char buf[4096];
	ZeroMemory(buf, 4096);
	int id = recv(sock, buf, 4096, 0);
	string userId = string(buf, 0, id);
	Sleep(10);
	ZeroMemory(buf, 4096);
	int password = recv(sock, buf, 4096, 0);
	string userPassword = string(buf, 0, id);
	int result = checkCreateNewAccount(userId, userPassword, listUser);
	User newUser = { userId, userPassword };
	listUser.push_back(newUser);
	if (result == 1) {
		send(sock, accept.c_str(), accept.size() + 1, 0);
	}
	else {
		send(sock, non_accept.c_str(), non_accept.size() + 1, 0);
	}
}

void main() {
	listUser.push_back(user1);
	listUser.push_back(user2);
	listUser.push_back(user3);
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	//Init socket
	int wsok = WSAStartup(ver, &wsData);
	if (wsok != 0) {
		cerr << "Can't Initialize winsock! Quitting" << endl;
		return;
	}

	//Create socket
	SOCKET	listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		cerr << "Can't create a socket! Quitting" << endl;
		return;
	}

	//Bind socket to an ip adress and port
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//Tell winsock the socket is for listening
	listen(listening, SOMAXCONN);
	cout << "Server is running..." << endl;
	fd_set master;

	FD_ZERO(&master);

	FD_SET(listening, &master);

	while (true) {
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++) {
			SOCKET sock = copy.fd_array[i];
			if (sock == listening) {
				//accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);
				ostringstream ss;
				ss << "Client #" << client << " is connecting" << "\r\n";
				string strOut = ss.str();
				cout << strOut << endl;
				//add the new connection to the list of connected client 
				FD_SET(client, &master);
				string welcomeMsg = "WELLCOME TO CONNECT SERVER";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}
			else {
				char buf[4096];
				ZeroMemory(buf, 4096);
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0) {
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else {
					int clientRequest = convertStringToInt(string(buf, 0, bytesIn));
					switch (clientRequest)
					{
						case CHECK_LOGIN: {
							s_Check_Login(sock, listUser);
							break;
						}
						case CREATE_NEW_ACCOUNT: {
							s_Check_Create_New_User(sock, listUser);
							break;
						}
					}
				}
			}
		}
	}

	WSACleanup();

	system("pause");
}
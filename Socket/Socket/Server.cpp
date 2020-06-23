#include <iostream>
#include <ws2tcpip.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

const int CHECK_LOGIN = 1;
const int CREATE_NEW_ACCOUNT = 2;
const int SEND_FILE = 3;
const int RECV_FILE = 4;

long GetFileSize(string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

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
vector<string> listFile;
vector<User> listUser;

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
	ss << "[+]Client #" << sock << " want to login" << "\r\n";
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
	ss << "[+]Client #" << sock << " want to create new account" << "\r\n";
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

void s_Send_File(SOCKET FileSendSocket) {
	string strList;
	vector<string>::iterator item;
	int i = 1;
	for (item = listFile.begin(); item != listFile.end(); item++) {
		strList = strList + to_string(i) + " " + *item + " - ";
		i++;
	}
	int sendList = send(FileSendSocket, strList.c_str(), strList.size(), 0);
	//gui qua client danh sach file
	char bufName[4096];
	ZeroMemory(bufName, 4096);
	int resultName = recv(FileSendSocket, bufName, 4096, 0);
	string fileName = string(bufName, 0, resultName);
	string FilePath = "E:\\bin\\" + fileName;
	//Nhan ve ten file
	streampos filesize = 0;
	ifstream fileIn(FilePath, ios::binary);
	unsigned int size;
	size = GetFileSize(FilePath);
	string sSize = to_string(size);
	cout << "[+]Size: " << sSize << endl;
	send(FileSendSocket, sSize.c_str(), sSize.size() + 1, 0);
	char *buf;
	buf = new char[size - 1];
	if (fileIn.is_open())
	{
		fileIn.read(buf, size);
		if (fileIn.eof())
		{
			cout << "[+]End of File sending from Client" << endl;
			fileIn.close();
		}
		else
		{
			send(FileSendSocket, buf, size , 0); //gui qua file
			cout << "[+]Sending success..." << endl;
		}
	}
}

void recvFile(SOCKET sock) {
	cout << endl << "[+] Server is receiving..." << endl;
	char bufFileName[4096];
	ZeroMemory(bufFileName, 4096);
	int resultName = recv(sock, bufFileName, 4096, 0);
	string fileName = string(bufFileName, 0, resultName);
	cout <<"[+] Ten file: " << fileName << endl;
	string fileNamePath = "E:\\bin\\";
	fileNamePath = fileNamePath + fileName;
	char buf[4096];
	ZeroMemory(&buf, 4096);
	unsigned int Size;
	int result = recv(sock, buf, 4096, 0);
	if (result == SOCKET_ERROR) {
		cerr << "ERROR in recv(). Quitting" << endl;
		return;
	}
	Size = atoi((const char*)buf);
	cout << "[+] Size: " << Size << endl;

	char *bufFile;
	bufFile = new char[Size];
	result = recv(sock, bufFile, Size, 0);
	if (result == SOCKET_ERROR) {
		cerr << "ERROR in recv(). Quitting" << endl;
		return;
	}

	ofstream fileRC(fileNamePath, ios::binary);
	if (fileRC.is_open()) {
		fileRC.write(bufFile, Size);
	}
	listFile.push_back(fileName);
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
				ss << "[+]Client #" << client << " is connecting" << "\r\n";
				string strOut = ss.str();
				cout << strOut << endl;
				//add the new connection to the list of connected client 
				FD_SET(client, &master);
				string welcomeMsg = "[+]WELLCOME TO SERVER";
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
					case SEND_FILE: {
						s_Send_File(sock);
						break;
					}
					case RECV_FILE: {
						recvFile(sock);
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
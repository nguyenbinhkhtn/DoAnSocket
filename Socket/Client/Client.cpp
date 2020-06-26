#include<iostream>
#include<ws2tcpip.h>
#include<string>
#include<cstdio>
#include<stdio.h>
#include<fstream>
#pragma comment (lib, "ws2_32.lib")

std::string getFileName(std::string filePath, bool withExtension = true, char seperator = '\\')
{
	// Get last dot position
	std::size_t dotPos = filePath.rfind('.');
	std::size_t sepPos = filePath.rfind(seperator);
	if (sepPos != std::string::npos)
	{
		return filePath.substr(sepPos + 1, filePath.size() - (withExtension || dotPos != std::string::npos ? 1 : dotPos));
	}
	return "";
}

using namespace std;

const int ACCEPT_REQUEST = 1;
const int LOGIN_REQUEST = 1;
const int CREATE_NEW_ACCOUNT_REQUEST = 2;
const int UPLOAD_REQUEST = 4;
const int DOWLOAD_REQUEST = 3;
const int DENIED = -1;
const int LOGOUT = 5;

int convertStringToInt(string s) {
	int l1 = s.length();
	int num1 = 0;
	for (int i = l1 - 1; i >= 0; --i) {
		num1 += (int)(s[i] - '0') * pow(10, l1 - i - 1);
	}
	return num1;
}

long GetFileSize(string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
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
				cout << "[+]Login access!" << endl;
			}
		}
	}
}

void dowloadFile(SOCKET sock) {
	string sendRequestDowload = "3";
	char bufList[4096];
	char buf[4096];
	int Size;
	string nameFile;
	int sendResult = send(sock, sendRequestDowload.c_str(), sendRequestDowload.size() + 1, 0);
	if (sendResult == SOCKET_ERROR) {
		cerr << "ERROR to send request" << endl;
		return;
	}

	ZeroMemory(&bufList, 4096);
	int resultList = recv(sock, bufList, 4096, 0);
	cout << "[+]" << string(bufList, 0, resultList) << endl;
	//Nhan ve danh sach file
	cout << "- Nhap ten file can tai ve >";
	getline(cin, nameFile);
	sendResult = send(sock, nameFile.c_str(), nameFile.size(), 0);
	// gui qua server ten file can lay
	if (sendResult == SOCKET_ERROR) {
		cerr << "ERROR to send name file" << endl;
		return;
	}
	ZeroMemory(buf, 4096);
	int result = recv(sock, buf, 4096, 0);
	if (result == SOCKET_ERROR) {
		cerr << "ERROR in recv() 1. Quitting" << endl;
		return;
	}
	Size = atoi((const char*)buf);
	char *bufFile;
	bufFile = new char[Size];
	result = recv(sock, bufFile, Size, 0);
	if (result == SOCKET_ERROR) {
		cerr << "ERROR in recv() 2. Quitting" << endl;
		return;
	}
	string fileNamePath = "E:\\source\\" + nameFile;
	ofstream fileRC(fileNamePath, ios::binary);
	if (fileRC.is_open()) {
		fileRC.write(bufFile, Size);
		return;
	}
}

void upLoadFile(SOCKET server) {
	string sendRequest = "4";
	int sendResult = send(server, sendRequest.c_str(), sendRequest.size() + 1, 0);
	string fPath;
	cout << "Chon file can upload > ";
	getline(cin, fPath);
	string fileName = getFileName(fPath);
	char bufName[4096];
	ZeroMemory(bufName, 4096);
	ifstream fileIn(fPath, ios::binary);
	unsigned int size;
	size = GetFileSize(fPath);
	string sSize = to_string(size);
	char *buf;
	buf = new char[size - 1];
	if (fileIn.is_open())
	{
		fileIn.read(buf, size);
		if (fileIn.eof())
		{
			cout << "End of File sending from Client" << endl;
			fileIn.close();
		}
		else
		{
			send(server, fileName.c_str(), fileName.size() + 1, 0);
			send(server, sSize.c_str(), sSize.size() + 1, 0);
			Sleep(100);
			send(server, buf, size, 0);
			cout << "[+] Upload file success..." << endl <<endl;
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
				cout << "[+]Regist is success" << endl;
			}
			else if (result == DENIED) {
				cout << "[-]Regist is fail" << endl;
			}
		}
	}
}

void logOut(SOCKET sock)
{
	string sendRequestLogOut = "5";
	int sendResult = send(sock, sendRequestLogOut.c_str(), sendRequestLogOut.size() + 1, 0);
	if (sendResult == SOCKET_ERROR) {
		cerr << "ERROR to send request" << endl;
		return;
	}
	closesocket(sock);
	WSACleanup();
	return;
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
	cout << "[+]Connecting to server..." << endl;
	Sleep(3000);
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR) {
		cerr << "[-]Can't connect to server, Er #" << WSAGetLastError() << endl;
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
		if (connResult == SOCKET_ERROR) {
			cerr << "[-]Can't connect to server, Er #" << WSAGetLastError() << endl;
			cout << "Server is shutdown" << endl;
			closesocket(sock);
			WSACleanup();
			return;
		}
		int request;
		cout <<endl << "[+]Enter your request send to server: " << endl;
		cout << "1. Login" << "2. Create new account" <<  " 3. Dowload file" << " 4. Upload file" << " 5. Log out"<<endl << "> ";
		fflush(stdin);
		cin >> request;
		string debug1;
		getline(cin, debug1);
		cout << debug1;
		fflush(stdin);
		switch (request)
		{
		case LOGIN_REQUEST:
			login_Request(sock, buf, isLogin);
			break;
		case CREATE_NEW_ACCOUNT_REQUEST:
			create_New_Account_Request(sock, buf);
			break;
		case DOWLOAD_REQUEST:
			dowloadFile(sock);
			break;
		case UPLOAD_REQUEST:
			upLoadFile(sock);
			break;
		case LOGOUT:
			logOut(sock);
			break;
		}
	} while (tt == 1);
	
	closesocket(sock);
	WSACleanup();
}
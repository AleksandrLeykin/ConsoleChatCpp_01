#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <string_view>

//Key constants
// Enter IPv4 address of Server
const char SERVER_IP[] = "127.0.0.1";
// Enter Listening port on Server side
const short SERVER_PORT_NUM = 7777;
// Maximum size of buffer for exchange info between server and client
const short BUFF_SIZE = 1024;

#if defined (__linux__)
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class myClient {
public:
	myClient();
	~myClient();

	void setMyString(const std::string& str) {
		m_str = str;
	}

	void clientLoading();
private:
	int clientSocket{};
	int connectionClient{};
	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	std::string m_str = "";	
	
	void userMessage(int client_sock);
	bool isExit() const;
	void clientExit() const;
	//sending message to server отправка сообщения на сервер
	void sendMessage();
	//receiving message from server прием сообщения с сервера
	void receivMessage();
	//password encoding кодировка пароля
	void pwdEncoding();

	//____________________________________________________________________
	//испровляю ошибки с передачей слов
	char package[BUFF_SIZE]{};
	//В отличие от std::string, std::string_view не выделяет память
	std::string_view pkg_in{ package, std::size(package) };
	// The size of sending / receiving packet in bytes // Размер отправляемого/принимаемого пакета в байтах
	short packet_size = 0;
	//____________________________________________________________________
	//приём Передача Сообщения
	std::string receptionTransmissionMes(int client_sock, const std::string& name);
	//ввод строки (line input)
	std::string getLineOfText() {
		m_str = "";
		getline(std::cin, m_str);
		return m_str;
	}
};

#elif defined(_WIN64)

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>
//#include <inaddr.h>

#pragma comment(lib, "Ws2_32.lib")

class myClient {
public:
	//client_loading();
	myClient() {}
	~myClient() { clientExit(); }

	void setMyString(const std::string& str) {
		m_str = str;
	}
	void clientLoading();
	
private:
	SOCKET clientSocket{};
	int connectionClient{};
	sockaddr_in serverAddr{};
	sockaddr_in clientAddr{};

	std::string m_str = "";
	void userMessage(SOCKET client_sock);
	bool isExit() const;
	void clientExit() const;
	//sending message to server отправка сообщения на сервер
	void sendMessage();
	//receiving message from server прием сообщения с сервера
	void receivMessage();
	//password encoding кодировка пароля
	void pwdEncoding();

	//____________________________________________________________________
	//испровляю ошибки с передачей слов
	char package[BUFF_SIZE]{};
	//В отличие от std::string, std::string_view не выделяет память
	std::string_view pkg_in{ package, std::size(package) };
	// The size of sending / receiving packet in bytes // Размер отправляемого/принимаемого пакета в байтах
	short packet_size = 0;
	//____________________________________________________________________

	std::string receptionTransmissionMes(SOCKET client_sock, const std::string& name);

	//ввод строки (line input)
	std::string getLineOfText() {
		m_str = "";
		getline(std::cin, m_str);
		return m_str;
	}
};
#endif
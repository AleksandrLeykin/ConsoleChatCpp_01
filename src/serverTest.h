#pragma once

#include <vector>
#include <string>
#include <memory>
#include <string_view>
#include "Logger.h"


//макрос дл¤ печати количества активных пользователей
#define PRINTUSERS if (nclients) std::cout << nclients << " - User on-line" << std::endl; \
						else std::cout << "No user on line\n";
//Key constants
const int PORT_NUM = 7777;
const short BUFF_SIZE = 1024;

#if defined (__linux__)

#include <strings.h>
#include <sys/socket.h>
#include <unistd.h> //для read()
#include <netinet/in.h> //для структуры sockaddr_in

class m_server {
public:
	m_server();
	~m_server();

	std::vector<int> clients;
	std::mutex clients_mutex;

	// Переменная для контроля выхода из программы
	std::atomic<bool> running = true;	

	void server_start();
	void closeServer();	
private:
	std::shared_ptr<Logger> Log{ new Logger };

	// Создадим сокет  Let's create a socket
	in_addr ip_to_num;
	int socket_server;
	int socket_client;	

	//Структура, описывающая адрес интернет-сокета.
	sockaddr_in clientaddr{};
	//переменная erStat для анализирования ошибок при подключении
	int erStat;	
	//имя получателя сообщения message recipient name
	std::string messageRecipientName;
	std::string clientName;
	char package[BUFF_SIZE];
	//В отличие от std::string, std::string_view не выделяет память
	std::string_view pkg_in{ package, std::size(package) };	

	//логирование
	void log_cout(const std::string& str);
	//работа с пользователем
	void SetToClient(int client_socket);

	void server_loading();
	bool isExit();
	void server_listeng();
	// получить сообщение от клиента
	void receivMessage();
	std::string receivForTable();
	//прием ответов клиента
	void requestAPI(int client_socket);
	//регистрация пользователя
	std::string userregistration(int client_socket);
	//вход пользователя
	std::string userLogin(int client_socket);
	std::string  receivedMessages(const std::string& name);
	//проверка имени
	bool userMessage(int client_socket);
	//написание сообщения
	std::string writingMessage(const std::string& name1, const std::string& name2, const std::string& strMsg);
};

#elif defined(_WIN32) || defined (_WIN64)

//#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

class m_server {
public:
	m_server();
	~m_server();
		
	std::vector<SOCKET> clients;
	std::mutex clients_mutex;

	// Переменная для контроля выхода из программы
	std::atomic<bool> running = true;		
		
	void server_start();
	void closeServer();	
private:
	std::shared_ptr<Logger> Log{ new Logger };

	//Initializing the Socket Library Инициализация Библиотеки cокетов
	WSADATA wsData;
	in_addr ip_to_num;
	SOCKET socket_server;
	SOCKET socket_client;
	//Структура, описывающая адрес интернет-сокета.
	sockaddr_in  clientaddr{};
	//переменная erStat для анализирования ошибок при подключении
	int erStat;
	std::string clientName;
	//имя получателя сообщения message recipient name
	std::string messageRecipientName;
	char package[BUFF_SIZE];
	//В отличие от std::string, std::string_view не выделяет память
	std::string_view pkg_in{ package, std::size(package) };	

	//логирование
	void log_cout(const std::string& str);
	//работа с пользователем
	void SetToClient(SOCKET client_socket);	

	void server_loading();
	bool isExit();
	void server_listeng();
	// получить сообщение от клиента
	void receivMessage();
	std::string receivForTable();
	//прием ответов клиента
	void requestAPI(SOCKET client_socket);
	//регистрация пользователя
	std::string userregistration(SOCKET client_socket);
	//вход пользователя
	std::string userLogin(SOCKET client_socket);
	std::string  receivedMessages(const std::string& name);
	//проверка имени
	bool userMessage(SOCKET client_socket);
	//написание сообщения
	std::string writingMessage(const std::string& name1, const std::string& name2, const std::string& strMsg);
};

#endif
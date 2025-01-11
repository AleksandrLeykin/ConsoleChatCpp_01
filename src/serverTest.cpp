#include "serverTest.h"
#include "mySQLTest.h"

#include <thread>

//переменная - количество активных пользователей
int nclients = 0;

m_server::m_server() {
#if defined (_WIN32) || defined (_WIN64)
	//Задает выходную кодовую страницу, используемую консолью, связанной с вызывающим процессом.
	SetConsoleOutputCP(65001);
	//Задает входную кодовую страницу, используемую консолью, связанной с вызывающим процессом.
	SetConsoleCP(1251);
#endif
	server_loading();
}

m_server::~m_server() {	
	closeServer();		
	std::cout << "server close!" << std::endl;
}

#if defined (__linux__)
void m_server::server_loading() {
	socket_server = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_server == -1) {
		log_cout("[server]ERROR: Socket not created! сокет не создан");
		return;
	}
	sockaddr_in serveraddr;
	//Address for receiving incoming messages. Адрес для приема входящих сообщений
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//port number for communication номер порта для связи 
	serveraddr.sin_port = htons(PORT_NUM);
	//IPv4
	serveraddr.sin_family = AF_INET;

	// Привяжем сокет
	erStat = bind(socket_server, (sockaddr*)&serveraddr, sizeof(serveraddr));
	if (erStat == -1) {
		log_cout("[server]Socket not bind! сокет не привязан!");		
		return;
	}
	else {
		log_cout("Socket binding completed. Привязка сокета выполнена");
	}	
}

#elif defined(_WIN32) || defined (_WIN64)
void m_server::server_loading() {
	erStat = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (erStat != 0) {
		log_cout("[server]ERROR:Error Winsock initialisation #" + WSAGetLastError());
		return;
	}

	socket_server = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_server == INVALID_SOCKET) {
		log_cout("[server]ERROR: Socket not created! сокет не создан");
		// закрываем сокет, завершаем соединение
		closesocket(socket_server);
		WSACleanup();
		return;
	}

	sockaddr_in serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));

	//Address for receiving incoming messages. Адрес для приема входящих сообщений
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	//port number for communication номер порта для связи 
	serveraddr.sin_port = htons(PORT_NUM);
	//IPv4
	serveraddr.sin_family = AF_INET;

	// Привяжем сокет
	erStat = bind(socket_server, (sockaddr*)&serveraddr, sizeof(serveraddr));
	if (erStat != 0) {
		log_cout("[server]Socket not bind! сокет не привязан!");
		closesocket(socket_server);
		WSACleanup();
		exit(1);
	}
	else {
		log_cout("Socket binding completed. Привязка сокета выполнена");
	}
}

#endif

void m_server::server_listeng() {
	//Let's set the server to receive data Поставим сервер на прием данных
	erStat = listen(socket_server, SOMAXCONN);
#if defined (__linux__)
	if (erStat == -1) {
		log_cout("The socket cannot listen for new connections.");
		return;
	}
#elif defined(_WIN32) || defined (_WIN64)
	if (erStat != 0) {
		log_cout("The socket cannot listen for new connections.");
		return;
	}
#endif
	else
		log_cout("[Server] is listening... Ожидание подключений...");
}

#if defined (__linux__)
void m_server::server_start() {
	server_listeng();	
	
	socklen_t length = sizeof(clientaddr);
	//cycle of extracting connection requests from the queue 
	//цикл извлечения запросов на подключение из очереди
	while(running) {
		// Принятие нового клиента
		socket_client = accept(socket_server, (sockaddr*)&clientaddr, &length);
		if (socket_client == -1) {
			log_cout("[server]Error accepting client. Ошибка подключения клиента");			
			continue;
		}
		{
			std::lock_guard<std::mutex> lock(clients_mutex);
			clients.push_back(socket_client);			
		}
		// увеличиваем счетчик подключившихся клиентов 
        nclients++;
		PRINTUSERS
		log_cout("[server]new client connect");

		std::jthread([this] { SetToClient(socket_client); }).detach();
	}
}
#elif defined(_WIN32) || defined (_WIN64)
void m_server::server_start() {
	server_listeng();	
	
	int length = sizeof(clientaddr);
	//cycle of extracting connection requests from the queue цикл извлечения запросов на подключение из очереди
	while (running) {
		// Принятие нового клиента
		socket_client = accept(socket_server, (sockaddr*)&clientaddr, &length);
		if (socket_client == INVALID_SOCKET) {
			log_cout("[server]Error accepting client. Ошибка подключения клиента");	
			closesocket(socket_client);
			continue;
		}
		{
			std::lock_guard<std::mutex> lock(clients_mutex);
			clients.push_back(socket_client);
		}
		// пытаемся получить имя хоста. 
		DWORD clHost;
		char hostName[NI_MAXHOST];
		char servInfo[NI_MAXSERV];
		// вывод сведений о клиенте
		clHost = getnameinfo((sockaddr*)&clientaddr, sizeof(sockaddr),
							hostName, NI_MAXHOST,
							servInfo, NI_MAXSERV, NI_NUMERICSERV);
				
		if (clHost != 0) {
			log_cout("Hostname error # " + WSAGetLastError());
		}
		else {
			// увеличиваем счетчик подключившихся клиентов 
			nclients++;
			PRINTUSERS
				log_cout("Hostname = " + std::string(hostName));
		}

		std::jthread([this] { SetToClient(socket_client); }).detach();
	}
}
#endif

#if defined (__linux__)
void m_server::SetToClient(int client_socket) {
	//receiving a string from a client прием строки от клиента
	while (true) {
		bzero(package, BUFF_SIZE);
		int bytes_rectv = recv(client_socket, package, sizeof(package), 0);

		if (isExit() || bytes_rectv == -1) {
			log_cout("[server]Connection closed Соединение закрыто по запросу клиента." + clientName);
			//nclients--;
			//PRINTUSERS
			break;
		}
		requestAPI(client_socket);
	}
	// если мы здесь, то произошел выход из цикла по причине
	// возращения функцией recv ошибки - соединение с клиентом разорвано
	{
		// Удаляем клиента из списка
		std::lock_guard<std::mutex> lock(clients_mutex);
		clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
	}
	nclients--;
	std::cout << "-disconnect\n";
	PRINTUSERS	
	if(client_socket != -1) {
		close(client_socket);
	}
}

#elif defined(_WIN32) || defined (_WIN64)
void m_server::SetToClient(SOCKET client_socket) {

	//receiving a string from a client прием строки от клиента
	while (true) {
		ZeroMemory(package, BUFF_SIZE);
		int bytes_rectv = recv(client_socket, package, sizeof(package), 0);

		if (isExit() || bytes_rectv <= 0) {
			log_cout("[server]Connection closed Соединение закрыто по запросу клиента." + clientName);
			break;
		}
 		requestAPI(client_socket);
	}
	// если мы здесь, то произошел выход из цикла по причине
	// возращения функцией recv ошибки - соединение с клиентом разорвано
	{
		// Удаляем клиента из списка
		std::lock_guard<std::mutex> lock(clients_mutex);
		clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
	}
		nclients--;
		std::cout << "-disconnect\n";
		PRINTUSERS		
	// закрываем сокет closesocket(client_socket);
	if (client_socket != SOCKET_ERROR) {
		closesocket(client_socket);
	}
}
#endif 

#if defined (__linux__)
void m_server::receivMessage() {
	bzero(package, BUFF_SIZE);
	recv(socket_client, package, sizeof(package), 0);
}
#elif defined(_WIN32) || defined (_WIN64)
void m_server::receivMessage() {
	ZeroMemory(package, BUFF_SIZE);
	int packet_size = recv(socket_client, package, sizeof(package), 0);
}
#endif

std::string m_server::receivForTable() {
	char str[BUFF_SIZE];
#if defined (__linux__)
	bzero(package, BUFF_SIZE);
#elif defined(_WIN32) || defined _WIN64)	
	ZeroMemory(str, BUFF_SIZE);	
#endif
	recv(socket_client, str, sizeof(str), 0);	
	return str;
}

bool m_server::isExit() {
#if defined (__linux__)
	if (pkg_in.starts_with("exit")) {		
		return true;
	}
#elif defined(_WIN32) || defined _WIN64)
	if (pkg_in._Starts_with("exit")) {		
		return true;
	}
#endif
	else
		return false;
}

#if defined (__linux__)
void m_server::requestAPI(int client_socket) {	
	if (pkg_in.starts_with("clientRegistration")) {		
		clientName = userregistration(client_socket);
	}
	else if (pkg_in.starts_with("enterChat")) {
		clientName = userLogin(client_socket);
		log_cout("[server]: user connect - " + clientName);
	}
	else if (pkg_in.starts_with("exit")) {
		close(client_socket);
		log_cout("[server]:Connection closed Соединение закрыто по запросу клиента.");
	}
	else if (pkg_in.starts_with("m")) {
		std::string str_out =  receivedMessages(clientName);
		if (str_out.empty()) {
                str_out = "you no messages!";
            }
            else
                str_out = "messages to you:\n" + str_out 
					+ "\nIf you want to send a message, enter - 'y', to exit - 'exit'";
            send(client_socket, str_out.c_str(), str_out.length(), 0);        
	}	
	else if (pkg_in.starts_with("y")) {
		if (userMessage(client_socket)) {
			std::string str_out = "Enter message!";
			send(client_socket, str_out.c_str(), str_out.length(), 0);
			
			std::string strMessage = receivForTable();
			str_out = writingMessage(messageRecipientName, clientName, strMessage) 
			            + "\nprodolzim?? Enter - 'y', for exit enter - 'exit'";
			send(client_socket, str_out.c_str(), str_out.length(), 0);
		}
		else {
			//std::cout << "not this user!" << std::endl;
			std::string str_out = "There is no such user!";
			send(client_socket, str_out.c_str(), str_out.length(), 0);
		}
	}
}
#elif defined(_WIN32) || defined (_WIN64)
void m_server::requestAPI(SOCKET client_socket) {	
	if (pkg_in._Starts_with("clientRegistration")) {		
		clientName = userregistration(client_socket);
	}
	else if (pkg_in._Starts_with("enterChat")) {
		clientName = userLogin(client_socket);
		log_cout("[server]: user connect - " + clientName);
	}	
	else if (pkg_in._Starts_with("y")) {
		if (userMessage(client_socket)) {
			std::string str_out = "Enter message!";
			send(client_socket, str_out.c_str(), str_out.length(), 0);
			
			std::string strMessage = receivForTable();
			str_out = writingMessage(messageRecipientName, clientName, strMessage) + "\nprodolzim?? Enter - 'y', for exit enter - 'exit'";
			send(client_socket, str_out.c_str(), str_out.length(), 0);
		}
		else {
			//std::cout << "not this user!" << std::endl;
			std::string str_out = "There is no such user!";
			send(client_socket, str_out.c_str(), str_out.length(), 0);
		}
	}
}
#endif

#if defined (__linux__)
std::string m_server::userregistration(int client_socket) {
	std::string str_out = "Enter you name:";
	send(client_socket, str_out.c_str(), str_out.length(), 0);
	receivMessage();
	std::string name = package;

	str_out = "Enter you surname:";
	send(client_socket, str_out.c_str(), str_out.length(), 0);
	receivMessage();
	std::string surname = package;

	str_out = "Enter you email: ";
	send(client_socket, str_out.c_str(), str_out.length(), 0);
	receivMessage();
	std::string email = package;
	
	str_out = "Enter you pass: ";
	send(client_socket, str_out.c_str(), str_out.length(), 0);
	receivMessage();
	std::string pass = package;

	std::unique_ptr<mySQLTest> sqlTest{new mySQLTest};
	std::string result = sqlTest->addingUser(name, surname, email, pass);	
	send(client_socket, result.c_str(), result.length(), 0);
	if (result == "You new user myChat! If you want to send a message, enter - 'y', to exit - 'exit'") {
		log_cout("[server]: new user - " + name);
	}	
	return name;
}
#elif defined(_WIN32) || defined (_WIN64)
std::string m_server::userregistration(SOCKET client_socket) {

	std::string str_out = "Enter you name:";
	send(client_socket, str_out.c_str(), str_out.length(), 0);
	receivMessage();
	std::string name = package;

	str_out = "Enter you surname:";
	send(client_socket, str_out.c_str(), str_out.length(), 0);
	receivMessage();
	std::string surname = package;

	str_out = "Enter you email: ";
	send(client_socket, str_out.c_str(), str_out.length(), 0);
	receivMessage();
	std::string email = package;
	
	str_out = "Enter you pass: ";
	send(client_socket, str_out.c_str(), str_out.length(), 0);
	receivMessage();
	std::string pass = package;

	std::unique_ptr<mySQLTest> sqlTest{new mySQLTest};
	std::string result = sqlTest->addingUser(name, surname, email, pass);	
	send(client_socket, result.c_str(), result.length(), 0);
	if (result == "You new user myChat! If you want to send a message, enter - 'y', to exit - 'exit'") {
		log_cout("[server]: new user - " + name);
	}		
	return name;
}
#endif

#if defined (__linux__)
std::string m_server::userLogin(int client_socket) {
	std::string str_out = "Enter you name:";
	send(client_socket, str_out.c_str(), str_out.length(), 0);
	receivMessage();
	std::string name = package;

	str_out = "Enter you pass: ";
	send(client_socket, str_out.c_str(), str_out.length(), 0);
	receivMessage();
	std::string pass = package;

	std::unique_ptr<mySQLTest> sqlTest{ new mySQLTest };
	std::string result = sqlTest->userLogin(name, pass);
	send(client_socket, result.c_str(), result.length(), 0);
	
	return name;
}

#elif defined(_WIN32) || defined (_WIN64)
std::string m_server::userLogin(SOCKET client_socket) {
	std::string str_out = "Enter you name:";
	send(client_socket, str_out.c_str(), str_out.length(), 0);
	receivMessage();
	std::string name = package;

	str_out = "Enter you pass: ";
	send(client_socket, str_out.c_str(), str_out.length(), 0);
	receivMessage();
	std::string pass = package;

	std::unique_ptr<mySQLTest> sqlTest{ new mySQLTest };
	std::string result = sqlTest->userLogin(name, pass);
	send(client_socket, result.c_str(), result.length(), 0);	
	return name;
}
#endif

std::string  m_server::receivedMessages(const std::string& name) {
	std::unique_ptr<mySQLTest> sqlTest{ new mySQLTest };
	std::string result = "";
    result = sqlTest->viewMessages(name);
    return result;	
}

#if defined (__linux__)
bool m_server::userMessage(int client_socket) {
	std::unique_ptr<mySQLTest> sqlTest{ new mySQLTest };
	std::string result = "Who to send message to? Registered users:\n" + sqlTest->getUser();
	send(client_socket, result.c_str(), result.length(), 0);
	receivMessage();
	
	messageRecipientName = package;

	result = sqlTest->getUser();
	//record names from table запись имен из таблицы
	std::vector<std::string> userName;
	//имя из списка name from the list
	std::string name = "";
	for (int i = 0; i < result.size(); i++) {
		name += result[i];
		if (result[i] == ' ' || result[i] == '\0') {
			userName.push_back(name);
			name = "";
		}
	}
	//сверка имени
	for (int i = 0; i < userName.size(); i++) {
		if (userName[i] == messageRecipientName + " ") {
			return true;
		}
		if (i == (userName.size() - 1) && userName[i] != (messageRecipientName + " ")) {
			return false;
		}
	}
	return false;}
#elif defined(_WIN32) || defined (_WIN64)
bool m_server::userMessage(SOCKET client_socket) {
	std::unique_ptr<mySQLTest> sqlTest{ new mySQLTest };
	std::string result = "Who to send message to? Registered users:\n" + sqlTest->getUser();
	send(client_socket, result.c_str(), result.length(), 0);

	receivMessage();
	//имя получателя сообщения message recipient name
	messageRecipientName = package;

	result = sqlTest->getUser();
	//record names from table запись имен из таблицы
	std::vector<std::string> userName;
	//имя из списка name from the list
	std::string name = "";
	for (int i = 0; i < result.size(); i++) {
		name += result[i];
		if (result[i] == ' ' || result[i] == '\0') {
			userName.push_back(name);
			name = "";
		}
	}
	//сверка имени
	for (int i = 0; i < userName.size(); i++) {
		if (userName[i] == messageRecipientName + " ") {
			return true;
		}
		if (i == (userName.size() - 1) && userName[i] != (messageRecipientName + " ")) {
			return false;
		}
	}
	return false;}
#endif

std::string m_server::writingMessage(const std::string& name1, 
									 const std::string& name2, 
									 const std::string& strMsg) {
	std::unique_ptr<mySQLTest> sqlTest{ new mySQLTest };
	if (sqlTest->writingMessage(name1, name2, strMsg)) {
		log_cout("[server]message from " + name2 + " for " + name1);
		return "Message sent!";
	}
	else {
		log_cout("[server]message transmission ERROR from " + name2 + " for " + name1);
		return "Message not sent!";
	}	
}

#if defined (__linux__)
void m_server::closeServer() {	
	for (int i = 0; i < clients.size(); ++i) {
		if (clients[i] != -1) {			
			close(socket_server);					
		}
	}
	if(!clients.empty())
	clients.clear();
	running = false;	
}
#elif defined(_WIN32) || defined (_WIN64)
void m_server::closeServer() {	
	for (int i = 0; i < clients.size(); ++i) {
		if (clients[i] != SOCKET_ERROR) {
			//Функция завершения работы отключает отправку или получение в сокете.
			shutdown(socket_client, SD_BOTH);
			closesocket(socket_server);					
		}
	}
	if(!clients.empty())
	clients.clear();
	running = false;
		
	WSACleanup();
}
#endif

void m_server::log_cout(const std::string& str) {
	std::thread record(&Logger::recordLogFile, Log, std::cref(str));	
	record.join();			
}

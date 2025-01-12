#include "clientTest.h"
#include "sha256.h"

#if defined (__linux__)
myClient::myClient()
{

}

myClient::~myClient() {
    clientExit();
    std::cout << "Test of exit!" << std::endl;
}

void myClient::clientLoading() {
    // Creating a socket Создадание сокета
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        close(clientSocket);
        std::cout << "Socket not created! Сокет не создан!" << std::endl;
        return;
    }

    bzero(&serverAddr,sizeof(serverAddr));    
    // Setting the server address Установка  адреса сервера
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(SERVER_PORT_NUM);
    serverAddr.sin_family = AF_INET;

    // Establishing a connection to the server  Установка соединения с сервером
    connectionClient = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (connectionClient == -1) {
        close(clientSocket);
        std::cout << "No connection to the server! Нет соединения с сервером!" << std::endl;
        return;
    }

    // Взаимодействие с сервером
    while (true) {        
     
        sendMessage();
        // Check whether client like to stop chatting Проверка, хочет ли клиент прекратить общение
        if(isExit()) {
            close(clientSocket);
            return;
        }
        receivMessage(); 

        if (pkg_in.starts_with("Enter you pass: ")) { 
            pwdEncoding();            
        }
        else {                
            getLineOfText();
        }   
        bzero(package, BUFF_SIZE);       
    }
}

void myClient::sendMessage() {
    packet_size = 0;    
    packet_size = send(clientSocket, m_str.c_str(), m_str.length(), 0);
    if (packet_size == -1) {
        std::cout << "Can't send message to Server. Error # "<< std::endl;
        close(clientSocket);        
        return;
    }
}

void myClient::receivMessage() {
    packet_size = 0;    
    bzero(package, sizeof(package));    
    packet_size = recv(clientSocket, package, sizeof(package), 0);
    if (packet_size == -1) {
        std::cout << "Can't receive message from Server. Error # " << std::endl;
        close(clientSocket);        
        return;
    }
    else {        
        std::cout << "Server message: ";
        for (int i = 0; i < pkg_in.size(); ++i) {
            if (pkg_in[i] == '\0') {
                break;
            }
            std::cout << pkg_in[i];
        }
        std::cout << "\n";
    }        
}

void myClient::pwdEncoding() {    
    SHA256 sha256;
    m_str = "";
    std::cin >> m_str;
    std::cin.clear();
    std::cin.ignore(32767, '\n');

    std::string pasword = sha256(m_str);
    //sending message to server отправка сообщения на сервер   
    if (send(clientSocket, pasword.c_str(), pasword.length(), 0) == -1) {
        close(clientSocket);      
        std::cout << "Can't send message to Server. Error # ";
    }
}

bool myClient::isExit() const {
    if (m_str == "exit") 
        return true;   
    else
        return false;
}

void myClient::clientExit() const{
    
    if (clientSocket != -1) {
        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket);
    }       
}

void myClient::userMessage(int client_sock) {
    setMyString("n");
    //запрос имени name request
    std::string result = receptionTransmissionMes(client_sock, m_str);
    std::cout << result << std::endl;
    //отправка имени sending name
    std::string username = getLineOfText();
    result = receptionTransmissionMes(client_sock, username);
    std::cout << result << std::endl;

    //набор сообщения typing a message
    std::string userMessage = getLineOfText();
    result = receptionTransmissionMes(client_sock, userMessage);
    std::cout << result << std::endl;
}

std::string myClient::receptionTransmissionMes(int client_sock, const std::string& name)
{
    //char serverBuff[BUFF_SIZE];
    std::vector<char> serverBuff(BUFF_SIZE);
    //sending message to server отправка сообщения на сервер  
    if (send(client_sock, name.c_str(), name.length(), 0) == -1) {
        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket);
        return "Can't send message to Server. Error # ";
    }
    //receiving message from server прием сообщения с сервера
    if (recv(client_sock, serverBuff.data(), serverBuff.size(), 0) == -1) {
        shutdown(clientSocket, SHUT_RDWR);
        close(clientSocket);
        return "Can't receive message from Server. Error # ";
    }

    std::string str = "";   
    for (int i = 0; i < serverBuff.size(); i++) {
        if (serverBuff[i] != '\0') {
            str += serverBuff[i];
        }
        else
            break;
    }    
    return str;
}


#elif defined(_WIN64)

void myClient::clientLoading() {   
    // For checking errors in sockets functions // Для проверки ошибок в функциях сокетов
    int erStat;
    // WinSock initialization // Инициализация WinSock
    WSADATA wsData;
    erStat = WSAStartup(MAKEWORD(2, 2), &wsData);
    if (erStat != 0) {
        std::cout << "Error WinSock version initializaion #";
        std::cout << WSAGetLastError();
        return;
    }
    //else 		std::cout << "WinSock initialization is OK" << std::endl;

    // Socket initialization
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cout << "Error initialization socket # " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
    }
    //else 		std::cout << "Client socket initialization is OK" << std::endl;

    // reset the address зануляем адрес   
    ZeroMemory(&serverAddr, sizeof(serverAddr));
    // Установим адрес сервера	
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    // Зададим номер порта
    serverAddr.sin_port = htons(SERVER_PORT_NUM);
    // Используем IPv4
    serverAddr.sin_family = AF_INET;

    // Establishing connection to the server Установим соединение с сервером
    erStat = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (erStat != 0) {
        std::cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }       
    /*else
        std::cout << "Connection established SUCCESSFULLY. Ready to send a message to Server"
        << "Соединение установлено УСПЕШНО.\nГотово к отправке сообщения на сервер" << std::endl;*/
          
    while (true) {              
        // Check whether client like to stop chatting Проверка, хочет ли клиент прекратить общение
        if (isExit()) {
            //The shutdown function disables sends or receives on a socket.
            //Функция выключения отключает отправку и получение данных на сокете.
            shutdown(clientSocket, SD_BOTH);
            closesocket(clientSocket);
            WSACleanup();
            return;
        }
        
        sendMessage();
        
        receivMessage();      
        if (pkg_in._Starts_with("Enter you pass: ")) { 
            pwdEncoding();            
        }
        else {                
            getLineOfText();
        }   
        ZeroMemory(package, BUFF_SIZE);
    }    
}


void myClient::sendMessage() {
    packet_size = 0;    
    packet_size = send(clientSocket, m_str.c_str(), m_str.length(), 0);
    if (packet_size == SOCKET_ERROR) {
        std::cout << "Can't send message to Server. Error # " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }
}

void myClient::receivMessage() {
    packet_size = 0;    
    ZeroMemory(package, BUFF_SIZE);    
    packet_size = recv(clientSocket, package, sizeof(package), 0);
    if (packet_size == SOCKET_ERROR) {
        std::cout << "Can't receive message from Server. Error # " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }
    else {        
        std::cout << "Server message: ";
        for (int i = 0; i < pkg_in.size(); ++i) {
            if (pkg_in[i] == '\0') {
                break;
            }
            std::cout << pkg_in[i];
        }
        std::cout << "\n";
    }
        
}

void myClient::pwdEncoding() {    
    SHA256 sha256;
    m_str = "";
    std::cin >> m_str;
    std::cin.clear();
    std::cin.ignore(32767, '\n');

    std::string pasword = sha256(m_str);
    //sending message to server отправка сообщения на сервер   
    if (send(clientSocket, pasword.c_str(), pasword.length(), 0) == SOCKET_ERROR) {
        closesocket(clientSocket);
        WSACleanup();
        std::cout << "Can't send message to Server. Error # ";
    }
}

void myClient::userMessage(SOCKET client_sock) {
    setMyString("n");
    //запрос имени name request
    std::string result = receptionTransmissionMes(client_sock, m_str);
     
    std::cout << result << std::endl;
    //отправка имени sending name
    std::string username = getLineOfText();
    result = receptionTransmissionMes(client_sock, username);

    std::cout << result << std::endl;

    //набор сообщения typing a message
    std::string userMessage = getLineOfText();
    result = receptionTransmissionMes(client_sock, userMessage);
       
    std::cout << result << std::endl;
}

bool myClient::isExit() const {
    if (m_str == "exit") 
        return true;   
    else
        return false;
}

void myClient::clientExit() const{
    
    if (clientSocket != SOCKET_ERROR) {
        shutdown(clientSocket, SD_BOTH);
        closesocket(clientSocket);
    }    
    WSACleanup();
}


std::string myClient::receptionTransmissionMes(SOCKET client_sock, const std::string& name)
{
    std::vector<char> serverBuff(BUFF_SIZE);

    //sending message to server отправка сообщения на сервер
    if (send(client_sock, name.c_str(), name.length(), 0) == SOCKET_ERROR) {
        closesocket(client_sock);
        WSACleanup();
        return "Can't send message to Server. Error # ";
    }
    //receiving message from server прием сообщения с сервера	
    if (recv(client_sock, serverBuff.data(), serverBuff.size(), 0) == SOCKET_ERROR) {
        closesocket(client_sock);
        WSACleanup();
        return "Can't receive message from Server. Error # ";
    }
    std::string str = "";
    for (int i = 0; i < serverBuff.size(); i++)
    {
        if (serverBuff[i] != '\0') {
            str += serverBuff[i];
        }
        else
            break;
    }
    return str;
}

#endif
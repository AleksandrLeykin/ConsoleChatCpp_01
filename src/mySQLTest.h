#pragma once
#include <iostream>
#include <string>
#include <memory>


#if defined (__linux__)
#include <mysql/mysql.h>
// #include "mysql_connection.h"
// #include <cppconn/driver.h>
// #include <cppconn/exception.h>
// #include <cppconn/prepared_statement.h>
#elif defined(_WIN64)
#include <mysql.h>
#endif

struct mySQLTest {
	mySQLTest();
	~mySQLTest();

	//добавление пользователя set user
	std::string addingUser(const std::string& name, const std::string& surname,
		const std::string& email, const std::string& pas);
	//вход пользователя enter user
	std::string userLogin(const std::string& name, const std::string& pass);

	//просмотр пользователей
	std::string getUser();

	//view messages просмотр сообщений
	std::string viewMessages(const std::string name);

	//writing Message запись сообщения
	bool writingMessage(const std::string& name1, const std::string& name2, const std::string& strMes);

private:
	//дескриптор
	MYSQL mysql;
	//ссылка на структуру MYSQL_RES результат
	MYSQL_RES* res;
	//Представляет одну строку из результата, содержащего строки.
	MYSQL_ROW row;

	//создание таблиц
	bool mySQLDB();

	//подключение или создание БД
	bool connectDB();
};
#include "mySQLTest.h"
#include "Logger.h"

mySQLTest::mySQLTest() {    
    if(mySQLDB())
    	std::cout << "connection to server SQL is OK!" << std::endl;
	else         
        std::cout << "Error: can`t connect to server " << std::endl;    
}

mySQLTest::~mySQLTest() {
    mysql_close(&mysql);
    std::cout << "exit sql!" << std::endl;
}

bool mySQLTest::mySQLDB() {
    if(connectDB()) { 
        //Сооздание базы данных 
	    std::string createDB = "CREATE DATABASE if not EXISTS chattestdb";
	    std::string useDB = "use chattestdb";
        if (mysql_real_query(&mysql, createDB.c_str(), createDB.length()) != 0 ) {
        std::cout << "Error DATABASE mySQL request not sent! " << std::endl;
        return false;
        }

        //подключаемся к базе
        if (mysql_real_query(&mysql, useDB.c_str(), useDB.length()) != 0)
        {
            std::cout << "Error use DATABASE mySQL!" << std::endl;
        }

        //Создание таблиц
        std::string createTable = "CREATE TABLE IF NOT EXISTS user("
		        "id SERIAL PRIMARY KEY AUTO_INCREMENT, "
		        "name VARCHAR(255) NOT NULL, "
		        "surname VARCHAR(255) NOT NULL, "
		        "email VARCHAR(255) NOT NULL, "
		        "password VARCHAR(100) NOT NULL)";
        if (mysql_real_query(&mysql, createTable.c_str(), createTable.length()) != 0) {
            std::cout << "Error table mySQL request not sent! " << std::endl;
            return false;
        }        

        //user1_id - получатель, user2_id - отправитель
        std::string createTableMessage = "CREATE TABLE IF NOT EXISTS messages("
		        "id SERIAL PRIMARY KEY AUTO_INCREMENT, "
		        "user1_id integer NOT NULL REFERENCES user(id), "
		        "user2_id integer NOT NULL REFERENCES user(id), "
		        "data_create DATETIME NOT NULL, "
		        "messages TEXT NOT NULL)";
        if (mysql_real_query(&mysql, createTableMessage.c_str(), createTableMessage.length()) != 0) {
            std::cout << "Error table mySQL request not sent! " << std::endl;
            return false;
        }       
        return true;
    }
    else
    return false;
}

bool mySQLTest::connectDB() {
    //mysql_init(&mysql);
	//if (&mysql == NULL)
    if (!mysql_init(&mysql)) {
		std::cout << "Error: can`t create MySQL - description" << std::endl;
		return false;
	}
    // Подключаемся к серверу
    if (!mysql_real_connect(&mysql, "localhost", "root", "root", nullptr, 0, nullptr, 0)) {
	    // Если нет возможности установить соединение с БД выводим сообщение об ошибке
	    std::cout << "Error: can`t connect to database " << mysql_error(&mysql) << std::endl;
	    return false;
    }
    mysql_set_character_set(&mysql, "utf8");
    return true;
}

std::string mySQLTest::addingUser(const std::string &name, const std::string &surname, const std::string &email, const std::string &pas)
{
    int i =0;
    //подготовка запроса
    std::string record_user = "INSERT INTO user(id, name, surname, email, password)"
		" VALUES(default, '" + name + "', '" + surname + "', '" + email + "', '" + pas + "')";
    
    //запрос
    std::string selectUser = "select name, surname from user";
	if (mysql_real_query(&mysql, selectUser.c_str(), selectUser.length()) != 0) {
		std::cout << "Error: not select name from user!" << mysql_error(&mysql) << std::endl;
	}

    //строка с новым именем
	std::string newName = name + surname;
	//проверка имени в таблице через цикл
	if (res = mysql_store_result(&mysql)) {
		while (row = mysql_fetch_row(res)) {
			std::string currentName = "";
			for (i = 0; i < mysql_num_fields(res); ++i) {
				currentName += row[i];
			}
			//просмотр зарегистрированных клиентов
			//std::cout << currentName << std::endl;
			if (newName == currentName) {
				mysql_close(&mysql);
				return "name taken!";				
			}
		}
	}
	else
	    std::cout << "Error MySql # " << mysql_error(&mysql) << std::endl;

	//если имя не занято записываем его
	if (mysql_real_query(&mysql, record_user.c_str(), record_user.length()) != 0) {
		std::cout << "Error: can`t connect to table user!! " << mysql_error(&mysql) << std::endl;
	}
	mysql_close(&mysql);
	return "You new user myChat! If you want to send a message, enter - 'y', to exit - 'exit'";
}

std::string mySQLTest::userLogin(const std::string &name, const std::string &pass) {
    int i = 0;

    //запрос
	std::string selectUser = "select name, password from user";
	mysql_real_query(&mysql, selectUser.c_str(), selectUser.length());

    //строка с новым именем
	std::string newName = name + pass;
	//проверка имени в таблице через цикл
	if (res = mysql_store_result(&mysql)) {
		while (row = mysql_fetch_row(res)) {
			std::string currentName = "";
			for (i = 0; i < mysql_num_fields(res); ++i) {
				currentName += row[i];
			}
			//просмотр зарегистрированных клиентов
			//std::cout << currentName << std::endl;
			if (newName == currentName) {
				mysql_close(&mysql);
				newName = "";				
				newName = "Hello " + name;
				return newName + "\nIf you want to send a message, enter - 'y'\nto check your messages - 'm', to exit - 'exit'";
			}
		}
	}
	else
		std::cout << "Error MySql # " << mysql_error(&mysql) << std::endl;

   return "name or password is incorrect!";
}

std::string mySQLTest::getUser() {
    int i = 0;
	std::string result = "";
	//запрос
	std::string selectUser = "select name from user";
 	mysql_real_query(&mysql, selectUser.c_str(), selectUser.length());
	if (res = mysql_store_result(&mysql)) {
		while (row = mysql_fetch_row(res)) {
			for (i = 0; i < mysql_num_fields(res); i++) {
				result += row[i];
			}
			result += " ";
		}
	}
	return result;
}

std::string mySQLTest::viewMessages(const std::string name) {
   int i = 0;
	std::string result = "";
	//запрос
	std::string selecMessages = "select u.name, m.data_create, m.messages "
								"from user as u join messages as m "
								"on u.id = m.user2_id and m.user1_id = "
								"(select id from user where name = '" + name + "')";

	mysql_real_query(&mysql, selecMessages.c_str(), selecMessages.length());
	if (res = mysql_store_result(&mysql)) {
		while (row = mysql_fetch_row(res)) {
			for (i = 0; i < mysql_num_fields(res); i++) {
				result = result + row[i] + " ";
			}
			result += "\n";
		}
	}
	return result; 
}

bool mySQLTest::writingMessage(const std::string &name1, const std::string &name2, const std::string &strMes) {
	//указатель на Logger
	std::shared_ptr<Logger> Log(new Logger);
	Log->recordLogFile(name2 + " from " + name1);

    //подготовка запроса //preparing request
	std::string selectMessage = "INSERT INTO messages(id, user1_id, user2_id, data_create, messages)"
		" VALUES(default, "
		"(select id from user where name = '" + name1 + "'), "
		"(select id from user where name = '" + name2 + "'), "
		"(select sysdate()), "
		"'" + strMes + "')";
	if (mysql_real_query(&mysql, selectMessage.c_str(), selectMessage.length()) == 0) {
		return true;
	}
	return false;
}
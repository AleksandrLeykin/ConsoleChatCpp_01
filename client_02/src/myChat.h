#pragma once
#include <iostream>
#include "clientTest.h"

class myChat {
public:
    myChat() {		
	}
    ~myChat() {}

    void interChat();
private:
    //variable for query переменная для запроса
    //std::string m_request = "";

    //очистка буфера (clear buffer)
	void cinClear() {
		std::cin.clear();
		std::cin.ignore(32767, '\n');
	}

    char getChar() {
        char ch = '\0';
        std::cin >> ch;
        cinClear();
        return ch;
    }
    
    //ввод числа (Enter a number)
	int getint() {
		while (true) {
			int num = 0;
			std::cin >> num;
			if (std::cin.fail()) {
				cinClear();
				std::cout << "Веден не верный символ! Invalid character entered!";
			}
			else
				return num;
		}
	}
};
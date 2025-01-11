#include "serverTest.h"
//#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32) || defined (_WIN64)
#include <conio.h>
#endif

std::unique_ptr<m_server> user{ new m_server };
//std::shared_ptr<Logger> Log{ new Logger };


// Переменная для контроля выхода из программы
bool running(true);
//экстренный выход из сервера  не доработано
void ToExit();

int main(int args, char* argv[]) {
	
    std::thread ta(ToExit);
	
	user->server_start();

    if (ta.joinable()) {
        ta.join();
    }
	
	return 0;
}

#if defined (__linux__)
void ToExit() {
    //выход из сервера не доработано
	std::this_thread::sleep_for(std::chrono::microseconds(10));    
    std::cout << "To exit, press ESC" << std::endl;

	while (running) {	
		char c;
    	c = getchar();       
    	if (c == 27) {
   		    running = false;	
			user->closeServer();   		    
    	}
		// Задержка для снижения нагрузки на процессор
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
	}
	exit(0);
}

#elif  defined(_WIN32) || defined(_WIN64)

void ToExit() {
	std::this_thread::sleep_for(std::chrono::microseconds(10));
	std::cout << "To exit, press ESC" << std::endl;
	while (running) {		
		// Проверяем, нажата ли клавиша
		if (_kbhit()) { 
			// Получаем нажатую клавишу
			char ch = _getch();
			// Если нажат Esc (код 27)
			if (ch == 27) {
				running = false;	
				user->closeServer();
			} 
			// Задержка для снижения нагрузки на процессор
			std::this_thread::sleep_for(std::chrono::milliseconds(100)); 
		}
	}	
	exit(0);
}
#endif
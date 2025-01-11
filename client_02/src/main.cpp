#include "myChat.h"
#include <memory>

int main() {
    setlocale(LC_ALL, "ru");
	
    std::cout << "\t\t\tConsole Chat!" << std::endl;
	
	std::unique_ptr<myChat> user1 = std::make_unique<myChat>();
	user1->interChat();
    return 0;
}
#include "myChat.h"
//#include "clientTest.h"
#include <memory>

void myChat::interChat()
{   
    while (true)
    {
        std::cout  << "To enter, press - v, to register - r, to exit - 3 or exit: ";
        char symbol = getChar();        

        std::unique_ptr<myClient> client{new myClient};

        switch (symbol)
        {
        case 'v':              
            client->setMyString("enterChat");
            client->clientLoading();             
            break;
        case 'r':  
            client->setMyString("clientRegistration");
            client->clientLoading();            
            break;
        case '3':	        
            client->setMyString("exit");        					
			return;    
        default:
            std::cout << "Invalid character entered!!" << "\n";
            break;
        }
    }   
}

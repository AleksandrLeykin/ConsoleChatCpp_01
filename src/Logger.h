
#include <iostream>
#include <fstream>
#include <shared_mutex>
#include <string>
#include <chrono>
#include <thread>

//timestamp отметка времени
std::string fileWriteTime();

class Logger {
public:
	Logger();
	~Logger();

	//write to file запись в файл
	void recordLogFile(const std::string& str);
	//reading from file  чтение из файла
	void readLogFile();	
private:	
	void m_fileClose();

	std::fstream m_log;
	std::shared_mutex m_mutex;
	std::string m_logEntry = "";	
#if defined (__linux__)
	std::string m_logfile = "./log.txt";
#elif defined (_WIN64)
	std::string m_logfile = "log.txt";
#endif
};
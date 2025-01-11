#define _CRT_SECURE_NO_WARNINGS

#include "Logger.h"
#include <ctime>

Logger::Logger() {
	//ios::in: file opens for input (reading) файл открывается для ввода (чтения)
	// ios::app: файл открывается для дозаписи. Старые данные не удаляются.
	//The file is opened for additional writing. Old data is not deleted.
	m_log.open(m_logfile, std::ios::in | std::ios::app);
}

Logger::~Logger() {	
	m_log.close();	
}

void Logger::recordLogFile(const std::string& str) {
	//protect flows защищаем потоки
	m_mutex.lock();

	m_log.open(m_logfile, std::ios::app);
	if (m_log.is_open()) {
		//I'm trying to remove the end of the line пробую удалить окончоние строки
		std::string str1 = fileWriteTime();
		str1 = str1.erase(str1.size() - 1);
		m_log << str1 << " " << str << "\n";		
	}
	std::cout << str << std::endl;
	m_fileClose();
	
	m_mutex.unlock();
}

void Logger::readLogFile() {
	//std::this_thread::sleep_for(std::chrono::microseconds(100));
	
	m_mutex.lock_shared();
	//open file for reading окрываем файл для чтения
	if (m_log.is_open()) {
		while (std::getline(m_log, m_logEntry)) {
			std::cout << m_logEntry << std::endl;
		}
	}
	else
		std::cout << "fail not open!" << std::endl;
	m_mutex.unlock_shared();
}

std::string fileWriteTime() {
	time_t now = time(0);	
	return  ctime(&now);
}

void Logger::m_fileClose() {
	m_log.close();
	//std::cout << "file close!";
}
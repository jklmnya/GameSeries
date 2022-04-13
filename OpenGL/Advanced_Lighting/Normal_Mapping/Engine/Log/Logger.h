#pragma once

#include "../Basic/Core.h"

class Logger {

private:
	static Logger* logger;
	Logger(const Logger& logger) = delete;
	Logger& operator=(const Logger& logger) = delete;
	Logger(Logger&& logger) = delete;
	Logger& operator=(Logger&& logger) = delete;

protected:
	Logger() {}

public:
	static Logger* GetLogger() {
		return logger;
	}

	void PrintMessage(const char* message) {
		std::cout << "MESSAGE:\n" << message << '\n';
		std::cout << std::endl;
	}

	void PrintWarring(const char* message) {
		std::cout << "WARRING:\n" << message << '\n';
		std::cout << std::endl;
	}

	void PrintError(const char* message) {
		std::cout << "ERROR:\n" << message << '\n';
		std::cout << std::endl;
	}
};

Logger* Logger::logger = new Logger();
#include "stdafx.h"
#include <iostream>
#include "Logger.h"
#include "FileLogger.h"
#include "Engine/Object.h"
#include "Engine/GameClock.h"

namespace LittleEngine {
	Logger::Severity Logger::logLevel = Logger::Severity::Info;
	std::unique_ptr<FileLogger> Logger::fileLogger = nullptr;

	std::string SeverityString(Logger::Severity severity) {
		char prefix = '[';
		char suffix = ']';
		std::string value = "I";
		switch (severity) {
		case Logger::Severity::Error:
			value = "E";
			break;
		case Logger::Severity::Warning:
			value = "W";
			break;
		case Logger::Severity::Info:
			value = "I";
			break;
		case Logger::Severity::Debug:
			value = "D";
			break;
		case Logger::Severity::HOT:
			value = "H";
			break;
		}
		return prefix + value + suffix;
	}

	void Logger::SetLogLevel(Severity logLevel) {
		Logger::logLevel = logLevel;
		Log(sudo(), "Log Level set to " + SeverityString(logLevel));
	}

	void Logger::Log(const Object& context, const std::string & message, Severity severity) {
		Log(context.GetName(), message, severity);
	}

	void Logger::Log(const std::string& message, Severity severity) {
		Log("", message, severity);
	}

	void Logger::Cout(const std::string& severity, const std::string& caller, const std::string& message) {
		std::string suffix = (caller.length() > 0) ? " [" + caller + "]" : "";
		suffix += (" [" + GameClock::ToString(clock->GetCurrentMilliseconds()) + "]");
		std::string log = severity + " " + message + suffix;
		if (fileLogger != nullptr) fileLogger->AddToBuffer(log);
		std::cout << log << std::endl;
	}

	void Logger::Log(const std::string& caller, const std::string& message, Severity severity) {
#if !DEBUG
		return;
#endif
		if (severity <= Logger::logLevel) {
			Instance().Cout(SeverityString(severity), caller, message);
		}
	}

	Logger::Logger() {
		fileLogger = std::make_unique<FileLogger>("debug.log");
		clock = std::make_unique<SystemClock>();
	}

	Logger& Logger::Instance() {
		static Logger logger;
		return logger;
	}

	void Logger::Log(const sudo& sudo, const std::string& message, Severity severity /* = Severity::Info */) {
		Instance().Cout(SeverityString(severity), "Logger", message);
	}
}

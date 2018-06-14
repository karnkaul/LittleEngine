#include "stdafx.h"
#include "EngineConfig.h"

namespace Game {
	EngineConfig::EngineConfig() {
		persistor = std::make_unique<PropRW>();
		windowTitle = Property(WINDOW_TITLE_KEY(), "Game Window");
		logLevel = Property(LOG_LEVEL_KEY(), "INFO");
		screenWidth = Property(SCREEN_WIDTH_KEY(), "1280");
		screenHeight = Property(SCREEN_HEIGHT_KEY(), "720");
		worldWidth = Property(WORLD_WIDTH_KEY(), "1920");
		worldHeight = Property(WORLD_HEIGHT_KEY(), "1080");
	}

	bool EngineConfig::Load(const std::string& path) {
		bool loaded = persistor->Load(path);

		int width = persistor->GetProp(SCREEN_WIDTH_KEY()).intValue();
		int height = persistor->GetProp(SCREEN_HEIGHT_KEY()).intValue();
		if (width > 0 && height > 0) {
			screenWidth.stringValue = std::to_string(width);
			screenHeight.stringValue = std::to_string(height);
		}

		width = persistor->GetProp(WORLD_WIDTH_KEY()).intValue();
		height = persistor->GetProp(WORLD_HEIGHT_KEY()).intValue();
		if (width > 0 && height > 0) {
			worldWidth.stringValue = std::to_string(width);
			worldHeight.stringValue = std::to_string(height);
		}

		std::string logLevelValue = persistor->GetProp(LOG_LEVEL_KEY());
		if (!logLevelValue.empty()) {
			logLevel.stringValue = logLevelValue;
		}

		std::string windowTitleValue = persistor->GetProp(WINDOW_TITLE_KEY());
		if (!windowTitleValue.empty()) {
			windowTitle.stringValue = windowTitleValue;
		}
		return loaded;
	}

	bool EngineConfig::Save(const std::string& path, bool loadFirst) {
		if (loadFirst) {
			Load(path);
		}
		persistor->SetProp(windowTitle);
		persistor->SetProp(logLevel);
		persistor->SetProp(screenWidth);
		persistor->SetProp(screenHeight);
		persistor->SetProp(worldWidth);
		persistor->SetProp(worldHeight);
		return persistor->Save(path);
	}

	std::string EngineConfig::GetWindowTitle() const {
		return windowTitle.stringValue;
	}

	Logger::Severity EngineConfig::GetLogLevel() const {
		return ParseLogLevel(logLevel.stringValue);
	}

	const Vector2 EngineConfig::GetScreenSize() const {
		return Vector2(screenWidth.intValue(), screenHeight.intValue());
	}

	const Vector2 EngineConfig::GetWorldSize() const {
		return Vector2(worldWidth.intValue(), worldHeight.intValue());
	}

	void EngineConfig::SetWindowTitle(const std::string& windowTitle) {
		this->windowTitle.stringValue = windowTitle;
	}

	void EngineConfig::SetLogLevel(Logger::Severity level) {
		logLevel.stringValue = ParseLogLevel(level);
	}

	void EngineConfig::SetScreenSize(const Vector2& screenSize) {
		screenWidth.stringValue = std::to_string(screenSize.x.GetInt());
		screenHeight.stringValue = std::to_string(screenSize.y.GetInt());
	}

	void EngineConfig::SetWorldSize(const Vector2& worldSize) {
		worldWidth.stringValue = worldSize.x.ToString();
		worldHeight.stringValue = worldSize.y.ToString();
	}

	Logger::Severity EngineConfig::ParseLogLevel(std::string str) {
		Logger::Severity severity = Logger::Severity::Info;
		if (str == "HOT") severity = Logger::Severity::HOT;
		else if (str == "Debug") severity = Logger::Severity::Debug;
		else if (str == "Info") severity = Logger::Severity::Info;
		else if (str == "Warning") severity = Logger::Severity::Warning;
		else if (str == "Error") severity = Logger::Severity::Error;
		return severity;
	}

	std::string EngineConfig::ParseLogLevel(Logger::Severity severity) {
		switch (severity) {
		case Logger::Severity::HOT:
			return "HOT";
		case Logger::Severity::Debug:
			return "Debug";
		case Logger::Severity::Info:
			return "Info";
		case Logger::Severity::Warning:
			return "Warning";
		case Logger::Severity::Error:
			return "Error";
		}
		return "Info";
	}
}

#include "stdafx.h"
#include "EngineConfig.h"

namespace LittleEngine {
	const std::string EngineConfig::WINDOW_TITLE_KEY = "WINDOW_TITLE";
	const std::string EngineConfig::LOG_LEVEL_KEY = "LOG_LEVEL";
	const std::string EngineConfig::SCREEN_WIDTH_KEY = "SCREEN_WIDTH";
	const std::string EngineConfig::SCREEN_HEIGHT_KEY = "SCREEN_HEIGHT";
	const std::string EngineConfig::COLLIDER_SHAPE_WIDTH_KEY = "COLLIDER_SHAPE_BORDER_WIDTH";

	EngineConfig::EngineConfig() {
		persistor = std::make_unique<PropRW>();
		windowTitle = Property(WINDOW_TITLE_KEY, "Game Window");
		logLevel = Property(LOG_LEVEL_KEY, "Info");
		screenWidth = Property(SCREEN_WIDTH_KEY, "1280");
		screenHeight = Property(SCREEN_HEIGHT_KEY, "720");
		colliderBorderWidth = Property(COLLIDER_SHAPE_WIDTH_KEY, "1");
	}

	bool EngineConfig::Load(const std::string& path) {
		bool loaded = persistor->Load(path);

		int width = persistor->GetProp(SCREEN_WIDTH_KEY).intValue();
		int height = persistor->GetProp(SCREEN_HEIGHT_KEY).intValue();
		if (width > 0 && height > 0) {
			screenWidth.stringValue = std::to_string(width);
			screenHeight.stringValue = std::to_string(height);
		}

		std::string logLevelValue = persistor->GetProp(LOG_LEVEL_KEY);
		if (!logLevelValue.empty()) {
			logLevel.stringValue = logLevelValue;
		}

		std::string windowTitleValue = persistor->GetProp(WINDOW_TITLE_KEY);
		if (!windowTitleValue.empty()) {
			windowTitle.stringValue = windowTitleValue;
		}

		int colliderWidth = persistor->GetProp(COLLIDER_SHAPE_WIDTH_KEY).intValue();
		if (colliderWidth > 0) {
			colliderBorderWidth.stringValue = std::to_string(colliderWidth);
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
		persistor->SetProp(colliderBorderWidth);
		return persistor->Save(path);
	}

	std::string EngineConfig::GetWindowTitle() const {
		return windowTitle.stringValue;
	}

	Fixed EngineConfig::GetColliderBorderWidth() const {
		return colliderBorderWidth.intValue();
	}

	Logger::Severity EngineConfig::GetLogLevel() const {
		return ParseLogLevel(logLevel.stringValue);
	}

	const Vector2 EngineConfig::GetScreenSize() const {
		return Vector2(screenWidth.intValue(), screenHeight.intValue());
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

	void EngineConfig::SetColliderBorderWidth(const Fixed& shapeWidth) {
		colliderBorderWidth.stringValue = std::to_string(shapeWidth.GetInt());
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

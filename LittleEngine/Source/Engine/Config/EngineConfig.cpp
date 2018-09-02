#include "stdafx.h"
#include "EngineConfig.h"

namespace LittleEngine {
	const std::string EngineConfig::Data::WINDOW_TITLE_KEY = "WINDOW_TITLE";
	const std::string EngineConfig::Data::LOG_LEVEL_KEY = "LOG_LEVEL";
	const std::string EngineConfig::Data::SCREEN_WIDTH_KEY = "SCREEN_WIDTH";
	const std::string EngineConfig::Data::SCREEN_HEIGHT_KEY = "SCREEN_HEIGHT";
	const std::string EngineConfig::Data::COLLIDER_SHAPE_WIDTH_KEY = "COLLIDER_SHAPE_BORDER_WIDTH";

	Logger::Severity ParseLogLevel(const std::string& str) {
		Logger::Severity severity = Logger::Severity::Info;
		if (str == "HOT") severity = Logger::Severity::HOT;
		else if (str == "Debug") severity = Logger::Severity::Debug;
		else if (str == "Info") severity = Logger::Severity::Info;
		else if (str == "Warning") severity = Logger::Severity::Warning;
		else if (str == "Error") severity = Logger::Severity::Error;
		return severity;
	}

	std::string ParseLogLevel(Logger::Severity severity) {
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

	EngineConfig::EngineConfig() {
		cache.windowTitle = Property(Data::WINDOW_TITLE_KEY, "Game Window");
		cache.logLevel = Property(Data::LOG_LEVEL_KEY, "Info");
		cache.screenWidth = Property(Data::SCREEN_WIDTH_KEY, "1280");
		cache.screenHeight = Property(Data::SCREEN_HEIGHT_KEY, "720");
		cache.colliderBorderWidth = Property(Data::COLLIDER_SHAPE_WIDTH_KEY, "1");
	}

	bool EngineConfig::Load(const std::string& path) {
		Utils::Property::Persistor persistor;
		bool loaded = persistor.Load(path);

		int width = persistor.GetProp(Data::SCREEN_WIDTH_KEY).intValue();
		int height = persistor.GetProp(Data::SCREEN_HEIGHT_KEY).intValue();
		if (width > 0 && height > 0) {
			cache.screenWidth.stringValue = std::to_string(width);
			cache.screenHeight.stringValue = std::to_string(height);
		}

		std::string logLevelValue = persistor.GetProp(Data::LOG_LEVEL_KEY);
		if (!logLevelValue.empty()) {
			cache.logLevel.stringValue = logLevelValue;
		}

		std::string windowTitleValue = persistor.GetProp(Data::WINDOW_TITLE_KEY);
		if (!windowTitleValue.empty()) {
			cache.windowTitle.stringValue = windowTitleValue;
		}

		int colliderWidth = persistor.GetProp(Data::COLLIDER_SHAPE_WIDTH_KEY).intValue();
		if (colliderWidth > 0) {
			cache.colliderBorderWidth.stringValue = std::to_string(colliderWidth);
		}
		return loaded;
	}

	bool EngineConfig::Save(const std::string& path, bool loadFirst) {
		if (loadFirst) {
			Load(path);
		}
		Utils::Property::Persistor persistor;
		persistor.SetProp(cache.windowTitle);
		persistor.SetProp(cache.logLevel);
		persistor.SetProp(cache.screenWidth);
		persistor.SetProp(cache.screenHeight);
		persistor.SetProp(cache.colliderBorderWidth);
		return persistor.Save(path);
	}

	std::string EngineConfig::GetWindowTitle() const {
		return cache.windowTitle.stringValue;
	}

	Fixed EngineConfig::GetColliderBorderWidth() const {
		return cache.colliderBorderWidth.intValue();
	}

	Logger::Severity EngineConfig::GetLogLevel() const {
		return ParseLogLevel(cache.logLevel.stringValue);
	}

	const Vector2 EngineConfig::GetScreenSize() const {
		return Vector2(cache.screenWidth.intValue(), cache.screenHeight.intValue());
	}

	void EngineConfig::SetWindowTitle(const std::string& windowTitle) {
		this->cache.windowTitle.stringValue = windowTitle;
	}

	void EngineConfig::SetLogLevel(const Logger::Severity& level) {
		cache.logLevel.stringValue = ParseLogLevel(level);
	}

	void EngineConfig::SetScreenSize(const Vector2& screenSize) {
		cache.screenWidth.stringValue = std::to_string(screenSize.x.GetInt());
		cache.screenHeight.stringValue = std::to_string(screenSize.y.GetInt());
	}

	void EngineConfig::SetColliderBorderWidth(const Fixed& shapeWidth) {
		cache.colliderBorderWidth.stringValue = std::to_string(shapeWidth.GetInt());
	}
}

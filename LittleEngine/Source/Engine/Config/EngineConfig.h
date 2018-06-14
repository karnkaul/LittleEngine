#pragma once
#include "stdafx.h"

#include "Utils/PropRW.h"	// PropRW
#include "Engine/Logger/Logger.h"	// Logger::Severity
#include "Utils/Vector2.h"	// Vector2

namespace Game {
	class EngineConfig {
	public:
		EngineConfig();
		bool Load(const std::string& path);
		bool Save(const std::string& path, bool loadFirst = true);
		std::string GetWindowTitle() const;
		Logger::Severity GetLogLevel() const;
		const Vector2 GetScreenSize() const;
		const Vector2 GetWorldSize() const;
		void SetWindowTitle(const std::string& windowTitle);
		void SetLogLevel(Logger::Severity level);
		void SetScreenSize(const Vector2& screenSize);
		void SetWorldSize(const Vector2& worldSize);
	private:
		static std::string WINDOW_TITLE_KEY() { return "WINDOW_TITLE"; }
		static std::string LOG_LEVEL_KEY() { return "LOG_LEVEL"; }
		static std::string SCREEN_WIDTH_KEY() { return "SCREEN_WIDTH"; }
		static std::string SCREEN_HEIGHT_KEY() { return "SCREEN_HEIGHT"; }
		static std::string WORLD_WIDTH_KEY() { return "WORLD_WIDTH"; }
		static std::string WORLD_HEIGHT_KEY() { return "WORLD_HEIGHT"; }

		Property windowTitle;
		Property logLevel;
		Property screenWidth;
		Property screenHeight;
		Property worldWidth;
		Property worldHeight;
		std::unique_ptr<PropRW> persistor;

		static Logger::Severity ParseLogLevel(std::string str);
		static std::string ParseLogLevel(Logger::Severity severity);
	};
}

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
		Fixed GetColliderBorderWidth() const;
		Logger::Severity GetLogLevel() const;
		const Vector2 GetScreenSize() const;
		void SetWindowTitle(const std::string& windowTitle);
		void SetLogLevel(Logger::Severity level);
		void SetScreenSize(const Vector2& screenSize);
		void SetColliderBorderWidth(Fixed shapeWidth);
	private:
		const static std::string WINDOW_TITLE_KEY;
		const static std::string LOG_LEVEL_KEY;
		const static std::string SCREEN_WIDTH_KEY;
		const static std::string SCREEN_HEIGHT_KEY;
		const static std::string COLLIDER_SHAPE_WIDTH_KEY;

		Property windowTitle;
		Property logLevel;
		Property screenWidth;
		Property screenHeight;
		Property colliderBorderWidth;
		std::unique_ptr<PropRW> persistor;

		static Logger::Severity ParseLogLevel(std::string str);
		static std::string ParseLogLevel(Logger::Severity severity);
	};
}

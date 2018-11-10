#pragma once
#include "le_stdafx.h"

#include "Property.h"	// PropRW
#include "Vector2.h"	// Vector2
#include "Engine/Logger/Logger.h"	// Logger::Severity

namespace LittleEngine {
	using Fixed = GameUtils::Fixed;
	using Vector2 = GameUtils::Vector2;
	using Property = GameUtils::Property;
	
	// \brief Wrapper to maintain properties saved to / loaded from config.ini
	class EngineConfig {
	public:
		EngineConfig();
		~EngineConfig();	// default destructor won't compile with unique_ptr member to incomplete type

		// Load config file from path and replace cache values if valid
		bool Load(const std::string& path);
		// Save cache to config file at path; loadFirst will call Load() before saving
		bool Save(const std::string& path, bool bLoadFirst = true);

		std::string GetWindowTitle() const;
		Fixed GetColliderBorderWidth() const;
		Logger::Severity GetLogLevel() const;
		const Vector2 GetScreenSize() const;

		void SetWindowTitle(const std::string& windowTitle);
		void SetLogLevel(const Logger::Severity& level);
		void SetScreenSize(const Vector2& screenSize);
		void SetColliderBorderWidth(const Fixed& shapeWidth);

	private:
		std::unique_ptr<struct Data> cache;		// PIMPL
	};
}

#pragma once
#include "le_stdafx.h"

#include "Engine/CoreGame.hpp"
#include "Engine/Logger/Logger.h"	// Logger::Severity

namespace LittleEngine {
	// \brief Wrapper to maintain properties saved to / loaded from config.ini
	class EngineConfig {
	private:
		GData m_data;
		bool m_bDirty = false;

	public:
		EngineConfig();

		// Load config file from path and replace cache values if valid
		bool Load(const std::string& path);
		// Save cache to config file at path; loadFirst will call Load() before saving
		bool Save(const std::string& path);

		std::string GetWindowTitle() const;
		Fixed GetColliderBorderWidth() const;
		Logger::Severity GetLogLevel() const;
		const Vector2 GetScreenSize() const;
		const Vector2 GetViewSize() const;

		bool SetWindowTitle(const std::string& windowTitle);
		bool SetLogLevel(const Logger::Severity& level);
		bool SetScreenSize(const Vector2& screenSize);
		bool SetColliderBorderWidth(const Fixed& shapeWidth);
		bool SetViewSize(const Vector2& viewSize);

	private:
		void Verify();
	};
}

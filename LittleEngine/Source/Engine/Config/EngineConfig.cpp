#include "le_stdafx.h"
#include <unordered_map>
#include "EngineConfig.h"
#include "Engine/CoreGame.hpp"
#include "Utils.h"

namespace LittleEngine {
	namespace {
		const std::string WINDOW_TITLE_KEY = "windowTitle";
		const std::string LOG_LEVEL_KEY = "logLevel";
		const std::string SCREEN_SIZE_KEY = "screenSize";
		const std::string VIEW_SIZE_KEY = "viewSize";
		const std::string COLLIDER_SHAPE_WIDTH_KEY = "colliderShapeBorderWidth";

		std::unordered_map<Logger::Severity, std::string> severityMap = {
			{ Logger::Severity::Error, "Error" },
			{ Logger::Severity::Warning, "Warning" },
			{ Logger::Severity::Info, "Info" },
			{ Logger::Severity::Debug, "Debug" },
			{ Logger::Severity::HOT, "HOT" }
		};

		Logger::Severity ParseLogLevel(const std::string& str) {
			for (const auto& severity : severityMap) {
				if (severity.second == str) return severity.first;
			}
			return Logger::Severity::Info;
		}

		void SetStringIfEmpty(GData& data, const std::string& key, const std::string& value) {
			if (data.GetString(key, "NULL") == "NULL") data.SetString(key, value);
		}
	}

	EngineConfig::EngineConfig() {
		m_data.Clear();
		Verify();
	}

	bool EngineConfig::Load(const std::string & path) {
		m_bDirty = true;
		FileRW file(path);
		m_data.Marshall(file.ReadAll(true));
		bool success = m_data.NumFields() > 0;
		Verify();
		return success;
	}

	bool EngineConfig::Save(const std::string & path) {
		if (m_bDirty) {
			FileRW file(path);
			return file.Write(m_data.Unmarshall());
		}
		return true;
	}

	std::string EngineConfig::GetWindowTitle() const {
		return m_data.GetString(WINDOW_TITLE_KEY);
	}

	Fixed EngineConfig::GetColliderBorderWidth() const {
		return Fixed(m_data.GetDouble(COLLIDER_SHAPE_WIDTH_KEY));
	}

	Logger::Severity EngineConfig::GetLogLevel() const {
		return ParseLogLevel(m_data.GetString(LOG_LEVEL_KEY));
	}

	const Vector2 EngineConfig::GetScreenSize() const {
		GData vec2 = m_data.GetGData(SCREEN_SIZE_KEY);
		return Vector2(vec2.GetInt("x"), vec2.GetInt("y"));
	}

	const Vector2 EngineConfig::GetViewSize() const {
		GData vec2 = m_data.GetGData(VIEW_SIZE_KEY);
		return Vector2(Fixed(vec2.GetDouble("x")), Fixed(vec2.GetDouble("y")));
	}

	bool EngineConfig::SetWindowTitle(const std::string & windowTitle) {
		return m_bDirty = m_data.SetString(WINDOW_TITLE_KEY, windowTitle);
	}

	bool EngineConfig::SetLogLevel(const Logger::Severity & level) {
		return m_bDirty = m_data.SetString(LOG_LEVEL_KEY, severityMap[level]);
	}

	bool EngineConfig::SetScreenSize(const Vector2 & screenSize) {
		GData gData;
		gData.SetString("x", screenSize.x.ToString());
		gData.SetString("y", screenSize.y.ToString());
		return m_bDirty = m_data.AddField(SCREEN_SIZE_KEY, gData);
	}

	bool EngineConfig::SetColliderBorderWidth(const Fixed & shapeWidth) {
		return m_bDirty = m_data.SetString(COLLIDER_SHAPE_WIDTH_KEY, shapeWidth.ToString());
	}

	bool EngineConfig::SetViewSize(const Vector2 & viewSize) {
		GData gData;
		gData.SetString("x", viewSize.x.ToString());
		gData.SetString("y", viewSize.y.ToString());
		return m_bDirty = m_data.AddField(VIEW_SIZE_KEY, gData);
	}
	
	void EngineConfig::Verify() {
		SetStringIfEmpty(m_data, WINDOW_TITLE_KEY, "Game Window");
		SetStringIfEmpty(m_data, LOG_LEVEL_KEY, "Info");
		SetStringIfEmpty(m_data, COLLIDER_SHAPE_WIDTH_KEY, "1.0");
		if (m_data.GetString(SCREEN_SIZE_KEY).empty()) SetScreenSize(Vector2(1920, 1080));
		if (m_data.GetString(VIEW_SIZE_KEY).empty()) SetViewSize(Vector2(1920, 1080));
		m_bDirty = false;
	}
}

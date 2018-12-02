#include "le_stdafx.h"
#include <unordered_map>
#include "EngineConfig.h"
#include "FileRW.h"
#include "Utils.h"

namespace LittleEngine {
	namespace {
		const std::string WINDOW_TITLE_KEY = "windowTitle";
		const std::string LOG_LEVEL_KEY = "logLevel";
		const std::string SCREEN_SIZE_KEY = "screenSize";
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
		data.Clear();
		Verify();
	}

	using FileRW = GameUtils::FileRW;

	bool EngineConfig::Load(const std::string & path) {
		_dirty = true;
		FileRW file(path);
		data.Marshall(file.ReadAll(true));
		bool success = data.NumFields() > 0;
		Verify();
		return success;
	}

	bool EngineConfig::Save(const std::string & path) {
		if (_dirty) {
			FileRW file(path);
			return file.Write(data.Unmarshall());
		}
		return true;
	}

	std::string EngineConfig::GetWindowTitle() const {
		return data.GetString(WINDOW_TITLE_KEY);
	}

	Fixed EngineConfig::GetColliderBorderWidth() const {
		return Fixed(data.GetDouble(COLLIDER_SHAPE_WIDTH_KEY));
	}

	Logger::Severity EngineConfig::GetLogLevel() const {
		return ParseLogLevel(data.GetString(LOG_LEVEL_KEY));
	}

	const Vector2 EngineConfig::GetScreenSize() const {
		GData vec2 = data.GetGData(SCREEN_SIZE_KEY);
		return Vector2(Fixed(vec2.GetInt("x")), vec2.GetInt("y"));
	}

	bool EngineConfig::SetWindowTitle(const std::string & windowTitle) {
		return _dirty = data.SetString(WINDOW_TITLE_KEY, windowTitle);
	}

	bool EngineConfig::SetLogLevel(const Logger::Severity & level) {
		return _dirty = data.SetString(LOG_LEVEL_KEY, severityMap[level]);
	}

	bool EngineConfig::SetScreenSize(const Vector2 & screenSize) {
		GData gData;
		gData.SetString("x", "1280");
		gData.SetString("y", "720");
		return _dirty = data.AddField(SCREEN_SIZE_KEY, gData);
	}

	bool EngineConfig::SetColliderBorderWidth(const Fixed & shapeWidth) {
		return _dirty = data.SetString(COLLIDER_SHAPE_WIDTH_KEY, shapeWidth.ToString());
	}
	
	void EngineConfig::Verify() {
		SetStringIfEmpty(data, WINDOW_TITLE_KEY, "Game Window");
		SetStringIfEmpty(data, LOG_LEVEL_KEY, "Info");
		SetStringIfEmpty(data, COLLIDER_SHAPE_WIDTH_KEY, "1.0");
		if (data.GetString(SCREEN_SIZE_KEY).empty()) SetScreenSize(Vector2(1280, 720));
		_dirty = false;
	}
}

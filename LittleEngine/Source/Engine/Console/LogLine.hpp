#pragma once
#include <string>
#include "SFMLInterface/Rendering/RenderParams.h"

namespace LittleEngine { namespace DebugConsole {
	extern const Colour LOG_TEXT_COLOUR;
	extern const Colour LOG_QUERY_COLOUR;
	extern const Colour LOG_WARNING_COLOUR;

	struct LogLine {
		std::string text;
		Colour colour;
		LogLine(const std::string& text, const Colour& colour)
			: text(text), colour(colour) {
		}
	};
}}

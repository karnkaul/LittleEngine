#pragma once
#include "CoreTypes.h"
#if ENABLED(CONSOLE)
#include "CircularList.hpp"
#include "LogLine.h"
#include "LittleEngine/Input/EngineInput.h"

namespace LittleEngine { namespace Debug {
	class ConsoleInput {
	public:
		static constexpr char CONSOLE_KEY = '`';

		LiveLine m_liveLine;
		String m_query;
		CircularList<String> queryCache;
		EngineInput::Token m_token;
		bool bCyclingQueries = false;

		ConsoleInput();

		bool OnInput(const EngineInput::Frame& frame);
		void Update();
		String GetConsoleLiveLine(bool bShowCursor) const;

	private:
		void UpdateLiveLine(const EngineInput::Frame& frame);
	};
} }
#endif

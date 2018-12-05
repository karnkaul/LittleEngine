#pragma once

#define ENABLED(x) x

#if defined(DEBUG) || defined(_DEBUG)
	#define GAME_DEBUG 1
#endif

#if GAME_DEBUG
	#define DEBUG_CONSOLE 1
	#define LOG_PROFILING 1
#endif

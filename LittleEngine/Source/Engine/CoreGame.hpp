#pragma once
#include "Fixed.h"
#include "FileRW.h"
#include "Vector2.h"
#include "Rect2.h"
#include "GData.h"
#include "UByte.h"
#include "TRange.hpp"
#include "Delegate.hpp"
#include "Transform.h"

#define ENABLED(x) x

#if defined(DEBUG) || defined(_DEBUG)
	#define GAME_DEBUG 1
#endif

#if GAME_DEBUG
	#define DEBUG_CONSOLE 1
	#define LOG_PROFILING 1
	#define DEBUG_PROFILER 1
#endif

using Fixed = GameUtils::Fixed;
using FileRW = GameUtils::FileRW;
using Vector2 = GameUtils::Vector2;
using Rect2 = GameUtils::Rect2;
using GData = GameUtils::GData;
using UByte = GameUtils::UByte;
using Transform = GameUtils::Transform;
template<typename T>
using TRange = GameUtils::TRange<T>;

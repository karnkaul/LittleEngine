#pragma once
#include "Core/Game/GameTypes/StdTypes.h"

namespace LittleEngine
{
enum class LayerID : s32
{
	Zero = 0,
	Background = 10,
	UnderlayFX = 50,
	Default = 100,
	Live = 200,
	WorldFX = 300,
	OverlayFX = 350,
#if defined(DEBUGGING)
	DebugWorld = 390,
#endif
	UI = 400,
	Top = 490,
	Max = 499,
#if defined(DEBUGGING)
	TopWorld = 500,
	TopOverlay = 550,
	_COUNT = TopOverlay + 20
#else
	_COUNT = Max + 1,
#endif
};

// enum class LayerID : s32
//{
//	LAYER_ZERO = 0,
//	LAYER_BACKGROUND = 1,
//	LAYER_DEFAULT = 10,
//	LAYER_LIVE = 20,
//	LAYER_FX = 30,
//	LAYER_UI = 40,
//	LAYER_TOP = 45,
//	LAYER_MAX = 49,
//	_LAYER_COUNT = LAYER_MAX + 1,
//};
} // namespace LittleEngine

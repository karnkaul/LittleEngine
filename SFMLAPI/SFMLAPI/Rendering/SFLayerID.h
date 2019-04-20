#pragma once
namespace LittleEngine
{
enum LayerID
{
	LAYER_ZERO = 0,
	LAYER_BACKGROUND = 10,
	LAYER_DEFAULT = 100,
	LAYER_LIVE = 200,
	LAYER_FX = 300,
	LAYER_UI = 400,
	LAYER_TOP = 490,
	LAYER_MAX = 499,
	_LAYER_COUNT = LAYER_MAX + 1,
};

//enum LayerID
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
}  // namespace LittleEngine

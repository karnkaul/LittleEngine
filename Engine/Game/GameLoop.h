#pragma once
#include "Core/CoreTypes.h"

namespace LittleEngine
{
namespace GameLoop
{
// Globals
extern Task g_createWorlds;
extern Task g_loadShaders;

namespace IDs
{
extern std::string MAIN_MANIFEST;
extern std::string DEFAULT_FONT;
extern std::string GAME_STYLE;
} // namespace IDs

namespace Res
{
extern std::string COOKED_ASSETS;
extern std::string ASSETS_ROOT;
extern std::string MUSIC_ROOT;
extern std::string MAIN_MANIFEST_FILE;

#if !defined(SHIPPING)
extern std::string GAME_CONFIG_FILE;
#endif
} // namespace Resources

enum class ReloadType
{
	World = 0,
	Game,
	App,
	_COUNT
};

bool Init();
s32 Run();
bool Reload(ReloadType type);
}; // namespace GameLoop
} // namespace LittleEngine

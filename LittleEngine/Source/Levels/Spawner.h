#pragma once
#include "Entities/Player.h"

namespace LittleEngine {
	class Level;
	class VFX;

	namespace Spawner {
		void Init(Level& activeLevel);
		void Cleanup();
#pragma region VFX
		// \brief: Loads Asset dependencies
		void VFXExplode_Warm();
		// \brief Dependencies: Assets/VFX/: 00-14.png, 00.wav
		VFX* VFXExplode(const Vector2& position);

#pragma endregion
	}
}

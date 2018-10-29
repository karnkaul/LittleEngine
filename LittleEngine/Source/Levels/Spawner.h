#pragma once
#include "Entities/Player.h"

namespace LittleEngine {
	class Level;
	class VFX;

	namespace Spawner {
		
#pragma region VFX
		// \brief: Loads Asset dependencies
		void VFXExplode_Warm(Level& level);
		// \brief Dependencies: Assets/VFX/: 00-14.png, 00.wav
		std::shared_ptr<VFX> VFXExplode(Level& level, const Vector2& position);

#pragma endregion
	}
}

#include "stdafx.h"
#include "Spawner.h"
#include "Utils/Vector2.h"
#include "Entities/VFX.h"
#include "Levels/Level.h"

namespace LittleEngine {
	namespace Spawner {
		std::shared_ptr<VFX> VFXExplode(Level& activeLevel, const Vector2& position) {
			std::weak_ptr<VFX> v = activeLevel.SpawnActor<VFX>("VFX", position);
			if (auto vfx = v.lock()) {
				AssetPaths spriteSheet("VFX/Explode", 14, "", ".png");
				AssetPaths sfx("VFX/Explode", 1, "", ".wav");
				vfx->Init(spriteSheet, sfx, Fixed(1000), Fixed(3, 10));
				return vfx;
			}
			return nullptr;
		}
	}
}

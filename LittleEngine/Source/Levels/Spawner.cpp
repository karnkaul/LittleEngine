#include "le_stdafx.h"
#include "Spawner.h"
#include "Vector2.h"
#include "Entities/VFX.h"
#include "Levels/Level.h"

namespace LittleEngine {
	namespace Spawner {
		void VFXExplode_Warm(Level& activeLevel) {
			AssetManifest manifest;
			manifest.AddDefinition(AssetType::Texture, AssetPaths("VFX/Explode", 14, "", ".png"));
			manifest.AddDefinition(AssetType::Sound, AssetPaths("VFX/Explode", 1, "", ".wav"));
			activeLevel.GetAssetManager().LoadAll(manifest);
		}

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

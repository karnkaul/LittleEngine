#include "le_stdafx.h"
#include "Spawner.h"
#include "Vector2.h"
#include "Entities/VFX.h"
#include "Levels/Level.h"

namespace LittleEngine {
	namespace Spawner {
		static Level* activeLevel;

		namespace VFXPrototypes {
			VFX* explode = nullptr;

			void Cleanup() {
				if (explode) {
					explode->Destruct();
				}
				explode = nullptr;
			}
		}

		void Init(Level& activeLevel) {
			Spawner::activeLevel = &activeLevel;
		}

		void Cleanup() {
			VFXPrototypes::Cleanup();
			activeLevel = nullptr;
		}

		void VFXExplode_Warm() {
			if (activeLevel) {
				AssetManifest manifest;
				manifest.AddDefinition(AssetType::Texture, AssetPaths("VFX/Explode", 14, "", ".png"));
				manifest.AddDefinition(AssetType::Sound, AssetPaths("VFX/Explode", 1, "", ".wav"));
				activeLevel->GetAssetManager().LoadAll(manifest);

				/*if (VFXPrototypes::explode = activeLevel->SpawnActor<VFX>("Explode", false)) {
					AssetPaths spriteSheet("VFX/Explode", 14, "", ".png");
					AssetPaths sfx("VFX/Explode", 1, "", ".wav");
					VFXPrototypes::explode->Init(spriteSheet, sfx, Fixed(1000), Fixed(3, 10), false);
				}*/
			}
		}

		VFX* VFXExplode(const Vector2&) {
			if (!activeLevel) return nullptr;

			/*if (VFXPrototypes::explode) {
				VFX* explode = (activeLevel->CloneActor<VFX>(*VFXPrototypes::explode));
				explode->GetTransform().localPosition = position;
				return explode;
			}*/
			return nullptr;
		}
	}
}

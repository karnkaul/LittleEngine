#include "le_stdafx.h"
#include "Spawner.h"
#include "Vector2.h"
#include "Entities/VFX.h"
#include "Levels/Level.h"

namespace LittleEngine {
	namespace Spawner {
		static Level* s_activeLevel;

		namespace VFXPrototypes {
			static std::shared_ptr<VFX> s_explode;

			static void Cleanup() {
				if (s_explode) {
					s_explode->Destruct();
				}
				s_explode = nullptr;
			}
		}

		void Init(Level& activeLevel) {
			s_activeLevel = &activeLevel;
		}

		void Cleanup() {
			VFXPrototypes::Cleanup();
			s_activeLevel = nullptr;
		}

		void VFXExplode_Warm() {
			if (s_activeLevel) {
				AssetManifest manifest;
				manifest.AddDefinition(AssetType::Texture, AssetPaths("VFX/Explode", 14, "", ".png"));
				manifest.AddDefinition(AssetType::Sound, AssetPaths("VFX/Explode", 1, "", ".wav"));
				s_activeLevel->GetAssetManager().LoadAll(manifest);

				if (VFXPrototypes::s_explode = s_activeLevel->SpawnActor<VFX>("Explode", false)) {
					AssetPaths spriteSheet("VFX/Explode", 14, "", ".png");
					AssetPaths sfx("VFX/Explode", 1, "", ".wav");
					VFXPrototypes::s_explode->Init(spriteSheet, sfx, Fixed(1000), Fixed(3, 10), false);
				}
			}
		}

		std::shared_ptr<VFX> VFXExplode(const Vector2& position) {
			if (!s_activeLevel) return nullptr;

			if (VFXPrototypes::s_explode) {
				std::shared_ptr<VFX> explode = (s_activeLevel->CloneActor<VFX>(*VFXPrototypes::s_explode));
				explode->GetTransform().localPosition = position;
				return explode;
			}
			return nullptr;
		}
	}
}

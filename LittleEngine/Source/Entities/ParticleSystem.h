#pragma once
#include <memory>
#include "Vector2.h"
#include "TRange.hpp"
#include "UByte.h"
#include "Actor.h"
#include "GData.h"
#include "Levels/Level.h"

namespace LittleEngine {
	struct RenderParams;
	class World;
	class TextureAsset;
	class SoundAsset;

	using Fixed = GameUtils::Fixed;
	using Vector2 = GameUtils::Vector2;
	using UByte = GameUtils::UByte;
	template<typename T>
	using TRange = GameUtils::TRange<T>;
	using GData = GameUtils::GData;

	struct ParticleSpawnData {
		TRange<Vector2> spawnPosition{ Vector2::Zero };
		TRange<Fixed> spreadAngle{ Fixed::Zero };
		TRange<Fixed> emitterAngle{ Fixed::Zero };
		TRange<Fixed> spawnSpeed{ Fixed(10), Fixed(20) };
		TRange<Fixed> spawnAngularSpeed{ Fixed::Zero };
		unsigned int preWarmNumTicks = 50;
		unsigned int numParticles;
		bool bPreWarm = false;
		bool bFireOnce = false;

		ParticleSpawnData(unsigned int numParticles) : numParticles(numParticles) {}

		void Deserialise(const GData& gData);
	};

	struct ParticleLifetimeData {
		TRange<Fixed> timeToLive{ Fixed::OneHalf, Fixed::One };
		TRange<Fixed> scaleOverTime{ Fixed::One };
		TRange<UByte> alphaOverTime{ UByte(255), UByte(0) };

		void Deserialise(const GData& gData);
	};

	struct EmitterData {
		ParticleSpawnData spawnData;
		ParticleLifetimeData lifetimeData;
		Fixed sfxVolume = Fixed::One;
		Fixed startDelay = Fixed::Zero;
		int layerDelta = 0;
		Transform* pParent;
		SoundAsset* pSound;
		
		EmitterData(const World& world, TextureAsset& texture, unsigned int numParticles, SoundAsset* pSound = nullptr);

		void Deserialise(const GData& gData);

		const World& GetWorld() const {
			return *pWorld;
		}

		TextureAsset& GetTexture() const {
			return *pTexture;
		}

	private:
		const World* pWorld;
		TextureAsset* pTexture;
	};

	struct ParticleSystemData {
		ParticleSystemData(Level& level, const GData& psGData);
		
		const std::vector<EmitterData>& GetEmitterDatas() const {
			return emitterDatas;
		}

	private:
		std::vector<EmitterData> emitterDatas;
	};
	
	class ParticleSystem : public Actor {
	public:
		ParticleSystem();	// Cannot default/inline impl, due to forward declared unique_ptr
		virtual ~ParticleSystem();

		void InitParticleSystem(ParticleSystemData&& data);
		void Start();
		void Stop();
		inline bool IsPlaying() const { return bIsPlaying; }

	protected:
		std::vector<std::unique_ptr<class Emitter>> emitters;
		bool bIsPlaying;

		virtual void Tick(const Fixed& deltaTime) override;
		virtual void Render(RenderParams& params) override;
	};
}

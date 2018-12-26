#pragma once
#include <memory>
#include "Vector2.h"
#include "TRange.hpp"
#include "UByte.h"
#include "Actor.h"
#include "GData.h"
#include "SFMLInterface/Assets.h"

namespace LittleEngine {
	struct RenderParams;
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
		
		EmitterData(TextureAsset& texture, unsigned int numParticles, SoundAsset* pSound = nullptr);

		void Deserialise(const GData& gData);

		TextureAsset& GetTexture() const {
			return *pTexture;
		}

	private:
		TextureAsset* pTexture;
	};

	struct ParticleSystemData {
		ParticleSystemData(AssetManager& assetManager, const GData& psGData);
		
		const std::vector<EmitterData>& GetEmitterDatas() const {
			return emitterDatas;
		}

	private:
		std::vector<EmitterData> emitterDatas;
	};
	
	class ParticleSystem : public Actor {
	protected:
		std::vector<std::unique_ptr<class Emitter>> m_emitters;
		bool m_bIsPlaying = false;

	public:
		ParticleSystem();	// Cannot default/inline impl, due to forward declared unique_ptr
		virtual ~ParticleSystem();

		void InitParticleSystem(ParticleSystemData&& data);
		void Start();
		void Stop();
		inline bool IsPlaying() const { return m_bIsPlaying; }

	protected:
		virtual void Tick(const Fixed& deltaMS) override;
		virtual void Render() override;
	};
}

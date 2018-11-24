#pragma once
#include "Vector2.h"
#include "TRange.hpp"
#include "UByte.h"
#include "Actor.h"

namespace LittleEngine {
	struct RenderParams;
	class World;
	class TextureAsset;

	using Fixed = GameUtils::Fixed;
	using Vector2 = GameUtils::Vector2;
	using UByte = GameUtils::UByte;
	template<typename T>
	using TRange = GameUtils::TRange<T>;

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
	};

	struct ParticleLifetimeData {
		TRange<Fixed> timeToLive{ Fixed::OneHalf, Fixed::One };
		TRange<Fixed> scaleOverTime{ Fixed::One };
		TRange<UByte> alphaOverTime{ UByte(255), UByte(0) };
	};

	struct EmitterData {
		ParticleSpawnData spawnData;
		ParticleLifetimeData lifetimeData;
		Fixed startDelay = Fixed::Zero;
		
		EmitterData(const World& world, TextureAsset& texture, unsigned int numParticles);

		const World& GetWorld() const {
			return *world;
		}

		TextureAsset& GetTexture() const {
			return *texture;
		}

	private:
		const World* world;
		TextureAsset* texture;
	};

	class ParticleSystem : public Actor {
	public:
		ParticleSystem(Level& level, const std::string& name, const Vector2& position, const Fixed& rotation);
		virtual ~ParticleSystem();

		void Init(const std::vector<EmitterData>& emitters);
		void Init(std::initializer_list<EmitterData> emitters);
		inline bool IsPlaying() const { return bIsPlaying; }
		void Stop();
		void Restart();

	protected:
		std::vector<class Emitter> emitters;
		bool bIsPlaying;

		virtual void Tick(const Fixed& deltaTime) override;
		virtual void Render(RenderParams& params) override;
	};
}

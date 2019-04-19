#pragma once
#include "TRange.hpp"
#include "LittleEngine/Game/Entity.h"

namespace Core
{
class GData;
}

namespace LittleEngine
{
using GData = Core::GData;
template <typename T>
using TRange = Core::TRange<T>;

struct ParticleSpawnData
{
	TRange<Vector2> spawnPosition{Vector2::Zero};
	TRange<Fixed> spreadAngle{Fixed::Zero};
	TRange<Fixed> emitterAngle{Fixed::Zero};
	TRange<Fixed> spawnSpeed{Fixed(10), Fixed(20)};
	TRange<Fixed> spawnAngularSpeed{Fixed::Zero};
	u32 preWarmNumTicks = 50;
	u32 numParticles;
	bool bPreWarm = false;
	bool bFireOnce = false;

	ParticleSpawnData(u32 numParticles);

	void Deserialise(const GData& gData);
};

struct ParticleLifetimeData
{
	TRange<Fixed> ttlSecs{Fixed::OneHalf, Fixed::One};
	TRange<Fixed> scaleOverTime{Fixed::One};
	TRange<UByte> alphaOverTime{UByte(255), UByte(0)};

	void Deserialise(const GData& gData);
};

struct EmitterData
{
	ParticleSpawnData spawnData;
	ParticleLifetimeData lifetimeData;
	Fixed sfxVolume = Fixed::One;
	Fixed startDelaySecs = Fixed::Zero;
	s32 layerDelta = 0;
	Transform* pParent;
	class SoundAsset* pSound;
	bool bStatic = false;

	EmitterData(class TextureAsset& texture, u32 numParticles, SoundAsset* pSound = nullptr);
	EmitterData(EmitterData&&) = default;

	void Deserialise(const GData& gData);

	TextureAsset& GetTexture() const
	{
		return *pTexture;
	}

private:
	TextureAsset* pTexture;
};

struct ParticleSystemData
{
	Vec<EmitterData> emitterDatas;
	ParticleSystemData(const GData& psGData);
	ParticleSystemData(ParticleSystemData&&) = default;
	ParticleSystemData& operator=(ParticleSystemData&&) = default;
};

class ParticleSystem : public Entity
{
protected:
	Vec<UPtr<class Emitter>> m_emitters;
	bool m_bIsPlaying = false;

public:
	ParticleSystem();
	~ParticleSystem() override;

	void OnCreated() override;
	void InitParticleSystem(ParticleSystemData data);
	void Start();
	void Stop();
	inline bool IsPlaying() const
	{
		return m_bIsPlaying;
	}

	void Tick(Time dt) override;
};
} // namespace LittleEngine

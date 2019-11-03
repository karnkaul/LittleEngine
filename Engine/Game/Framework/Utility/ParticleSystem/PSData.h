#pragma once
#include "Core/CoreTypes.h"
#include "Core/Game/GData.h"
#include "SFMLAPI/Rendering/Colour.h"

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
	Colour spawnColour = Colour::White;
	u32 numParticles;
	bool bPreWarm = false;
	bool bIsOverlay = false;
	bool bIsUnderlay = false;
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
	std::string id;
	Fixed sfxVolume = Fixed::One;
	Fixed startDelaySecs = Fixed::Zero;
	s32 layerDelta = 0;
	class SoundAsset* pSound;
	bool bStatic = false;

	EmitterData(class TextureAsset& texture, u32 numParticles, SoundAsset* pSound = nullptr);
	EmitterData(EmitterData&&) = default;

	void Deserialise(const GData& gData);
	void SetOwner(Entity& owner);

	inline TextureAsset& Texture() const
	{
		return *pTexture;
	}

private:
	Entity* pOwner = nullptr;
	TextureAsset* pTexture;

	friend class Emitter;
};

struct ParticleSystemData
{
	std::vector<EmitterData> emitterDatas;
	ParticleSystemData(const GData& psGData);
	ParticleSystemData(ParticleSystemData&&) = default;
	ParticleSystemData& operator=(ParticleSystemData&&) = default;
};
} // namespace LittleEngine

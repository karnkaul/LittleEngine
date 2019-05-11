#include "stdafx.h"
#include "Core/ArchiveReader.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LittleEngine/Game/GameManager.h"
#include "LittleEngine/Repository/EngineRepository.h"
#include "LittleEngine/Services/Services.h"
#include "PSData.h"

namespace LittleEngine
{
namespace
{
TRange<Fixed> GetTRangeF(const GData& minMax, Fixed defaultMin, Fixed defaultMax)
{
	return TRange<Fixed>(Fixed(minMax.GetF64("min", defaultMin.ToF32())),
						 Fixed(minMax.GetF64("max", defaultMax.ToF32())));
}

TRange<Vector2> GetTRangeV2(const GData& minMaxVec)
{
	GData min = minMaxVec.GetGData("min");
	GData max = minMaxVec.GetGData("max");
	return TRange<Vector2>(Vector2(Fixed(min.GetF64("x", 0.0)), Fixed(min.GetF64("y", 0.0))),
						   Vector2(Fixed(max.GetF64("x", 0.0)), Fixed(max.GetF64("y", 0.0))));
}

TRange<UByte> GetTRangeUB(const GData& minMax)
{
	return TRange<UByte>(UByte(minMax.GetS32("min", 0)), UByte(minMax.GetS32("max", 0)));
}
} // namespace

#define DESERIALISE_TFIXED(gData, m_x) m_x = GetTRangeF(gData.GetGData(#m_x), m_x.min, m_x.max)
#define DESERIALISE_FIXED(gData, m_x) m_x = Fixed(gData.GetF64(#m_x, m_x.ToF64()))
#define DESERIALISE_INT(gData, m_x) m_x = gData.GetS32(#m_x, m_x)
#define DESERIALISE_BOOL(gData, m_x) m_x = gData.GetBool(#m_x, m_x)

ParticleSpawnData::ParticleSpawnData(u32 numParticles) : numParticles(numParticles)
{
}

void ParticleSpawnData::Deserialise(const GData& gData)
{
	spawnPosition = GetTRangeV2(gData.GetGData("spawnPosition"));
	DESERIALISE_TFIXED(gData, spreadAngle);
	DESERIALISE_TFIXED(gData, emitterAngle);
	DESERIALISE_TFIXED(gData, spawnSpeed);
	DESERIALISE_TFIXED(gData, spawnAngularSpeed);
	DESERIALISE_INT(gData, numParticles);
	DESERIALISE_BOOL(gData, bPreWarm);
	DESERIALISE_BOOL(gData, bFireOnce);
}

void ParticleLifetimeData::Deserialise(const GData& gData)
{
	alphaOverTime = GetTRangeUB(gData.GetGData("alphaOverTime"));
	DESERIALISE_TFIXED(gData, scaleOverTime);
	DESERIALISE_TFIXED(gData, ttlSecs);
}

EmitterData::EmitterData(TextureAsset& texture, u32 numParticles, SoundAsset* pSound)
	: spawnData(numParticles), pParent(nullptr), pSound(pSound), pTexture(&texture)
{
}

void EmitterData::Deserialise(const GData& gData)
{
	spawnData.Deserialise(gData.GetGData("spawnData"));
	lifetimeData.Deserialise(gData.GetGData("lifetimeData"));
	DESERIALISE_BOOL(gData, bStatic);
	DESERIALISE_INT(gData, layerDelta);
	DESERIALISE_FIXED(gData, startDelaySecs);
	DESERIALISE_FIXED(gData, sfxVolume);
}

ParticleSystemData::ParticleSystemData(const GData& psGData)
{
	Vec<GData> emitterGDatas = psGData.GetVectorGData("emitters");
	if (!emitterGDatas.empty())
	{
		EngineRepository* pRepo = Services::Game()->Repository();
		for (auto& emitterGData : emitterGDatas)
		{
			String texturePath = emitterGData.GetString("texturePath");
			auto pTexture = pRepo->Load<TextureAsset>(texturePath);
			if (!pTexture)
			{
				LOG_W(
					"Invalid texture path in ParticleSystemManifest [%s]! Ignoring creation of "
					"emitter",
					texturePath.c_str());
				continue;
			}
			SoundAsset* sound = nullptr;
			String soundPath = emitterGData.GetString("soundPath");
			if (!soundPath.empty())
			{
				sound = pRepo->Load<SoundAsset>(soundPath);
			}
			EmitterData emitterData(*pTexture, 10, sound);
			emitterData.Deserialise(emitterGData);
			emitterDatas.emplace_back(std::move(emitterData));
		}
	}
}
} // namespace LittleEngine
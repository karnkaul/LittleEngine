#include "stdafx.h"
#include "ArchiveReader.h"
#include "GData.h"
#include "SFMLAPI/Rendering/Colour.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LittleEngine/Audio/EngineAudio.h"
#include "LittleEngine/Game/GameManager.h"
#include "LittleEngine/RenderLoop/RenderHeap.h"
#include "LittleEngine/Repository/EngineRepository.h"
#include "LittleEngine/Services/Services.h"
#include "ParticleSystem.h"

namespace LittleEngine
{
namespace
{
template <typename T>
T GetRandom(TRange<T> tRange)
{
	return tRange.IsFuzzy() ? Maths::Random::Range(tRange.min, tRange.max) : tRange.min;
}

Vector2 GetRandom(TRange<Vector2> tRange)
{
	return tRange.IsFuzzy() ? Vector2(Maths::Random::Range(tRange.min.x, tRange.max.x),
									  Maths::Random::Range(tRange.min.y, tRange.max.y))
							: tRange.min;
}

Fixed Lerp(TRange<Fixed> tRange, Fixed t)
{
	return Maths::Lerp(tRange.min, tRange.max, t);
}

UByte Lerp(TRange<UByte> tRange, Fixed alpha)
{
	float _t = (Fixed::One - alpha).ToF32();
	float t = alpha.ToF32();
	return _t * tRange.min + t * tRange.max;
}

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
	DESERIALISE_INT(gData, preWarmNumTicks);
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
	DESERIALISE_INT(gData, layerDelta);
	DESERIALISE_FIXED(gData, startDelaySecs);
	DESERIALISE_FIXED(gData, sfxVolume);
}

class Particle final
{
public:
	Transform m_transform;
	Vector2 m_v;
	Fixed m_w;
	Time m_ttl;
	Time m_t;
	LayerID m_layer = LAYER_FX;
	Colour m_c;

private:
	SFPrimitive* m_pSFPrimitive;
	const TextureAsset* m_pTexture;
	bool m_bInUse = false;

public:
	Particle(const TextureAsset& texture);
	Particle(const Particle& copy);
	Particle& operator=(const Particle& copy) = delete;
	~Particle();

	void Init(Vector2 u,
			  const Time& ttl,
			  const Transform& transform = Transform::IDENTITY,
			  Fixed w = Fixed::Zero,
			  Colour c = Colour::White,
			  s32 layerDelta = 0);

	void Tick(Time dt);

private:
	friend class Emitter;
};

Particle::Particle(const TextureAsset& texture) : m_pTexture(&texture)
{
	m_pSFPrimitive = Services::RHeap()->New();
}

Particle::Particle(const Particle& copy) : m_pTexture(copy.m_pTexture)
{
	m_pSFPrimitive = Services::RHeap()->New();
}

Particle::~Particle()
{
	Services::RHeap()->Destroy(m_pSFPrimitive);
}

void Particle::Init(Vector2 u, const Time& ttl, const Transform& transform, Fixed w, Colour c, s32 layerDelta)
{
	m_bInUse = true;
	this->m_ttl = ttl;
	m_t = Time::Zero;
	m_c = c;
	m_v = u;
	this->m_w = w;
	this->m_transform = transform;
	m_layer = static_cast<LayerID>(LAYER_FX + layerDelta);

	m_pSFPrimitive->SetLayer(m_layer)
		->SetPosition(transform.Position(), true)
		->SetOrientation(transform.Orientation(), true)
		->SetTexture(*m_pTexture)
		->SetEnabled(true);
}

void Particle::Tick(Time dt)
{
	if (m_bInUse)
	{
		m_pSFPrimitive->SetPrimaryColour(m_c);
		m_pSFPrimitive->SetScale(m_transform.Scale());
		m_pSFPrimitive->SetOrientation(m_transform.Orientation());
		m_pSFPrimitive->SetPosition(m_transform.Position());

		Fixed ms(dt.AsMilliseconds());
		m_transform.localPosition += ms * m_v;
		m_transform.localOrientation += ms * m_w;
		m_t += dt;

		m_bInUse = m_t < m_ttl;
		m_pSFPrimitive->SetEnabled(m_bInUse);
	}
}

class Emitter final
{
private:
	Vec<Particle> m_particles;
	const EmitterData m_data;
	Time m_elapsed;
	Transform* m_pParent = nullptr;
	SoundPlayer* m_pSoundPlayer = nullptr;
	bool m_bSpawnNewParticles = true;
	bool m_bWaiting = false;
	bool m_bSoundPlayed = false;

public:
	bool m_bEnabled = true;

public:
	Emitter(EmitterData data, bool bSetEnabled = true);
	~Emitter();

	void Reset(bool bSetEnabled);
	void PreWarm(u32 numTicks = 50);
	void Tick(Time dt);

private:
	void Init();
	Particle* Provision();
	Vec<Particle*> ProvisionLot(u32 count);
	void InitParticle(Particle& p);
	void InitParticles();
	void TickInternal(Time dt);
};

Emitter::Emitter(EmitterData data, bool bSetEnabled)
	: m_data(std::move(data)), m_pParent(data.pParent), m_bEnabled(bSetEnabled)
{
	m_particles.reserve(data.spawnData.numParticles);
	for (size_t i = 0; i < data.spawnData.numParticles; ++i)
	{
		TextureAsset& texture = data.GetTexture();
		m_particles.emplace_back(texture);
	}
	Init();
}

Emitter::~Emitter()
{
	if (m_pSoundPlayer)
		m_pSoundPlayer->Stop();
	m_particles.clear();
}

void Emitter::Reset(bool bSetEnabled)
{
	for (size_t i = 0; i < m_particles.size(); ++i)
	{
		m_particles[i].m_bInUse = false;
		if (!bSetEnabled)
			m_particles[i].m_pSFPrimitive->SetEnabled(false);
	}
	if (m_bSoundPlayed && !bSetEnabled && m_pSoundPlayer)
		m_pSoundPlayer->Stop();
	if (bSetEnabled)
		Init();
	m_bEnabled = bSetEnabled;
}

void Emitter::PreWarm(u32 numTicks)
{
	Time dt = Time::Seconds(m_data.lifetimeData.ttlSecs.min.ToF32());
	bool bTemp = m_bSpawnNewParticles;
	m_bSpawnNewParticles = true;
	for (u32 i = 0; i < numTicks; ++i)
		TickInternal(dt);
	m_bSpawnNewParticles = bTemp;
}

void Emitter::Tick(Time dt)
{
	if (!m_bEnabled)
		return;
	if (m_bWaiting)
	{
		m_elapsed += dt;
		if (Fixed(m_elapsed.AsSeconds()) >= m_data.startDelaySecs.ToF32())
		{
			m_bWaiting = false;
		}
		else
		{
			return;
		}
	}

	if (!m_bSoundPlayed && m_data.pSound)
	{
		m_pSoundPlayer = Services::Game()->Audio()->PlaySFX(*m_data.pSound, m_data.sfxVolume,
															Fixed::Zero, m_bSpawnNewParticles);
		m_bSoundPlayed = true;
	}

	TickInternal(dt);
}

void Emitter::Init()
{
	m_bSpawnNewParticles = !m_data.spawnData.bFireOnce;
	m_bWaiting = m_data.startDelaySecs > Fixed::Zero;
	m_elapsed = Time::Zero;
	m_bSoundPlayed = false;
	InitParticles();
	if (m_data.spawnData.bPreWarm)
	{
		PreWarm(m_data.spawnData.preWarmNumTicks);
	}
}

Particle* Emitter::Provision()
{
	auto iter = m_particles.begin();
	while (iter != m_particles.end())
	{
		if (!iter->m_bInUse)
		{
			return &(*iter);
		}
		++iter;
	}
	return nullptr;
}

Vec<Particle*> Emitter::ProvisionLot(u32 count)
{
	Vec<Particle*> ret;
	auto iter = m_particles.begin();
	u32 total = 0;
	while (iter != m_particles.end() && total < count)
	{
		if (!iter->m_bInUse)
		{
			ret.push_back(&*iter);
		}
		++iter;
		++total;
	}
	return ret;
}

void Emitter::InitParticle(Particle& p)
{
	Vector2 velocity;
	if (!m_data.spawnData.spreadAngle.IsFuzzy() && m_data.spawnData.spreadAngle.min == Fixed::Zero)
	{
		TRange<Fixed> velRange{-Fixed::One, Fixed::One};
		velocity = Vector2(GetRandom(velRange), GetRandom(velRange));
		velocity.Normalise();
	}
	else
	{
		velocity = Vector2::ToOrientation(GetRandom(m_data.spawnData.spreadAngle));
	}
	velocity *= GetRandom(m_data.spawnData.spawnSpeed);
	Transform transform = Transform::IDENTITY;
	if (m_pParent)
		transform.SetParent(*m_pParent);
	transform.localPosition = GetRandom(m_data.spawnData.spawnPosition);
	p.Init(velocity, Time::Seconds(GetRandom(m_data.lifetimeData.ttlSecs).ToF32()), transform,
		   GetRandom(m_data.spawnData.spawnAngularSpeed), Colour::White, m_data.layerDelta);
}

void Emitter::InitParticles()
{
	Vec<Particle*> provisioned = ProvisionLot(m_data.spawnData.numParticles);
	for (auto p : provisioned)
	{
		InitParticle(*p);
	}
}

void Emitter::TickInternal(Time dt)
{
	// Update in use
	size_t alive = 0;
	for (size_t i = 0; i < m_particles.size(); ++i)
	{
		Particle& p = m_particles[i];
		p.Tick(dt);
		if (p.m_bInUse)
		{
			++alive;
			Fixed t(p.m_t.AsMilliseconds(), p.m_ttl.AsMilliseconds());
			// Alpha
			p.m_c.a = Lerp(m_data.lifetimeData.alphaOverTime, t);

			// Scale
			Fixed scale = Lerp(m_data.lifetimeData.scaleOverTime, t);
			p.m_transform.localScale = Vector2(scale, scale);
		}
	}

	// Re provision if required
	if (m_bSpawnNewParticles)
	{
		size_t spawnCount = ToIdx(m_data.spawnData.numParticles) - alive;
		for (size_t i = 0; i < spawnCount; ++i)
		{
			Particle* p = Provision();
			if (p)
			{
				InitParticle(*p);
			}
		}
	}
	else if (alive == 0)
	{
		m_bEnabled = false;
	}
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
			TextureAsset* pTexture = pRepo->Load<TextureAsset>(texturePath);
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
				sound = pRepo->Load<SoundAsset>(soundPath);

			EmitterData emitterData(*pTexture, 10, sound);
			emitterData.Deserialise(emitterGData);
			emitterDatas.emplace_back(std::move(emitterData));
		}
	}
}

ParticleSystem::ParticleSystem(String name) : Entity(std::move(name))
{
	SetName(name, "ParticleSystem");
}

ParticleSystem::~ParticleSystem() = default;

void ParticleSystem::InitParticleSystem(ParticleSystemData data)
{
	Vec<EmitterData>& emitters = data.emitterDatas;
	String particles;
	for (EmitterData& eData : emitters)
	{
		eData.pParent = &m_transform;
		particles += (Strings::ToString(eData.spawnData.numParticles) + ", ");
		UPtr<Emitter> emitter = MakeUnique<Emitter>(std::move(eData), false);
		m_emitters.emplace_back(std::move(emitter));
	}
	Core::LogSeverity logSeverity =
		particles.empty() || emitters.empty() ? Core::LogSeverity::Warning : Core::LogSeverity::Debug;
	if (!particles.empty())
		particles = particles.substr(0, particles.length() - 2);
	else
		particles = "0";
	particles = "[" + particles + "] particles";

	Core::Log(logSeverity, "%s initialised: [%d] emitters %s", LogNameStr(), emitters.size(),
			  particles.c_str());
}

void ParticleSystem::Start()
{
	for (auto& uEmitter : m_emitters)
	{
		uEmitter->Reset(true);
	}
	m_bIsPlaying = true;
	LOG_D("%s (re)started", LogNameStr());
}

void ParticleSystem::Stop()
{
	for (auto& uEmitter : m_emitters)
	{
		uEmitter->Reset(false);
	}
	m_bIsPlaying = false;
	LOG_D("%s stopped", LogNameStr());
}

void ParticleSystem::Tick(Time dt)
{
	Entity::Tick(dt);

	bool bAnyPlaying = false;
	for (auto& emitter : m_emitters)
	{
		emitter->Tick(dt);
		bAnyPlaying |= emitter->m_bEnabled;
	}
	m_bIsPlaying = bAnyPlaying;
}
} // namespace LittleEngine

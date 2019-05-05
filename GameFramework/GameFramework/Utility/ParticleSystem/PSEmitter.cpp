#include "stdafx.h"
#include "GData.h"
#include "Utils.h"
#include "SFMLAPI/Rendering/Colour.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "SFMLAPI/Rendering/SFQuad.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LittleEngine/Audio/EngineAudio.h"
#include "LittleEngine/Game/GameManager.h"
#include "LittleEngine/RenderLoop/RenderFactory.h"
#include "LittleEngine/Services/Services.h"
#include "PSEmitter.h"

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
} // namespace

Particle::Particle(SFQuadVec& quadVec, bool& bDraw) : m_pDraw(&bDraw)
{
	m_pQuad = quadVec.AddQuad();
	m_pQuad->SetEnabled(false);
}

Particle::~Particle() = default;

void Particle::Init(Vector2 u, Time ttl, Transform transform, Fixed w, TRange<UByte> alphaOverTime, TRange<Fixed> scaleOverTime)
{
	m_bInUse = true;
	m_ttl = ttl;
	m_t = Time::Zero;
	m_v = u;
	m_w = w;
	m_transform = transform;
	m_aot = alphaOverTime;
	m_sot = scaleOverTime;

	m_pQuad->SetScale({m_sot.min, m_sot.min})
		->SetLocalOrientation(transform.localOrientation)
		->SetPosition(transform.Position());
}

void Particle::Tick(Time dt)
{
	if (m_bInUse)
	{
		Fixed t(m_t.AsMilliseconds(), m_ttl.AsMilliseconds());
		Colour c = Colour::White;
		Fixed s = Lerp(m_sot, t);
		c.a = Lerp(m_aot, t);
		// Scale + Orientation + Position
		if (m_sot.IsFuzzy() && m_w != Fixed::Zero)
		{
			m_pQuad->SetColour(c)
				->SetPosition(Vector2::Zero)
				->SetScale({s, s})
				->SetWorldOrientation(m_transform.localOrientation)
				->SetPosition(m_transform.Position());
		}
		// Scale + Position
		else if (m_sot.IsFuzzy() && m_w == Fixed::Zero)
		{
			m_pQuad->SetColour(c)
				->SetPosition(Vector2::Zero)
				->SetScale({s, s})
				->SetPosition(m_transform.Position());
		}
		// Orientation + Position
		else if (!m_sot.IsFuzzy() && m_w != Fixed::Zero)
		{
			m_pQuad->SetColour(c)
				->SetPosition(Vector2::Zero)
				->SetWorldOrientation(m_transform.localOrientation)
				->SetPosition(m_transform.Position());
		}
		// Position
		else if (!m_sot.IsFuzzy() && m_w == Fixed::Zero)
		{
			m_pQuad->SetColour(c)->SetPosition(m_transform.Position());
		}

		Fixed ms(dt.AsMilliseconds());
		m_transform.localPosition += ms * m_v;
		m_transform.localOrientation += ms * m_w;
		m_t += dt;

		m_bInUse = m_t < m_ttl;
		m_pQuad->SetEnabled(m_bInUse && *m_pDraw);
	}
}

Emitter::Emitter(EmitterData data, bool bSetEnabled)
	: m_data(std::move(data)), m_pParent(data.pParent), m_bEnabled(bSetEnabled)
{
	m_particles.reserve(m_data.spawnData.numParticles);
	m_pSFPrimitive = Services::RFactory()->New(static_cast<LayerID>(LAYER_FX + m_data.layerDelta));
	SFQuadVec* quadVec = m_pSFPrimitive->GetQuadVec();
	quadVec->SetTexture(m_data.GetTexture());
	for (size_t i = 0; i < m_data.spawnData.numParticles; ++i)
	{
		m_particles.emplace_back(*quadVec, m_bDraw);
	}
}

Emitter::~Emitter()
{
	if (m_pSoundPlayer)
	{
		m_pSoundPlayer->Stop();
	}
	m_particles.clear();
	m_pSFPrimitive->Destroy();
}

void Emitter::Reset(bool bSetEnabled)
{
	m_bDraw = false;
	for (size_t i = 0; i < m_particles.size(); ++i)
	{
		m_particles[i].m_bInUse = false;
		m_particles[i].m_pQuad->SetEnabled(false);
	}
	if (m_bSoundPlayed && !bSetEnabled && m_pSoundPlayer)
	{
		m_pSoundPlayer->Stop();
	}
	if (bSetEnabled)
	{
		Init();
	}
	m_bEnabled = bSetEnabled;
}

void Emitter::PreWarm()
{
	bool bTemp = m_bSpawnNewParticles;
	m_bSpawnNewParticles = true;
	TickInternal(Time::Zero, true);
	m_bSpawnNewParticles = bTemp;
}

void Emitter::Tick(Time dt)
{
	if (!m_bEnabled)
	{
		return;
	}
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
		PreWarm();
	}
	m_bDraw = true;
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
	{
		transform.SetParent(*m_pParent);
	}
	transform.localPosition = GetRandom(m_data.spawnData.spawnPosition);
	Time ttl = Time::Seconds(GetRandom(m_data.lifetimeData.ttlSecs).ToF32());
	Fixed w = GetRandom(m_data.spawnData.spawnAngularSpeed);
	TRange<UByte> aot = m_data.lifetimeData.alphaOverTime;
	TRange<Fixed> sot = m_data.lifetimeData.scaleOverTime;
	p.Init(velocity, ttl, transform, w, aot, sot);
}

void Emitter::InitParticles()
{
	Vec<Particle*> provisioned = ProvisionLot(m_data.spawnData.numParticles);
	for (auto p : provisioned)
	{
		InitParticle(*p);
	}
}

void Emitter::TickInternal(Time dt, bool bPreWarming)
{
	// Update in use
	size_t alive = 0;
	Time _dt = dt;
	for (size_t i = 0; i < m_particles.size(); ++i)
	{
		Particle& p = m_particles[i];
		if (bPreWarming)
		{
			Fixed ttlSecs = p.m_ttl.AsSeconds();
			TRange<Fixed> dtRange(Fixed::Zero, ttlSecs);
			_dt = Time::Seconds(GetRandom<Fixed>(dtRange).ToF32());
		}
		p.Tick(_dt);
		if (p.m_bInUse)
		{
			++alive;
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
	// Turn off if bFireOnce and all particles are dead
	else if (alive == 0)
	{
		m_bEnabled = false;
	}
}
} // namespace LittleEngine
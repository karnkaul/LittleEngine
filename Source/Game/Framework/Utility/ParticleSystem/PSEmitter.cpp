#include "Core/GData.h"
#include "Core/Utils.h"
#include "SFMLAPI/Rendering/Colour.h"
#include "SFMLAPI/Rendering/Primitives/Quads.h"
#include "SFMLAPI/Rendering/Primitives/Quad.h"
#include "SFMLAPI/System/Assets.h"
#include "Engine/Audio/LEAudio.h"
#include "Model/GameManager.h"
#include "Model/World/Entity.h"
#include "Engine/Renderer/LERenderer.h"
#include "Engine/Debug/Tweakable.h"
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
	return tRange.IsFuzzy() ? Vector2(Maths::Random::Range(tRange.min.x, tRange.max.x), Maths::Random::Range(tRange.min.y, tRange.max.y))
							: tRange.min;
}

Fixed Lerp(TRange<Fixed> tRange, Fixed t)
{
	return Maths::Lerp(tRange.min, tRange.max, t);
}

UByte Lerp(TRange<UByte> tRange, Fixed alpha)
{
	f32 _t = (Fixed::One - alpha).ToF32();
	f32 t = alpha.ToF32();
	return _t * tRange.min + t * tRange.max;
}
} // namespace

Particle::Particle(Quads& quads, Entity& owner, bool& bDraw) : m_pOwner(&owner), m_pDraw(&bDraw)
{
	Assert(m_pDraw && m_pOwner, "Invariant violated");
	m_pQuad = quads.AddQuad();
	m_pQuad->SetEnabled(false);
}

Particle::~Particle() = default;

void Particle::Init(Vector2 u, Time ttl, Vector2 initPos, Fixed w, TRange<UByte> alphaOverTime, TRange<Fixed> scaleOverTime, Colour colour)
{
	m_bInUse = true;
	m_ttl = ttl;
	m_t = Time::Zero;
	m_v = u;
	m_w = w;
	m_scale = m_pOwner->m_transform.WorldScale();
	m_rot = Vector2::ToOrientation(Vector2::Right);
	m_pos = initPos;
	m_aot = alphaOverTime;
	m_sot = scaleOverTime;
	m_c = colour;

	Matrix3 mat = m_pOwner->WorldMatrix();
	Vector2 worldPos = mat.Position() + m_pos;
	m_pQuad->SetScale({m_sot.min, m_sot.min}, true)->SetPosition(worldPos, true);
}

void Particle::Tick(Time dt)
{
	if (m_bInUse)
	{
		Fixed t(m_t.AsMilliseconds(), m_ttl.AsMilliseconds());
		Colour c = m_c;
		Fixed s = Lerp(m_sot, t);
		c.a = Lerp(m_aot, t);
		bool bImmediate = !m_bWasInUse && m_bInUse;
		m_bWasInUse = true;
		Matrix3 mat = m_pOwner->WorldMatrix();
		Vector2 worldPos = mat.Position() + m_pos;
		m_pQuad->SetColour(c, bImmediate)
			->SetScale({s, s}, bImmediate)
			->SetOrientation(Vector2::ToOrientation(m_rot), bImmediate)
			->SetPosition(worldPos, bImmediate);

		Fixed ms(dt.AsMilliseconds());
		m_pos += ms * m_v;
		m_rot += ms * m_w;
		m_t += dt;

		m_bInUse = m_t < m_ttl;
		m_pQuad->SetEnabled(m_bInUse && *m_pDraw);
	}
}

Emitter::Emitter(EmitterData data, bool bSetEnabled) : m_data(std::move(data)), m_pOwner(data.pOwner), m_bEnabled(bSetEnabled)
{
	Assert(m_pOwner, "Invariant violated");
	m_particles.reserve(m_data.spawnData.numParticles);
	LayerID layer =
		m_data.spawnData.bIsOverlay ? LayerID::OverlayFX : m_data.spawnData.bIsUnderlay ? LayerID::UnderlayFX : LayerID::WorldFX;
	layer = static_cast<LayerID>(ToS32(layer) + m_data.layerDelta);
	m_pQuads = g_pGameManager->Renderer()->New<Quads>(layer);
	m_pQuads->SetTexture(m_data.Texture(), m_data.spawnData.numParticles)->SetEnabled(m_bEnabled);
	for (size_t i = 0; i < m_data.spawnData.numParticles; ++i)
	{
		m_particles.emplace_back(*m_pQuads, *m_pOwner, m_bDraw);
	}
}

Emitter::~Emitter()
{
	if (m_pSoundPlayer)
	{
		m_pSoundPlayer->Stop();
	}
	m_particles.clear();
	m_pQuads->Destroy();
}

Emitter::OnTick::Token Emitter::RegisterOnTick(OnTick::Callback callback)
{
	return m_onTick.Register(callback);
}

void Emitter::Reset(bool bSetEnabled)
{
	m_bDraw = false;
	for (auto& particle : m_particles)
	{
		particle.m_bInUse = false;
		particle.m_pQuad->SetEnabled(false);
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
	m_pQuads->SetEnabled(m_bEnabled);
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
		if (m_elapsed.AsSeconds() >= m_data.startDelaySecs.ToF32())
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
		m_pSoundPlayer = g_pAudio->PlaySFX(*m_data.pSound, m_data.sfxVolume, Fixed::Zero, m_bSpawnNewParticles);
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
	for (auto& particle : m_particles)
	{
		particle.m_bWasInUse = false;
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
	TRange<Vector2> tSpawnPos = m_data.spawnData.spawnPosition;
	if (m_data.spawnData.bIsOverlay || m_data.spawnData.bIsUnderlay)
	{
		tSpawnPos.min = g_pGFX->OverlayProjection(tSpawnPos.min);
		tSpawnPos.max = g_pGFX->OverlayProjection(tSpawnPos.max);
	}
	Vector2 initPos = GetRandom(tSpawnPos);
	Time ttl = Time::Seconds(GetRandom(m_data.lifetimeData.ttlSecs).ToF32());
	Fixed w = GetRandom(m_data.spawnData.spawnAngularSpeed);
	TRange<UByte> aot = m_data.lifetimeData.alphaOverTime;
	TRange<Fixed> sot = m_data.lifetimeData.scaleOverTime;
	Colour colour = m_data.spawnData.spawnColour;
	p.Init(velocity, ttl, initPos, w, aot, sot, colour);
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
	std::atomic<size_t> alive = 0;
	auto updateParticle = [this, bPreWarming, &alive](size_t idx, Time dt) {
		Particle& p = m_particles[idx];
		if (bPreWarming)
		{
			Fixed ttlSecs(p.m_ttl.AsSeconds());
			TRange<Fixed> dtRange(Fixed::Zero, ttlSecs);
			dt = Time::Seconds(GetRandom<Fixed>(dtRange).ToF32());
		}
		m_onTick(p);
		p.Tick(dt);
		if (p.m_bInUse)
		{
			alive.fetch_add(1);
		}
	};

	if (bPreWarming
		|| (m_particles.size() < Quads::s_JOB_QUAD_LIMIT
#ifdef DEBUGGING
			|| !Quads::s_bUSE_JOBS
#endif
			))
	{
		for (size_t i = 0; i < m_particles.size(); ++i)
		{
			updateParticle(i, dt);
		}
	}

	else
	{
		Core::Jobs::ForEach([&](size_t idx) { updateParticle(idx, dt); }, m_particles.size(), Quads::s_JOB_QUAD_LIMIT);
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

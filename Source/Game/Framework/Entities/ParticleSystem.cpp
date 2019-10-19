#include "SFMLAPI/Rendering/Primitives/SFRect.h"
#include "Engine/Debug/Tweakable.h"
#include "ParticleSystem.h"
#include "Engine/Debug/Profiler.h"
#include "Framework/Utility/Debug/DebugProfiler.h"

namespace LittleEngine
{
Fixed g_maxParticlesScale = Fixed::One;

ParticleSystem::ParticleSystem() = default;
ParticleSystem::~ParticleSystem() = default;

void ParticleSystem::OnCreated()
{
	SetType("ParticleSystem");
}

void ParticleSystem::InitParticleSystem(ParticleSystemData data)
{
	std::vector<EmitterData>& emitters = data.emitterDatas;
	std::string particles;
	particles.reserve(32);
	for (EmitterData& eData : emitters)
	{
		if (eData.spawnData.numParticles > 100)
		{
			Fixed numParticles = ToS32(eData.spawnData.numParticles);
			numParticles *= g_maxParticlesScale;
			eData.spawnData.numParticles = numParticles.ToU32();
		}
		eData.SetOwner(*this);
		particles += Strings::ToString(eData.spawnData.numParticles);
		particles += ", ";
		std::unique_ptr<Emitter> emitter = std::make_unique<Emitter>(std::move(eData), false);
		m_emitters.emplace_back(std::move(emitter));
	}
	auto logSeverity = particles.empty() || emitters.empty() ? LE::LogSeverity::Warning : LE::LogSeverity::Debug;
	if (!particles.empty())
	{
		particles = particles.substr(0, particles.length() - 2);
	}
	else
	{
		particles = "0";
	}

	Log(logSeverity, "%s initialised: [%d] emitters [%s] particles", m_logName.data(), emitters.size(), particles.c_str());
#if defined(DEBUGGING)
	m_pO_x->SetEnabled(false);
	m_pO_y->SetEnabled(false);
#endif
}

Emitter* ParticleSystem::GetEmitter(const std::string& id)
{
	auto search =
		std::find_if(m_emitters.begin(), m_emitters.end(), [id](const std::unique_ptr<Emitter>& uEmitter) { return uEmitter->m_data.id == id; });
	return search != m_emitters.end() ? search->get() : nullptr;
}

void ParticleSystem::Start()
{
	for (auto& uEmitter : m_emitters)
	{
		uEmitter->Reset(true);
	}
	m_bIsPlaying = true;
	LOG_D("%s (re)started", m_logName.data());
}

void ParticleSystem::Stop()
{
	for (auto& uEmitter : m_emitters)
	{
		uEmitter->Reset(false);
	}
	m_bIsPlaying = false;
	LOG_D("%s stopped", m_logName.data());
}

void ParticleSystem::Tick(Time dt)
{
#if ENABLED(PROFILER)
	std::string id = m_name;
	Strings::ToUpper(id);
	if (m_profileColour == Colour())
	{
		Maths::Random r(50, 250);
		m_profileColour = Colour(static_cast<u8>(r.Next(false)), static_cast<u8>(r.Next(false)), static_cast<u8>(r.Next(false)));
	}
#endif
	PROFILE_START(id, m_profileColour);
	Super::Tick(dt);

	bool bAnyPlaying = false;
	for (auto& emitter : m_emitters)
	{
		emitter->Tick(dt);
		bAnyPlaying |= emitter->m_bEnabled;
	}
	m_bIsPlaying = bAnyPlaying;
	PROFILE_STOP(id);

#if defined(DEBUGGING)
	m_pO_x->SetEnabled(s_bShowOrientation && m_bIsPlaying);
	m_pO_y->SetEnabled(s_bShowOrientation && m_bIsPlaying);
#endif
}
} // namespace LittleEngine

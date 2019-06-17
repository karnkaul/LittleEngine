#include "stdafx.h"
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/Primitives/SFRect.h"
#include "LittleEngine/Debug/Tweakable.h"
#include "ParticleSystem.h"
#include "LittleEngine/Debug/Profiler.h"
#include "LEGame/Utility/Debug/DebugProfiler.h"

namespace LittleEngine
{
ParticleSystem::ParticleSystem() = default;
ParticleSystem::~ParticleSystem() = default;

void ParticleSystem::OnCreated()
{
	SetType("ParticleSystem");
}

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
	{
		particles = particles.substr(0, particles.length() - 2);
	}
	else
	{
		particles = "0";
	}
	particles = "[" + particles + "] particles";

	Core::Log(logSeverity, "%s initialised: [%d] emitters %s", LogNameStr(), emitters.size(),
			  particles.c_str());
#if DEBUGGING
	m_pO_x->SetEnabled(false);
	m_pO_y->SetEnabled(false);
#endif
}

Emitter* ParticleSystem::GetEmitter(const String& id)
{
	auto search = std::find_if(m_emitters.begin(), m_emitters.end(), [id](const UPtr<Emitter>& uEmitter) {
		return uEmitter->m_data.id == id;
	});
	return search != m_emitters.end() ? search->get() : nullptr;
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
#if ENABLED(PROFILER)
	String id = m_name;
	Strings::ToUpper(id);
	if (m_profileColour == Colour())
	{
		Maths::Random r(50, 250);
		m_profileColour = Colour(r.Next(false), r.Next(false), r.Next(false));
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

#if DEBUGGING
	m_pO_x->SetEnabled(s_bShowOrientation && m_bIsPlaying);
	m_pO_y->SetEnabled(s_bShowOrientation && m_bIsPlaying);
#endif
}
} // namespace LittleEngine

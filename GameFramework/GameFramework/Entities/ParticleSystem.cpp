#include "stdafx.h"
#include "Core/Logger.h"
#include "LittleEngine/Debug/DebugProfiler.h"
#include "LittleEngine/Debug/Console/Tweakable.h"
#include "ParticleSystem.h"
#include "GameFramework/Utility/ParticleSystem/PSEmitter.h"

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
	PROFILE_START(m_name, Colour::Yellow);
	Entity::Tick(dt);

	bool bAnyPlaying = false;
	for (auto& emitter : m_emitters)
	{
		emitter->Tick(dt);
		bAnyPlaying |= emitter->m_bEnabled;
	}
	m_bIsPlaying = bAnyPlaying;
	PROFILE_STOP(m_name);
}
} // namespace LittleEngine

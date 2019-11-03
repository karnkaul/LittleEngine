#pragma once
#include "Game/Model/World/Entity.h"
#include "Game/Framework/Utility/ParticleSystem/PSData.h"
#include "Game/Framework/Utility/ParticleSystem/PSEmitter.h"

namespace Core
{
class GData;
}

namespace LittleEngine
{
class ParticleSystem : public Entity
{
protected:
	std::vector<std::unique_ptr<Emitter>> m_emitters;
	bool m_bIsPlaying = false;
#if ENABLED(PROFILER)
	Colour m_profileColour;
#endif

public:
	ParticleSystem();
	~ParticleSystem() override;

	void OnCreated() override;
	void InitParticleSystem(ParticleSystemData data);
	Emitter* GetEmitter(const std::string& id);

	void Start();
	void Stop();
	inline bool IsPlaying() const
	{
		return m_bIsPlaying;
	}

	void Tick(Time dt) override;
};
} // namespace LittleEngine

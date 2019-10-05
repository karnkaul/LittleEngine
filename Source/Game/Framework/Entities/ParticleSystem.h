#pragma once
#include "Core/TRange.h"
#include "Model/World/Entity.h"
#include "Framework/Utility/ParticleSystem/PSData.h"
#include "Framework/Utility/ParticleSystem/PSEmitter.h"

namespace Core
{
class GData;
}

namespace LittleEngine
{
class ParticleSystem : public Entity
{
protected:
	std::vector<UPtr<Emitter>> m_emitters;
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

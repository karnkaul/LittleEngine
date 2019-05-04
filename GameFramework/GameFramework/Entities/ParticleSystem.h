#pragma once
#include "TRange.hpp"
#include "LittleEngine/Game/Entity.h"
#include "GameFramework/Utility/ParticleSystem/PSData.h"

namespace Core
{
class GData;
}

namespace LittleEngine
{
class ParticleSystem : public Entity
{
protected:
	Vec<UPtr<class Emitter>> m_emitters;
	bool m_bIsPlaying = false;

public:
	ParticleSystem();
	~ParticleSystem() override;

	void OnCreated() override;
	void InitParticleSystem(ParticleSystemData data);
	void Start();
	void Stop();
	inline bool IsPlaying() const
	{
		return m_bIsPlaying;
	}

	void Tick(Time dt) override;
};
} // namespace LittleEngine

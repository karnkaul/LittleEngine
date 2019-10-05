#pragma once
#include "Core/Delegate.h"
#include "PSData.h"

namespace LittleEngine
{
class Particle final
{
public:
	Vector2 m_pos;
	Vector2 m_scale = Vector2::One;
	Fixed m_rot;
	Vector2 m_v;
	TRange<Fixed> m_sot = TRange<Fixed>(Fixed::One);
	TRange<UByte> m_aot = TRange<UByte>(255);
	Fixed m_w;
	Time m_ttl;
	Time m_t;
	Colour m_c;
	class Entity* m_pOwner = nullptr;

private:
	class Quad* m_pQuad = nullptr;
	bool m_bInUse = false;
	bool m_bWasInUse = false;
	bool* m_pDraw;

public:
	Particle(class Quads& quads, Entity& owner, bool& bDraw);
	~Particle();

	void Init(Vector2 u, Time ttl, Vector2 initPos, Fixed w = Fixed::Zero, TRange<UByte> alphaOverTime = TRange<UByte>(255),
			  TRange<Fixed> scaleOverTime = TRange<Fixed>(Fixed::One), Colour colour = Colour::White);

	void Tick(Time dt);

private:
	friend class Emitter;
};

class Emitter final
{
public:
	using OnTick = Core::Delegate<Particle&>;

private:
	std::vector<Particle> m_particles;
	const EmitterData m_data;
	OnTick m_onTick;
	Time m_elapsed;
	Entity* m_pOwner = nullptr;
	class Quads* m_pQuads = nullptr;
	class SoundPlayer* m_pSoundPlayer = nullptr;
	bool m_bSpawnNewParticles = true;
	bool m_bWaiting = false;
	bool m_bSoundPlayed = false;
	bool m_bDraw = false;

public:
	bool m_bEnabled = true;

public:
	Emitter(EmitterData data, bool bSetEnabled = true);
	~Emitter();

	Token RegisterOnTick(OnTick::Callback callback);

	void Reset(bool bSetEnabled);
	void PreWarm();
	void Tick(Time dt);

private:
	void Init();
	Particle* Provision();
	std::vector<Particle*> ProvisionLot(u32 count);
	void InitParticle(Particle& p);
	void InitParticles();
	void TickInternal(Time dt, bool bPreWarming = false);

	friend class ParticleSystem;
};
} // namespace LittleEngine

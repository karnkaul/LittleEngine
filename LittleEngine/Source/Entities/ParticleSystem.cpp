#include "le_stdafx.h"
#include <memory>
#include <array>
#include <vector>
#include "ParticleSystem.h"
#include "Transform.h"
#include "Utils.h"
#include "SFMLInterface/Rendering/Renderable.h"
#include "SFMLInterface/Rendering/RenderParams.h"
#include "SFMLInterface/Rendering/SpriteRenderable.h"
#include "Engine/World.h"
#include "Engine/Logger/Logger.h"

namespace LittleEngine {
	using Transform = GameUtils::Transform;

	namespace {
		const World* pWorld = nullptr;

		template<typename T>
		T GetRandom(TRange<T> tRange) {
			return tRange.IsFuzzy() ? Maths::Random::Range(tRange.min, tRange.max) : tRange.min;
		}

		Vector2 GetRandom(TRange<Vector2> tRange) {
			return tRange.IsFuzzy() ? Vector2(Maths::Random::Range(tRange.min.x, tRange.max.x), Maths::Random::Range(tRange.min.y, tRange.max.y)) : tRange.min;
		}

		Fixed Lerp(TRange<Fixed> tRange, const Fixed& t) {
			return Maths::Lerp(tRange.min, tRange.max, t);
		}
	}

	EmitterData::EmitterData(const World& world, TextureAsset & texture, unsigned int numParticles) : world(&world), texture(&texture), spawnData(numParticles) {
	}

	struct Particle {
		Transform transform;
		Colour colour;
		Vector2 v;
		Fixed w;
		Fixed elapsed;
		Fixed ttl;
		bool bInUse = false;

		void SetSprite(SpriteData&& sprite) {
			renderable = std::make_unique<SpriteRenderable>(sprite, true);
			renderable->layer = LayerID::FX;
		}

		void Init(Vector2 initVelocity, Fixed timeToLive, Transform initTransform = Transform::IDENTITY, Fixed initAngularVelocity = Fixed::Zero, Colour initColour = Colour::White) {
			id = nextID++;
			bInUse = true;
			ttl = timeToLive;
			elapsed = Fixed::Zero;
			colour = initColour;
			v = initVelocity;
			w = initAngularVelocity;
			transform = initTransform;
		}

		unsigned int GetID() const {
			return id;
		}

		void Tick(const Fixed& deltaTime) {
			if (bInUse) {
				transform.localPosition += (deltaTime * v);
				transform.localOrientation += (deltaTime * w);
				elapsed += deltaTime;
				if (elapsed >= ttl) {
					bInUse = false;
				}
			}
		}

		void Render(RenderParams& params) {
			if (bInUse && pWorld && renderable) {
				params.screenPosition = pWorld->WorldToScreenPoint(transform.Position());
				params.screenRotation = pWorld->WorldToScreenRotation(transform.Orientation());
				params.screenScale = transform.Scale();
				SpriteData& data = renderable->GetData();
				data.colour = colour;
				renderable->Render(params);
			}
		}

	private:
		std::unique_ptr<SpriteRenderable> renderable;
		static unsigned int nextID;
		unsigned int id;
	};

	unsigned int Particle::nextID = 0;

	class Emitter {
	public:
		bool bEnabled = true;

		Emitter(const EmitterData& data, bool bSetEnabled = true) : data(data), bEnabled(bSetEnabled) {
			if (!pWorld) pWorld = &data.GetWorld();
			for (size_t i = 0; i < MAX_PARTICLES; ++i) {
				particles[i].SetSprite(SpriteData(this->data.GetTexture()));
			}
			Init();
		}

		void Reset(bool bSetEnabled) {
			for (size_t i = 0; i < MAX_PARTICLES; ++i) {
				particles[i].bInUse = false;
			}
			Init();
			bEnabled = bSetEnabled;
		}

		void PreWarm(unsigned int numTicks = 50) {
			Fixed deltaTime = data.lifetimeData.timeToLive.min * Fixed(100);
			bool bTemp = bSpawnNewParticles;
			bSpawnNewParticles = true;
			for (unsigned int i = 0; i < numTicks; ++i) {
				TickInternal(deltaTime);
			}
			bSpawnNewParticles = bTemp;
		}

		void Tick(const Fixed& deltaTime) {
			if (!bEnabled) return;
			if (bWaiting) {
				elapsed += deltaTime;
				if (elapsed >= data.startDelay) bWaiting = false;
				else return;
			}
			TickInternal(deltaTime);
		}

		void Render(RenderParams& params) {
			if (!bEnabled || bWaiting) return;
			for (size_t i = 0; i < MAX_PARTICLES; ++i) {
				Particle& p = particles[i];
				p.Render(params);
			}
		}
	
	private:
		const static size_t MAX_PARTICLES = 100;
		std::array<Particle, MAX_PARTICLES> particles;
		const EmitterData data;
		Fixed elapsed;
		bool bSpawnNewParticles;
		bool bWaiting;

		void Init() {
			bSpawnNewParticles = !data.spawnData.bFireOnce;
			bWaiting = data.startDelay > Fixed::Zero;
			elapsed = Fixed::Zero;
			InitParticles();
			if (data.spawnData.bPreWarm) {
				PreWarm(data.spawnData.preWarmNumTicks);
			}
		}

		Particle* Provision() {
			auto iter = particles.begin();
			while (iter != particles.end()) {
				if (!iter->bInUse) {
					return &(*iter);
				}
				++iter;
			}
			return nullptr;
		}

		std::vector<Particle*> ProvisionLot(int count) {
			std::vector<Particle*> ret;
			auto iter = particles.begin();
			int total = 0;
			while (iter != particles.end() && total < count) {
				if (!iter->bInUse) ret.push_back(&*iter);
				++iter;
				++total;
			}
			return ret;
		}

		void InitParticle(Particle& p) {
			Vector2 velocity;
			if (!data.spawnData.spreadAngle.IsFuzzy() && data.spawnData.spreadAngle.min == Fixed::Zero) {
				TRange<Fixed> velRange { -Fixed::One, Fixed::One };
				velocity = Vector2(GetRandom(velRange), GetRandom(velRange));
				velocity.Normalise();
			}
			else {
				velocity = Vector2::ToOrientation(GetRandom(data.spawnData.spreadAngle));
			}
			velocity *= GetRandom(data.spawnData.spawnSpeed);
			Transform transform;
			transform.localPosition = GetRandom(data.spawnData.spawnPosition);
			p.Init(velocity, GetRandom(data.lifetimeData.timeToLive) * 1000, transform, GetRandom(data.spawnData.spawnAngularSpeed));
		}

		void InitParticles() {
			std::vector<Particle*> provisioned = ProvisionLot(data.spawnData.numParticles);
			for (auto p : provisioned) {
				InitParticle(*p);
			}
		}

		void TickInternal(const Fixed& deltaTime) {
			// Update in use
			size_t alive = 0;
			for (size_t i = 0; i < MAX_PARTICLES; ++i) {
				Particle& p = particles[i];
				p.Tick(deltaTime);
				if (p.bInUse) {
					++alive;
					Fixed t = p.elapsed / p.ttl;
					// Alpha
					Fixed alpha = Maths::Lerp(data.lifetimeData.alphaOverTime.min.ToUInt(), data.lifetimeData.alphaOverTime.max.ToUInt(), t);
					p.colour.a = UByte(alpha.ToInt());

					// Scale
					Fixed scale = Lerp(data.lifetimeData.scaleOverTime, t);
					p.transform.localScale = Vector2(scale, scale);
				}
			}

			// Re provision if required
			if (bSpawnNewParticles) {
				size_t spawnCount = static_cast<size_t>(data.spawnData.numParticles) - alive;
				for (size_t i = 0; i < spawnCount; ++i) {
					Particle* p = Provision();
					if (p) {
						InitParticle(*p);
					}
				}
			}
			else if (alive == 0) {
				bEnabled = false;
			}
		}
	};

	ParticleSystem::ParticleSystem(Level & level, const std::string & name, const Vector2& position, const Fixed& rotation) : Actor(level, name, position, rotation) {
		Logger::Log(*this, GetNameInBrackets() + " Particle System spawned");
	}


	ParticleSystem::~ParticleSystem() {
		Logger::Log(*this, GetNameInBrackets() + " Particle System destroyed");
	}

	void ParticleSystem::Init(const std::vector<EmitterData>& emitters) {
		for (const EmitterData& data : emitters) {
			this->emitters.emplace_back(data);
		}
		bIsPlaying = true;
	}

	void ParticleSystem::Init(std::initializer_list<EmitterData> emitters) {
		for (const EmitterData& data : emitters) {
			this->emitters.emplace_back(data);
		}
		bIsPlaying = true;
	}

	void ParticleSystem::Stop() {
		for (Emitter& emitter : emitters) {
			emitter.Reset(false);
		}
		bIsPlaying = false;
		Logger::Log(*this, GetNameInBrackets() + " Particle System stopped", Logger::Severity::Debug);
	}

	void ParticleSystem::Restart() {
		for (Emitter& emitter : emitters) {
			emitter.Reset(true);
		}
		bIsPlaying = true;
		Logger::Log(*this, GetNameInBrackets() + " Particle System restarted", Logger::Severity::Debug);
	}

	void ParticleSystem::Tick(const Fixed & deltaTime) {
		for (Emitter& emitter : emitters) {
			emitter.Tick(deltaTime);
		}
	}

	void ParticleSystem::Render(RenderParams & params) {
		for (Emitter& emitter : emitters) {
			emitter.Render(params);
		}
	}
}

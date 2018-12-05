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
#include "Engine/Audio/AudioManager.h"
#include "Levels/Level.h"

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

		TRange<Fixed> GetTRangeF(const GData& minMax, const Fixed& defaultMin, const Fixed& defaultMax) {
			return TRange<Fixed>(Fixed(minMax.GetDouble("min", defaultMin.ToDouble())), Fixed(minMax.GetDouble("max", defaultMax.ToDouble())));
		}

		TRange<Vector2> GetTRangeV2(const GData& minMaxVec) {
			GData min = minMaxVec.GetGData("min");
			GData max = minMaxVec.GetGData("max");
			return TRange<Vector2>(
				Vector2(Fixed(min.GetDouble("x", 0.0f)), Fixed(min.GetDouble("y", 0.0f))),
				Vector2(Fixed(max.GetDouble("x", 0.0f)), Fixed(max.GetDouble("y", 0.0f)))
				);
		}

		TRange<UByte> GetTRangeUB(const GData& minMax) {
			return TRange<UByte>(UByte(minMax.GetInt("min", 0)), UByte(minMax.GetInt("max", 0)));
		}
	}

	struct Particle {
		Transform transform;
		Colour colour;
		Vector2 v;
		Fixed w;
		Fixed elapsed;
		Fixed ttl;
		LayerInfo layer;
		bool bInUse = false;

		void SetSprite(SpriteData&& sprite) {
			renderable = std::make_unique<SpriteRenderable>(sprite, true);
		}

		void Init(Vector2 initVelocity, Fixed timeToLive, Transform initTransform = Transform::IDENTITY, Fixed initAngularVelocity = Fixed::Zero, Colour initColour = Colour::White, int layerDelta = 0) {
			id = nextID++;
			bInUse = true;
			ttl = timeToLive;
			elapsed = Fixed::Zero;
			colour = initColour;
			v = initVelocity;
			w = initAngularVelocity;
			transform = initTransform;
			layer = LayerInfo(static_cast<int>(LayerID::FX) + layerDelta);
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
				params.Reset();
				params.screenPosition = pWorld->WorldToScreenPoint(transform.Position());
				params.screenRotation = pWorld->WorldToScreenRotation(transform.Orientation());
				params.screenScale = transform.Scale();
				SpriteData& data = renderable->GetData();
				data.colour = colour;
				renderable->m_layer = layer;
				renderable->Render(params);
			}
		}

	private:
		std::unique_ptr<SpriteRenderable> renderable;
		static unsigned int nextID;
		unsigned int id;
	};

	unsigned int Particle::nextID = 0;

#define DESERIALISE_TFIXED(gData, m_x) m_x = GetTRangeF(gData.GetGData(#m_x), m_x.min, m_x.max)
#define DESERIALISE_FIXED(gData, m_x) m_x = Fixed(gData.GetDouble(#m_x, m_x.ToDouble()))
#define DESERIALISE_INT(gData, m_x) m_x = gData.GetInt(#m_x, m_x)
#define DESERIALISE_BOOL(gData, m_x) m_x = gData.GetBool(#m_x, m_x)

	void ParticleSpawnData::Deserialise(const GData & gData) {
		spawnPosition = GetTRangeV2(gData.GetGData("spawnPosition"));
		DESERIALISE_TFIXED(gData, spreadAngle);
		DESERIALISE_TFIXED(gData, emitterAngle);
		DESERIALISE_TFIXED(gData, spawnSpeed);
		DESERIALISE_TFIXED(gData, spawnAngularSpeed);
		DESERIALISE_INT(gData, numParticles);
		DESERIALISE_INT(gData, preWarmNumTicks);
		DESERIALISE_BOOL(gData, bPreWarm);
		DESERIALISE_BOOL(gData, bFireOnce);
	}

	void ParticleLifetimeData::Deserialise(const GData & gData) {
		alphaOverTime = GetTRangeUB(gData.GetGData("alphaOverTime"));
		DESERIALISE_TFIXED(gData, scaleOverTime);
		DESERIALISE_TFIXED(gData, timeToLive);
	}

	EmitterData::EmitterData(const World& world, TextureAsset & texture, unsigned int numParticles, SoundAsset* pSound) : spawnData(numParticles), pParent(nullptr), pSound(pSound), pWorld(&world), pTexture(&texture) {
	}

	void EmitterData::Deserialise(const GData & gData) {
		spawnData.Deserialise(gData.GetGData("spawnData"));
		lifetimeData.Deserialise(gData.GetGData("lifetimeData"));
		DESERIALISE_INT(gData, layerDelta);
		DESERIALISE_FIXED(gData, startDelay);
		DESERIALISE_FIXED(gData, sfxVolume);
	}

	class Emitter {
	public:
		Emitter(AudioManager& audioManager, const EmitterData& data, bool bSetEnabled = true) : data(data), audioManager(&audioManager), bEnabled(bSetEnabled) {
			if (!pWorld) pWorld = &data.GetWorld();
			pParent = data.pParent;
			for (size_t i = 0; i < MAX_PARTICLES; ++i) {
				particles[i].SetSprite(SpriteData(this->data.GetTexture()));
			}
			Init();
		}

		~Emitter();

		void Reset(bool bSetEnabled) {
			for (size_t i = 0; i < MAX_PARTICLES; ++i) {
				particles[i].bInUse = false;
			}
			if (bSoundPlayed && !bSetEnabled && pSoundPlayer) pSoundPlayer->Stop();
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
			if (!bSoundPlayed && data.pSound) {
				pSoundPlayer = audioManager->PlaySFX(*data.pSound, data.sfxVolume, Fixed::Zero, bSpawnNewParticles);
				bSoundPlayed = true;
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
		static const size_t MAX_PARTICLES = 100;
		std::array<Particle, MAX_PARTICLES> particles;

		const EmitterData data;
		Fixed elapsed;
		Transform* pParent = nullptr;
		AudioManager* audioManager;
		SoundPlayer* pSoundPlayer = nullptr;
		bool bSpawnNewParticles;
		bool bWaiting;
		bool bSoundPlayed;

	public:
		bool bEnabled = true;

	private:
		void Init() {
			bSpawnNewParticles = !data.spawnData.bFireOnce;
			bWaiting = data.startDelay > Fixed::Zero;
			elapsed = Fixed::Zero;
			bSoundPlayed = false;
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
			if (pParent) transform.SetParent(*pParent);
			transform.localPosition = GetRandom(data.spawnData.spawnPosition);
			p.Init(velocity, GetRandom(data.lifetimeData.timeToLive) * 1000, transform, GetRandom(data.spawnData.spawnAngularSpeed), Colour::White, data.layerDelta);
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

	Emitter::~Emitter() {
		if (pSoundPlayer) pSoundPlayer->Stop();
	}

	ParticleSystem::ParticleSystem() = default;

	ParticleSystem::~ParticleSystem() {
		Logger::Log(*this, GetNameInBrackets() + " Particle System destroyed");
	}

	void ParticleSystem::InitParticleSystem(ParticleSystemData&& data) {
		std::vector<EmitterData> emitters = data.GetEmitterDatas();
		std::string particles;
		for (EmitterData& data : emitters) {
			data.pParent = &m_transform;
			std::unique_ptr<Emitter> emitter = std::make_unique<Emitter>(m_pLevel->GetAudioManager(), data, false);
			particles += (Strings::ToString(data.spawnData.numParticles) + ", ");
			this->m_emitters.push_back(std::move(emitter));
		}
		Logger::Severity logSeverity = particles.empty() || emitters.empty() ? Logger::Severity::Warning : Logger::Severity::Debug;
		if (!particles.empty()) particles = particles.substr(0, particles.length() - 2);
		else particles = "0";
		particles = "[" + particles + "] particles";
		
		Logger::Log(*this, GetNameInBrackets() + " Particle System initialised: [" + Strings::ToString(emitters.size()) + "] emitters " + particles, logSeverity);
	}

	void ParticleSystem::Start() {
		for (std::unique_ptr<Emitter>& emitter : m_emitters) {
			emitter->Reset(true);
		}
		m_bIsPlaying = true;
		Logger::Log(*this, GetNameInBrackets() + " Particle System (re)started", Logger::Severity::Debug);
	}

	void ParticleSystem::Stop() {
		for (std::unique_ptr<Emitter>& emitter : m_emitters) {
			emitter->Reset(false);
		}
		m_bIsPlaying = false;
		Logger::Log(*this, GetNameInBrackets() + " Particle System stopped", Logger::Severity::Debug);
	}

	void ParticleSystem::Tick(const Fixed & deltaTime) {
		for (std::unique_ptr<Emitter>& emitter : m_emitters) {
			emitter->Tick(deltaTime);
			m_bIsPlaying &= emitter->bEnabled;
		}
	}

	void ParticleSystem::Render(RenderParams & params) {
		for (std::unique_ptr<Emitter>& emitter : m_emitters) {
			emitter->Render(params);
		}
	}

	ParticleSystemData::ParticleSystemData(Level& level, const GData & psGData) {
		std::vector<GData> emitterGDatas = psGData.GetVectorGData("emitters");
		if (!emitterGDatas.empty()) {
			for (auto& emitterGData : emitterGDatas) {
				std::string texturePath = emitterGData.GetString("texturePath");
				TextureAsset* textureAsset = level.GetAssetManager().Load<TextureAsset>(texturePath);
				if (!textureAsset) {
					Logger::Log("Invalid texture path in ParticleSystemManifest [" + texturePath + "]! Ignoring creation of emitter", Logger::Severity::Warning);
					continue;
				}
				SoundAsset* sound = nullptr;
				std::string soundPath = emitterGData.GetString("soundPath");
				if (!soundPath.empty()) sound = level.GetAssetManager().Load<SoundAsset>(soundPath);
				
				EmitterData emitterData(level.GetWorld(), *textureAsset, 10, sound);
				emitterData.pSound = sound; 
				emitterData.Deserialise(emitterGData);
				emitterDatas.emplace_back(std::move(emitterData));
			}
		}
	}
}

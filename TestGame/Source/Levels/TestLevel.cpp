#include "stdafx.h"
#include "TestLevel.h"
#include "Entities/ParticleSystem.h"
#include "Utils.h"
#include "FileRW.h"
#include "GData.h"
#include "SFMLInterface/All.h"
#include "Components/All.h"
#include "Engine/All.h"
#include "Entities/All.h"
#include "UI/All.h"

namespace LittleEngine {
	using FileRW = GameUtils::FileRW;

	// Tests
	namespace _TestLevel {
		Level* pLevel;
		int actor0ID, playerID;
		bool bParented = false;
		bool bSoundPlayed = false, bMusicPlayed = false, bMusicStopped = false;
		
		void OnXPressed() {
			auto actor0 = pLevel->FindActor(actor0ID);
			auto player = pLevel->FindActor(playerID);
			if (actor0 != nullptr && player != nullptr) {
				if (!bParented) {
					actor0->GetTransform().SetParent(player->GetTransform());
					bParented = true;
				}
				else {
					actor0->GetTransform().UnsetParent();
					bParented = false;
				}
			}
		}
		void OnYPressed() {
			auto actor0 = pLevel->FindActor(actor0ID);
			auto player = pLevel->FindActor(playerID);
			if (actor0 != nullptr && player != nullptr) {
				if (!bParented) {
					actor0->GetTransform().SetParent(player->GetTransform(), false);
					bParented = true;
				}
				else {
					actor0->GetTransform().UnsetParent(false);
					bParented = false;
				}
			}
		}

		//Actor::wPtr _actor2, _actor3;
		int _actor2ID = 0, _actor3ID = 0;
		void OnEnterPressed() {
			Actor* pActor2 = pLevel->FindActor(_actor2ID);
			Actor* pActor3 = pLevel->FindActor(_actor3ID);
			/*auto actor2 = _actor2.lock();
			auto actor3 = _actor3.lock();*/
			if (pActor2 == nullptr && pActor3 == nullptr) {
				pActor2 = pLevel->SpawnActor<Actor>("Yellow Circle", true, Vector2(-300, 300));
				if (pActor2) {
					_actor2ID = pActor2->GetActorID();
					auto rc0 = pActor2->AddComponent<RenderComponent>();
					rc0->SetCircleRenderable(ShapeData(Vector2(100, 0), Colour::Yellow));
					auto t0 = pActor2->AddCollider<CircleCollider>();
					t0->SetCircle(100);
				}
				
				pActor3 = pLevel->SpawnActor<Actor>("Blue Rectangle", true);
				if (pActor3) {
					_actor3ID = pActor3->GetActorID();
					auto rc1 = pActor3->AddComponent<RenderComponent>();
					rc1->SetRectangleRenderable(ShapeData(Vector2(600, 100), Colour::Blue));
					auto t1 = pActor3->AddCollider<AABBCollider>();
					t1->SetBounds(AABBData(300, 50));
				}
			}
			else {
				pActor2->Destruct();
				pActor3->Destruct();
			}
		}

		Actor* _pTextActor = nullptr;
		Actor* _pCloneTest = nullptr;
		ParticleSystem* _pParticlesTest = nullptr;
		bool bContinuous = true;
		void OnSelectPressed() {
			if (_pParticlesTest->IsPlaying()) _pParticlesTest->Stop();
			else {
				_pParticlesTest->ToggleActive(true);
				_pParticlesTest->SetNormalisedPosition(GameUtils::Vector2(Maths::Random::Range(-Fixed::One, Fixed::One), Maths::Random::Range(-Fixed::One, Fixed::One)));
				_pParticlesTest->Start();
			}
		}

		std::shared_ptr<UIElement> uUIEl0, uUIEl1;
		void UpdateTests(const Fixed& deltaTime) {
			if (!uUIEl0) {
				uUIEl0 = std::make_shared<UIElement>("UIEl0");
				uUIEl0->SetLevel(*pLevel);
				uUIEl0->m_transform.size = { 400, 300 };
				Vector2 pivot = -Vector2::One;
				//uUIEl0->m_transform.anchor = pivot;
				uUIEl0->m_transform.nPosition = pivot;
				uUIEl0->m_transform.pixelPad = { 200 * -pivot.x, 150 * -pivot.y };
				uUIEl0->SetPanel();
			}
			if (!uUIEl1) {
				uUIEl1 = std::make_shared<UIElement>("UIEl1");
				uUIEl1->SetLevel(*pLevel);
				uUIEl1->m_transform.nPosition = { 0, Fixed(0.5f) };
				uUIEl1->m_transform.size = { 200, 100 };
				uUIEl1->m_transform.SetParent(uUIEl0->m_transform);
				uUIEl1->SetPanel(Colour(100, 50, 255, 255));
				uUIEl1->SetText("Button");
			}
			if (uUIEl0) {
				uUIEl0->Tick(deltaTime);
			}
			if (uUIEl1) {
				uUIEl1->Tick(deltaTime);
			}
		}

		void RenderTests() {
			if (uUIEl0) {
				uUIEl0->Render();
			}
			if (uUIEl1) {
				uUIEl1->Render();
			}
		}

		void CleanupTests() {
			if (auto actor2 = pLevel->FindActor(_actor2ID)) {
				actor2->Destruct();
			}
			if (auto actor3 = pLevel->FindActor(_actor3ID)) {
				actor3->Destruct();
			}
			if (_pTextActor) _pTextActor = nullptr;
			if (_pCloneTest) _pCloneTest->Destruct();
			_pCloneTest = nullptr;
			if (_pParticlesTest) _pParticlesTest->Destruct();
			_pParticlesTest = nullptr;
			if (uUIEl0) uUIEl0 = nullptr;
			if (uUIEl1) uUIEl1 = nullptr;
		}
	}

	void TestLevel::LoadAssets() {
		Logger::Log(*this, "Loading Assets...", Logger::Severity::Debug);
		m_pEngine->GetAssetManager().LoadAll(AssetManifestData("AssetManifests/TestLevel.amf").GetManifest());
	}

	void TestLevel::Tick(Fixed deltaTime) {
		_TestLevel::UpdateTests(deltaTime);
		Level::Tick(deltaTime);
	}

	void TestLevel::OnActivated() {
		Logger::Log(*this, "Running Level", Logger::Severity::Debug);

		Actor* actor0 = SpawnActor<Actor>("Actor0-RectangleRenderer", true, Vector2(300, 200));
		if (actor0) {
			_TestLevel::actor0ID = actor0->GetActorID();
			auto rc0 = actor0->AddComponent<RenderComponent>();
			rc0->SetRectangleRenderable(ShapeData(Vector2(300, 100), Colour::Magenta));
			_TestLevel::_pTextActor = actor0;
		}
		
		if (auto actor1 = SpawnActor<Actor>("Actor1-TextRenderer", true)) {
			actor1->GetTransform().localPosition = Graphics::NToWorld({ 0, Fixed(0.9f) });
			auto rc = actor1->AddComponent<RenderComponent>();
			auto& tr = rc->SetTextRenderable("Hello World!");
			tr.m_layer = LayerID::UI;
			tr.SetColour(Colour(200, 150, 50)).SetSize(50);
		}

		Player* player = SpawnActor<Player>("Player", true);
		if (player) {
			TextureAsset* texture = GetAssetManager().Load<TextureAsset>("Ship.png");
			player->InitPlayer(*this, *texture, AABBData(40, 40));
			player->GetTransform().localPosition = Vector2(-200, -300);
		}
		_TestLevel::_pTextActor = player;

		RegisterScopedInput(GameInput::Return, std::bind(&TestLevel::OnQuitPressed, this), OnKey::Released);
		RegisterScopedInput(GameInput::X, &_TestLevel::OnXPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Y, &_TestLevel::OnYPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Enter, &_TestLevel::OnEnterPressed, OnKey::Released);
		RegisterScopedInput(GameInput::Select, &_TestLevel::OnSelectPressed, OnKey::Released);

		FileRW reader("Assets/VFX/Fire0/Fire0_noloop.psdata");
		GData psGData(reader.ReadAll(true));
		ParticleSystemData psData(GetAssetManager(), psGData);

		_TestLevel::_pParticlesTest = SpawnActor<ParticleSystem>("ExplodePS", false);
		_TestLevel::_pParticlesTest->InitParticleSystem(std::move(psData));

		// Tests
		_TestLevel::playerID = player->GetActorID();
		_TestLevel::pLevel = this;
		_TestLevel::bSoundPlayed = _TestLevel::bMusicPlayed = false;
		
		GetAudioManager().PlayMusic("TestMusic.ogg", Fixed::OneHalf);
		/*Spawner::Init(*this);
		Spawner::VFXExplode_Warm();*/
	}

	void TestLevel::OnClearing() {
		_TestLevel::CleanupTests();
	}

	void RenderTests(Level* level) {
		_TestLevel::RenderTests();
	}

	void TestLevel::PostRender() {
		if (m_clock.GetElapsedMilliSeconds() > 2000 && !_TestLevel::bSoundPlayed) {
			_TestLevel::bSoundPlayed = true;
			GetAudioManager().PlaySFX("TestSound.wav", Fixed(2, 10));
		}	
		/*if (clock.GetElapsedMilliSeconds() > 2100 && !_TestLevel::musicPlayed) {
			_TestLevel::musicPlayed = true;
			if (!GetAudioManager().PlaySFX("TestSound_b.wav", Fixed::Half)) {
				Logger::Log(*this, "Could not play SFX!", Logger::Severity::Error);
			}
			GetAudioManager().SetMusicVolume(Fixed::One);
			GetAudioManager().SwitchTrack("TestMusic_0.ogg", Fixed::Half);
		}
		if (clock.GetElapsedMilliSeconds() > 5000 && !_TestLevel::musicStopped) {
			_TestLevel::musicStopped = true;
			GetAudioManager().SwitchTrack("TestMusic.ogg", Fixed::Half, Fixed(3));
		}*/
		RenderTests(this);
	}

	void TestLevel::OnQuitPressed() {
		LoadLevel(m_pEngine->GetActiveLevelID() - 1);
	}
}

#include "stdafx.h"
#include "BootLevel.h"
#include "LittleEngine.h"

namespace LittleEngine {
	namespace {
		UIButtonDrawerData GetDrawerData() {
			UIButtonData* pCustomButton = nullptr;
			return UIButtonDrawerData::CreateDrawer(true, { 600, 500 }, Colour::Transparent, pCustomButton);
		}
	}

	BootLevel::BootLevel() : Level("BootLevel") {}

	void BootLevel::OnActivated() {
		Logger::Log(*this, "Running Level", Logger::Severity::Debug);
		m_pLogoDrawer = nullptr;
		m_pLogoHeader = nullptr;
		
		if ((m_pLogoDrawer = GetUIController().SpawnContext<UIButtonDrawer>())) {
			m_pLogoDrawer->InitButtonDrawer(GetDrawerData());
			m_pLogoHeader = m_pLogoDrawer->AddElement<UIElement>("Logo Header");
			m_pLogoHeader->SetText(UIText("Little Engine", 80, Colour::White));
			if (m_pLogoFont) m_pLogoHeader->GetTextData()->SetFont(*m_pLogoFont);
			m_pLogoHeader->m_transform.UnsetParent();
			m_pLogoHeader->m_transform.nPosition = { 0, Fixed(0.8f) };
			m_tokenHandler.AddToken(m_pLogoDrawer->AddButton("Start", std::bind(&BootLevel::OnLoadNextLevel, this)));
			m_tokenHandler.AddToken(m_pLogoDrawer->AddButton("Quit", std::bind(&Level::Quit, this)));
			m_pLogoDrawer->SetActive(true);
		}

		RegisterScopedInput(GameInput::Enter, std::bind(&BootLevel::OnLoadNextLevel, this), OnKey::Released);
		RegisterScopedInput(GameInput::Return, std::bind(&Level::Quit, this), OnKey::Released);

		//this->engine->GetAudioManager().PlayMusic("TestMusic.ogg");
	}

	void BootLevel::LoadAndSpawn() {
		m_pEngine->GetAssetManager().LoadAll(AssetManifestData("AssetManifests/BootLevel.amf").GetManifest());
		m_pLogoFont = GetAssetManager().Load<FontAsset>("Sunscreen.otf");
	}

	void BootLevel::Tick(Fixed deltaMS) {
		// Fade logo
		if (m_pLogoDrawer && m_pLogoHeader) {
			Fixed seconds = Fixed(static_cast<int>(LevelTimeMilliSeconds()), 1000);
			Fixed speed = 2;
			Fixed alpha = (seconds * speed).Sin().Abs() * 255;
			if (m_pLogoHeader) {
				TextData* const textData = m_pLogoHeader->GetTextData();
				Colour colour = textData->GetFillColour();
				colour.a = UByte(alpha.ToInt());
				textData->SetFillColour(colour);
			}
		}

		// Tick all actors etc
		Level::Tick(deltaMS);
	}

	void BootLevel::OnLoadNextLevel() {
		LoadLevel(GetActiveLevelID() + 1);
	}
}

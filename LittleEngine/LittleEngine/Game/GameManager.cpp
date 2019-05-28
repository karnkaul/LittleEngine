#include "stdafx.h"
#include "Core/Logger.h"
#include "Core/Utils.h"
#include "Component.h"
#include "Entity.h"
#include "GameManager.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/Engine/OS.h"
#include "LittleEngine/Game/Camera.h"
#include "LittleEngine/Physics/CollisionManager.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/UI/UIManager.h"

namespace LittleEngine
{
using Version = Core::Version;

namespace
{
Version gameVersion = "0.1";
}

const Version& GameManager::GetGameVersion()
{
	Version fileGameVersion = OS::Env()->GetFileGameVersion();
	if (fileGameVersion != Version())
	{
		gameVersion = fileGameVersion;
	}
	return gameVersion;
}

GameManager::GameManager() : m_logName("GameManager")
{
	m_uUIManager = MakeUnique<UIManager>();
	m_uCollisionManager = MakeUnique<CollisionManager>();
	m_uWorldCamera = MakeUnique<Camera>();
	Services::ProvideGameManager(*this);
}

GameManager::~GameManager()
{
	Services::UnprovideGameManager(*this);
	for (auto& componentVec : m_uComponents)
	{
		componentVec.clear();
	}
	m_uCollisionManager = nullptr;
	m_uEntities.clear();
	m_uUIManager = nullptr;
	m_uWorldCamera = nullptr;
}

UIManager* GameManager::UI() const
{
	return m_uUIManager.get();
}

EngineInput* GameManager::Input() const
{
	return Services::Engine()->Input();
}

EngineAudio* GameManager::Audio() const
{
	return Services::Engine()->Audio();
}

EngineRepository* GameManager::Repository() const
{
	return Services::Engine()->Repository();
}

WorldStateMachine* GameManager::Worlds() const
{
	return Services::Engine()->Worlds();
}

Camera* GameManager::WorldCamera() const
{
	return m_uWorldCamera.get();
}

CollisionManager* GameManager::Physics() const
{
	return m_uCollisionManager.get();
}

void GameManager::SetWorldCamera(UPtr<Camera> uCamera)
{
	m_uWorldCamera = std::move(uCamera);
}

const char* GameManager::LogNameStr() const
{
	return m_logName.c_str();
}

void GameManager::Tick(Time dt)
{
	m_uCollisionManager->Tick(dt);
	for (auto& componentVec : m_uComponents)
	{
		Core::RemoveIf<UPtr<AComponent>>(componentVec,
										   [](UPtr<AComponent>& uC) { return uC->m_bDestroyed; });
		for (auto& uComponent : componentVec)
		{
			uComponent->Tick(dt);
		}
	}
	Core::RemoveIf<UPtr<Entity>>(m_uEntities, [](UPtr<Entity>& uE) { return uE->m_bDestroyed; });
	for (auto& uEntity : m_uEntities)
	{
		uEntity->Tick(dt);
	}
	m_uUIManager->Tick(dt);
	m_uWorldCamera->Tick(dt);
}
} // namespace LittleEngine

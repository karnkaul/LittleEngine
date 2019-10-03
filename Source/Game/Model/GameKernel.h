#pragma once
#include "Core/CoreTypes.h"
#include "Engine/GFX.h"

namespace LittleEngine
{
class GameKernel final : private NoCopy
{
private:
	UPtr<class LEContext> m_uContext;
	UPtr<class GameManager> m_uGame;
	UPtr<class WorldStateMachine> m_uWSM;
	GFX m_gfx;

public:
	bool Boot(const class GameConfig& config);
	void Start(String coreManifestID = "", String gameStyleID = "", Task onManifestLoaded = nullptr);
	void Step(Time fdt);
	void Tick(Time dt);
	void Shutdown();
	
	LEContext* Context() const;

private:
	friend class World;
	friend class WorldStateMachine;
	friend class GameManager;
};
}

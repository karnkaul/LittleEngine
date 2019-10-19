#pragma once
#include "Core/CoreTypes.h"
#include "Engine/GFX.h"

namespace LittleEngine
{
class GameKernel final : private NoCopy
{
private:
	std::unique_ptr<class LEContext> m_uContext;
	std::unique_ptr<class GameManager> m_uGame;
	std::unique_ptr<class WorldStateMachine> m_uWSM;
	GFX m_gfx;

public:
	bool Boot(const class GameConfig& config);
	void Start(std::string coreManifestID = "", std::string gameStyleID = "", Task onManifestLoaded = nullptr);
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

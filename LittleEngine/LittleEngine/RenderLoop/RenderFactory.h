#pragma once
#include <mutex>
#include "SimpleTime.h"
#include "SFMLAPI/Rendering/SFPrimitive.h"
#include "SFMLAPI/Rendering/ISFRenderBuffer.h"
#include "LittleEngine/Services/IService.h"

namespace LittleEngine
{
class RenderFactory final : public IRenderBuffer, public IService
{
private:
	using PrimVec = Vec<UPtr<SFPrimitive>>;
	using PrimMat = Array<PrimVec, _LAYER_COUNT>;
	using Lock = std::lock_guard<std::mutex>;

private:
	PrimMat m_active;
	PrimVec m_standby;
	std::mutex m_mutex;
	Time m_lastSwapTime;
	static const u32 LAYER_RESERVE = 1024;

public:
	RenderFactory();
	~RenderFactory();
	SFPrimitive* New(LayerID layer);
	
	Time GetLastSwapTime() const override;

private:
	void Reconcile();
	void Lock_Swap() override;
	void Lock_Traverse(std::function<void(Vec<SFPrimitive*>)> procedure) override;
	void Lock_TraverseMatrix(std::function<void(PrimMat&)> procedure);

private:
	friend class AsyncRenderLoop;
	friend class EngineLoop;
	friend class WorldStateMachine;
};
}
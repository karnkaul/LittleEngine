#pragma once
#include "CoreTypes.h"
#include "LittleEngine/Services/IService.h"

namespace LittleEngine
{
class RenderHeap : public IService
{
private:
	using UPrimitive = UPtr<class SFPrimitive>;
	using UPrimitives = Vec<UPrimitive>;
	UPrimitives m_uPrimitives;
	class GFXBuffer* m_pBuffer;

public:
	RenderHeap(GFXBuffer& buffer);
	~RenderHeap();

public:
	SFPrimitive* New();
	SFPrimitive* New(const struct SFPrimitiveData& data);
	struct GFXDataFrame ConstructDataFrame();
	bool Destroy(SFPrimitive* pPrimitive);
	void Destroy(InitList<SFPrimitive*> pPrimitives);

private:
	void Reconcile();
	void DestroyAll();

	friend class EngineLoop;
	friend class AsyncRenderLoop;
	friend class WorldStateMachine;
};
} // namespace LittleEngine

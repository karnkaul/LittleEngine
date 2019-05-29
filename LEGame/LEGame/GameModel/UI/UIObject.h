#pragma once
#include "Core/CoreTypes.h"
#include "LEGame/GameModel/Inheritable.h"

namespace LittleEngine
{
class UIObject : public Inheritable
{
protected:
	class LEContext* m_pContext = nullptr;
	bool m_bDestroyed = false;

public:
	UIObject(bool bSilent = false);
	UIObject(String name, bool bSilent = false);
	~UIObject() override;

	virtual void Tick(Time dt) = 0;

	void SetContext(LEContext& context);
	Vector2 GetViewSize() const;

protected:
	class LERenderer* Renderer() const;
};
} // namespace LittleEngine

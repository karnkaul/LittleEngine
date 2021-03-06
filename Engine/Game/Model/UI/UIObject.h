#pragma once
#include "Core/CoreTypes.h"
#include "Game/Model/Inheritable.h"

namespace LittleEngine
{
class UIObject : public Inheritable, private NoCopy
{
protected:
	class LEContext* m_pContext = nullptr;
	bool m_bDestroyed = false;

public:
	UIObject(bool bSilent = false);
	UIObject(std::string name, bool bSilent = false);
	~UIObject() override;

	virtual void Tick(Time dt = Time::Zero) = 0;

	void SetContext(LEContext& context);

protected:
	class LERenderer* Renderer() const;
};
} // namespace LittleEngine

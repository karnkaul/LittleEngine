#pragma once
#include "SFMLAPI/Rendering/Colour.h"
#include "Game/Model/UI/UIContext.h"
#if ENABLED(CONSOLE)
#include "Game/Framework/Utility/Debug/Console/DebugCommands.h"
#endif

namespace Core
{
class GData;
}

namespace LittleEngine
{
class UIContainer : public UIContext
{
private:
	struct Obj
	{
		std::string id;
		UIObject* pObj;
	};

private:
	std::vector<Obj> m_objs;

public:
	UIContainer();
	~UIContainer() override;

	void Deserialise(std::string serialised);

	UIObject* GetObj(const std::string& id);
	template <typename T>
	T* Find(const std::string& id);

private:
	void SetupChildren(class UIElement* pParent, std::vector<Core::GData> uiObjects);
};

template <typename T>
T* UIContainer::Find(const std::string& id)
{
	static_assert(IsDerived<UIObject, T>(), "T must derive from UIObject!");
	return dynamic_cast<T*>(GetObj(id));
}

#if ENABLED(CONSOLE)
namespace Debug::Commands
{
class CMD_UIContainer : public Command
{
private:
	UIContainer* m_pContainer = nullptr;

public:
	CMD_UIContainer();

	void FillExecuteResult(std::string params) override;
};
} // namespace Debug::Commands
#endif
} // namespace LittleEngine

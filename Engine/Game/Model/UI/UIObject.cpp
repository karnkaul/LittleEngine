#include "UIObject.h"
#include "SFMLAPI/System/Assets.h"
#include "Engine/Context/LEContext.h"
#include "Engine/Repository/LERepository.h"

namespace LittleEngine
{
UIObject::UIObject(bool bSilent) : Inheritable(bSilent) {}

UIObject::UIObject(std::string name, bool bSilent) : Inheritable(std::move(name), "UIObject", bSilent) {}

UIObject::~UIObject() = default;

void UIObject::SetContext(LEContext& context)
{
	m_pContext = &context;
}

LERenderer* UIObject::Renderer() const
{
	Assert(m_pContext, "Context is null!");
	return m_pContext->Renderer();
}
} // namespace LittleEngine

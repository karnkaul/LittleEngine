#include "UIObject.h"
#include "SFMLAPI/System/Assets.h"
#include "Engine/Context/LEContext.h"
#include "Engine/Repository/LERepository.h"

namespace LittleEngine
{
UIObject::UIObject(bool bSilent) : Inheritable(bSilent) {}

UIObject::UIObject(String name, bool bSilent) : Inheritable(std::move(name), "UIObject", bSilent) {}

UIObject::~UIObject() = default;

void UIObject::SetContext(LEContext& context)
{
	m_pContext = &context;
}

Vector2 UIObject::ViewSize() const
{
	return m_pContext ? m_pContext->ViewSize() : Vector2::Zero;
}

LERenderer* UIObject::Renderer() const
{
	Assert(m_pContext, "Context is null!");
	return m_pContext->Renderer();
}
} // namespace LittleEngine

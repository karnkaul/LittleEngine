#include "stdafx.h"
#include "UIObject.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Repository/EngineRepository.h"

namespace LittleEngine
{
UIObject::UIObject(const String& name) : m_name(name)
{
}
UIObject::~UIObject() = default;

String UIObject::LogName() const
{
	return "[" + m_name + " (" + m_baseClass + ")]";
}

EngineRepository* UIObject::Repository() const
{
	return Services::Engine()->Repository();
}
} // namespace LittleEngine

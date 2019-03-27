#include "stdafx.h"
#include "UIObject.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Repository/EngineRepository.h"

namespace LittleEngine
{
UIObject::UIObject(const String& name) : Inheritable(name, "UIObject")
{
}
UIObject::~UIObject() = default;

EngineRepository* UIObject::Repository() const
{
	return Services::Engine()->Repository();
}
} // namespace LittleEngine

#include "stdafx.h"
#include "UIObject.h"
#include "SFMLAPI/System/SFAssets.h"
#include "LittleEngine/Engine/EngineService.h"
#include "LittleEngine/Services/Services.h"
#include "LittleEngine/Repository/EngineRepository.h"

namespace LittleEngine
{
UIObject::UIObject(String name, bool bSilent)
	: Inheritable(std::move(name), "UIObject", bSilent)
{
}

UIObject::~UIObject() = default;

EngineRepository* UIObject::Repository() const
{
	return Services::Engine()->Repository();
}
} // namespace LittleEngine

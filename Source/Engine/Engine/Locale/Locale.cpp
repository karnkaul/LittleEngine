#include "Core/GData.h"
#include "Locale.h"
#include "SFMLAPI/System/Assets.h"
#include "Engine/Repository/LERepository.h"

namespace LittleEngine
{
namespace
{
UMap<String, String> locMap;
}

void Locale::Init(const String& locFileID, const String& enLocFileID)
{
	Assert(g_pRepository->IsPresent(enLocFileID), "EN Loc file missing!");
	auto pText = g_pRepository->Preload<TextAsset>(locFileID);
	if (!pText && locFileID != enLocFileID)
	{
		pText = g_pRepository->Preload<TextAsset>(enLocFileID);
	}
	Assert(pText && !pText->Text().empty(), "Loc text is null!");
	if (pText)
	{
		Core::GData locMapData(pText->Text());
		locMap = locMapData.AllFields();
	}
}

bool Locale::Switch(const String& newLocFileID)
{
	if (g_pRepository->IsPresent(newLocFileID))
	{
		auto pText = g_pRepository->Load<TextAsset>(newLocFileID);
		if (pText)
		{
			Core::GData locMapData(pText->Text());
			const auto& fields = locMapData.AllFields();
			if (!locMapData.AllFields().empty())
			{
				locMap = fields;
				return true;
			}
		}
	}
	LOG_E("[Locale] Error loading %s!", newLocFileID.c_str());
	return false;
}

const String& Locale::Localise(const String& id)
{
	auto iter = locMap.find(id);
	if (iter != locMap.end())
	{
		return iter->second;
	}
	return id;
}
} // namespace LittleEngine

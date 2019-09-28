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

void Locale::Init(String locFileID, String enLocFileID)
{
	Assert(g_pRepository->IsPresent(enLocFileID), "EN Loc file missing!");
	bool bIsEn = locFileID == enLocFileID;
	auto pText = g_pRepository->Preload<TextAsset>(std::move(locFileID));
	if (!pText && !bIsEn)
	{
		pText = g_pRepository->Preload<TextAsset>(std::move(enLocFileID));
	}
	Assert(pText && !pText->Text().empty(), "Loc text is null!");
	if (pText)
	{
		Core::GData locMapData(pText->Text());
		locMap = locMapData.AllFields();
	}
}

bool Locale::Switch(VString newLocFileID)
{
	if (g_pRepository->IsPresent(newLocFileID))
	{
		auto pText = g_pRepository->Load<TextAsset>(String(newLocFileID));
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
	LOG_E("[Locale] Error loading %s!", newLocFileID.data());
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

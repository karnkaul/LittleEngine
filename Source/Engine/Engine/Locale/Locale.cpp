#include "Core/Game/GData.h"
#include "Locale.h"
#include "SFMLAPI/System/Assets.h"
#include "Engine/Repository/LERepository.h"

namespace LittleEngine
{
namespace
{
std::unordered_map<std::string, std::string> locMap;
}

void Locale::Init(std::string locFileID, std::string enLocFileID)
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

bool Locale::Switch(const std::string& newLocFileID)
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
	LOG_E("[Locale] Error loading %s!", newLocFileID.data());
	return false;
}

const std::string& Locale::Localise(const std::string& id)
{
	auto iter = locMap.find(id);
	if (iter != locMap.end())
	{
		return iter->second;
	}
	return id;
}
} // namespace LittleEngine

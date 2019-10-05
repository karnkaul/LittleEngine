#pragma once
#include "Core/CoreTypes.h"
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/Shader.h"
#include "Engine/Repository/LERepository.h"

namespace LittleEngine
{
struct ShadersData
{
	std::string assetIDPrefix = "Texts/Shaders/";
	std::string vertExt = ".vsh";
	std::string fragExt = ".fsh";
};

namespace LEShaders
{
extern ShadersData s_data;
extern UMap<std::string, UPtr<Shader>> s_shaderMap;

void Init(ShadersData data = {});
void Clear();

template <typename T>
T* GetShader(const std::string& id);
template <typename T>
T* CreateShader(const std::string& id, const std::string& vertCode, const std::string& fragCode);

template <typename T>
static T* LoadShader(const std::string& id, Shader::Type asType);
}; // namespace LEShaders

template <typename T>
T* LEShaders::GetShader(const std::string& id)
{
	static_assert(IsDerived<Shader, T>(), "T must derive from SFShader");
	auto search = s_shaderMap.find(id);
	if (search != s_shaderMap.end())
	{
		return dynamic_cast<T*>(search->second.get());
	}
	return nullptr;
}

template <typename T>
T* LEShaders::CreateShader(const std::string& id, const std::string& vertCode, const std::string& fragCode)
{
	if (s_shaderMap.find(id) != s_shaderMap.end())
	{
		LOG_E("[ShaderFactory] [%s] shader already loaded!");
		return nullptr;
	}

	UPtr<T> uT = MakeUnique<T>(id);
	uT->Compile(vertCode, fragCode);
	if (uT->GetType().any())
	{
		T* pT = uT.get();
		s_shaderMap.emplace(id, std::move(uT));
		return pT;
	}
	else
	{
		LOG_E("Error compiling shader [%s]!", id.c_str());
	}
	return nullptr;
}

template <typename T>
T* LEShaders::LoadShader(const std::string& id, Shader::Type asType)
{
#if ENABLED(ASSERTS)
	std::string err = id + " Shader already loaded!";
	AssertVar(s_shaderMap.find(id) == s_shaderMap.end(), err.c_str());
	Assert(g_pRepository, "Repository is null!");
	Assert(asType.any(), "Invalid Shader type");
#endif
	std::string assetID = s_data.assetIDPrefix + id;
	std::string vertCode;
	std::string fragCode;
	if (asType[ToIdx(Shader::Flag::Vertex)])
	{
		std::string vsAssetID = assetID + s_data.vertExt;
		auto pText = g_pRepository->Load<TextAsset>(vsAssetID);
		if (pText)
		{
			vertCode = pText->Text();
		}
	}
	if (asType[ToIdx(Shader::Flag::Fragment)])
	{
		std::string fsAssetID = assetID + s_data.fragExt;
		auto pText = g_pRepository->Load<TextAsset>(fsAssetID);
		if (pText)
		{
			fragCode = pText->Text();
		}
	}
	UPtr<T> uT = MakeUnique<T>(id);
	uT->Compile(std::move(vertCode), std::move(fragCode));
	if (uT->GetType().any())
	{
		LOG_I("== [%s] %s Shader created", uT->ID().c_str(), g_szShaderTypes[uT->GetType()].data());
		T* pT = uT.get();
		s_shaderMap.emplace(id, std::move(uT));
		return pT;
	}
	else
	{
		LOG_E("Error loading/compiling shader code [%s] from [%s]!", id.c_str(), assetID.c_str());
	}
	return nullptr;
}
} // namespace LittleEngine

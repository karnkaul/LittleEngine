#pragma once
#include "Core/CoreTypes.h"
#include "Core/Logger.h"
#include "SFMLAPI/Rendering/Shader.h"
#include "LittleEngine/Repository/LERepository.h"

namespace LittleEngine
{
struct ShadersData
{
	String codePrefix = "#version 120\n";
	String assetIDPrefix = "Texts/Shaders/";
	String vertExt = ".vsh";
	String fragExt = ".fsh";
};

extern class LEShaders* g_pShaders;

class LEShaders final
{
private:
	static ShadersData s_data;
	static UMap<String, UPtr<Shader>> s_shaderMap;

public:
	LEShaders(ShadersData data = {});
	~LEShaders();

	template <typename T>
	T* GetShader(const String& id);
	template <typename T>
	T* CreateShader(const String& id, const String& vertCode, const String& fragCode);

	template <typename T>
	static T* LoadShader(const String& id, ShaderType asType);
};

template <typename T>
T* LEShaders::GetShader(const String& id)
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
T* LEShaders::CreateShader(const String& id, const String& vertCode, const String& fragCode)
{
	if (s_shaderMap.find(id) != s_shaderMap.end())
	{
		LOG_E("[ShaderFactory] [%s] shader already loaded!");
		return nullptr;
	}

	UPtr<T> uT = MakeUnique<T>(id);
	uT->Compile(vertCode, fragCode);
	if (uT->GetType() != ShaderType::Invalid)
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
T* LEShaders::LoadShader(const String& id, ShaderType asType)
{
	Assert(s_shaderMap.find(id) == s_shaderMap.end(), "Shader ID already loaded!");
	Assert(g_pRepository, "Repository is null!");
	Assert(asType != ShaderType::Invalid, "Invalid Shader type");
	String assetID = s_data.assetIDPrefix + id;
	String vertCode;
	String fragCode;

	if (asType == ShaderType::Vertex || asType == ShaderType::VertFrag)
	{
		String vsAssetID = assetID + s_data.vertExt;
		auto pText = g_pRepository->Load<TextAsset>(vsAssetID);
		if (pText)
		{
			vertCode = s_data.codePrefix + pText->GetText();
		}
	}

	if (asType == ShaderType::Fragment || asType == ShaderType::VertFrag)
	{
		String fsAssetID = assetID + s_data.fragExt;
		auto pText = g_pRepository->Load<TextAsset>(fsAssetID);
		if (pText)
		{
			fragCode = s_data.codePrefix + pText->GetText();
		}
	}

	UPtr<T> uT = MakeUnique<T>(id);
	switch (asType)
	{
	default:
		break;
	case ShaderType::Vertex:
	{
		uT->Compile(std::move(vertCode), ShaderType::Vertex);
	}
	break;
	case ShaderType::Fragment:
	{
		uT->Compile(std::move(fragCode), ShaderType::Fragment);
	}
	break;
	case ShaderType::VertFrag:
	{
		uT->Compile(std::move(vertCode), std::move(fragCode));
	}
	break;
	}

	if (uT->GetType() != ShaderType::Invalid)
	{
		LOG_I("== [%s] %s Shader created", uT->GetID().c_str(),
			  g_szShaderTypes[ToIdx(uT->GetType())]);
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

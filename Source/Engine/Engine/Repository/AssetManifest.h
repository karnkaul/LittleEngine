#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/System/AssetType.h"
#include "SFMLAPI/Rendering/Shader.h"

namespace LittleEngine
{
// \brief Auto-constructs a set of generateable asset paths based on input parameters
struct AssetIDContainer
{
	Vec<String> assetIDs;

	AssetIDContainer(AssetIDContainer&&) = default;
	AssetIDContainer& operator=(AssetIDContainer&&) = default;
	// Single asset's path
	AssetIDContainer(String assetPath);
	// Multiple assets' paths
	AssetIDContainer(InitList<String> assetPaths);
	// Multiple assets' paths with a prefix for each
	AssetIDContainer(const String& pathPrefix, InitList<String> assetPaths);
	// Multiple assets's paths with a prefix for each path, prefix and suffix for each asset
	AssetIDContainer(const String& pathPrefix, u32 count, const String& assetPrefix, const String& assetSuffix);

	// Get a random path from vector of paths
	String Random() const;
};

// \brief Complete data specifying particular set of identical-type Assets
struct AssetDefinition
{
	AssetIDContainer assetIDs;
	AssetType type;

	AssetDefinition(AssetDefinition&&) = default;
	AssetDefinition(const AssetDefinition&) = delete;
	// Create an AssetDefinition given a resource type and its AssetPaths
	AssetDefinition(AssetType type, AssetIDContainer assetIDs);
};

// \brief Collection of AssetDefinitions
struct AssetManifest
{
public:
	Vec<AssetDefinition> assetDefs;

public:
	AssetManifest() = default;
	AssetManifest(Vec<AssetDefinition> assetDefinitions);

	// Add definition to the manifest
	void AddDefinition(AssetDefinition definition);
	// Create and add an AssetDefinition to the manifest
	void AddDefinition(AssetType type, AssetIDContainer resourcePaths);
	void Clear();
	// Convenience callback that iterates through each AssetDefinition
	void ForEach(std::function<void(const AssetDefinition& definition)> callback) const;
};

// \brief Generates AssetManifest from serialised data loaded from a file, given its path
struct AssetManifestData
{
private:
	AssetManifest manifest;

public:
	// \brief Note: this is not a const function! Manifests are designed to move their data
	AssetManifest& Manifest();
	// \brief Pass the path to the asset manifest file (.amf)
	void Load(String amfPath);
	void Deserialise(String serialised);
};
} // namespace LittleEngine

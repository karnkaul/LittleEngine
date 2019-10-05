#pragma once
#include "Core/CoreTypes.h"
#include "SFMLAPI/System/AssetType.h"
#include "SFMLAPI/Rendering/Shader.h"

namespace LittleEngine
{
// \brief Auto-constructs a set of generateable asset paths based on input parameters
struct AssetIDContainer
{
	std::vector<std::string> assetIDs;

	AssetIDContainer(AssetIDContainer&&) = default;
	AssetIDContainer& operator=(AssetIDContainer&&) = default;
	// Single asset's path
	AssetIDContainer(std::string assetPath);
	// Multiple assets' paths
	AssetIDContainer(std::initializer_list<std::string> assetPaths);
	// Multiple assets' paths with a prefix for each
	AssetIDContainer(const std::string& pathPrefix, std::initializer_list<std::string> assetPaths);
	// Multiple assets's paths with a prefix for each path, prefix and suffix for each asset
	AssetIDContainer(const std::string& pathPrefix, u32 count, const std::string& assetPrefix, const std::string& assetSuffix);

	// Get a random path from vector of paths
	std::string Random() const;
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
	std::vector<AssetDefinition> assetDefs;

public:
	AssetManifest() = default;
	AssetManifest(std::vector<AssetDefinition> assetDefinitions);

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
	void Load(std::string amfPath);
	void Deserialise(std::string serialised);
};
} // namespace LittleEngine

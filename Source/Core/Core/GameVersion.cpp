#include "BuildVersion.h"
#include "GameVersion.h"

const Core::Version BUILD_VERSION = Core::Version(BUILD_VERSION_MAJOR, BUILD_VERSION_MINOR, BUILD_VERSION_PATCH, BUILD_VERSION_PRE);
const std::string_view BUILD_VERSION_WITH_COMMIT = BUILD_VERSION_FULL;

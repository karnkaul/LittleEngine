#include "Core/CoreTypes.h"

#define LOC(x) LittleEngine::Locale::Localise(x)

namespace LittleEngine
{
namespace Locale
{
void Init(std::string locFileID, std::string enLocFileID);
bool Switch(const std::string& newLocFileID);
const std::string& Localise(const std::string& id);
} // namespace Locale
} // namespace LittleEngine

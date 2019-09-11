#include "Core/CoreTypes.h"

#define LOC(x) LittleEngine::Locale::Localise(x)

namespace LittleEngine
{
namespace Locale
{
void Init(const String& locFileID, const String& enLocFileID);
bool Switch(const String& newLocFileID);
const String& Localise(const String& id);
} // namespace Locale
} // namespace LittleEngine

#pragma once

namespace Core
{
class NoCopy
{
protected:
	NoCopy() = default;

private:
	NoCopy(const NoCopy&) = delete;
	NoCopy& operator=(const NoCopy&) = delete;
};
}

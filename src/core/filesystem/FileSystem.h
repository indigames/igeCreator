#pragma once

#define NOMINMAX

#include <istream>
#include <vector>
#include "utils/filesystem.h"

namespace fs
{
	using namespace ghc::filesystem;
	std::vector<path> split_until(const path& _path, const path& _predicate);
}

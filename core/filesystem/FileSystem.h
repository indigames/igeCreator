#pragma once

#define NOMINMAX

#include <istream>
#include <vector>
#include <filesystem>

namespace fs {
	using namespace std::filesystem;
}

namespace fs
{
std::vector<path> split_until(const path& _path, const path& _predicate);
}

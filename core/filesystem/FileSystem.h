#pragma once

#define NOMINMAX

#include <istream>
#include <vector>
#include <filesystem>

namespace fs
{
	using namespace std::filesystem;
	std::vector<path> split_until(const path& _path, const path& _predicate);
	std::string createScript(const std::string& name);
}

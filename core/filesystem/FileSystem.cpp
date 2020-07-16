#include "filesystem.h"
#include <algorithm>
#include <limits>

namespace fs
{
std::vector<path> split_until(const path& _path, const path& _predicate)
{
	std::vector<path> result;

	auto f = _path;

	while(f.has_parent_path() && f.has_filename() && f != _predicate)
	{
		result.push_back(f);
		f = f.parent_path();
	}

	result.push_back(_predicate);
	std::reverse(std::begin(result), std::end(result));

	return result;
}
}

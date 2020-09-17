#include "core/filesystem/FileSystem.h"

#include <algorithm>
#include <limits>
#include <fstream>
#include <cctype>
#include <string>

namespace fs
{
	const auto script_template =
"from igeScene import Script\n\
\n\
class %s(Script):\n\
    def __init__(self, owner):\n\
        super().__init__(owner)\n\
        print(f'Hello {self.owner.name}')\n\
    \n\
    def onAwake(self):\n\
        print('onAwake() called!')\n\
    \n\
    def onStart(self):\n\
        print('onStart() called!')\n\
    \n\
    def onUpdate(self, dt):\n\
        pass\n\
    \n\
    def onClick(self):\n\
        print(f'onClick(): {self.owner.name}!')\n\
    \n\
    def onDestroy(self):\n\
        print('onDestroy called!')\n\
    \n";

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

	std::string createScript(const std::string& name)
	{
		char script[512] = { 0 };
		sprintf(script, script_template, name.c_str());

		auto fileName = name;
		std::transform(fileName.begin(), fileName.end(), fileName.begin(),
			[](unsigned char c) { return std::tolower(c); });
		std::filesystem::path path{ "scripts/" + fileName + ".py" };
		std::ofstream ofs(path);
		ofs << script;
		ofs.close();
		return fileName;
	}
}

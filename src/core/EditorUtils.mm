#ifdef __APPLE__

#include "core/EditorUtils.h"

#include <utils/filesystem.h>
namespace fs = ghc::filesystem;

NS_IGE_BEGIN

void EditorUtils::openExplorer(const std::string &path) {
    auto fsPath = fs::path(path);
    NSString* folder = [NSString stringWithUTF8String:(fs::is_directory(fsPath) ? path.c_str() : fsPath.parent_path().string().c_str())];
    [[NSWorkspace sharedWorkspace]openFile:folder withApplication:@"Finder"];
}

void EditorUtils::openFile(const std::string& path) {
    if(fs::is_directory(fs::path(path))) {
        return openExplorer(path);
    }
    NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];
    [[NSWorkspace sharedWorkspace]openFile:nsPath];
}

NS_IGE_END
#endif // __APPLE__

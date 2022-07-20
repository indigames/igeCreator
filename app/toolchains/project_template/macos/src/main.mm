//
//  main.mm
//  igeLauncher
//
//  Created by Indi Games on 11/20/19.
//  Copyright © 2019 Indi Games. All rights reserved.
//

#include "main.h"
#ifdef USE_IGESCENE
    #include <utils/PyxieHeaders.h>
    #include "Game.h"
    #include <utils/filesystem.h>
    namespace fs = ghc::filesystem;
#else
    #include "pythonStarter.h"
#endif

#include <pyxieApplication.h>
#include <pyxieFios.h>
#include <pyxieResourceManager.h>
#include <pyxieSystemInfo.h>
using namespace pyxie;

#import <Foundation/NSPathUtilities.h>
#import <Foundation/NSBundle.h>
#import <Foundation/NSFileManager.h>

#import <Cocoa/Cocoa.h>
#include <string>
#include <fstream>
#include <ftw.h>
#include <sys/stat.h>

std::string getDocumentPath() {
    @autoreleasepool {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDir = [paths objectAtIndex:0];
        return std::string([documentsDir UTF8String]);
    }
}

std::string getBundlePath() {
    @autoreleasepool {
        NSString *bundleDir = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@".bundle"];
        return std::string([bundleDir UTF8String]);
    }
}

#ifdef __cplusplus
extern "C" {
#endif

std::string src_root;
std::string dst_root;

constexpr int ftw_max_fd = 20;

int getRelative(const char* path, const char* root){
    int i=0;
    while(root[i]){
        if(path[i]!=root[i]) break;
        i++;
    }
    return i;
}
    
int copy_file(const char* src_path, const struct stat* sb, int typeflag) {

    int i = getRelative(src_path, src_root.c_str());
    const char* src_relative = &(src_path[i]);
    if(strlen(src_relative)==0) return 0;
    std::string dst_path = dst_root + src_relative;
    switch(typeflag) {
        case FTW_D:
            mkdir(dst_path.c_str(), sb->st_mode);
            break;
        case FTW_F:
            // Delete old item if it exists
            NSError *error;
            NSString *path = @(dst_path.c_str());
            if ([[NSFileManager defaultManager] isDeletableFileAtPath:path]) {
                BOOL success = [[NSFileManager defaultManager] removeItemAtPath:path error:&error];
                if (!success) {
                    NSLog(@"Error removing file at path: %@", error.localizedDescription);
                }
            }
            // Copy new item
            std::ifstream  src(src_path, std::ios::binary);
            std::ofstream  dst(dst_path, std::ios::binary);
            dst << src.rdbuf();
    }
    return 0;
}

#define SCREEN_WIDTH 540
#define SCREEN_HEIGHT 960

extern std::shared_ptr<pyxieApplication> gApp;

int main(int argc, const char *argv[])
{
    @autoreleasepool {
        src_root = getBundlePath();
        dst_root = getDocumentPath();
        ftw(src_root.c_str(), copy_file, ftw_max_fd);
        pyxieFios::Instance().SetRoot(getDocumentPath().c_str());

    #ifdef USE_IGESCENE
        fs::current_path(fs::path(dst_root));

        // Create the game instance
        gApp = std::make_shared<Game>();
        gApp->createAppWindow();
        
        // Initialize
        if(gApp->isInitialized())
        {
            // Set game size width based
            pyxieSystemInfo::Instance().SetGemeScreenSize(SCREEN_WIDTH);

            // Show window
            gApp->showAppWindow(true, SCREEN_WIDTH, SCREEN_HEIGHT);

            // Main loop
            while (gApp->isRunning())
            {
                // Update
                gApp->update();

                // Swap buffers
                gApp->swap();
            }
        }

        // Destroy
        gApp->destroy();
        gApp = nullptr;
    #else
        pyxieFios::Instance().SetRoot(".");
        pyxieResourceManager::New();

        // Run Python script
        auto pythonStarter = std::make_shared<PythonStarter>();
        pythonStarter->Run();

        // If script exit but app still run, it's a crash
        if (gApp != nullptr && gApp->isRunning()) {
            pythonStarter->ShowError();

            // Destroy
            gApp->destroy();
            gApp = nullptr;
        }
    #endif
    }
    return NSApplicationMain(argc, argv);
}
#ifdef __cplusplus
}
#endif

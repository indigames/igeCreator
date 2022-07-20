#include <jni.h>
#include <android/log.h>
#include <android/api-level.h>

#ifdef USE_IGESCENE
    #include <utils/PyxieHeaders.h>
    #include "Game.h"
#else
    #include "pythonStarter.h"
#endif

#ifdef USE_IGESDK
    #include <utils/JniHelper.h>
#endif

#include <pyxieApplication.h>
#include <pyxieFios.h>
#include <pyxieResourceManager.h>
#include <pyxieSystemInfo.h>
using namespace pyxie;

#if __ANDROID_API__ < __ANDROID_API_M__
/* Before M the actual symbols for stdin and friends had different names. */
extern FILE __sF[] __REMOVED_IN(23);

#undef stdin
#undef stdout
#undef stderr

FILE *stdin = &__sF[0];
FILE *stdout = &__sF[1];
FILE *stderr = &__sF[2];
#endif


JavaVM* g_JavaVM = nullptr;

#ifdef USE_OPENCV
    extern bool AndroidCameraCapture_setJavaVM(JavaVM* vm);
#endif

#ifdef USE_IGEWEBVIEW
    extern void Webview_NativeInit(JavaVM* vm);
#endif

#ifdef USE_IGESOCIAL
    extern void Social_NativeInit(JavaVM* vm);
#endif

#ifdef USE_IGEGAMESSERVICES
    extern void GamesServices_NativeInit(JavaVM* vm);
#endif

#ifdef USE_IGECAMERA
    extern void Camera_NativeInit(JavaVM* vm);
#endif

#ifdef USE_IGEPAL
    extern void PAL_NativeInit(JavaVM* vm);
#endif

extern void Core_NativeInit(JavaVM* vm);
extern "C" void SDL2_setJavaVM(JavaVM* vm);

jint JNI_OnLoad(JavaVM* vm, void* )
{
    g_JavaVM = vm;
    JNIEnv* env;
    if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK)
        return -1;

    // Init sdl2's vm
    SDL2_setJavaVM(vm);

    // Library init
    Core_NativeInit(vm);

#ifdef USE_IGESDK
    JniHelper::setJavaVM(vm);
#endif

#ifdef USE_OPENCV
    // Init camera capture's vm
    AndroidCameraCapture_setJavaVM(vm);
#endif

#ifdef USE_IGEWEBVIEW
    Webview_NativeInit(vm);
#endif

#ifdef USE_IGESOCIAL
    Social_NativeInit(vm);
#endif

#ifdef USE_IGEGAMESSERVICES
    GamesServices_NativeInit(vm);
#endif

#ifdef USE_IGECAMERA
    Camera_NativeInit(vm);
#endif

#ifdef USE_IGEPAL
    PAL_NativeInit(vm);
#endif

    return JNI_VERSION_1_6;
}

static const char* getDataRoot()
{
    JNIEnv* env;
    bool envAttached = false;
    int getEnvStat = g_JavaVM->GetEnv((void **)&env, JNI_VERSION_1_6);
    if (getEnvStat == JNI_EDETACHED)
    {
        g_JavaVM->AttachCurrentThread(&env, NULL);
        envAttached = true;
    }

    const char* str = NULL;
    jclass storageManagerClass = env->FindClass("net/indigames/lib/StorageManager");
    jfieldID sInstanceId = env->GetStaticFieldID(storageManagerClass, "sInstance", "Lnet/indigames/lib/StorageManager;");
    jmethodID getDataPathId = env->GetMethodID(storageManagerClass, "getDataPath", "()Ljava/lang/String;");
    jobject sInstance = env->GetStaticObjectField(storageManagerClass, sInstanceId);
    auto objDataPath = (jstring)env->CallObjectMethod(sInstance, getDataPathId);
    str = env->GetStringUTFChars(objDataPath, NULL);
    env->DeleteLocalRef(objDataPath);

#ifdef USE_IGESDK
    JniHelper::init();
#endif

    if(envAttached)
        g_JavaVM->DetachCurrentThread();

    return str;
}

extern std::shared_ptr<pyxieApplication> gApp;

extern "C" __attribute__((visibility("default"))) int SDL_main(int argc, char** argv);
int SDL_main(int argc, char** argv)
{
    pyxieFios::Instance().SetRoot(getDataRoot());

#ifdef USE_IGESCENE
    // Create the game instance
    gApp = std::make_shared<Game>();
    gApp->createAppWindow();

    // Initialize
    if (gApp->isInitialized())
    {
        // Show window
        gApp->showAppWindow(true);

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
    return 0;
}
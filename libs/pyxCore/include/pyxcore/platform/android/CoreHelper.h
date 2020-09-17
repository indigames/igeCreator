#pragma once

#include <jni.h>
#include <android/log.h>
#include "pyxieHelper.h"

static const char* coreClassName = "net/indigames/lib/CoreHelper";
static const char* PALClassName = "net/indigames/lib/PALHelper";

extern JavaVM* core_VM;
extern jclass mCoreClass;
extern jclass mPALClass;

/* method signatures */
extern jmethodID midAddImageToGallery;

// utility
extern JNIEnv* CoreGetJniEnv(bool &envAttached);
extern void CoreDetachVM(bool envAttached);


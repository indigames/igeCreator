#ifndef __OBJECTIVE_C_INTERFACE_H__
#define __OBJECTIVE_C_INTERFACE_H__

#include "pyxieHelper.h"

// This is the C "trampoline" function that will be used
// to invoke a specific Objective-C method FROM C++
int MyObjectDoSomethingWith (void *myObjectInstance, void *parameter);
void PrintNSLog(const char* msg);
#endif

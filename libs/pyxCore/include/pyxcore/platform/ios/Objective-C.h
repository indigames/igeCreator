#import "CoreHelper.h"

#if defined __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE            //iOS
#import <Foundation/Foundation.h>
#else                           //OSX
#import <Cocoa/Cocoa.h>
#endif
#endif


// An Objective-C class that needs to be accessed from C++
@interface ObjectiveCObject : NSObject
{
    int someVar;
}
@end

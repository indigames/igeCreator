#ifndef __MACROS_H__
#define __MACROS_H__

#define NS_IGE_BEGIN					namespace ige::creator {
#define NS_IGE_END                      }
#define USING_NS_IGE_CREATOR            using namespace ige::creator;
#define USING_NS_IGE_SCENE				using namespace ige::scene;
#define NS_IGE_SCENE					ige::scene
#define USING_NS_PYXIE					using namespace pyxie;	


#define SYSTEM_KEYPRESSED(X)            (1 << ((X)-1))
#define SYSTEM_KEYCODE_ALT_VALUE        1
#define SYSTEM_KEYCODE_CTRL_VALUE       2
#define SYSTEM_KEYCODE_SHIFT_VALUE      3
#define SYSTEM_KEYCODE_ALT_MASK         SYSTEM_KEYPRESSED(SYSTEM_KEYCODE_ALT_VALUE)
#define SYSTEM_KEYCODE_CTRL_MASK        SYSTEM_KEYPRESSED(SYSTEM_KEYCODE_CTRL_VALUE)
#define SYSTEM_KEYCODE_SHIFT_MASK       SYSTEM_KEYPRESSED(SYSTEM_KEYCODE_SHIFT_VALUE)

#define INSPECTOR_EDITOR_DELTA_LIMIT	0.02f

#define RAD_TO_DEGREE                   57.2957795f
#define DEGREE_TO_RAD                   0.01745329f
#define DEG360_TO_RAD                   6.2831844f

#define CALLBACK_0(__selector__,__target__, ...) std::bind(&__selector__,__target__, ##__VA_ARGS__)
#define CALLBACK_1(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, ##__VA_ARGS__)
#define CALLBACK_2(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define CALLBACK_3(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, ##__VA_ARGS__)

#endif
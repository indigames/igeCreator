#include "pythonExtension.h"

struct _inittab g_customInitTabs[] = {
#ifdef USE_IGE
    {"igeCore._igeCore", _PyInit__igeCore},
    {"igeCore.devtool._igeTools", PyInit__igeTools},
    {"igeVmath", PyInit_igeVmath},
    {"igeScene", PyInit_igeScene},
#endif

#ifdef USE_NUMPY
    {"numpy.core._multiarray_umath", PyInit__multiarray_umath},
    {"numpy.core._multiarray_tests", PyInit__multiarray_tests},
    {"numpy.core._rational_tests", PyInit__rational_tests},
    {"numpy.core._operand_flag_tests", PyInit__operand_flag_tests},
    {"numpy.core._struct_ufunc_tests", PyInit__struct_ufunc_tests},
    {"numpy.core._umath_tests", PyInit__umath_tests},
    {"numpy.linalg.lapack_lite", PyInit_lapack_lite},
    {"numpy.linalg._umath_linalg", PyInit__umath_linalg},
    {"numpy.fft._pocketfft_internal", PyInit__pocketfft_internal},
    {"numpy.random._bit_generator", PyInit__bit_generator},
    {"numpy.random._bounded_integers", PyInit__bounded_integers},
    {"numpy.random._common", PyInit__common},
    {"numpy.random._generator", PyInit__generator},
    {"numpy.random._mt19937", PyInit__mt19937},
    {"numpy.random.mtrand", PyInit_mtrand},
    {"numpy.random._pcg64", PyInit__pcg64},
    {"numpy.random._philox", PyInit__philox},
    {"numpy.random._sfc64", PyInit__sfc64},
#endif

#ifdef USE_IMGUI
    {"imguicore", PyInit_core},
#endif

#ifdef USE_BOX2D
    {"_Box2D", PyInit__Box2D},
#endif

#ifdef USE_IGE_BULLET
    {"igeBullet", PyInit_igeBullet},
#endif

#ifdef USE_OPENCV
    {"cv2.cv2", PyInit_cv2},
#endif

#ifdef USE_DLIB
    {"dlib", PyInit_dlib},
#endif

#ifdef USE_IGE_DLIB_EXT
    {"igeDlibExt", PyInit_igeDlibExt},
#endif

#ifdef USE_IGE_FIREBASE
    {"igeFirebase", PyInit_igeFirebase},
#endif

#ifdef USE_IGE_WEBVIEW
    {"igeWebview", PyInit_igeWebview},
#endif

#ifdef USE_IGE_ADS
    {"igeAds", PyInit_igeAds},
#endif

#ifdef USE_IGE_SOCIAL
    {"igeSocial", PyInit_igeSocial},
#endif

#ifdef USE_IGE_GAME_SERVICES
    {"igeGamesServices", PyInit_igeGamesServices},
#endif

#ifdef USE_IGE_IN_APP_PURCHASE
    {"igeInAppPurchase", PyInit_igeInAppPurchase},
#endif

#ifdef USE_IGE_APPFLYER
    {"igeAppsFlyer", PyInit_igeAppsFlyer},
#endif

#ifdef USE_IGE_SOUND
    {"igeSound", PyInit_igeSound},
#endif

#ifdef USE_IGE_NANOGUI
    {"igeNanoGUI", PyInit_igeNanoGUI},
#endif

#ifdef USE_IGE_NOTIFY
    {"igeNotify", PyInit_igeNotify},
#endif

#ifdef USE_IGE_CAMERA
    {"igeCamera", PyInit_igeCamera},
#endif

#ifdef USE_IGE_PAL
    {"igePAL", PyInit__igePAL},
#endif

#ifdef USE_IGE_ADJUST
    {"igeAdjust", PyInit_igeAdjust},
#endif

    {0, 0}};

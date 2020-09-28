#pragma once
#include <Python.h>

/// BEGIN - External Modules
#ifdef USE_IGE
extern PyObject *_PyInit__igeCore(void);
extern PyObject *PyInit__igeTools(void);
extern PyObject *PyInit_igeVmath(void);
extern PyObject* PyInit_igeScene();
#endif

#ifdef USE_IMGUI
extern PyObject *PyInit_core(void);
#endif

#ifdef USE_BOX2D
extern PyObject *PyInit__Box2D(void);
#endif

#ifdef USE_IGE_BULLET
extern PyObject *PyInit_igeBullet();
#endif

#ifdef USE_OPENCV
extern PyObject *PyInit_cv2(void);
#endif

#ifdef USE_DLIB
extern PyObject *PyInit_dlib();
#endif

#ifdef USE_IGE_DLIB_EXT
extern PyObject *PyInit_igeDlibExt();
#endif

#ifdef USE_IGE_FIREBASE
extern PyObject *PyInit_igeFirebase(void);
#endif

#ifdef USE_IGE_WEBVIEW
extern PyObject *PyInit_igeWebview(void);
#endif

#ifdef USE_IGE_WEBVIEW
extern PyObject *PyInit_igeWebview(void);
#endif

#ifdef USE_IGE_ADS
extern PyObject *PyInit_igeAds(void);
#endif

#ifdef USE_IGE_SOCIAL
extern PyObject *PyInit_igeSocial(void);
#endif

#ifdef USE_IGE_GAME_SERVICES
extern PyObject *PyInit_igeGamesServices(void);
#endif

#ifdef USE_IGE_IN_APP_PURCHASE
extern PyObject *PyInit_igeInAppPurchase(void);
#endif

#ifdef USE_IGE_APPFLYER
extern PyObject *PyInit_igeAppsFlyer(void);
#endif

#ifdef USE_IGE_SOUND
extern PyObject *PyInit_igeSound(void);
#endif

#ifdef USE_IGE_NANOGUI
extern PyObject *PyInit_igeNanoGUI(void);
#endif

#ifdef USE_IGE_NOTIFY
extern PyObject *PyInit_igeNotify(void);
#endif

#ifdef USE_IGE_CAMERA
extern PyObject *PyInit_igeCamera(void);
#endif

#ifdef USE_IGE_PAL
extern PyObject *PyInit__igePAL(void);
#endif

#ifdef USE_IGE_ADJUST
extern PyObject *PyInit_igeAdjust(void);
#endif

#ifdef USE_NUMPY
extern PyObject *PyInit__multiarray_umath(void);
extern PyObject *PyInit__multiarray_tests(void);
extern PyObject *PyInit__rational_tests(void);
extern PyObject *PyInit__operand_flag_tests(void);
extern PyObject *PyInit__struct_ufunc_tests(void);
extern PyObject *PyInit__umath_tests(void);
extern PyObject *PyInit_lapack_lite(void);
extern PyObject *PyInit__umath_linalg(void);
extern PyObject *PyInit__pocketfft_internal(void);
extern PyObject *PyInit__bit_generator(void);
extern PyObject *PyInit__bounded_integers(void);
extern PyObject *PyInit__common(void);
extern PyObject *PyInit__generator(void);
extern PyObject *PyInit__mt19937(void);
extern PyObject *PyInit_mtrand(void);
extern PyObject *PyInit__pcg64(void);
extern PyObject *PyInit__philox(void);
extern PyObject *PyInit__sfc64(void);
// extern PyObject* PyInit_entropy(void); // [IGE]: entropy was removed, use random.SeedSequence instead
#endif

/// END External Modules

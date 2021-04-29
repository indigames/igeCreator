#pragma once
#include <Python.h>

/// BEGIN - External Modules
#ifdef USE_IGECORE
extern PyObject *PyInit__igeCore(void);
extern PyObject *PyInit__igeTools(void);
extern PyObject *PyInit_igeVmath(void);
extern PyObject* PyInit_igeScene();
#endif

#ifdef USE_PYIMGUI
extern PyObject *PyInit_core(void);
#endif

#ifdef USE_PYBOX2D
extern PyObject *PyInit__Box2D(void);
#endif

#ifdef USE_IGEBULLET
extern PyObject *PyInit_igeBullet();
#endif

#ifdef USE_OPENCV
extern PyObject *PyInit_cv2(void);
#endif

#ifdef USE_DLIB
extern PyObject *PyInit_dlib();
#endif

#ifdef USE_IGEDLIBEXT
extern PyObject *PyInit_igeDlibExt();
#endif

#ifdef USE_IGEFIREBASE
extern PyObject *PyInit_igeFirebase(void);
#endif

#ifdef USE_IGEWEBVIEW
extern PyObject *PyInit_igeWebview(void);
#endif

#ifdef USE_IGEADS
extern PyObject *PyInit_igeAds(void);
#endif

#ifdef USE_IGESOCIAL
extern PyObject *PyInit_igeSocial(void);
#endif

#ifdef USE_IGEGAMESSERVICES
extern PyObject *PyInit_igeGamesServices(void);
#endif

#ifdef USE_IGEINAPPPURCHASE
extern PyObject *PyInit_igeInAppPurchase(void);
#endif

#ifdef USE_IGEAPPSFLYER
extern PyObject *PyInit_igeAppsFlyer(void);
#endif

#ifdef USE_IGESOUND
extern PyObject *PyInit_igeSound(void);
#endif

#ifdef USE_IGENANOGUI
extern PyObject *PyInit_igeNanoGUI(void);
#endif

#ifdef USE_IGENOTIFY
extern PyObject *PyInit_igeNotify(void);
#endif

#ifdef USE_IGECAMERA
extern PyObject *PyInit_igeCamera(void);
#endif

#ifdef USE_IGEPAL
extern PyObject *PyInit__igePAL(void);
#endif

#ifdef USE_IGEADJUST
extern PyObject *PyInit_igeAdjust(void);
#endif

#ifdef USE_IGEGAMEANALYTICS
extern PyObject *PyInit_igeGameAnalytics(void);
#endif

#ifdef USE_IGEAUTOTEST
extern PyObject *PyInit_igeAutoTest(void);
#endif

#ifdef USE_IGEEFFEKSEER
extern PyObject *PyInit_igeEffekseer(void);
#endif

#ifdef USE_TENSORFLOW
extern PyObject *PyInit__pywrap_tensorflow_interpreter_wrapper(void);
#endif

#ifdef USE_PILLOW
extern PyObject *PyInit__webp(void);
extern PyObject *PyInit__imagingtk(void);
extern PyObject *PyInit__imagingmorph(void);
extern PyObject *PyInit__imagingmath(void);
extern PyObject *PyInit__imagingft(void);
extern PyObject *PyInit__imagingcms(void);
extern PyObject *PyInit__imaging(void);
#endif

#ifdef USE_BULLET
extern PyObject *PyInit_pybullet(void);
#endif

#ifdef USE_NUMPY
extern PyObject *PyInit__multiarray_umath(void);
extern PyObject *PyInit__multiarray_tests(void);
extern PyObject *PyInit__operand_flag_tests(void);
extern PyObject *PyInit__rational_tests(void);
extern PyObject *PyInit__struct_ufunc_tests(void);
extern PyObject *PyInit__umath_tests(void);
extern PyObject *PyInit__umath_linalg(void);
extern PyObject *PyInit__pocketfft_internal(void);
extern PyObject *PyInit_lapack_lite(void);
extern PyObject *PyInit_bit_generator(void);
extern PyObject *PyInit_mtrand(void);
extern PyObject* PyInit__bounded_integers(void);
extern PyObject* PyInit__common(void);
extern PyObject* PyInit__generator(void);
extern PyObject* PyInit__mt19937(void);
extern PyObject* PyInit__pcg64(void);
extern PyObject* PyInit__philox(void);
extern PyObject* PyInit__sfc64(void);
#endif

/// END External Modules

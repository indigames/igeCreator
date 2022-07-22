#include "pythonExtension.h"

struct _inittab g_customInitTabs[] = {
#ifdef USE_IGECORE
    {"igeCore._igeCore", PyInit__igeCore},
    {"igeCore.devtool._igeTools", PyInit__igeTools},
    {"igeVmath", PyInit_igeVmath},
    {"igeScene", PyInit_igeScene},
#endif

#ifdef USE_IGESDK
    {"igeSdk", PyInit_igeSdk},
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
    {"numpy.random.bit_generator", PyInit_bit_generator},
    {"numpy.random._bounded_integers", PyInit__bounded_integers},
    {"numpy.random._common", PyInit__common},
    {"numpy.random._generator", PyInit__generator},
    {"numpy.random._mt19937", PyInit__mt19937},
    {"numpy.random.mtrand", PyInit_mtrand},
    {"numpy.random._pcg64", PyInit__pcg64},
    {"numpy.random._philox", PyInit__philox},
    {"numpy.random._sfc64", PyInit__sfc64},
#endif

#ifdef USE_PYIMGUI
    {"imguicore", PyInit_core},
#endif

#ifdef USE_PYBOX2D
    {"_Box2D", PyInit__Box2D},
#endif

#ifdef USE_IGEBULLET
    {"igeBullet", PyInit_igeBullet},
#endif

#ifdef USE_OPENCV
    {"cv2.cv2", PyInit_cv2},
#endif

#ifdef USE_DLIB
    {"dlib", PyInit_dlib},
#endif

#ifdef USE_IGEFIREBASE
    {"igeFirebase", PyInit_igeFirebase},
#endif

#ifdef USE_IGEWEBVIEW
    {"igeWebview", PyInit_igeWebview},
#endif

#ifdef USE_IGESOCIAL
    {"igeSocial", PyInit_igeSocial},
#endif

#ifdef USE_IGEGAMESSERVICES
    {"igeGamesServices", PyInit_igeGamesServices},
#endif

#ifdef USE_IGESOUND
    {"igeSound", PyInit_igeSound},
#endif

#ifdef USE_IGENANOGUI
    {"igeNanoGUI", PyInit_igeNanoGUI},
#endif

#ifdef USE_IGENOTIFY
    {"igeNotify", PyInit_igeNotify},
#endif

#ifdef USE_IGECAMERA
    {"igeCamera", PyInit_igeCamera},
#endif

#ifdef USE_IGEPAL
    {"igePAL", PyInit__igePAL},
#endif

#ifdef USE_IGEAUTOTEST
    {"igeAutoTest", PyInit_igeAutoTest},
#endif

#ifdef USE_IGEEFFEKSEER
    {"igeEffekseer", PyInit__igeEffekseer},
#endif

#ifdef USE_TENSORFLOW
    {"_pywrap_tensorflow_interpreter_wrapper", PyInit__pywrap_tensorflow_interpreter_wrapper },
#endif

#ifdef USE_PILLOW
    {"_webp", PyInit__webp },
    {"_imagingtk", PyInit__imagingtk },
    {"_imagingmorph", PyInit__imagingmorph },
    {"_imagingmath", PyInit__imagingmath },
    {"_imagingft", PyInit__imagingft },
    {"_imagingcms", PyInit__imagingcms },
    {"_imaging", PyInit__imaging },
#endif

#ifdef USE_BULLET
    {"pybullet", PyInit_pybullet },
#endif

    {0, 0}};

#include <Python.h>
#include "pyxieEffekseer.h"

typedef struct {
    PyObject_HEAD
        pyxieEffekseer* effekseer;
} effekseer_obj;

extern PyTypeObject EffekseerType;

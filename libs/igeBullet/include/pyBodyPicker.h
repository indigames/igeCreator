#pragma once

#include <Python.h>

namespace ige::bullet
{
    // Object declaration
    typedef struct
    {
        PyObject_HEAD;
    } bodypicker_obj;

    // Type declaration
    extern PyTypeObject BodyPickerType;

} // namespace ige::bullet

#pragma once

#include <Python.h>

namespace ige::bullet
{
    // Object declaration
    typedef struct
    {
        PyObject_HEAD;
        void *btshape;
    } shape_obj;

    // Type declaration
    extern PyTypeObject ShapeType;

    // New/dealloc
    PyObject *shape_new(PyTypeObject *type, PyObject *args, PyObject *kw);
    void shape_dealloc(shape_obj *self);

    // String representation
    PyObject *shape_str(shape_obj *self);

    // Get mesh data
    static PyObject *shape_getMeshData(shape_obj *self);

    // Add child shape
    static PyObject *shape_addChildShape(shape_obj *self, PyObject *args);

} // namespace ige::bullet
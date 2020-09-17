#pragma once

#include <Python.h>

namespace ige::bullet
{
    // Object declaration
    typedef struct
    {
        PyObject_HEAD;
        void *btconstraint;
    } constraint_obj;

    // Type declaration
    extern PyTypeObject ConstraintType;

    // New/Dealloc
    PyObject *constraint_new(PyTypeObject *type, PyObject *args, PyObject *kw);
    void constraint_dealloc(constraint_obj *self);

    // String representation
    PyObject *constraint_str(constraint_obj *self);

    // Set limit
    static PyObject *constraint_setLimit(constraint_obj *self, PyObject *args);

} // namespace ige::bullet

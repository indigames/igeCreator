#pragma once

#include <Python.h>

namespace ige::bullet
{
    // Object declaration
    typedef struct
    {
        PyObject_HEAD;
        void *btbody;
        PyObject *world;
        short collisionGroup;
        short collisionMask;
        char *name;
        int *orgMap;
        int numOrgPoss;
    } softbody_obj;

    // Type declaration
    extern PyTypeObject SoftBodyType;

    // New/Dealloc
    PyObject *softbody_new(PyTypeObject *type, PyObject *args, PyObject *kw);
    void softbody_dealloc(softbody_obj *self);

    // String representation
    PyObject *softbody_str(softbody_obj *self);

    // Name
    PyObject *softbody_getname(softbody_obj *self);
    int softbody_setname(softbody_obj *self, PyObject *value);

    // Transform
    PyObject *softbody_gettransform(softbody_obj *self);
    int softbody_settransform(softbody_obj *self, PyObject *value);

    // Position
    PyObject *softbody_getposition(softbody_obj *self);
    int softbody_setposition(softbody_obj *self, PyObject *value);

    // Rotation
    PyObject *softbody_getrotation(softbody_obj *self);
    int softbody_setrotation(softbody_obj *self, PyObject *value);

    // Get shape
    PyObject *softbody_getshape(softbody_obj *self);

    // Append deformable anchor
    static PyObject *softbody_appendDeformableAnchor(softbody_obj *self, PyObject *args);\

    // Get mesh position
    static PyObject *softbody_meshPositions(softbody_obj *self, PyObject *args);

    // Get mesh normal
    static PyObject *softbody_meshNormals(softbody_obj *self, PyObject *args);

    // Find nearest node
    static PyObject *softbody_findNearestNode(softbody_obj *self, PyObject *args);
} // namespace ige::bullet

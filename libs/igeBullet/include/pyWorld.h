#pragma once

#include <Python.h>

namespace ige::bullet
{
    // Object declaration
    typedef struct
    {
        PyObject_HEAD;
        void *broadphase;
        void *dispatcher;
        void *solver;
        void *ghostPairCallback;
        void *collisionConfiguration;
        void *btworld;
        void *vehicles;
        uint64_t lasttime;
        double rate;
        bool deformable;
    } world_obj;

    // Type declaration
    extern PyTypeObject DynamicsWorldType;

    // New/Deallocate
    PyObject *dynworld_new(PyTypeObject *type, PyObject *args, PyObject *kw);
    void dynworld_dealloc(world_obj *self);

    // String representation
    PyObject *dynworld_str(world_obj *self);

    // Gravity
    PyObject *dynworld_getgravity(world_obj *self);
    int dynworld_setgravity(world_obj *self, PyObject *value);

    // Update rate
    PyObject *dynworld_getupdateRate(world_obj *self);
    int dynworld_setupdateRate(world_obj *self, PyObject *value);

    // Add/remove physic object
    static PyObject *dynworld_add(world_obj *self, PyObject *args);
    static PyObject *dynworld_remove(world_obj *self, PyObject *args);

    // Update physic world
    static PyObject *dynworld_step(world_obj *self, PyObject *args);

    // Wait
    static PyObject *dynworld_wait(world_obj *self);

    // Clear world
    static PyObject *dynworld_clear(world_obj *self);

    // Get number of collistion objects
    static PyObject *dynworld_getNumCollisionObjects(world_obj *self);

    // Get rigid body
    static PyObject *dynworld_getRigidBody(world_obj *self, PyObject *args);

    // Raytest one: find nearest intersection
    static PyObject *dynworld_rayTestOne(world_obj *self, PyObject *args);

    // Raytest all: find all intersection
    static PyObject *dynworld_rayTestAll(world_obj *self, PyObject *args);

    // Contact test
    static PyObject *dynworld_contactTest(world_obj *self, PyObject *args);

    // Contact pair test
    static PyObject *dynworld_contactPairTest(world_obj *self, PyObject *args);

    // Convex sweep test
    static PyObject *dynworld_convexSweepTest(world_obj *self, PyObject *args);

} // namespace ige::bullet

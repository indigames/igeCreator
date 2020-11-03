#pragma once

#include <Python.h>
#include "pyWorld.h"

namespace ige::bullet
{
    // Object declaration
    typedef struct
    {
        PyObject_HEAD;
        void* btbody;
        world_obj *world;
        short collisionGroup;
        short collisionMask;
        char *name;
    } rigidbody_obj;

    // Type declaration
    extern PyTypeObject RigidBodyType;

    // New/dealloc
    PyObject *rigidbody_new(PyTypeObject *type, PyObject *args, PyObject *kw);
    void rigidbody_dealloc(rigidbody_obj *self);

    // String representation
    PyObject *rigidbody_str(rigidbody_obj *self);

    // Name
    PyObject *rigidbody_getname(rigidbody_obj *self);
    int rigidbody_setname(rigidbody_obj *self, PyObject *value);

    // Transform
    PyObject *rigidbody_gettransform(rigidbody_obj *self);
    int rigidbody_settransform(rigidbody_obj *self, PyObject *value);

    // Position
    PyObject *rigidbody_getposition(rigidbody_obj *self);
    int rigidbody_setposition(rigidbody_obj *self, PyObject *value);

    // Rotation
    PyObject *rigidbody_getrotation(rigidbody_obj *self);
    int rigidbody_setrotation(rigidbody_obj *self, PyObject *value);

    // Friction
    PyObject *rigidbody_getfriction(rigidbody_obj *self);
    int rigidbody_setfriction(rigidbody_obj *self, PyObject *value);

    // Restitution
    PyObject *rigidbody_getrestitution(rigidbody_obj *self);
    int rigidbody_setrestitution(rigidbody_obj *self, PyObject *value);

    // Shape
    PyObject *rigidbody_getshape(rigidbody_obj *self);
    int rigidbody_setshape(rigidbody_obj *self, PyObject *value);

    // Collision callback
    PyObject *rigidbody_getenableCollisionCallback(rigidbody_obj *self);
    int rigidbody_setenableCollisionCallback(rigidbody_obj *self, PyObject *value);

    // Contact response
    PyObject *rigidbody_getenableContactResponse(rigidbody_obj *self);
    int rigidbody_setenableContactResponse(rigidbody_obj *self, PyObject *value);

    // Collision group bits
    PyObject *rigidbody_getcollisionGroupBit(rigidbody_obj *self);
    int rigidbody_setcollisionGroupBit(rigidbody_obj *self, PyObject *value);

    // Collision group mask
    PyObject *rigidbody_getcollisionGroupMask(rigidbody_obj *self);
    int rigidbody_setcollisionGroupMask(rigidbody_obj *self, PyObject *value);

    // Linear damping
    PyObject *rigidbody_getlinearDamping(rigidbody_obj *self);
    int rigidbody_setlinearDamping(rigidbody_obj *self, PyObject *value);

    // Angular damping
    PyObject *rigidbody_getangularDamping(rigidbody_obj *self);
    int rigidbody_setangularDamping(rigidbody_obj *self, PyObject *value);

    // Linear velocity
    PyObject *rigidbody_getlinearVelocity(rigidbody_obj *self);
    int rigidbody_setlinearVelocity(rigidbody_obj *self, PyObject *value);

    // Angular velocity
    PyObject *rigidbody_getangularVelocity(rigidbody_obj *self);
    int rigidbody_setangularVelocity(rigidbody_obj *self, PyObject *value);

    // Linear factor
    PyObject *rigidbody_getlinearFactor(rigidbody_obj *self);
    int rigidbody_setlinearFactor(rigidbody_obj *self, PyObject *value);

    // Angular factor
    PyObject *rigidbody_getangularFactor(rigidbody_obj *self);
    int rigidbody_setangularFactor(rigidbody_obj *self, PyObject *value);

    // Linear sleeping threshold
    PyObject *rigidbody_getlinearSleepingThreshold(rigidbody_obj *self);
    int rigidbody_setlinearSleepingThreshold(rigidbody_obj *self, PyObject *value);

    // Angular sleeping threshold
    PyObject *rigidbody_getangularSleepingThreshold(rigidbody_obj *self);
    int rigidbody_setangularSleepingThreshold(rigidbody_obj *self, PyObject *value);

    // Deactivation time
    PyObject *rigidbody_getdeactivationTime(rigidbody_obj *self);
    int rigidbody_setdeactivationTime(rigidbody_obj *self, PyObject *value);

    // Activation state
    PyObject *rigidbody_getactivationState(rigidbody_obj *self);
    int rigidbody_setactivationState(rigidbody_obj *self, PyObject *value);

    // Mass
    PyObject *rigidbody_getMass(rigidbody_obj *self);
    int rigidbody_setMass(rigidbody_obj *self, PyObject *value);

    // Apply torque
    static PyObject *rigidbody_applyTorque(rigidbody_obj *self, PyObject *args);

    // Apply force
    static PyObject *rigidbody_applyForce(rigidbody_obj *self, PyObject *args);

    // Apply impulse
    static PyObject *rigidbody_applyImpulse(rigidbody_obj *self, PyObject *args);

    // Clear forces
    static PyObject *rigidbody_clearForces(rigidbody_obj *self);

    // Get AABB
    static PyObject *rigidbody_getAabb(rigidbody_obj *self);
} // namespace ige::bullet
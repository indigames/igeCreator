#pragma once

#include <Python.h>

namespace ige::bullet
{
    // Object declaration
    typedef struct
    {
        PyObject_HEAD;
        void *btVehicle;
        void *btTuning;
        void *btVehicleRayCaster;
    } vehicle_obj;

    // Type declaration
    extern PyTypeObject VehicleType;

    // New/Dealloc
    PyObject *vehicle_new(PyTypeObject *type, PyObject *args, PyObject *kw);
    void vehicle_dealloc(vehicle_obj *self);

    // String representation
    PyObject *vehicle_str(vehicle_obj *self);

    // Add wheel
    static PyObject *vehicle_addWheel(vehicle_obj *self, PyObject *args, PyObject *kw);

    // Add steering value
    static PyObject *vehicle_setSteeringValue(vehicle_obj *self, PyObject *args);

    // Apply engine force
    static PyObject *vehicle_applyEngineForce(vehicle_obj *self, PyObject *args);

    // Set brake
    static PyObject *vehicle_setBrake(vehicle_obj *self, PyObject *args);

    // Get wheel position
    static PyObject *vehicle_getWheelPosition(vehicle_obj *self, PyObject *args);

    // Get wheel rotation
    static PyObject *vehicle_getWheelRotation(vehicle_obj *self, PyObject *args);
} // namespace ige::bullet

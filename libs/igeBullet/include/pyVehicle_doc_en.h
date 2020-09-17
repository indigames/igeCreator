#pragma once

#include <Python.h>

namespace ige::bullet
{
    PyDoc_STRVAR(vehicle_doc,
                 "Simulates wheeled vehicles.\n"
                 "\n"
                 "Constructors\n"
                 "-----------\n"
                 "	igeBullet.vehicle(world, body)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "	world : igeBullet.world\n"
                 "		dynamics world object.\n"
                 "	body : igeBullet.rigidBody\n"
                 "		rigidBody of vehicle body.\n");

    PyDoc_STRVAR(addWheel_doc,
                 "Adds a wheel to the vehicle.\n"
                 "	\n"
                 "vehicle.addWheel(connectionPointCS, wheelDirectionCS0, wheelAxleCS, suspensionRestLength, wheelRadius, isFrontWheel)\n"
                 "Parameters\n"
                 "----------\n"
                 "	connectionPointCS : (x,y,z)\n"
                 "	wheelDirectionCS0 : (x,y,z)\n"
                 "	wheelAxleCS : (x,y,z)\n"
                 "	suspensionRestLength : float\n"
                 "	wheelRadius : float\n"
                 "	isFrontWheel : bool\n");

    PyDoc_STRVAR(setSteeringValue_doc,
                 "Steering value of vehicle.\n"
                 "	\n"
                 "vehicle.setSteeringValue(value, index)\n"
                 "Parameters\n"
                 "----------\n"
                 "	value : float\n"
                 "		Steering value"
                 "	index : int\n"
                 "		Index of weel");

    PyDoc_STRVAR(applyEngineForce_doc,
                 "Apply engine force.\n"
                 "	\n"
                 "vehicle.applyEngineForce(force, index)\n"
                 "Parameters\n"
                 "----------\n"
                 "	force : float\n"
                 "		Engine force"
                 "	index : int\n"
                 "		Index of weel");

    PyDoc_STRVAR(setBrake_doc,
                 "Set break.\n"
                 "	\n"
                 "vehicle.setBrake(break, index)\n"
                 "Parameters\n"
                 "----------\n"
                 "	break : float\n"
                 "		break force"
                 "	index : int\n"
                 "		Index of weel");

    PyDoc_STRVAR(getWheelPosition_doc,
                 "Get wheel position\n"
                 "	\n"
                 "vehicle.getWheelPosition(index)\n"
                 "	index : int\n"
                 "		Index of weel");

    PyDoc_STRVAR(getWheelRotation_doc,
                 "Get wheel rotation\n"
                 "	\n"
                 "vehicle.getWheelRotation(index)\n"
                 "	index : int\n"
                 "		Index of weel");
} // namespace ige::bullet

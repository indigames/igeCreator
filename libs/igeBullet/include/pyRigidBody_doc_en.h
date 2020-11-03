#pragma once

#include <Python.h>

namespace ige::bullet
{
    PyDoc_STRVAR(rigidbody_doc,
                 "3d physics rigid body\n"
                 "\n"
                 "igeBullet.rigidBody(shape, mass, pos, rot, activate)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "    shape : igeBullet.shape\n"
                 "        shape object\n"
                 "    mass : float\n"
                 "        mass of object\n"
                 "    pos : tuple of float (x,y,z)\n"
                 "        start location of object\n"
                 "    rot : tuple of float (x,y,z,w)\n"
                 "        start orientation of object\n"
                 "    activate : bool\n"
                 "        Whether the physics calculation is active.\n"
                 "        Do not calculate unless external force is applied\n");

    PyDoc_STRVAR(rigidbody_transform_doc,
                 "rigit body transform\n"
                 "\n"
                 "	type :  tuple  of (position , rotation)  ((x,y,z), (x,y,z,w))\n	"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_name_doc,
                 "rigit body name\n"
                 "\n"
                 "	type :  string\n	"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_position_doc,
                 "rigit body position\n"
                 "\n"
                 "	type :  tuple of float (x,y,z)\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_rotation_doc,
                 "rigit body position\n"
                 "\n"
                 "	type :  tuple of float(x,y,z,w)\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_friction_doc,
                 "rigit body friction\n"
                 "\n"
                 "	type :  float\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_restitution_doc,
                 "rigit body restitution\n"
                 "\n"
                 "	type :  float\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_shape_doc,
                 "shape object\n"
                 "\n"
                 "	type :  igeBullet.shape\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_enableCollisionCallback_doc,
                 "Whether collision callback is enabled\n"
                 "\n"
                 "	type :  bool\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_enableContactResponse_doc,
                 "Whether contact response is enabled\n"
                 "\n"
                 "	type :  bool\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_collisionGroupBit_doc,
                 "Own collision filter bit\n"
                 "\n"
                 "	type :  int\n"
                 "	(1, 2, 4, 8, 16...16384)\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_collisionGroupMask_doc,
                 "Collision filter bit for which you want to enable collision\n"
                 "\n"
                 "	type :  int\n"
                 "	(1, 2, 4, 8, 16...16384)\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_linearDamping_doc,
                 "rigid body linear damping\n"
                 "\n"
                 "	type :  float\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_angularDamping_doc,
                 "rigid body angular damping\n"
                 "\n"
                 "	type :  float\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_linearVelocity_doc,
                 "rigid body linear velocity\n"
                 "\n"
                 "	type :  tuple of float(x,y,z)\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_angularVelocity_doc,
                 "rigid body angular velocity\n"
                 "\n"
                 "	type :  tuple of float(x,y,z)\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_angularSleepingThreshold_doc,
                 "The maximum amount of rotation that rigid body can go to sleep.\n"
                 "If the amount of rotation is less than this value, I think it will go to sleep.\n"
                 "\n"
                 "	type :  float  (read / write)");
    PyDoc_STRVAR(rigidbody_linearSleepingThreshold_doc,
                 "The maximum amount of movement that rigid body can go to sleep.\n"
                 "If the amount of movement is less than this value, I think it will go to sleep.\n"
                 "\n"
                 "	type :  float (read / write)");

    PyDoc_STRVAR(rigidbody_deactivationTime_doc,
                 "The time after which a resting rigidbody gets deactived.\n"
                 "\n"
                 "	type :  float (read / write)");

    PyDoc_STRVAR(rigidbody_activationState_doc,
                 "Whether rigidbody is active.\n"
                 "Setting False will clear the Force.\n"
                 "\n"
                 "	type :  bool (read / write)");

    PyDoc_STRVAR(rigidbody_Mass_doc,
                 "Mass of rigid body.\n"
                 "\n"
                 "	type :  float (read / write)");

    PyDoc_STRVAR(rigidbody_ccdSweptSphereRadius_doc,
        "CcdSweptSphereRadius of rigid body.\n"
        "\n"
        "	type :  float (read / write)");

    PyDoc_STRVAR(rigidbody_ccdMotionThreshold_doc,
        "CcdMotionThreshold of rigid body.\n"
        "\n"
        "	type :  float (read / write)");

    PyDoc_STRVAR(rigidbody_linearFactor_doc,
                 "rigid body linear factor\n"
                 "\n"
                 "	type :  tuple of float(x,y,z)\n"
                 "	read / write");

    PyDoc_STRVAR(rigidbody_angularFactor_doc,
                 "rigid body angular factor\n"
                 "\n"
                 "	type :  tuple of float(x,y,z)\n"
                 "	read / write");

    PyDoc_STRVAR(applyTorque_doc,
                 " Applies a torque on a rigid body.\n"
                 "\n"
                 "digidBody.applyTorque(torque)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "    torque : tuple of float(x,y,z)\n");

    PyDoc_STRVAR(applyForce_doc,
                 "Applies a force on a rigid body.\n"
                 "If position is not specified, force is applied to the center.\n"
                 "\n"
                 "digidBody.applyForce(torque, position)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "    force : tuple of float(x,y,z)\n"
                 "    position : tuple of float(x,y,z)  (optional)\n");

    PyDoc_STRVAR(applyImpulse_doc,
                 " Applies a impulse on a rigid body.\n"
                 "If position is not specified, force is applied to the center.\n"
                 "\n"
                 "digidBody.applyImpulse(impulse)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "    impulse : tuple of float(x,y,z)\n"
                 "    position : tuple of float(x,y,z)  (optional)\n");

    PyDoc_STRVAR(clearForces_doc,
                 "The forces on each rigidbody is accumulating together with gravity.\n"
                 "clear this after each timestep.\n"
                 "\n"
                 "digidBody.clearForces()");

    PyDoc_STRVAR(setMass_doc,
                 "Change the mass of rigid body.\n"
                 "\n"
                 "	To update correctly\n"
                 "	world.rerease(body)\n"
                 "	body.setMass(mass)\n"
                 "	world.add(body)\n"
                 "	will do\n"
                 "\n"
                 "	digidBody.setMass(mass)\n"
                 "Parameters\n"
                 "----------\n"
                 "	mass : float\n");

    PyDoc_STRVAR(getAabb_doc,
                 "get the transformed aabb value\n"
                 "\n"
                 "min, max = world.getAabb()\n"
                 "\n"
                 "Returns\n"
                 "-------\n"
                 "    min : (float,float,float)\n"
                 "        tuple of (x,y,z) float vector value.\n"
                 "    max :  (float,float,float)\n"
                 "        tuple of (x,y,z) float vector value.\n");






} // namespace ige::bullet
#pragma once

#include <Python.h>

namespace ige::bullet
{
    PyDoc_STRVAR(dynworld_doc,
                 "3d physics world\n");

    PyDoc_STRVAR(gravity_doc,
                 "gravity vector\n"
                 "\n"
                 "    type :  tuple of float(x,y,z)\n"
                 "        read / write");

    PyDoc_STRVAR(updateRate_doc,
                 "symulation update rate.\n"
                 "\n"
                 "    type : float\n"
                 "        read / write");

    PyDoc_STRVAR(add_doc,
                 "Add rigidbody or constarain to world\n"
                 "\n"
                 "world.add(object)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "    object : rigidBody or constrain\n"
                 "        rigidBody or constrain objecrt add to world.");

    PyDoc_STRVAR(remove_doc,
                 "Remove rigidbody or constarain from world\n"
                 "\n"
                 "world.remove(object)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "    object : rigidBody or constrain\n"
                 "        rigidBody or constrain objecrt remove from world.");

    PyDoc_STRVAR(clear_doc,
                 "Clear all added rigidbody and constarain\n"
                 "\n"
                 "world.clear()\n"
                 "\n");

    PyDoc_STRVAR(step_doc,
                 "step simulation\n"
                 "\n"
                 "world.step(step, maxSubStep, fixedTimeStep)\n"
                 "Parameters\n"
                 "----------\n"
                 "    step : float (optional)\n"
                 "        how many seconds to advance the time.\n"
                 "        Default value is system elapsed time.\n"
                 "    maxSubStep : float (optional)\n"
                 "        How many times can the time be divided internally.\n"
                 "        Default value is 1\n"
                 "    fixedTimeStep : float (optional)\n"
                 "        Internally, calculate for each time interval.\n"
                 "        Default value is 1/60");

    PyDoc_STRVAR(wait_doc,
                 "wait until async simultion is end\n"
                 "\n"
                 "world.wait()\n"
                 "\n");

    PyDoc_STRVAR(getNumCollisionObjects_doc,
                 "get number of collision objects in the world\n"
                 "\n"
                 "num = world.getNumCollisionObjects()\n"
                 "\n"
                 "Returns\n"
                 "-------\n"
                 "    num : int\n"
                 "        number of collision objects");

    PyDoc_STRVAR(getRigidBody_doc,
                 "get collision object\n"
                 "\n"
                 "obj = world.getRigidBody(index)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "    index : int\n"
                 "        index of added rigidBody(Order added)\n"
                 "\n"
                 "Returns\n"
                 "-------\n"
                 "    obj : rigidBody\n"
                 "        rigidBody object");

    PyDoc_STRVAR(rayTestOne_doc,
                 "Find most nearest Intersection between \n"
                 "line segment(start-end)and a collision object in the world.\n"
                 "\n"
                 "result = world.rayTestOne(start, end, mask)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "    start : tuple of float(x,y,z)\n"
                 "        start position of ray line segment\n"
                 "    end : tuple of float(x,y,z)\n"
                 "        end position of ray line segment\n"
                 "    mask : int  (optional)\n"
                 "        Collision filter mask (default -1 (all objects))\n"
                 "\n"
                 "Returns\n"
                 "-------\n"
                 "    result\n"
                 "        dictionary of contact point infomation\n"
                 "        {'collisionObject' : igeBullet.rigidbody,\n"
                 "         'hitPosition': tuple of float(x,y,z),\n"
                 "         'hitNormal' : tuple of float(x,y,z),\n"
                 "         'index' : int}");

    PyDoc_STRVAR(rayTestAll_doc,
                 "Find all Intersection between \n"
                 "line segment(start-end)and a collision object in the world.\n"
                 "\n"
                 "result = world.rayTestOne(start, end, mask)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "    start : tuple of float(x,y,z)\n"
                 "        start position of ray line segment\n"
                 "    end : tuple of float(x,y,z)\n"
                 "        end position of ray line segment\n"
                 "    mask : int  (optional)\n"
                 "        Collision filter mask (default -1 (all objects))\n"
                 "\n"
                 "Returns\n"
                 "-------\n"
                 "    result\n"
                 "        tuple of all dictionary of contact point infomation\n"
                 "        ({'collisionObject' : igeBullet.rigidbody,\n"
                 "         'hitPosition': tuple of float(x,y,z),\n"
                 "         'hitNormal' : tuple of float(x,y,z),\n"
                 "         'index' : int},\n"
                 "         {},{}...)");

    PyDoc_STRVAR(contactTest_doc,
                 "Collision between specified object and other objects in the world\n"
                 "\n"
                 "result = world.contactTest(object, mask)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "    object : igeBullet.rigidBody\n"
                 "        Object that you want to detect collision\n"
                 "    mask : int  (optional)\n"
                 "        Collision filter mask (default -1 (all objects))\n"
                 "\n"
                 "Returns\n"
                 "-------\n"
                 "    result\n"
                 "    tuple of dictionary of contact infomation\n"
                 "    ({'objectA' : igeBullet.rigidBody,\n"
                 "      'objectB' : igeBullet.rigidBody,\n"
                 "      'localPosA' : tuple of float(x,y,z),\n"
                 "      'localPosB' : tuple of float(x,y,z),\n"
                 "      'worldPosA' : tuple of float(x,y,z),\n"
                 "      'worldPosB' : tuple of float(x,y,z),\n"
                 "      'normal' : tuple of float(x,y,z)}, {}, {},...)\n");

    PyDoc_STRVAR(contactPairTest_doc,
                 "Collision detection between two objects\n"
                 "\n"
                 "result = world.contactPairTest(objectA,objectB, mask)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "    objectA : igeBullet.rigidBody\n"
                 "        Object that you want to detect collision\n"
                 "    objectB : igeBullet.rigidBody\n"
                 "        Object that you want to detect collision\n"
                 "    mask : int  (optional)\n"
                 "        Collision filter mask (default -1 (all objects))\n"
                 "\n"
                 "Returns\n"
                 "-------\n"
                 "    result\n"
                 "    tuple of dictionary of contact infomation\n"
                 "    ({'objectA' : igeBullet.rigidBody,\n"
                 "      'objectB' : igeBullet.rigidBody,\n"
                 "      'localPosA' : tuple of float(x,y,z),\n"
                 "      'localPosB' : tuple of float(x,y,z),\n"
                 "      'worldPosA' : tuple of float(x,y,z),\n"
                 "      'worldPosB' : tuple of float(x,y,z),\n"
                 "      'normal' : tuple of float(x,y,z)}, {}, {},...)\n");

    PyDoc_STRVAR(convexSweepTest_doc,
                 "Detects a collision while the convex shape \n"
                 "moves from the start point to the end.\n"
                 "\n"
                 "result = world.convexSweepTest(shape, from, to, allowedCcdPenetration, mask)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "	shape : igeBullet.rigidBody\n"
                 "		Object that you want to detect collision\n"
                 "	from : (position, rotation)  ((x,y,z),(x,y,z,w))\n"
                 "		Start Transform\n"
                 "	to : (position, rotation)  ((x,y,z),(x,y,z,w))\n"
                 "		End Transform\n"
                 "	allowedCcdPenetration : float\n"
                 "		Allowable penetration range\n"
                 "	mask : int  (optional)\n"
                 "		Collision filter mask (default -1 (all objects))\n"
                 "\n"
                 "Returns\n"
                 "-------\n"
                 "    result\n"
                 "    tuple of dictionary of contact infomation\n"
                 "    ({'collisionObject' : igeBullet.rigidBody,\n"
                 "      'hitPosition' : tuple of float(x,y,z),\n"
                 "      'hitNormal' : tuple of float(x,y,z),\n"
                 "      'hitFlaction' : float}, {}, {},...\n");

} // namespace ige::bullet

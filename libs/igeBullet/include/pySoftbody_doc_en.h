#pragma once

#include <Python.h>

namespace ige::bullet
{
PyDoc_STRVAR(softbody_doc,
    "soft body\n"
    "\n"
    "igeBullet.softBody(world, positions, triangles, corner, resx, resy, fixeds, perturbation, mass, springStiffness)\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "   world : igeBullet.world\n"
    "       physics world object\n"
    "   positions : [(x,y,z),(x,y,z), ...]\n"
    "       vertex list of polygon shape\n"
    "   triangles : [int, int, ...]\n"
    "       triangle indices of polygon shape\n"
    "   corner : [(x,y,z), (x,y,z), (x,y,z), (x,y,z)]\n"
    "       4 corners of rectangle cloth object to create\n"
    "   resx : int\n"
    "       X division number of rectangle cloth object to create\n"
    "   resy : int\n"
    "       Y division number of rectangle cloth object to create\n"
    "   fixeds int(bit field)\n"
    "       Do not move each of the 4 corners.\n"
    "       1:corner0, 2:corner1, 4:corner2, 5:corner3\n"
    "   perturbation : float\n"
    "       perturbation distance of eatch vertex\n"
    "   mass : float\n"
    "        mass of object\n"
    "   springStiffness : float\n"
    "       stiffness of soft body spring \n"
    "\n"
    "Examples\n"
    "----------\n"
    "   Generate a soft body from arbitrary polygon data\n"
    "       igeBullet.softBody(world, positions, triangles, mass, springStiffness)\n"
    "   Generate a square cloth soft body\n"
    "       igeBullet.softBody(world, corner, resx, resy, fixeds, perturbation, mass, springStiffness)\n"
);


PyDoc_STRVAR(softbody_findNearestNode_doc,
    "Finds the nearest node from the specified position \n"
    "Node is the vertex of a polygon\n"
    "\n"
    "index, nodePos = softBody.findNearestNode(pos)\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "    position : (x,y,z)\n"
    "Returns\n"
    "----------\n"
    "   index : int\n"
    "       index no of node\n"
    "   nodePos : (x,y,z)\n"
    "       position of node\n"
);

PyDoc_STRVAR(softbody_appendDeformableAnchor_doc,
    "Fix the soft body node and rigid body.\n"
    "\n"
    "softBody.appendDeformableAnchor(node, body)\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "   node : int\n"
    "       index no of node\n"
    "   body : igeBullet.rigidBody\n"
    "       Rigid body to fix the node\n"
);

PyDoc_STRVAR(softbody_removeDeformableAnchor_doc,
    "Remove deformable anchoor.\n"
    "\n"
    "softBody.removeDeformableAnchor(node)\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "   node : int\n"
    "       index no of node\n"
);

PyDoc_STRVAR(softbody_meshPositions_doc,
    "Get the deformed vertices \n"
    "\n"
    "verts = softBody.meshPositions()\n"
    "\n"
    "Returns\n"
    "----------\n"
    "   verts : [(x,y,z), (x,y,z), ...]\n"
    "       deformed vertices\n"
);

PyDoc_STRVAR(softbody_meshNormals_doc,
    "Get the deformed normal\n"
    "\n"
    "verts = softBody.meshPositions()\n"
    "\n"
    "Returns\n"
    "----------\n"
    "   verts : [(x,y,z), (x,y,z), ...]\n"
    "       deformed vertices\n"
);

PyDoc_STRVAR(softbody_addVelocity_doc,
    "Add velocity to the soft body.\n"
    "\n"
    "softBody.addVelocity(vect, node)\n"
    "\n"
    "Parameters\n"
    "----------\n"
    "   vect : (x,y,z)\n"
    "       vector of velocity\n"
    "   node : int\n"
    "       node no to add velocity.    \n"
    "       If you don't specify node, it works on the center of mass\n"
);

PyDoc_STRVAR(softbody_name_doc,
    "rigit body name\n"
    "\n"
    "	type :  string\n	"
    "	read / write");

PyDoc_STRVAR(softbody_position_doc,
    "rigit body position\n"
    "\n"
    "	type :  tuple of float (x,y,z)\n"
    "	read / write");

PyDoc_STRVAR(softbody_rotation_doc,
    "rigit body position\n"
    "\n"
    "	type :  tuple of float(x,y,z,w)\n"
    "	read / write");

PyDoc_STRVAR(softbody_transform_doc,
    "rigit body transform\n"
    "\n"
    "	type :  tuple  of (position , rotation)  ((x,y,z), (x,y,z,w))\n	"
    "	read / write");

PyDoc_STRVAR(softbody_collisionGroupBit_doc,
    "Own collision filter bit\n"
    "\n"
    "	type :  int\n"
    "	(1, 2, 4, 8, 16...16384)\n"
    "	read / write");

PyDoc_STRVAR(softbody_collisionGroupMask_doc,
    "Collision filter bit for which you want to enable collision\n"
    "\n"
    "	type :  int\n"
    "	(1, 2, 4, 8, 16...16384)\n"
    "	read / write");

PyDoc_STRVAR(softbody_linearVelocity_doc,
    "The linear velocity of the center of mass\n"
    "\n"
    "	type :  tuple of float(x,y,z)\n"
    "	read / write");

PyDoc_STRVAR(softbody_angularVelocity_doc,
    "The angular velocity of the center of mass\n"
    "\n"
    "	type :  tuple of float(x,y,z)\n"
    "	write only");


}




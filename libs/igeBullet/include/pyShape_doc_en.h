#pragma once

#include <Python.h>

namespace ige::bullet
{
    PyDoc_STRVAR(shape_doc,
                 "3d physics shape\n"
                 "\n"
                 "Formula\n"
                 "----------\n"
                 "    create box shape\n"
                 "        shape = igeBullet.shape(igeBullet.BOX_SHAPE_PROXYTYPE, halfExtents = (1, 1, 1))\n"
                 "    create sphere shape\n"
                 "        shape = igeBullet.shape(igeBullet.SPHERE_SHAPE_PROXYTYPE, radius = 1)\n"
                 "    create capsule shape\n"
                 "        shape = igeBullet.shape(igeBullet.CAPSULE_SHAPE_PROXYTYPE, radius = 1, height = 1, axis = 1)\n"
                 "    create cone shape\n"
                 "        shape = igeBullet.shape(igeBullet.CONE_SHAPE_PROXYTYPE, radius = 1, height = 1, axis = 1)\n"
                 "    create cylinder shape\n"
                 "        shape = igeBullet.shape(igeBullet.CYLINDER_SHAPE_PROXYTYPE, halfExtents = (x, y, z), axis = 1)\n"
                 "    create static plane shape\n"
                 "        shape = igeBullet.shape(igeBullet.STATIC_PLANE_PROXYTYPE, normal = (x, y, z), constant = 1)\n"
                 "    create compound shape\n"
                 "        shape = igeBullet.shape(igeBullet.COMPOUND_SHAPE_PROXYTYPE)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "    type\n"
                 "        shape type\n"
                 "        igeBullet.BOX_SHAPE_PROXYTYPE : box shape\n"
                 "        igeBullet.SPHERE_SHAPE_PROXYTYPE : sphere shape\n"
                 "        igeBullet.CAPSULE_SHAPE_PROXYTYPE : capsule shape\n"
                 "        igeBullet.CONE_SHAPE_PROXYTYPE : cone shape\n"
                 "        igeBullet.CYLINDER_SHAPE_PROXYTYPE : cylinder shape\n"
                 "        igeBullet.STATIC_PLANE_PROXYTYPE static plane shape\n"
                 "        igeBullet.COMPOUND_SHAPE_PROXYTYPE compound shape\n"
                 "    radius\n"
                 "        radius of sphere or capsule or cone\n"
                 "    height\n"
                 "        height of capsule or cone\n"
                 "    halfExtents\n"
                 "        half length of each side x,y,z\n"
                 "    normal\n"
                 "        normal of plane\n"
                 "    constant\n"
                 "        plane position along normal\n"
                 "    axis\n"
                 "        the value indicating the vertical direction of the shape	\n"
                 "        0:X,  1:Y,  2:Z (default is Y) ");

    PyDoc_STRVAR(getMeshData_doc,
                 "get rendering data\n"
                 "\n"
                 "pos, nom, uv, idx = shape.getMeshData()\n"
                 "\n"
                 "Returns\n"
                 "-------\n"
                 "    pos\n"
                 "        list of vertex positions (x,y,z, x,y,z, ...)\n"
                 "    nom\n"
                 "        list of vertex normals (x,y,z, x,y,z, ...)\n"
                 "    uv\n"
                 "        list of vertex uvs (u,v, u,v, ...)\n"
                 "    idx\n"
                 "        list of triangle indices\n");

    PyDoc_STRVAR(addChildShape_doc,
                 "Use this method for adding children.\n"
                 "Only Convex shapes are allowed.\n"
                 "\n"
                 "shape.addChildShape(shape, pos, rot)\n"
                 "\n"
                 "Parameters\n"
                 "----------\n"
                 "    shape : igeBullet.shape\n"
                 "        shape object\n"
                 "    pos : tuple of float (x,y,z)\n"
                 "        offset location of object\n"
                 "    rot : tuple of float (x,y,z,w)\n"
                 "        offset orientation of object\n");

} // namespace ige::bullet
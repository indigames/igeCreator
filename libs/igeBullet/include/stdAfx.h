#pragma once

#include <Python.h>
#include <stdio.h>
#include <vector>

#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletDynamics/Featherstone/btMultiBodyConstraintSolver.h>

#include <BulletSoftBody/btDeformableMultiBodyDynamicsWorld.h>
#include <BulletSoftBody/btSoftBody.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#include <BulletSoftBody/btDeformableBodySolver.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>

#include "igeBullet.h"
#include "pyBulletUtils.h"

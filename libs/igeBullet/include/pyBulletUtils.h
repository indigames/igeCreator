#pragma once

#include <Python.h>

///Math library & Utils
#include <LinearMath/btQuaternion.h>
#include <LinearMath/btTransform.h>

namespace ige::bullet
{
	// Vec types
	extern PyTypeObject *Vec2Type;
	extern PyTypeObject *Vec3Type;
	extern PyTypeObject *Vec4Type;

	// Quat type
	extern PyTypeObject *QuatType;

	// Math types
	extern PyTypeObject *Mat22Type;
	extern PyTypeObject *Mat33Type;
	extern PyTypeObject *Mat44Type;

	typedef struct
	{
		PyObject_HEAD;
		float v[4];
		int d;
	} vec_obj;

	typedef struct
	{
		PyObject_HEAD;
		float m[16];
		int d;
	} mat_obj;

	typedef struct
	{
		PyObject_HEAD;
		float min[4];
		float max[4];
		int d;
	} aabb_obj;

} // namespace ige::bullet

inline bool almostEqual(float a, float b)
{
	return fabs(a - b) < 0.0001f;
}

class Vec3
{
public:
	float v[3];
	inline bool operator<(const Vec3 &l) const
	{
		for (int i = 0; i < 3; i++)
		{
			if (!almostEqual(v[i], l.v[i]))
				return (v[i] < l.v[i]);
		}
		return false;
	}
};

// Convert PyObject to btVector3
bool pyObjToVector(PyObject *obj, btVector3 &v);
int pyObjToVector3(PyObject *obj, btVector3 *f);
int pyObjToVec3(PyObject *obj, Vec3 *f);

// Convert PyObject to array
int pyObjToVector3FloatArray(PyObject *obj, float *f);
int pyObjToIntArray(PyObject *obj, int *idx);

// LengthSqr between two vectors
float lengthSqr(const btVector3 &v1, btVector3 &v2);

// Convert PyObject to btTransfrom
bool pyObjToTransform(PyObject *obj, btTransform &t);

// Get system time in miliseconds
uint64_t currentTimeMiliseconds();

// Create collision shape object
PyObject *createCollisionShapeGraphicsObject(btCollisionShape *collisionShape);

// Optimize mesh
void OptimizeMesh(const Vec3 *orgPoss, int numOrgPoss, int *indices, int numIndeces, float *&optPoss, int &numOptPoss, int *&orgMap);

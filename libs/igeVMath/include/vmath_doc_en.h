
PyDoc_STRVAR(vec2_doc,
	"a 2-D vector in array-of-structures format\n"\
	"\n"\
	"Constructors\n"\
	"----------\n"\
	"    igeVmath.vec2(float\n"\
	"    igeVmath.vec2(float,float)\n"\
	"    igeVmath.vec2((float,float))\n"\
	"    igeVmath.vec2([float,float])\n"\
	"    igeVmath.vec2(vec2)\n"\
	"    igeVmath.vec2(vec3)\n"\
	"    igeVmath.vec2(vec4)\n"\
	"        Missing elements will be zero\n"\
	"        Extra elements are ignored\n");


PyDoc_STRVAR(vec3_doc,
	"a 3-D vector in array-of-structures formatn\n"\
	"\n"\
	"Constructors\n"\
	"----------\n"\
	"    igeVmath.vec3(float, ...)\n"\
	"    igeVmath.vec3(float,...))\n"\
	"    igeVmath.vec3([float,...])\n"\
	"    igeVmath.vec3(vec2)\n"\
	"    igeVmath.vec3(vec3)\n"\
	"    igeVmath.vec3(vec4)\n"\
	"            Missing elements will be zero\n"\
	"            Extra elements are ignored\n"\
	"            Any other combination is possible\n");



PyDoc_STRVAR(vec4_doc, 
	"a 4-D vector in array-of-structures format\n"\
	"\n"\
	"Constructors\n"\
	"----------\n"\
	"    igeVmath.vec4(float, ...)\n"\
	"    igeVmath.vec4(float,...))\n"\
	"    igeVmath.vec4([float,...])\n"\
	"    igeVmath.vec4(vec2)\n"\
	"    igeVmath.vec4(vec3)\n"\
	"    igeVmath.vec4(vec4)\n"\
	"    igeVmath.vec4(quat)\n"\
	"            Missing elements will be zero\n"\
	"            Extra elements are ignored\n"\
	"            Any other combination is possible\n");

PyDoc_STRVAR(quat_doc,
	"a quaternion in array-of-structures format\n"\
	"\n"\
	"Constructors\n"\
	"----------\n"\
	"    igeVmath.quat(float, ...)\n"\
	"    igeVmath.quat(float,...))\n"\
	"    igeVmath.quat([float,...])\n"\
	"    igeVmath.quat(vec2)\n"\
	"    igeVmath.quat(vec3)\n"\
	"    igeVmath.quat(vec4)\n"\
	"    igeVmath.quat(quat)\n"\
	"            Missing elements will be zero\n"\
	"            Extra elements are ignored\n"\
	"            Any other combination is possible\n");


PyDoc_STRVAR(euler_doc,
	"a structure of auler angles\n"\
	"\n"\
	"Constructors\n"\
	"----------\n"\
	"	igeVmath.euler()\n"\
	"		all values are set 0\n"\
	"	igeVmath.euler(tilt, pan, roll)\n"\
	"	igeVmath.euler(euler)\n"\
	"	igeVmath.euler(quat))\n"\
	"		convert from quaternion\n"\
	"	igeVmath.euler(mat3)\n"\
	"		convert from 3x3 rotation matrix\n"\
	"Parameters\n"\
	"----------\n"\
	"	tilt : float\n"\
	"	pan : float\n"\
	"	roll : float\n"\
	"	euler : igeVmath.euler\n"\
	"	quat : igeVmath.quat\n"\
	"	mat3 : igeVmath.mat3\n");



PyDoc_STRVAR(mat22_doc, 
	"a 2x2 matrix in array-of-structures format\n"\
	"\n"\
	"Constructors\n"\
	"----------\n"\
	"    igeVmath.mat22(flaot, ...)\n"\
	"    igeVmath.mat22((), ())\n"\
	"    igeVmath.mat22([], [])\n"\
	"    igeVmath.mat22(vec2, vec2)\n"\
	"            Missing elements will be zero\n"\
	"            Extra elements are ignored\n"\
	"            Any other combination is possible\n");


PyDoc_STRVAR(mat33_doc, 
	"a 3x3 matrix in array-of-structures format\n"\
	"\n"\
	"Constructors\n"\
	"----------\n"\
	"    igeVmath.mat33(flaot, ...)\n"\
	"    igeVmath.mat33((), ...)\n"\
	"    igeVmath.mat33([], ...)\n"\
	"    igeVmath.mat33(vec3, ...)\n"\
	"            Missing elements will be zero\n"\
	"            Extra elements are ignored\n"\
	"            Any other combination is possible\n");

PyDoc_STRVAR(mat44_doc, 
	"a 4x4 matrix in array-of-structures format\n"\
	"\n"\
	"Constructors\n"\
	"----------\n"\
	"    igeVmath.mat44(flaot, ...)\n"\
	"    igeVmath.mat44((), ...)\n"\
	"    igeVmath.mat44([], ...)\n"\
	"    igeVmath.mat44(vec3, ...)\n"\
	"    igeVmath.mat44(vec4, ...)\n"\
	"            Missing elements will be zero but m[3][3] is 1.0\n"\
	"            Extra elements are ignored\n"\
	"            Any other combination is possible\n");

//add
PyDoc_STRVAR(add_doc,
	"add two vectors or two matrices\n"\
	"\n"\
	
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.add(vecN, vecM)\n"\
	"    matNN = igeVmath.add(matNN, matMM)\n"\
	"        N,M = 2 or 3 or 4\n"\
	"        type of return value is depend on first parameter type\n");


//sub
PyDoc_STRVAR(sub_doc,
	"sub two vectors or two matrices\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.sub(vecN, vecM)\n"\
	"    matNN = igeVmath.sub(matNN, matMM)\n"\
	"        N,M = 2 or 3 or 4\n"\
	"        type of return value is depend on first parameter type\n");

//mul
PyDoc_STRVAR(mul_doc,
	"multiply 2 elements  \n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.mul(vecN, float)\n"\
	"    matNN = igeVmath.mul(matNN, float)\n"\
	"    vecN = igeVmath.mul(matNN, vecN)\n"\
	"    matNN = igeVmath.mul(matNN, matNN)\n"\
	"    quat = igeVmath.mul(quat, quat)\n"\
	"    vecN = igeVmath.mul(vecN, vecN) (multiply per element)\n"\
	"        (N = 2 or 3 or 4)");


//div
PyDoc_STRVAR(div_doc,
	"division vector by a scalar\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.div(vecN, float)\n"\
	"    vecN = igeVmath.div(vecN, vecN) division vector per element\n"\
	"        (N = 2 or 3 or 4)");


//recip
PyDoc_STRVAR(recip_doc,
	"compute the reciprocal of a vector per element\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.recip(vecN)\n"\
	"        (N = 2 or 3 or 4)");

//sqrt
PyDoc_STRVAR(sqrt_doc,
	"compute the square root of a vector per element\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.sqrt(vecN)\n"\
	"        (N = 2 or 3 or 4)");

//rsqrt
PyDoc_STRVAR(rsqrt_doc,
	"compute the reciprocal square root of a vector per element\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.rsqrt(vecN)\n"\
	"        (N = 2 or 3 or 4)");

//abs
PyDoc_STRVAR(abs_doc,
	"compute the absolute value of a vector per element\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.abs(vecN)\n"\
	"        (N = 2 or 3 or 4)");

//max
PyDoc_STRVAR(max_doc,
	"maximum element of a vector\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    float = igeVmath.max(vecN)\n"\
	"        (N = 2 or 3 or 4)");

//min
PyDoc_STRVAR(min_doc,
	"minimum element of a vector\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    float = igeVmath.min(vecN)\n"\
	"        (N = 2 or 3 or 4)");

//maxElem
PyDoc_STRVAR(maxElem_doc,
	"maximum of two vectors per element\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.maxElem(vecN,vecN)\n"\
	"        (N = 2 or 3 or 4)");

//minElem
PyDoc_STRVAR(minElem_doc,
	"minimum of two vectors per element\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.minElem(vecN,vecN)\n"\
	"        (N = 2 or 3 or 4)");

//sum
PyDoc_STRVAR(sum_doc,
	"compute the sum of all elements of a vector\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    float = igeVmath.sum(vecN)\n"\
	"        (N = 2 or 3 or 4)");

//dot
PyDoc_STRVAR(dot_doc,
	"compute the dot product of two vectors\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    float = igeVmath.dot(vecN, vecN)\n"\
	"        (N = 2 or 3 or 4)");

//lengthSqr
PyDoc_STRVAR(lengthSqr_doc,
	"compute the square of the length of a vector\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    float = igeVmath.lengthSqr(vecN)\n"\
	"        (N = 2 or 3 or 4)");

//length
PyDoc_STRVAR(length_doc,
	"compute the length of a vector\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    float = igeVmath.length(vecN)\n"\
	"        (N = 2 or 3 or 4)");

//normalize
PyDoc_STRVAR(normalize_doc,
	"normalize a vector\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.normalize(vecN)\n"\
	"        (N = 2 or 3 or 4)");

//cross
PyDoc_STRVAR(cross_doc,
	"compute cross product of two vectors\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    float = igeVmath.cross(vec2, vec2)\n"\
	"    vec3 = igeVmath.cross(vec3, vec3)");

//lerp
PyDoc_STRVAR(lerp_doc,
	"linear interpolation between two vectors\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.lerp(t, vecN, vecN)  (0<= t <= 1)\n"\
	"        (N = 2 or 3 or 4)");

//slerp
PyDoc_STRVAR(slerp_doc,
	"spherical linear interpolation between two vectors\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.slerp(t, vecN, vecN)  (0<= t <= 1)\n"\
	"        (N = 2 or 3 or 4)");

//almostEqual
PyDoc_STRVAR(almostEqual_doc,
	"Compare two values and if the difference is \n"\
	"smaller than EPSILON, it will be considered identical\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vecN = igeVmath.almostEqual(vecN, vecN)\n"\
	"        (N = 2 or 3 or 4)");

//quat_rotation
PyDoc_STRVAR(quat_rotation_doc,
	"construct a quaternion\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    quat = igeVmath.quat_rotation(vec3, vec3)\n"\
	"        construct a quaternion to rotate between two unit - length 3D vectors\n"\
	"        the result is unpredictable if 2 vectors point in opposite directions\n"\
	"    quat = igeVmath.quat_rotation(float, vec3)\n"\
	"        construct a quaternion to rotate around a unit-length 3D vector\n"\
	"    quat = igeVmath.quat_rotation(float)\n"\
	"        construct a quaternion to rotate around a Z(0,0,1) axis");

//quat_rotationX
PyDoc_STRVAR(quat_rotationX_doc,
	"construct a quaternion to rotate around the x axis\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    quat = igeVmath.quat_rotationX(radian)");

//quat_rotationY
PyDoc_STRVAR(quat_rotationY_doc,
	"construct a quaternion to rotate around the y axis\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    quat = igeVmath.quat_rotationY(radian)");

//quat_rotationZ
PyDoc_STRVAR(quat_rotationZ_doc,
	"construct a quaternion to rotate around the z axis\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    quat = igeVmath.quat_rotationZ(radian)");

PyDoc_STRVAR(quat_rotationZYX_doc,
	"construct a quaternion to rotate around the x, y, and z axes\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    quat = igeVmath.quat_rotationZYX( (xradian, yradian, zradian) )");


//conj
PyDoc_STRVAR(conj_doc,
	"compute the conjugate of a quaternion\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    quat = igeVmath.conj(quat)");

//squad
PyDoc_STRVAR(squad_doc,
	"spherical quadrangle interpolation\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    quat = igeVmath.squad(t,quat, quat, quat, quat)");

//rotate
PyDoc_STRVAR(rotate_doc,
	"use a unit - length quaternion to rotate a 3D vector\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    vec = igeVmath.rotate(vec, quat)");

//mat_rotation
PyDoc_STRVAR(mat_rotation_doc,
	"construct a matrix to rotate around a unit-length 3D vector\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix = igeVmath.mat_rotation(radian, dimension, vector)\n"\
	"        dimension is 2 or 3 or 4 to output matrix\n"\
	"        if you omit vector, Zaxis(0,0,1) will be entered as default");

//mat_rotationX
PyDoc_STRVAR(mat_rotationX_doc,
	"construct a matrix to rotate around the Xaxis\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix = igeVmath.mat_rotationX(radian, dimension)\n"\
	"		radian : float\n"\
	"		dimension : int\n"\
	"			dimension of output matrix (2,3,4)");

//mat_rotationY
PyDoc_STRVAR(mat_rotationY_doc,
	"construct a matrix to rotate around the Yaxis\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix = igeVmath.mat_rotationY(radian, dimension)\n"\
	"		radian : float\n"\
	"		dimension : int\n"\
	"			dimension of output matrix (2,3,4)");

//mat_rotationZ
PyDoc_STRVAR(mat_rotationZ_doc,
	"construct a matrix to rotate around the Zaxis\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"	matrix = igeVmath.mat_rotationZ(radian, dimension)\n"\
	"		radian : float\n"\
	"		dimension : int\n"\
	"			dimension of output matrix (2,3,4)");

//mat_rotationZYX
PyDoc_STRVAR(mat_rotationZYX_doc,
	"construct a matrix to rotate around the x, y, and z axes\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"	matrix = igeVmath.mat_rotationZYX(dimension, (xradian, yradian, zradian) )"\
	"		dimension : int\n"\
	"			dimention of output matrix (2,3,4)\n"\
	"		xradian:float\n"\
	"		yradian:float\n"\
	"		zradian:float\n");

//mat_identity
PyDoc_STRVAR(mat_identity_doc,
	"construct an identity matrix\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix = igeVmath.mat_identity(dimension)\n"\
	"		dimension : int\n"\
	"			dimention of output matrix (2,3,4)\n");

//mat_scale
PyDoc_STRVAR(mat_scale_doc,
	"construct a matrix to perform scaling\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix = igeVmath.mat_scale(dimension, vector)\n"\
	"        dimension is 2 or 3 or 4 to output matrix");

//mat_translation
PyDoc_STRVAR(mat_translation_doc,
	"construct a 4x4 matrix to perform translation\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix = igeVmath.mat_translation(vector)");

//transpose
PyDoc_STRVAR(transpose_doc,
	"transpose of a matrix\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix = igeVmath.transpose(matrix)");

//inverse
PyDoc_STRVAR(inverse_doc,
	"compute the inverse of a matrix or quaternion\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix = igeVmath.inverse(matrix)\n"\
	"    quat = igeVmath.inverse(quat)");


//orthoInverse
PyDoc_STRVAR(orthoInverse_doc,
	"compute the inverse of a 4x4 matrix, \n"\
	"which is expected to be an affine matrix with an orthogonal upper-left 3x3 submatrix\n"\
	"this can be used to achieve better performance than a general inverse\n"\
	"when the specified 4x4 matrix meets the given restrictions\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix = igeVmath.orthoInverse(matrix)");


//determinant
PyDoc_STRVAR(determinant_doc,
	"determinant of a matrix\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    scalar = igeVmath.determinant(matrix)");

//appendScale
PyDoc_STRVAR(appendScale_doc,
	"append (post-multiply) a scale transformation to a matrix\n"\
	"faster than creating and multiplying a scale transformation matrix\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix = igeVmath.appendScale(matrix, vector)");

//prependScale
PyDoc_STRVAR(prependScale_doc,
	"prepend (pre-multiply) a scale transformation to a 4x4 matrix\n"\
	"faster than creating and multiplying a scale transformation matrix\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix = igeVmath.prependScale(matrix, vector)");

//lookAt
PyDoc_STRVAR(lookAt_doc,
	"construct viewing matrix based on eye position, position looked at, and up direction\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix4 = igeVmath.lookAt(eyeVector, lookatVector, upVector)");

//perspective
PyDoc_STRVAR(perspective_doc,
	"construct a perspective projection matrix\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix4 = igeVmath.perspective(fovyRadians, aspect, zNear, zFar)");

//frustum
PyDoc_STRVAR(frustum_doc,
	"construct a perspective projection matrix based on frustum\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix4 = igeVmath.frustum(left, right, bottom, top, zNear, zFar)  (all scalar value)");

//orthographic
PyDoc_STRVAR(orthographic_doc,
	"construct an orthographic projection matrix\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"    matrix4 = igeVmath.orthographic(left, right, bottom, top, zNear, zFar)  (all scalar value)");


//setElem
PyDoc_STRVAR(vec_setElem_doc,
	"Set a value using an index\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"	obj.setElem(index, value)\n"\
	"		obj : vec2 vec3 vec4 euler\n"\
	"		index : int\n"\
	"		value : float\n");

//getElem
PyDoc_STRVAR(vec_getElem_doc,
	"Get a value using an index\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"	value = obj.getElem(index)\n"\
	"		obj : vec2 vec3 vec4 euler\n"\
	"		index : int\n"\
	"		value : float\n");

PyDoc_STRVAR(mat_setElem_doc,
	"Set a value using an index\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"	obj.setElem(colmn, row, value)\n"\
	"		obj : mat2 mat3 mat4\n"\
	"		colmn : int\n"\
	"		row : int\n"\
	"		value : float\n");

PyDoc_STRVAR(mat_getElem_doc,
	"Get a value using an index\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"	value = obj.getElem(colmn, row)\n"\
	"		obj : mat2 mat3 mat4\n"\
	"		colmn : int\n"\
	"		row : int\n"\
	"		value : float\n");

PyDoc_STRVAR(mat_getCol_doc,
	"Returns the vector of column corresponding to the specified row.\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"	vec = obj.getCol(row)\n"\
	"		vec : vec2,vec3,vec4\n"\
	"		obj : mat2,mat3,mat4\n"\
	"		row : int\n");

PyDoc_STRVAR(mat_getRow_doc,
	"Returns the vector of row corresponding to the specified column.\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"	vec = obj.getRow(row)\n"\
	"		vec : vec2,vec3,vec4\n"\
	"		obj : mat2,mat3,mat4\n"\
	"		column : int\n");

PyDoc_STRVAR(mat_setCol_doc,
	"Set the column corresponding to the specified row.\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"	obj.setCol(row, value)\n"\
	"		obj : mat2,mat3,mat4\n"\
	"		row : int\n"\
	"		value : vec2,vec3,vec4\n");

PyDoc_STRVAR(mat_setRow_doc,
	"Set the row corresponding to the specified column.\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"	obj.setCol(column, value)\n"\
	"		obj : mat2,mat3,mat4\n"\
	"		column : int\n"\
	"		value : vec2,vec3,vec4\n");

PyDoc_STRVAR(toQuat_doc,
	"Compute the quaternion corresponding to Euler angles.\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"	obj.toQuat()\n");


//mat_transform
PyDoc_STRVAR(mat_transform_doc,
	"construct a 4x4 matrix from vector of position, rotation, scale and matrix of shear\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"	matrix = igeVmath.mat_transform(position, rotation, scale, shear)\n"\
	"		position : igeVmath.vec3  (or (...), [...])\n"\
	"		rotation : igeVmath.euler or igeVmath.quat (or (...), [...])\n"\
	"			Rotateion can be specified in either Euler angles or quaternions.\n"\
	"			In the case of tuples and lists, if the number of elements is \n"\
	"			three it is interpreted as Euler angles, and if it is four it is \n"\
	"			interpreted as a quaternion.\n"\
	"		scale : igeVmath.vec3  (or (...), [...])\n"\
	"		shear : igeVmath.mat44 (optional)\n");

//mat_getTransform
PyDoc_STRVAR(mat_getTransform_doc,
	"construct a 4x4 matrix from vector of position, rotation, scale and matrix of shear\n"\
	"\n"\
	"Forms\n"\
	"----------\n"\
	"	position, rotation, scale, shear = mat4.getTransform(rotationInEuler)\n"\
	"		rotationInEuler : bool\n"\
	"			Retriave the rotation in Euler angles. False, quaternion.\n"\
	"		position : igeVmath.vec3\n"\
	"		rotation : igeVmath.vec3\n"\
	"		scale : igeVmath.vec3\n"\
	"		shear : igeVmath.mat44\n");

PyDoc_STRVAR(aabb_doc,
	"Axis aligned bounding box.\n");

PyDoc_STRVAR(minEdge_doc,
	"");

PyDoc_STRVAR(maxEdge_doc,
	"");

PyDoc_STRVAR(center_doc,
	"");

PyDoc_STRVAR(extent_doc,
	"");

PyDoc_STRVAR(volume_doc,
	"");

PyDoc_STRVAR(area_doc,
	"");

PyDoc_STRVAR(aabb_lengthSqr_doc,
	"");

PyDoc_STRVAR(reset_doc,
	"");

PyDoc_STRVAR(insert_doc,
	"");

PyDoc_STRVAR(repair_doc,
	"");

PyDoc_STRVAR(isInside_doc,
	"");

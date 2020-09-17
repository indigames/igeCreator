/* SCE CONFIDENTIAL
PlayStation(R)3 Programmer Tool Runtime Library 310.001
* Copyright (C) 2008 Sony Computer Entertainment Inc.
* All Rights Reserved.
*/

#ifndef _VECTORMATH_MAT_AOS_CPP_H
#define _VECTORMATH_MAT_AOS_CPP_H

namespace Vectormath {
namespace Aos {

//-----------------------------------------------------------------------------
// Constants

#define _VECTORMATH_PI_OVER_2 1.570796327f

//-----------------------------------------------------------------------------
// Definitions

inline Matrix3::Matrix3(const Matrix3& mat)
{
    mCol0=mat.mCol0;
    mCol1=mat.mCol1;
    mCol2=mat.mCol2;
}

inline Matrix3::Matrix3(float scalar)
{
	Vector3 vs=Vector3(scalar);
    mCol0=vs;
    mCol1=vs;
    mCol2=vs;
}

inline Matrix3::Matrix3(const Quat& unitQuat)
{
#if 1
	//TODO:optimize
    float qx=unitQuat.getX();
    float qy=unitQuat.getY();
    float qz=unitQuat.getZ();
    float qw=unitQuat.getW();
    float qx2=qx+qx;
    float qy2=qy+qy;
    float qz2=qz+qz;
    float qxqx2=qx*qx2;
    float qxqy2=qx*qy2;
    float qxqz2=qx*qz2;
    float qxqw2=qw*qx2;
    float qyqy2=qy*qy2;
    float qyqz2=qy*qz2;
    float qyqw2=qw*qy2;
    float qzqz2=qz*qz2;
    float qzqw2=qw*qz2;
    mCol0=Vector3(1.f-qyqy2-qzqz2,qxqy2+qzqw2,qxqz2-qyqw2);
    mCol1=Vector3(qxqy2-qzqw2,1.f-qxqx2-qzqz2,qyqz2+qxqw2);
    mCol2=Vector3(qxqz2+qyqw2,qyqz2-qxqw2,1.f-qxqx2-qyqy2);
#else
	float32x4_t q=unitQuat.get128(); //qxyzw
	float32x4_t qxyz2=vaddq_f32(q,q); //qx*2,y*2,z*2
	float32x4_t qxxyz2=vmulq_n_f32(qxyz2,vgetq_lane_f32(q,0)); //qx*(qx*2,y*2,z*2)
	//...
#endif
}

inline Matrix3::Matrix3(const Vector3& _col0,const Vector3& _col1,const Vector3& _col2)
{
    mCol0=_col0;
    mCol1=_col1;
    mCol2=_col2;
}

inline Matrix3& Matrix3::setCol0(const Vector3& _col0)
{
    mCol0=_col0;
    return *this;
}

inline Matrix3& Matrix3::setCol1(const Vector3& _col1)
{
    mCol1=_col1;
    return *this;
}

inline Matrix3& Matrix3::setCol2(const Vector3& _col2)
{
    mCol2=_col2;
    return *this;
}

inline Matrix3& Matrix3::setCol(int col,const Vector3& vec)
{
    *(&mCol0+col)=vec;
    return *this;
}

inline Matrix3 & Matrix3::setRow(int row,const Vector3& vec)
{
    mCol0.setElem(row,vec.getElem(0));
    mCol1.setElem(row,vec.getElem(1));
    mCol2.setElem(row,vec.getElem(2));
    return *this;
}

inline Matrix3 & Matrix3::setElem(int col,int row,float val)
{
    Vector3 tmpV3_0;
    tmpV3_0=this->getCol(col);
    tmpV3_0.setElem(row,val);
    this->setCol(col,tmpV3_0);
    return *this;
}

inline float Matrix3::getElem(int col,int row) const
{
    return this->getCol(col).getElem(row);
}

inline const Vector3 Matrix3::getCol0() const
{
    return mCol0;
}

inline const Vector3 Matrix3::getCol1() const
{
    return mCol1;
}

inline const Vector3 Matrix3::getCol2() const
{
    return mCol2;
}

inline const Vector3 Matrix3::getCol(int col) const
{
    return *(&mCol0+col);
}

inline const Vector3 Matrix3::getRow(int row) const
{
    return Vector3(mCol0.getElem(row),mCol1.getElem(row),mCol2.getElem(row));
}

inline Vector3& Matrix3::operator[](int col)
{
    return *(&mCol0+col);
}

inline const Vector3 Matrix3::operator[](int col) const
{
    return *(&mCol0+col);
}

inline Matrix3& Matrix3::operator=(const Matrix3& mat)
{
    mCol0=mat.mCol0;
    mCol1=mat.mCol1;
    mCol2=mat.mCol2;
    return *this;
}

inline const Matrix3 transpose(const Matrix3& mat)
{
	float32x4_t r0=mat.getCol0().get128();
	float32x4_t r1=mat.getCol1().get128();
	float32x4_t r2=mat.getCol2().get128();
	float32x4_t r3;
	float32x2x2_t tr1=vtrn_f32(vget_low_f32(r0),vget_low_f32(r1));
	float32x2x2_t tr2=vtrn_f32(vget_low_f32(r2),vget_low_f32(r3));
	float32x2x2_t tr3=vtrn_f32(vget_high_f32(r0),vget_high_f32(r1));
	return Matrix3(Vector3(vcombine_f32(tr1.val[0],tr2.val[0])),
				   Vector3(vcombine_f32(tr1.val[1],tr2.val[1])),
				   Vector3(vcombine_f32(tr3.val[0],vget_high_f32(r2))));
}

inline const Matrix3 inverse(const Matrix3& mat)
{
    Vector3 tmp0=cross(mat.getCol1(),mat.getCol2());
    Vector3 tmp1=cross(mat.getCol2(),mat.getCol0());
    Vector3 tmp2=cross(mat.getCol0(),mat.getCol1());
	float32x2_t det=vdup_n_f32(dot(mat.getCol2(),tmp2));
	float32x2_t detinv=vrecpe_f32(det);
	detinv=vmul_f32(vrecps_f32(det,detinv),detinv);
	float32x4_t detinvd=vcombine_f32(detinv,detinv);
	Matrix3 m(tmp0,tmp1,tmp2);
	m=transpose(m);
	return Matrix3(Vector3(vmulq_f32(m.getCol0().get128(),detinvd)),
				   Vector3(vmulq_f32(m.getCol1().get128(),detinvd)),
				   Vector3(vmulq_f32(m.getCol2().get128(),detinvd)));
}

inline float determinant(const Matrix3& mat)
{
    return dot(mat.getCol2(),cross( mat.getCol0(),mat.getCol1()));
}

inline const Matrix3 Matrix3::operator+(const Matrix3& mat) const
{
    return Matrix3((mCol0+mat.mCol0),
				   (mCol1+mat.mCol1),
				   (mCol2+mat.mCol2));
}

inline const Matrix3 Matrix3::operator-(const Matrix3& mat) const
{
    return Matrix3((mCol0-mat.mCol0),
				   (mCol1-mat.mCol1),
				   (mCol2-mat.mCol2));
}

inline Matrix3& Matrix3::operator+=(const Matrix3& mat)
{
    *this=*this+mat;
    return *this;
}

inline Matrix3& Matrix3::operator-=(const Matrix3& mat)
{
    *this=*this-mat;
    return *this;
}

inline const Matrix3 Matrix3::operator-() const
{
    return Matrix3(-mCol0,-mCol1,-mCol2);
}

inline const Matrix3 absPerElem(const Matrix3& mat)
{
    return Matrix3(absPerElem(mat.getCol0()),absPerElem(mat.getCol1()),absPerElem(mat.getCol2()));
}

inline const Matrix3 Matrix3::operator*(float scalar) const
{
    return Matrix3(mCol0*scalar,mCol1*scalar,mCol2*scalar);
}

inline Matrix3& Matrix3::operator*=(float scalar)
{
    *this=*this*scalar;
    return *this;
}

inline const Matrix3 operator*(float scalar,const Matrix3& mat)
{
    return mat*scalar;
}

inline const Vector3 Matrix3::operator*(const Vector3& vec) const
{
	float32x4_t v=vmulq_n_f32(mCol0.get128(),vec.getX());
	v=vmlaq_n_f32(v,mCol1.get128(),vec.getY());
	v=vmlaq_n_f32(v,mCol2.get128(),vec.getZ());
	return Vector3(v);
}

inline const Matrix3 Matrix3::operator*(const Matrix3& mat) const
{
    return Matrix3(*this*mat.mCol0,*this*mat.mCol1,*this*mat.mCol2);
}

inline Matrix3& Matrix3::operator*=(const Matrix3& mat)
{
    *this=*this*mat;
    return *this;
}

inline const Matrix3 mulPerElem(const Matrix3& mat0,const Matrix3& mat1)
{
    return Matrix3(mulPerElem(mat0.getCol0(),mat1.getCol0()),
				   mulPerElem(mat0.getCol1(),mat1.getCol1()),
				   mulPerElem(mat0.getCol2(),mat1.getCol2()));
}

inline const Matrix3 Matrix3::identity()
{
    return Matrix3(Vector3::xAxis(),Vector3::yAxis(),Vector3::zAxis());
}

inline const Matrix3 Matrix3::rotationX(float radians)
{
    float r[2];
	neon_sincosf(radians,r);
    return Matrix3(Vector3::xAxis(),Vector3(0.f,r[1],r[0]),Vector3(0.0f,-r[0],r[1]));
}

inline const Matrix3 Matrix3::rotationY(float radians)
{
    float r[2];
	neon_sincosf(radians,r);
    return Matrix3(Vector3(r[1],0.f,-r[0]),Vector3::yAxis(),Vector3(r[0],0.f,r[1]));
}

inline const Matrix3 Matrix3::rotationZ(float radians)
{
    float r[2];
    neon_sincosf(radians,r);
	return Matrix3(Vector3(r[1],r[0],0.f),Vector3(-r[0],r[1],0.f),Vector3::zAxis());
}

inline const Matrix3 Matrix3::rotationZYX(const Vector3& radiansXYZ)
{
	float32x4_t s,c;
	neon_vsincosf(radiansXYZ.get128(),&s,&c);
	float32_t ss[4] __attribute((aligned(16)));
	float32_t sc[4] __attribute((aligned(16)));
	vst1q_f32(ss,s);
	vst1q_f32(sc,c);
    float tmp0=sc[2]*ss[1];
	float tmp1=ss[2]*ss[1];
	return Matrix3(Vector3(sc[2]*sc[1],ss[2]*sc[1],-ss[1]),
				   Vector3(tmp0*ss[0]-ss[2]*sc[0],tmp1*ss[0]+sc[2]*sc[0],sc[1]*ss[0]),
				   Vector3(tmp0*sc[0]+ss[2]*ss[0],tmp1*sc[0]-sc[2]*ss[0],sc[1]*sc[0]));
}

inline const Matrix3 Matrix3::rotation(float radians,const Vector3& unitVec)
{
	float r[2];
	neon_sincosf(radians,r);
    float x=unitVec.getX();
    float y=unitVec.getY();
    float z=unitVec.getZ();
    float xy=x*y;
    float yz=y*z;
    float zx=z*x;
	float oneMinusC=1.f-r[1];
	return Matrix3(Vector3(x*x*oneMinusC+r[1],xy*oneMinusC+z*r[0],zx*oneMinusC-y*r[0]),
				   Vector3(xy*oneMinusC-z*r[0],y*y*oneMinusC+r[1],yz*oneMinusC+x*r[0]),
				   Vector3(zx*oneMinusC+y*r[0],yz*oneMinusC-x*r[0],z*z*oneMinusC+r[1]));
}

inline const Matrix3 Matrix3::rotation(const Quat& unitQuat)
{
    return Matrix3(unitQuat);
}

inline const Matrix3 Matrix3::scale(const Vector3& scaleVec)
{
    return Matrix3(Vector3(scaleVec.getX(),0.f,0.f),
				   Vector3(0.f,scaleVec.getY(),0.f),
				   Vector3(0.f,0.f,scaleVec.getZ()));
}

inline const Matrix3 appendScale(const Matrix3& mat,const Vector3& scaleVec)
{
    return Matrix3(mat.getCol0()*scaleVec.getX(),mat.getCol1()*scaleVec.getY(),mat.getCol2()*scaleVec.getZ());
}

inline const Matrix3 prependScale(const Vector3& scaleVec,const Matrix3& mat)
{
    return Matrix3(mulPerElem(mat.getCol0(),scaleVec),
				   mulPerElem(mat.getCol1(),scaleVec),
				   mulPerElem(mat.getCol2(),scaleVec));
}

inline const Matrix3 select(const Matrix3& mat0,const Matrix3& mat1,bool select1)
{
    return Matrix3(select(mat0.getCol0(),mat1.getCol0(),select1),
				   select(mat0.getCol1(),mat1.getCol1(),select1),
				   select(mat0.getCol2(),mat1.getCol2(),select1));
}

#ifdef _VECTORMATH_DEBUG

inline void print(const Matrix3& mat)
{
    print(mat.getRow(0));
    print(mat.getRow(1));
    print(mat.getRow(2));
}

inline void print(const Matrix3& mat,const char* name)
{
    printf("%s:\n",name);
    print(mat);
}

#endif

inline Matrix4::Matrix4(const Matrix4& mat)
{
    mCol0=mat.mCol0;
    mCol1=mat.mCol1;
    mCol2=mat.mCol2;
    mCol3=mat.mCol3;
}

inline Matrix4::Matrix4(float scalar)
{
	Vector4 v(scalar);
    mCol0=v;
    mCol1=v;
    mCol2=v;
    mCol3=v;
}

inline Matrix4::Matrix4(const Transform3& mat)
{
    mCol0=Vector4(mat.getCol0(),0.f);
    mCol1=Vector4(mat.getCol1(),0.f);
    mCol2=Vector4(mat.getCol2(),0.f);
    mCol3=Vector4(mat.getCol3(),1.f);
}

inline Matrix4::Matrix4(const Vector4& _col0,const Vector4& _col1,const Vector4& _col2,const Vector4& _col3)
{
    mCol0=_col0;
    mCol1=_col1;
    mCol2=_col2;
    mCol3=_col3;
}

inline Matrix4::Matrix4(const Matrix3& mat,const Vector3& translateVec)
{
    mCol0=Vector4(mat.getCol0(),0.f);
    mCol1=Vector4(mat.getCol1(),0.f);
    mCol2=Vector4(mat.getCol2(),0.f);
    mCol3=Vector4(translateVec,1.f);
}

inline Matrix4::Matrix4(const Quat& unitQuat,const Vector3& translateVec)
{
    Matrix3 mat=Matrix3(unitQuat);
    mCol0=Vector4(mat.getCol0(),0.f);
    mCol1=Vector4(mat.getCol1(),0.f);
    mCol2=Vector4(mat.getCol2(),0.f);
    mCol3=Vector4(translateVec,1.f);
}

inline Matrix4& Matrix4::setCol0(const Vector4& _col0)
{
    mCol0=_col0;
    return *this;
}

inline Matrix4& Matrix4::setCol1(const Vector4& _col1)
{
    mCol1=_col1;
    return *this;
}

inline Matrix4& Matrix4::setCol2(const Vector4& _col2)
{
    mCol2=_col2;
    return *this;
}

inline Matrix4& Matrix4::setCol3(const Vector4& _col3)
{
    mCol3=_col3;
    return *this;
}

inline Matrix4& Matrix4::setCol(int col,const Vector4& vec)
{
    *(&mCol0+col)=vec;
    return *this;
}

inline Matrix4& Matrix4::setRow(int row,const Vector4& vec)
{
    mCol0.setElem(row,vec.getElem(0));
    mCol1.setElem(row,vec.getElem(1));
    mCol2.setElem(row,vec.getElem(2));
    mCol3.setElem(row,vec.getElem(3));
    return *this;
}

inline Matrix4& Matrix4::setElem(int col,int row,float val)
{
    Vector4 tmpV3_0=this->getCol(col);
    tmpV3_0.setElem(row,val);
    this->setCol(col,tmpV3_0);
    return *this;
}

inline float Matrix4::getElem(int col,int row) const
{
    return this->getCol(col).getElem(row);
}

inline const Vector4 Matrix4::getCol0() const
{
    return mCol0;
}

inline const Vector4 Matrix4::getCol1() const
{
    return mCol1;
}

inline const Vector4 Matrix4::getCol2() const
{
    return mCol2;
}

inline const Vector4 Matrix4::getCol3() const
{
    return mCol3;
}

inline const Vector4 Matrix4::getCol(int col) const
{
    return *(&mCol0+col);
}

inline const Vector4 Matrix4::getRow(int row) const
{
    return Vector4(mCol0.getElem(row),mCol1.getElem(row),mCol2.getElem(row),mCol3.getElem(row));
}

inline Vector4& Matrix4::operator[](int col)
{
    return *(&mCol0+col);
}

inline const Vector4 Matrix4::operator[](int col) const
{
    return *(&mCol0+col);
}

inline Matrix4& Matrix4::operator=(const Matrix4& mat)
{
    mCol0=mat.mCol0;
    mCol1=mat.mCol1;
    mCol2=mat.mCol2;
    mCol3=mat.mCol3;
    return *this;
}

inline const Matrix4 transpose(const Matrix4& mat)
{
	float32x4_t r1=mat.getCol0().get128();
	float32x4_t r2=mat.getCol1().get128();
	float32x4_t r3=mat.getCol2().get128();
	float32x4_t r4=mat.getCol3().get128();
	float32x2x2_t tr1=vtrn_f32(vget_low_f32(r1),vget_low_f32(r2));
	float32x2x2_t tr2=vtrn_f32(vget_high_f32(r1),vget_high_f32(r2));
	float32x2x2_t tr3=vtrn_f32(vget_low_f32(r3),vget_low_f32(r4));
	float32x2x2_t tr4=vtrn_f32(vget_high_f32(r3),vget_high_f32(r4));
	return Matrix4(Vector4(vcombine_f32(tr1.val[0],tr3.val[0])),
				   Vector4(vcombine_f32(tr1.val[1],tr3.val[1])),
				   Vector4(vcombine_f32(tr2.val[0],tr4.val[0])),
				   Vector4(vcombine_f32(tr2.val[1],tr4.val[1])));
}

inline const Matrix4 inverse(const Matrix4& mat)
{
	float32x4_t mABCD=mat.getCol0().get128();
	float32x4_t mEFGH=mat.getCol1().get128();
	float32x4_t mIJKL=mat.getCol2().get128();
	float32x4_t mMNOP=mat.getCol3().get128();
	float32x2_t mCD=vget_high_f32(mABCD);
	float32x2_t mGH=vget_high_f32(mEFGH);
	float32x2x2_t trCDGH=vtrn_f32(mCD,mGH);
	float32x2_t mCG=trCDGH.val[0];
	float32x2_t mDH=trCDGH.val[1];
	float32x2_t mKL=vget_high_f32(mIJKL);
	float32x2_t mOP=vget_high_f32(mMNOP);
	float32x2x2_t trKLOP=vtrn_f32(mKL,mOP);
	float32x2_t mKO=trKLOP.val[0];
	float32x2_t mLP=trKLOP.val[1];
	float32x2_t tmp01=vmul_f32(mKO,mDH);
	tmp01=vmls_f32(tmp01,mCG,mLP);
	float32x2_t mAB=vget_low_f32(mABCD);
	float32x2_t mEF=vget_low_f32(mEFGH);
	float32x2x2_t trABEF=vtrn_f32(mAB,mEF);
	float32x2_t mAE=trABEF.val[0];
	float32x2_t mBF=trABEF.val[1];
	float32x2_t mIJ=vget_low_f32(mIJKL);
	float32x2_t mMN=vget_low_f32(mMNOP);
	float32x2x2_t trIJMN=vtrn_f32(mIJ,mMN);
	float32x2_t mIM=trIJMN.val[0];
	float32x2_t mJN=trIJMN.val[1];
	float32x2_t tmp23=vmul_f32(mBF,mKO);
	tmp23=vmls_f32(tmp23,mJN,mCG);
	float32x2_t tmp45=vmul_f32(mJN,mDH);
	tmp45=vmls_f32(tmp45,mBF,mLP);
	float32x2_t tmp10=vext_f32(tmp01,tmp01,1);
	float32x2_t tmp32=vext_f32(tmp23,tmp23,1);
	float32x2_t tmp54=vext_f32(tmp45,tmp45,1);
	float32x2_t res0xy=vmul_f32(mJN,tmp10);
	float32x2_t res0zw=vmul_f32(mDH,tmp32);
	res0xy=vmls_f32(res0xy,mLP,tmp32);
	res0zw=vmla_f32(res0zw,mCG,tmp54);
	res0xy=vmls_f32(res0xy,mKO,tmp54);
	res0zw=vmls_f32(res0zw,mBF,tmp10);
	float32x2_t det=vmul_f32(mAE,res0xy);
	det=vmla_f32(det,mIM,res0zw);
	det=vpadd_f32(det,det);
	float32x2_t detinv=vrecpe_f32(det);
	detinv=vmul_f32(vrecps_f32(det,detinv),detinv);
	float32x2_t res1xy=vmul_f32(mIM,tmp10);
	float32x2_t res1zw=vmul_f32(mAE,tmp10);
	float32x2_t res3xy=vmul_f32(mIM,tmp32);
	float32x2_t res3zw=vmul_f32(mAE,tmp32);
	float32x2_t res2xy=vmul_f32(mIM,tmp54);
	float32x2_t res2zw=vmul_f32(mAE,tmp54);
	tmp01=vmul_f32(mIM,mBF);
	tmp01=vmls_f32(tmp01,mAE,mJN);
	tmp23=vmul_f32(mIM,mDH);
	tmp23=vmls_f32(tmp23,mAE,mLP);
	tmp45=vmul_f32(mIM,mCG);
	tmp45=vmls_f32(tmp45,mAE,mKO);
	tmp10=vext_f32(tmp01,tmp01,1);
	tmp32=vext_f32(tmp23,tmp23,1);
	tmp54=vext_f32(tmp45,tmp45,1);
	res2zw=vneg_f32(res2zw);
	res2xy=vmla_f32(res2xy,mLP,tmp10);
	res2zw=vmla_f32(res2zw,mBF,tmp32);
	res2xy=vmls_f32(res2xy,mJN,tmp32);
	res2zw=vmls_f32(res2zw,mDH,tmp10);
	res3zw=vneg_f32(res3zw);
	res3xy=vmla_f32(res3xy,mJN,tmp54);
	res3zw=vmla_f32(res3zw,mCG,tmp10);
	res3xy=vmls_f32(res3xy,mKO,tmp10);
	res3zw=vmls_f32(res3zw,mBF,tmp54);
	res1xy=vneg_f32(res1xy);
	res1zw=vmla_f32(res1zw,mDH,tmp54);
	res1xy=vmla_f32(res1xy,mKO,tmp32);
	res1zw=vmls_f32(res1zw,mCG,tmp32);
	res1xy=vmls_f32(res1xy,mLP,tmp54);
	float32x4_t res0=vcombine_f32(res0xy,res0zw);
	float32x4_t res1=vcombine_f32(res1xy,res1zw);
	float32x4_t res2=vcombine_f32(res2xy,res2zw);
	float32x4_t res3=vcombine_f32(res3xy,res3zw);
	float32x4_t detinvd=vcombine_f32(detinv,detinv);
	res0=vmulq_f32(res0,detinvd);
	res1=vmulq_f32(res1,detinvd);
	res2=vmulq_f32(res2,detinvd);
	res3=vmulq_f32(res3,detinvd);
	return Matrix4(Vector4(res0),Vector4(res1),Vector4(res2),Vector4(res3));
}

inline const Matrix4 affineInverse(const Matrix4& mat)
{
    Transform3 affineMat;
    affineMat.setCol0(mat.getCol0().getXYZ());
    affineMat.setCol1(mat.getCol1().getXYZ());
    affineMat.setCol2(mat.getCol2().getXYZ());
    affineMat.setCol3(mat.getCol3().getXYZ());
    return Matrix4(inverse(affineMat));
}

inline const Matrix4 orthoInverse(const Matrix4& mat)
{
    Transform3 affineMat;
    affineMat.setCol0(mat.getCol0().getXYZ());
    affineMat.setCol1(mat.getCol1().getXYZ());
    affineMat.setCol2(mat.getCol2().getXYZ());
    affineMat.setCol3(mat.getCol3().getXYZ());
    return Matrix4(orthoInverse(affineMat));
}

inline float determinant(const Matrix4& mat)
{
	float32x4_t mABCD=mat.getCol0().get128();
	float32x4_t mEFGH=mat.getCol1().get128();
	float32x4_t mIJKL=mat.getCol2().get128();
	float32x4_t mMNOP=mat.getCol3().get128();
	float32x2_t mAB=vget_low_f32(mABCD);
	float32x2_t mCD=vget_high_f32(mABCD);
	float32x2_t mEF=vget_low_f32(mEFGH);
	float32x2_t mGH=vget_high_f32(mEFGH);
	float32x2_t mIJ=vget_low_f32(mIJKL);
	float32x2_t mKL=vget_high_f32(mIJKL);
	float32x2_t mMN=vget_low_f32(mMNOP);
	float32x2_t mOP=vget_high_f32(mMNOP);
	float32x2x2_t trABEF=vtrn_f32(mAB,mEF);
	float32x2x2_t trCDGH=vtrn_f32(mCD,mGH);
	float32x2x2_t trIJMN=vtrn_f32(mIJ,mMN);
	float32x2x2_t trKLOP=vtrn_f32(mKL,mOP);
	float32x2_t mAE=trABEF.val[0];
	float32x2_t mBF=trABEF.val[1];
	float32x2_t mCG=trCDGH.val[0];
	float32x2_t mDH=trCDGH.val[1];
	float32x2_t mIM=trIJMN.val[0];
	float32x2_t mJN=trIJMN.val[1];
	float32x2_t mKO=trKLOP.val[0];
	float32x2_t mLP=trKLOP.val[1];
	float32x2_t tmp01=vmul_f32(mKO,mDH);
	tmp01=vmls_f32(tmp01,mCG,mLP);
	float32x2_t tmp23=vmul_f32(mBF,mKO);
	tmp23=vmls_f32(tmp23,mJN,mCG);
	float32x2_t tmp45=vmul_f32(mJN,mDH);
	tmp45=vmls_f32(tmp45,mBF,mLP);
	float32x2_t tmp10=vext_f32(tmp01,tmp01,1);
	float32x2_t tmp32=vext_f32(tmp23,tmp23,1);
	float32x2_t tmp54=vext_f32(tmp45,tmp45,1);
	float32x2_t dxy=vmul_f32(mJN,tmp10);
	dxy=vmls_f32(dxy,mLP,tmp32);
	dxy=vmls_f32(dxy,mKO,tmp54);
	float32x2_t dzw=vmul_f32(mDH,tmp32);
	dzw=vmla_f32(dzw,mCG,tmp54);
	dzw=vmls_f32(dzw,mBF,tmp10);
	float32x2_t det=vmul_f32(mAE,dxy);
	det=vmla_f32(det,mIM,dzw);
	det=vpadd_f32(det,det);
	return vget_lane_f32(det,0);
}

inline const Matrix4 Matrix4::operator+(const Matrix4& mat) const
{
    return Matrix4((mCol0+mat.mCol0),(mCol1+mat.mCol1),(mCol2+mat.mCol2),(mCol3+mat.mCol3));
}

inline const Matrix4 Matrix4::operator-(const Matrix4& mat) const
{
    return Matrix4((mCol0-mat.mCol0),(mCol1-mat.mCol1),(mCol2-mat.mCol2),(mCol3-mat.mCol3));
}

inline Matrix4& Matrix4::operator+=(const Matrix4& mat)
{
    *this=*this+mat;
    return *this;
}

inline Matrix4& Matrix4::operator-=(const Matrix4& mat)
{
    *this=*this-mat;
    return *this;
}

inline const Matrix4 Matrix4::operator-() const
{
    return Matrix4(-mCol0,-mCol1,-mCol2,-mCol3);
}

inline const Matrix4 absPerElem(const Matrix4& mat)
{
    return Matrix4(absPerElem(mat.getCol0()),absPerElem(mat.getCol1()),
				   absPerElem(mat.getCol2()),absPerElem(mat.getCol3()));
}

inline const Matrix4 Matrix4::operator*(float scalar) const
{
    return Matrix4(mCol0*scalar,mCol1*scalar,mCol2*scalar,mCol3*scalar);
}

inline Matrix4& Matrix4::operator*=(float scalar)
{
    *this=*this*scalar;
    return *this;
}

inline const Matrix4 operator*(float scalar,const Matrix4& mat)
{
    return mat*scalar;
}

inline const Vector4 Matrix4::operator*(const Vector4& vec) const
{
	float32x4_t v=vec.get128();
	float32x4_t col0=mCol0.get128();
	float32x4_t col1=mCol1.get128();
	float32x4_t col2=mCol2.get128();
	float32x4_t col3=mCol3.get128();
	float32x4_t vv=vmulq_n_f32(col0,vgetq_lane_f32(v,0));
	vv=vmlaq_n_f32(vv,col1,vgetq_lane_f32(v,1));
	vv=vmlaq_n_f32(vv,col2,vgetq_lane_f32(v,2));
	vv=vmlaq_n_f32(vv,col3,vgetq_lane_f32(v,3));
	return Vector4(vv);
}

inline const Vector4 Matrix4::operator*(const Vector3& vec) const
{
	float32x4_t v=vec.get128();
	float32x4_t col0=mCol0.get128();
	float32x4_t col1=mCol1.get128();
	float32x4_t col2=mCol2.get128();
	float32x4_t vv=vmulq_n_f32(col0,vgetq_lane_f32(v,0));
	vv=vmlaq_n_f32(vv,col1,vgetq_lane_f32(v,1));
	vv=vmlaq_n_f32(vv,col2,vgetq_lane_f32(v,2));
	return Vector4(vv);
}

inline const Vector4 Matrix4::operator*(const Point3& pnt) const
{
	float32x4_t p=pnt.get128();
	float32x4_t col0=mCol0.get128();
	float32x4_t col1=mCol1.get128();
	float32x4_t col2=mCol2.get128();
	float32x4_t vv=mCol3.get128();
	vv=vmlaq_n_f32(vv,col0,vgetq_lane_f32(p,0));
	vv=vmlaq_n_f32(vv,col1,vgetq_lane_f32(p,1));
	vv=vmlaq_n_f32(vv,col2,vgetq_lane_f32(p,2));
	return Vector4(vv);
}

inline const Matrix4 Matrix4::operator*(const Matrix4& mat) const
{
    return Matrix4(*this*mat.mCol0,*this*mat.mCol1,*this*mat.mCol2,*this*mat.mCol3);
}

inline Matrix4& Matrix4::operator*=(const Matrix4& mat)
{
    *this=*this*mat;
    return *this;
}

inline const Matrix4 Matrix4::operator*(const Transform3& tfrm) const
{
    return Matrix4(*this*tfrm.getCol0(),*this*tfrm.getCol1(),*this*tfrm.getCol2(),*this*Point3(tfrm.getCol3()));
}

inline Matrix4& Matrix4::operator*=(const Transform3& tfrm)
{
    *this=*this*tfrm;
    return *this;
}

inline const Matrix4 mulPerElem(const Matrix4& mat0,const Matrix4& mat1)
{
    return Matrix4(mulPerElem(mat0.getCol0(),mat1.getCol0()),mulPerElem(mat0.getCol1(),mat1.getCol1()),
				   mulPerElem(mat0.getCol2(),mat1.getCol2()),mulPerElem(mat0.getCol3(),mat1.getCol3()));
}

inline const Matrix4 Matrix4::identity()
{
    return Matrix4(Vector4::xAxis(),Vector4::yAxis(),Vector4::zAxis(),Vector4::wAxis());
}

inline Matrix4& Matrix4::setUpper3x3(const Matrix3& mat3)
{
    mCol0.setXYZ(mat3.getCol0());
    mCol1.setXYZ(mat3.getCol1());
    mCol2.setXYZ(mat3.getCol2());
    return *this;
}

inline const Matrix3 Matrix4::getUpper3x3() const
{
    return Matrix3(mCol0.getXYZ(),mCol1.getXYZ(),mCol2.getXYZ());
}

inline Matrix4& Matrix4::setTranslation(const Vector3& translateVec)
{
    mCol3.setXYZ(translateVec);
    return *this;
}

inline const Vector3 Matrix4::getTranslation() const
{
    return mCol3.getXYZ();
}

inline const Matrix4 Matrix4::rotationX(float radians)
{
	float r[2];
	neon_sincosf(radians,r);
    return Matrix4(Vector4::xAxis(),Vector4(0.f,r[1],r[0],0.f),Vector4(0.f,-r[0],r[1],0.f),Vector4::wAxis());
}

inline const Matrix4 Matrix4::rotationY(float radians)
{
    float r[2];
	neon_sincosf(radians,r);
	return Matrix4(Vector4(r[1],0.f,-r[0],0.f),Vector4::yAxis(),Vector4(r[0],0.f,r[1],0.f),Vector4::wAxis());
}

inline const Matrix4 Matrix4::rotationZ(float radians)
{
	float r[2];
	neon_sincosf(radians,r);
    return Matrix4(Vector4(r[1],r[0],0.f,0.f),Vector4(-r[0],r[1],0.f,0.f),Vector4::zAxis(),Vector4::wAxis());
}

inline const Matrix4 Matrix4::rotationZYX(const Vector3& radiansXYZ)
{
	float32x4_t s,c;
	neon_vsincosf(radiansXYZ.get128(),&s,&c);
	float32_t ss[4] __attribute__((aligned(16)));
	float32_t sc[4] __attribute__((aligned(16)));
	vst1q_f32(ss,s);
	vst1q_f32(sc,c);
    float tmp0=sc[2]*ss[1];
    float tmp1=ss[2]*ss[1];
    return Matrix4(Vector4(sc[2]*sc[1],ss[2]*sc[1],-ss[1],0.f),
				   Vector4(tmp0*ss[0]-ss[2]*sc[0],tmp1*ss[0]+sc[2]*sc[0],sc[1]*ss[0],0.f),
				   Vector4(tmp0*sc[0]+ss[2]*ss[0],tmp1*sc[0]-sc[2]*ss[0],sc[1]*sc[0],0.f),Vector4::wAxis());
}

inline const Matrix4 Matrix4::rotation(float radians,const Vector3& unitVec)
{
    float x,y,z,r[2],oneMinusC,xy,yz,zx;
	neon_sincosf(radians,r);
    x=unitVec.getX();
    y=unitVec.getY();
    z=unitVec.getZ();
    xy=x*y;
    yz=y*z;
    zx=z*x;
    oneMinusC=1.f-r[1];
    return Matrix4(Vector4(x*x*oneMinusC+r[1],xy*oneMinusC+z*r[0],zx*oneMinusC-y*r[0],0.f),
				   Vector4(xy*oneMinusC-z*r[0],y*y*oneMinusC+r[1],yz*oneMinusC+x*r[0],0.f),
				   Vector4(zx*oneMinusC+y*r[0],yz*oneMinusC-x*r[0],z*z*oneMinusC+r[1],0.f),
				   Vector4::wAxis());
}

inline const Matrix4 Matrix4::rotation(const Quat& unitQuat)
{
    return Matrix4(Transform3::rotation(unitQuat));
}

inline const Matrix4 Matrix4::scale(const Vector3& scaleVec)
{
    return Matrix4(Vector4(scaleVec.getX(),0.f,0.f,0.f),
				   Vector4(0.f,scaleVec.getY(),0.f,0.f),
				   Vector4(0.f,0.f,scaleVec.getZ(),0.f),
				   Vector4::wAxis());
}

inline const Matrix4 appendScale(const Matrix4& mat,const Vector3& scaleVec)
{
    return Matrix4(mat.getCol0()*scaleVec.getX(),mat.getCol1()*scaleVec.getY(),
				   mat.getCol2()*scaleVec.getZ(),mat.getCol3());
}

inline const Matrix4 prependScale(const Vector3& scaleVec,const Matrix4& mat)
{
    Vector4 scale4=Vector4(scaleVec,1.f);
    return Matrix4(mulPerElem(mat.getCol0(),scale4),mulPerElem(mat.getCol1(),scale4),
				   mulPerElem( mat.getCol2(),scale4),mulPerElem(mat.getCol3(),scale4));
}

inline const Matrix4 Matrix4::translation(const Vector3& translateVec)
{
    return Matrix4(Vector4::xAxis(),Vector4::yAxis(),Vector4::zAxis(),Vector4(translateVec,1.f));
}

inline const Matrix4 Matrix4::lookAt(const Point3& eyePos,const Point3& lookAtPos,const Vector3& upVec)
{
    Vector3 v3Y=normalize(upVec);
    Vector3 v3Z=normalize(eyePos-lookAtPos);
    Vector3 v3X=normalize(cross(v3Y,v3Z));
    v3Y=cross(v3Z,v3X);
    Matrix4 m4EyeFrame=Matrix4(Vector4(v3X),Vector4(v3Y),Vector4(v3Z),Vector4(eyePos));
    return orthoInverse(m4EyeFrame);
}

inline const Matrix4 Matrix4::perspective(float fovyRadians,float aspect,float zNear,float zFar)
{
    float f=tanf(_VECTORMATH_PI_OVER_2-0.5f*fovyRadians);
    float rangeInv=1.f/(zNear-zFar);
    return Matrix4(Vector4(f/aspect,0.f,0.f,0.f),
        Vector4(0.f,f,0.f,0.f),
        Vector4(0.f,0.f,(zNear+zFar)*rangeInv,-1.f),
        Vector4(0.f,0.f,((zNear*zFar)*rangeInv)*2.f,0.f));
}

inline const Matrix4 Matrix4::frustum(float left,float right,float bottom,float top,float zNear,float zFar)
{
    float sum_rl,sum_tb,sum_nf,inv_rl,inv_tb,inv_nf,n2;
    sum_rl=right+left;
    sum_tb=top+bottom;
    sum_nf=zNear+zFar;
	float32_t f[4] __attribute__((aligned(16)))={right-left,top-bottom,zNear-zFar,0.f};
	float32x4_t vf=vld1q_f32(f);
	float32x4_t vrf=vrecpeq_f32(vf); //estimate
	vrf=vmulq_f32(vrecpsq_f32(vf,vrf),vrf); //iteration 1
	//vrf=vmulq_f32(vrecpsq_f32(vf,vrf),vrf); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
	vst1q_f32(f,vrf);
	inv_rl=f[0];
    inv_tb=f[1];
    inv_nf=f[2];
	n2=zNear+zNear;
    return Matrix4(Vector4(n2*inv_rl,0.f,0.f,0.f),Vector4(0.f,n2*inv_tb,0.f,0.f),
				   Vector4(sum_rl*inv_rl,sum_tb*inv_tb,sum_nf*inv_nf,-1.f),Vector4(0.f,0.f,n2*inv_nf*zFar,0.f));
}

inline const Matrix4 Matrix4::orthographic(float left,float right,float bottom,float top,float zNear,float zFar)
{
    float sum_rl,sum_tb,sum_nf,inv_rl,inv_tb,inv_nf;
    sum_rl=right+left;
    sum_tb=top+bottom;
    sum_nf=zNear+zFar;
	float32_t f[4] __attribute__((aligned(16)))={right-left,top-bottom,zNear-zFar,0.f};
	float32x4_t vf=vld1q_f32(f);
	float32x4_t vrf=vrecpeq_f32(vf); //estimate
	vrf=vmulq_f32(vrecpsq_f32(vf,vrf),vrf); //iteration 1
	//vrf=vmulq_f32(vrecpsq_f32(vf,vrf),vrf); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
	vst1q_f32(f,vrf);
	inv_rl=f[0];
    inv_tb=f[1];
    inv_nf=f[2];
    return Matrix4(Vector4(inv_rl+inv_rl,0.f,0.f,0.f),Vector4(0.f,inv_tb+inv_tb,0.f,0.f),
				   Vector4(0.f,0.f,inv_nf+inv_nf,0.f),Vector4(-sum_rl*inv_rl,-sum_tb*inv_tb,sum_nf*inv_nf,1.f));
}

inline const Matrix4 select(const Matrix4& mat0,const Matrix4& mat1,bool select1)
{
    return Matrix4(select(mat0.getCol0(),mat1.getCol0(),select1),
				   select(mat0.getCol1(),mat1.getCol1(),select1),
				   select(mat0.getCol2(),mat1.getCol2(),select1),
				   select(mat0.getCol3(),mat1.getCol3(),select1));
}

#ifdef _VECTORMATH_DEBUG

inline void print(const Matrix4& mat)
{
    print(mat.getRow(0));
    print(mat.getRow(1));
    print(mat.getRow(2));
    print(mat.getRow(3));
}

inline void print(const Matrix4& mat,const char* name)
{
    printf("%s:\n",name);
    print(mat);
}

#endif

inline Transform3::Transform3(const Transform3& tfrm)
{
    mCol0=tfrm.mCol0;
    mCol1=tfrm.mCol1;
    mCol2=tfrm.mCol2;
    mCol3=tfrm.mCol3;
}

inline Transform3::Transform3(float scalar)
{
	Vector3 v=Vector3(scalar);
    mCol0=v;
    mCol1=v;
    mCol2=v;
    mCol3=v;
}

inline Transform3::Transform3(const Vector3& _col0,const Vector3& _col1,const Vector3& _col2,const Vector3& _col3 )
{
    mCol0=_col0;
    mCol1=_col1;
    mCol2=_col2;
    mCol3=_col3;
}

inline Transform3::Transform3(const Matrix3& tfrm,const Vector3& translateVec)
{
    this->setUpper3x3(tfrm);
    this->setTranslation(translateVec);
}

inline Transform3::Transform3(const Quat& unitQuat,const Vector3& translateVec)
{
    this->setUpper3x3(Matrix3(unitQuat));
    this->setTranslation(translateVec);
}

inline Transform3& Transform3::setCol0(const Vector3& _col0)
{
    mCol0=_col0;
    return *this;
}

inline Transform3& Transform3::setCol1(const Vector3& _col1)
{
    mCol1=_col1;
    return *this;
}

inline Transform3& Transform3::setCol2(const Vector3& _col2)
{
    mCol2=_col2;
    return *this;
}

inline Transform3& Transform3::setCol3(const Vector3& _col3)
{
    mCol3=_col3;
    return *this;
}

inline Transform3& Transform3::setCol(int col,const Vector3& vec)
{
    *(&mCol0+col)=vec;
    return *this;
}

inline Transform3 & Transform3::setRow(int row,const Vector4& vec)
{
    mCol0.setElem(row,vec.getElem(0));
    mCol1.setElem(row,vec.getElem(1));
    mCol2.setElem(row,vec.getElem(2));
    mCol3.setElem(row,vec.getElem(3));
    return *this;
}

inline Transform3& Transform3::setElem(int col,int row,float val)
{
    Vector3 tmpV3_0=this->getCol(col);
    tmpV3_0.setElem(row,val);
    this->setCol(col,tmpV3_0);
    return *this;
}

inline float Transform3::getElem(int col,int row) const
{
    return this->getCol(col).getElem(row);
}

inline const Vector3 Transform3::getCol0() const
{
    return mCol0;
}

inline const Vector3 Transform3::getCol1() const
{
    return mCol1;
}

inline const Vector3 Transform3::getCol2() const
{
    return mCol2;
}

inline const Vector3 Transform3::getCol3() const
{
    return mCol3;
}

inline const Vector3 Transform3::getCol(int col) const
{
    return *(&mCol0+col);
}

inline const Vector4 Transform3::getRow(int row) const
{
    return Vector4(mCol0.getElem(row),mCol1.getElem(row),mCol2.getElem(row),mCol3.getElem(row));
}

inline Vector3& Transform3::operator[](int col)
{
    return *(&mCol0+col);
}

inline const Vector3 Transform3::operator[](int col) const
{
    return *(&mCol0+col);
}

inline Transform3& Transform3::operator=(const Transform3& tfrm)
{
    mCol0=tfrm.mCol0;
    mCol1=tfrm.mCol1;
    mCol2=tfrm.mCol2;
    mCol3=tfrm.mCol3;
    return *this;
}

inline const Transform3 inverse(const Transform3& tfrm)
{
    Vector3 tmp0=cross(tfrm.getCol1(),tfrm.getCol2());
    Vector3 tmp1=cross(tfrm.getCol2(),tfrm.getCol0());
    Vector3 tmp2=cross(tfrm.getCol0(),tfrm.getCol1());
	float32x2_t det=vdup_n_f32(dot(tfrm.getCol2(),tmp2));
	float32x2_t invdet=vrecpe_f32(det); //estimate
	invdet=vmul_f32(vrecps_f32(det,invdet),invdet); //iteration 1
	//invdet=vmul_f32(vrecps_f32(det,invdet),invdet); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
	float32x4_t invdet2=vcombine_f32(invdet,invdet);
	float32x4_t vt0=tmp0.get128();
	float32x4_t vt1=tmp1.get128();
	vt0=vmulq_f32(vt0,invdet2);
	float32x4_t vt2=tmp2.get128();
	vt1=vmulq_f32(vt1,invdet2);
	vt2=vmulq_f32(vt2,invdet2);
	float32x2_t vt0l=vget_low_f32(vt0),vt0h=vget_high_f32(vt0);
	float32x2_t vt1l=vget_low_f32(vt1),vt1h=vget_high_f32(vt1);
	float32x2_t vt2l=vget_low_f32(vt2),vt2h=vget_high_f32(vt2);
	float32x2x2_t vtt0=vtrn_f32(vt0l,vt1l);
	float32x2x2_t vtt1=vtrn_f32(vt0h,vt1h);
	float32x2x2_t vtt2=vtrn_f32(vt2l,vt2l);
	Vector3 inv0=Vector3(vcombine_f32(vtt0.val[0],vtt2.val[0]));
	Vector3 inv1=Vector3(vcombine_f32(vtt0.val[1],vtt2.val[1]));
	Vector3 inv2=Vector3(vcombine_f32(vtt1.val[0],vt2h));
	return Transform3(inv0,inv1,inv2,Vector3(inv1*tfrm.getCol3().getY()+
											 inv2*tfrm.getCol3().getZ()-
											 inv0*tfrm.getCol3().getX()));
}

inline const Transform3 orthoInverse(const Transform3& tfrm)
{
	float32x4_t c0=tfrm.getCol0().get128();
	float32x4_t c1=tfrm.getCol1().get128();
	float32x4_t c2=tfrm.getCol2().get128();
	float32x2_t c0l=vget_low_f32(c0),c0h=vget_high_f32(c0);
	float32x2_t c1l=vget_low_f32(c1),c1h=vget_high_f32(c1);
	float32x2_t c2l=vget_low_f32(c2),c2h=vget_high_f32(c2);
	float32x2x2_t tc0=vtrn_f32(c0l,c1l);
	float32x2x2_t tc1=vtrn_f32(c0h,c1h);
	float32x2x2_t tc2=vtrn_f32(c2l,c2l);
	Vector3 inv0=Vector3(vcombine_f32(tc0.val[0],tc2.val[0]));
	Vector3 inv1=Vector3(vcombine_f32(tc0.val[1],tc2.val[1]));
	Vector3 inv2=Vector3(vcombine_f32(tc1.val[0],c2h));
    return Transform3(inv0,inv1,inv2,Vector3(inv1*tfrm.getCol3().getY()+
											 inv2*tfrm.getCol3().getZ()-
											 inv0*tfrm.getCol3().getX()));
}

inline const Transform3 absPerElem(const Transform3& tfrm)
{
    return Transform3(absPerElem(tfrm.getCol0()),absPerElem(tfrm.getCol1()),
					  absPerElem(tfrm.getCol2()),absPerElem(tfrm.getCol3()));
}

inline const Vector3 Transform3::operator*(const Vector3& vec) const
{
	float32x4_t v=vmulq_n_f32(mCol0.get128(),vec.getX());
	v=vmlaq_n_f32(v,mCol1.get128(),vec.getY());
	v=vmlaq_n_f32(v,mCol2.get128(),vec.getZ());
	return Vector3(v);
}

inline const Point3 Transform3::operator*(const Point3& pnt) const
{
	float32x4_t v=vmulq_n_f32(mCol0.get128(),pnt.getX());
	v=vmlaq_n_f32(v,mCol1.get128(),pnt.getY());
	v=vmlaq_n_f32(v,mCol2.get128(),pnt.getZ());
	return Point3(v);
}

inline const Transform3 Transform3::operator*(const Transform3& tfrm) const
{
    return Transform3(*this*tfrm.mCol0,*this*tfrm.mCol1,*this*tfrm.mCol2,Vector3(*this*Point3(tfrm.mCol3)));
}

inline Transform3& Transform3::operator*=(const Transform3& tfrm)
{
    *this=*this*tfrm;
    return *this;
}

inline const Transform3 mulPerElem(const Transform3& tfrm0,const Transform3& tfrm1)
{
    return Transform3(mulPerElem(tfrm0.getCol0(),tfrm1.getCol0()),mulPerElem(tfrm0.getCol1(),tfrm1.getCol1()),
					  mulPerElem(tfrm0.getCol2(),tfrm1.getCol2()),mulPerElem(tfrm0.getCol3(),tfrm1.getCol3()));
}

inline const Transform3 Transform3::identity()
{
    return Transform3(Vector3::xAxis(),Vector3::yAxis(),Vector3::zAxis(),Vector3(0.f));
}

inline Transform3& Transform3::setUpper3x3(const Matrix3& tfrm)
{
    mCol0=tfrm.getCol0();
    mCol1=tfrm.getCol1();
    mCol2=tfrm.getCol2();
    return *this;
}

inline const Matrix3 Transform3::getUpper3x3() const
{
    return Matrix3(mCol0,mCol1,mCol2);
}

inline Transform3& Transform3::setTranslation(const Vector3& translateVec)
{
    mCol3=translateVec;
    return *this;
}

inline const Vector3 Transform3::getTranslation() const
{
    return mCol3;
}

inline const Transform3 Transform3::rotationX(float radians)
{
	float r[2];
	neon_sincosf(radians,r);
    return Transform3(Vector3::xAxis(),Vector3(0.f,r[1],r[0]),Vector3(0.f,-r[0],r[1]),Vector3(0.f));
}

inline const Transform3 Transform3::rotationY(float radians)
{
    float r[2];
	neon_sincosf(radians,r);
	return Transform3(Vector3(r[1],0.f,-r[0]),Vector3::yAxis(),Vector3(r[0],0.f,r[1]),Vector3(0.f));
}

inline const Transform3 Transform3::rotationZ( float radians )
{
	float r[2];
	neon_sincosf(radians,r);
    return Transform3(Vector3(r[1],r[0],0.f),Vector3(-r[0],r[1],0.f),Vector3::zAxis(),Vector3(0.f));
}

inline const Transform3 Transform3::rotationZYX(const Vector3& radiansXYZ)
{
	float32x4_t s,c;
	neon_vsincosf(radiansXYZ.get128(),&s,&c);
	float32_t ss[4] __attribute__((aligned(16)));
	float32_t sc[4] __attribute__((aligned(16)));
	vst1q_f32(ss,s);
	vst1q_f32(sc,c);
    float tmp0=sc[2]*ss[1];
    float tmp1=ss[2]*ss[1];
    return Transform3(Vector3(sc[2]*sc[1],ss[2]*sc[1],-ss[1]),
					  Vector3(tmp0*ss[0]-ss[2]*sc[0],tmp1*ss[0]+sc[2]*sc[0],sc[1]*ss[0]),
					  Vector3(tmp0*sc[0]+ss[2]*ss[0],tmp1*sc[0]-sc[2]*ss[0],sc[1]*sc[0]),Vector3(0.f));
}

inline const Transform3 Transform3::rotation(float radians,const Vector3& unitVec)
{
    return Transform3(Matrix3::rotation(radians,unitVec),Vector3(0.0f));
}

inline const Transform3 Transform3::rotation(const Quat& unitQuat)
{
    return Transform3(Matrix3(unitQuat),Vector3(0.f));
}

inline const Transform3 Transform3::scale(const Vector3& scaleVec)
{
    return Transform3(Vector3(scaleVec.getX(),0.f,0.f),Vector3(0.f,scaleVec.getY(),0.f),
					  Vector3(0.f,0.f,scaleVec.getZ()),Vector3(0.f));
}

inline const Transform3 appendScale(const Transform3& tfrm,const Vector3& scaleVec)
{
    return Transform3(tfrm.getCol0()*scaleVec.getX(),tfrm.getCol1()*scaleVec.getY(),
					  tfrm.getCol2()*scaleVec.getZ(),tfrm.getCol3());
}

inline const Transform3 prependScale(const Vector3& scaleVec,const Transform3& tfrm)
{
    return Transform3(mulPerElem(tfrm.getCol0(),scaleVec),mulPerElem(tfrm.getCol1(),scaleVec),
					  mulPerElem(tfrm.getCol2(),scaleVec),mulPerElem(tfrm.getCol3(),scaleVec));
}

inline const Transform3 Transform3::translation(const Vector3& translateVec)
{
    return Transform3(Vector3::xAxis(),Vector3::yAxis(),Vector3::zAxis(),translateVec);
}

inline const Transform3 select(const Transform3& tfrm0,const Transform3& tfrm1,bool select1)
{
    return Transform3(select(tfrm0.getCol0(),tfrm1.getCol0(),select1),
					  select(tfrm0.getCol1(),tfrm1.getCol1(),select1),
					  select(tfrm0.getCol2(),tfrm1.getCol2(),select1),
					  select(tfrm0.getCol3(),tfrm1.getCol3(),select1));
}

#ifdef _VECTORMATH_DEBUG

inline void print(const Transform3& tfrm)
{
    print(tfrm.getRow(0));
    print(tfrm.getRow(1));
    print(tfrm.getRow(2));
}

inline void print(const Transform3& tfrm,const char* name)
{
    printf("%s:\n",name);
    print(tfrm);
}

#endif

inline Quat::Quat(const Matrix3& tfrm)
{
	//TODO:optimize
    float xx=tfrm.getCol0().getX();
    float yx=tfrm.getCol0().getY();
    float zx=tfrm.getCol0().getZ();
    float xy=tfrm.getCol1().getX();
    float yy=tfrm.getCol1().getY();
    float zy=tfrm.getCol1().getZ();
    float xz=tfrm.getCol2().getX();
    float yz=tfrm.getCol2().getY();
    float zz=tfrm.getCol2().getZ();

    float trace=xx+yy+zz;

    float negTrace=trace<0.f;
    int ZgtX=zz>xx;
    int ZgtY=zz>yy;
    int YgtX=yy>xx;
    int largestXorY=(!ZgtX||!ZgtY)&&negTrace;
    int largestYorZ=(YgtX||ZgtX)&&negTrace;
    int largestZorX=(ZgtY||!YgtX)&&negTrace;
    
    if(largestXorY)
    {
        zz=-zz;
        xy=-xy;
    }
    if(largestYorZ)
    {
        xx=-xx;
        yz=-yz;
    }
    if(largestZorX)
    {
        yy=-yy;
        zx=-zx;
    }

    float radicand=xx+yy+zz+1.0f;
    float scale=0.5f*(1.f/sqrtf(radicand));

    float tmpx=(zy-yz)*scale;
    float tmpy=(xz-zx)*scale;
    float tmpz=(yx-xy)*scale;
    float tmpw=radicand*scale;
    float qx=tmpx;
    float qy=tmpy;
    float qz=tmpz;
    float qw=tmpw;

    if(largestXorY)
    {
        qx=tmpw;
        qy=tmpz;
        qz=tmpy;
        qw=tmpx;
    }
    if(largestYorZ)
    {
        tmpx=qx;
        tmpz=qz;
        qx=qy;
        qy=tmpx;
        qz=qw;
        qw=tmpz;
    }

    setX(qx);
    setY(qy);
    setZ(qz);
    setW(qw);
}

inline const Matrix3 outer(const Vector3& tfrm0,const Vector3& tfrm1)
{
    return Matrix3(tfrm0*tfrm1.getX(),tfrm0*tfrm1.getY(),tfrm0*tfrm1.getZ());
}

inline const Matrix4 outer(const Vector4& tfrm0,const Vector4& tfrm1)
{
    return Matrix4(tfrm0*tfrm1.getX(),tfrm0*tfrm1.getY(),tfrm0*tfrm1.getZ(),tfrm0*tfrm1.getW());
}

inline const Vector3 rowMul(const Vector3& vec,const Matrix3& mat)
{
	float32x4_t v=vec.get128();
	v=vsetq_lane_f32(0.f,v,3);
	float32x4_t c0=mat.getCol0().get128();
	c0=vsetq_lane_f32(0.f,c0,3);
	float32x4_t c1=mat.getCol1().get128();
	c1=vsetq_lane_f32(0.f,c1,3);
	float32x4_t c0v=vmulq_f32(c0,v);
	float32x4_t c2=mat.getCol2().get128();
	c2=vsetq_lane_f32(0.f,c2,3);
	float32x2_t c0hs=vadd_f32(vget_low_f32(c0v),vget_high_f32(c0v));
	float32x4_t c1v=vmulq_f32(c1,v);
	float32x4_t c2v=vmulq_f32(c2,v);
	c0hs=vpadd_f32(c0hs,c0hs);
	float32x2_t c1hs=vadd_f32(vget_low_f32(c1v),vget_high_f32(c1v));
	float32x2_t c2hs=vadd_f32(vget_low_f32(c2v),vget_high_f32(c2v));
	c1hs=vpadd_f32(c1hs,c1hs);
	c2hs=vpadd_f32(c2hs,c2hs);
	float32x2_t c01=vext_f32(c0hs,c1hs,1);
	return Vector3(vcombine_f32(c01,c2hs));
}

inline const Matrix3 crossMatrix(const Vector3& vec)
{
    return Matrix3(Vector3(0.f,vec.getZ(),-vec.getY()),
				   Vector3(-vec.getZ(),0.f,vec.getX()),
				   Vector3(vec.getY(),-vec.getX(),0.f));
}

inline const Matrix3 crossMatrixMul(const Vector3& vec,const Matrix3& mat)
{
    return Matrix3(cross(vec,mat.getCol0()), cross(vec,mat.getCol1()), cross(vec,mat.getCol2()));
}

} // namespace Aos
} // namespace Vectormath

#endif

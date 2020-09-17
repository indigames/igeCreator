/* SCE CONFIDENTIAL
PlayStation(R)3 Programmer Tool Runtime Library 310.001
* Copyright (C) 2008 Sony Computer Entertainment Inc.
* All Rights Reserved.
*/

#ifndef _VECTORMATH_QUAT_AOS_NEON_CPP_H
#define _VECTORMATH_QUAT_AOS_NEON_CPP_H

//-----------------------------------------------------------------------------
// Definitions

#ifndef _VECTORMATH_INTERNAL_FUNCTIONS
#define _VECTORMATH_INTERNAL_FUNCTIONS

#endif

namespace Vectormath {
namespace Aos {

inline Quat::Quat(const float32x4_t quat)
{
	q=quat;
}

inline const float32x4_t Quat::get128() const
{
	return q;
}

inline Quat::Quat(const Quat& quat)
{
	q=quat.q;
}

inline Quat::Quat(float _x,float _y,float _z,float _w)
{
	q=vsetq_lane_f32((float32_t)_x,q,0);
	q=vsetq_lane_f32((float32_t)_y,q,1);
	q=vsetq_lane_f32((float32_t)_z,q,2);
	q=vsetq_lane_f32((float32_t)_w,q,3);
}

inline Quat::Quat(const Vector3& xyz,float _w)
{
	q=xyz.get128();
	q=vsetq_lane_f32((float32_t)_w,q,3);
}

inline Quat::Quat(const Vector4& vec)
{
	q=vec.get128();
}

inline Quat::Quat(float scalar)
{
	q=vdupq_n_f32((float32_t)scalar);
}

inline const Quat Quat::identity()
{
	static const float32_t i[4] __attribute__((aligned(16)))={0.f,0.f,0.f,1.f};
	return Quat(vld1q_f32(i));
}

inline const Quat lerp(float t,const Quat& quat0,const Quat& quat1)
{
    return (quat0+((quat1-quat0)*t));
}

inline const Quat slerp(float t,const Quat& unitQuat0,const Quat& unitQuat1)
{
    Quat start;
    float recipSinAngle,scale0,scale1,cosAngle,angle;
    cosAngle=dot(unitQuat0,unitQuat1);
    if(cosAngle<0.0f)
	{
        cosAngle=-cosAngle;
        start=(-unitQuat0);
    }
	else
	{
        start=unitQuat0;
    }
    if(cosAngle<_VECTORMATH_SLERP_TOL)
	{
        angle=neon_acosf(cosAngle);
		const float32_t sx[4] __attribute__((aligned(16)))={angle,(1.f-t)*angle,t*angle,0.f};
		float32x4_t x=vld1q_f32(sx),s,c;
		neon_vsincosf(x,&s,&c);
		float32_t ss[4] __attribute__((aligned(16)));
		vst1q_f32(ss,s);
        recipSinAngle=1.f/ss[0];
        scale0=ss[1]*recipSinAngle;
        scale1=ss[2]*recipSinAngle;
    }
	else
	{
        scale0=(1.f-t);
        scale1=t;
    }
    return ((start*scale0)+(unitQuat1*scale1));
}

inline const Quat squad(float t,const Quat& unitQuat0,const Quat& unitQuat1,const Quat& unitQuat2,
						const Quat& unitQuat3)
{
    Quat tmp0,tmp1;
    tmp0=slerp(t,unitQuat0,unitQuat3);
    tmp1=slerp(t,unitQuat1,unitQuat2);
    return slerp(((2.f*t)*(1.f-t)),tmp0,tmp1);
}

inline void loadXYZW(Quat& quat,const float* fptr)
{
    //quat=Quat(fptr[0],fptr[1],fptr[2],fptr[3]); //safe way
	quat=Quat(vld1q_f32((const float32_t*)fptr)); //speedy way,fptr align==16 assumed
}

inline void storeXYZW(const Quat& quat,float* fptr)
{
    //fptr[0]=quat.getX(); //safe way
    //fptr[1]=quat.getY();
    //fptr[2]=quat.getZ();
    //fptr[3]=quat.getW();
	vst1q_f32((float32_t*)fptr,quat.get128()); //speedy way,fptr align==16 assumed
}

inline Quat& Quat::operator=(const Quat& quat)
{
	q=quat.q;
    return *this;
}

inline Quat& Quat::setXYZ(const Vector3& vec)
{
	setX(vec.getX());
	setY(vec.getY());
	setZ(vec.getZ());
    return *this;
}

inline const Vector3 Quat::getXYZ() const
{
    return Vector3(q);
}

inline Quat& Quat::setX(float _x)
{
	q=vsetq_lane_f32((float32_t)_x,q,0);
    return *this;
}

inline float Quat::getX() const
{
    return (float)vgetq_lane_f32(q,0);
}

inline Quat& Quat::setY(float _y)
{
    q=vsetq_lane_f32((float32_t)_y,q,1);
    return *this;
}

inline float Quat::getY() const
{
    return (float)vgetq_lane_f32(q,1);
}

inline Quat & Quat::setZ(float _z)
{
	q=vsetq_lane_f32((float32_t)_z,q,2);
    return *this;
}

inline float Quat::getZ() const
{
    return (float)vgetq_lane_f32(q,2);
}

inline Quat& Quat::setW(float _w)
{
    q=vsetq_lane_f32((float32_t)_w,q,3);
    return *this;
}

inline float Quat::getW() const
{
    return (float)vgetq_lane_f32(q,3);
}

inline Quat& Quat::setElem(int idx,float value)
{
#ifdef SN_TARGET_PSP2
	q=vsetq_lane_f32((float32_t)value,q,idx);
#else
    switch (idx)
    {
        case 0:
            q=vsetq_lane_f32((float32_t)value,q,0);
            break;
        case 1:
            q=vsetq_lane_f32((float32_t)value,q,1);
            break;
        case 2:
            q=vsetq_lane_f32((float32_t)value,q,2);
            break;
        case 3:
            q=vsetq_lane_f32((float32_t)value,q,3);
    }
#endif
    return *this;
}

inline float Quat::getElem(int idx) const
{
#ifdef SN_TARGET_PSP2
    return (float)vgetq_lane_f32(q,idx);
#else
    switch(idx)
    {
        case 0:
            return (float)vgetq_lane_f32(q,0);
            break;
        case 1:
            return (float)vgetq_lane_f32(q,1);
            break;
        case 2:
            return (float)vgetq_lane_f32(q,2);
            break;
        case 3:
            return (float)vgetq_lane_f32(q,3);
    }
#endif
}

inline float& Quat::operator[](int idx)
{ //NEON limitation - cannot return reference or pointer to NEON register. use return value for read only
#ifdef SN_TARGET_PSP2
    float vq=(float)vgetq_lane_f32(q,idx);
#else
    float vq;
    switch (idx)
    {
        case 0:
            vq=(float)vgetq_lane_f32(q,0);
            break;
        case 1:
            vq=(float)vgetq_lane_f32(q,1);
            break;
        case 2:
            vq=(float)vgetq_lane_f32(q,2);
            break;
        case 3:
            vq=(float)vgetq_lane_f32(q,3);
    }
#endif
    return vq;
}

inline float Quat::operator[](int idx) const
{
#ifdef SN_TARGET_PSP2
    return (float)vgetq_lane_f32(q,idx);
#else
	switch(idx)
	{
		case 0:
			return (float)vgetq_lane_f32(q,0);
			break;
		case 1:
			return (float)vgetq_lane_f32(q,1);
			break;
		case 2:
			return (float)vgetq_lane_f32(q,2);
			break;
		case 3:
			return (float)vgetq_lane_f32(q,3);
	}
#endif
}

inline const Quat Quat::operator+(const Quat& quat) const
{
    return Quat(vaddq_f32(q,quat.q));
}

inline const Quat Quat::operator-(const Quat& quat) const
{
    return Quat(vsubq_f32(q,quat.q));
}

inline const Quat Quat::operator*(float scalar) const
{
    return Quat(vmulq_n_f32(q,(float32_t)scalar));
}

inline Quat& Quat::operator+=(const Quat& quat)
{
    *this=*this+quat;
    return *this;
}

inline Quat& Quat::operator-=(const Quat& quat)
{
    *this=*this-quat;
    return *this;
}

inline Quat& Quat::operator*=(float scalar)
{
    *this=*this*scalar;
    return *this;
}

inline const Quat Quat::operator/(float scalar) const
{
	float32x2_t ns=vmov_n_f32((float32_t)scalar);
	float32x2_t rscalar=vrecpe_f32(ns);
	rscalar=vmul_f32(vrecps_f32(ns,rscalar),rscalar);
    return Quat(vmulq_n_f32(q,vget_lane_f32(rscalar,0)));
}

inline Quat& Quat::operator/=(float scalar)
{
    *this=*this/scalar;
    return *this;
}

inline const Quat Quat::operator-() const
{
    return Quat(vnegq_f32(q));
}

inline const Quat operator*(float scalar,const Quat& quat)
{
    return quat*scalar;
}

inline float dot(const Quat& quat0,const Quat& quat1)
{
    float32x4_t quat0128=quat0.get128();
	float32x4_t quat1128=quat1.get128();
	float32x2_t r=vmul_f32(vget_high_f32(quat0128),vget_high_f32(quat1128));
	r=vmla_f32(r,vget_low_f32(quat0128),vget_low_f32(quat1128));
	return (float)vget_lane_f32(vpadd_f32(r,r),0);
}

inline float norm(const Quat& quat)
{
	float32x4_t quat128=quat.get128();
	float32x2_t quat64h=vget_high_f32(quat128),quat64l=vget_low_f32(quat128);
	float32x2_t r=vmul_f32(quat64h,quat64h);
	r=vmla_f32(r,quat64l,quat64l);
	return (float)vget_lane_f32(vpadd_f32(r,r),0);
}

inline float length(const Quat& quat)
{
	float32x2_t lsq=vdup_n_f32((float32_t)norm(quat));
	float32x2_t l=vrsqrte_f32(lsq); //estimate
	l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 1
	//l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
    return (float)vget_lane_f32(vmul_f32(lsq,l),0);
}

inline const Quat normalize(const Quat& quat)
{
	float32x4_t quat128=quat.get128();
	float32x2_t quat64h=vget_high_f32(quat128),quat64l=vget_low_f32(quat128);
	float32x2_t r=vmul_f32(quat64h,quat64h);
	r=vmla_f32(r,quat64l,quat64l);
	float32x2_t lsq=vdup_n_f32(vget_lane_f32(vpadd_f32(r,r),0));
	float32x2_t l=vrsqrte_f32(lsq); //estimate
	l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 1
	//l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
	return Quat(vmulq_f32(quat128,vcombine_f32(l,l)));
}

inline const Quat Quat::rotation(const Vector3& unitVec0,const Vector3& unitVec1)
{
	float32x2_t lsq=vdup_n_f32((float32_t)dot(unitVec0,unitVec1));
	lsq=vadd_f32(lsq,vdup_n_f32(1.f));
	lsq=vmul_n_f32(lsq,2.f);
	float32x2_t l=vrsqrte_f32(lsq); //estimate
	l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 1
	//l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
	float32_t recipCosHalfAngleX2=vget_lane_f32(l,0);
    float32_t cosHalfAngleX2=vget_lane_f32(vmul_n_f32(vmul_f32(lsq,l),0.5f),0);
	return Quat((cross(unitVec0,unitVec1)*recipCosHalfAngleX2),cosHalfAngleX2);
}

inline const Quat Quat::rotation(float radians,const Vector3& unitVec)
{
	float r[2],angle=(radians*0.5f);
	neon_sincosf(angle,r);
    return Quat((unitVec*r[0]),r[1]);
}

inline const Quat Quat::rotationX(float radians)
{
    float r[2],angle=radians*0.5f;
    neon_sincosf(angle,r);
    return Quat(r[0],0.f,0.f,r[1]);
}

inline const Quat Quat::rotationY(float radians)
{
    float r[2],angle=radians*0.5f;
    neon_sincosf(angle,r);
    return Quat(0.f,r[0],0.f,r[1]);
}

inline const Quat Quat::rotationZ(float radians)
{
    float r[2],angle=radians*0.5f;
    neon_sincosf(angle,r);
    return Quat(0.f,0.f,r[0],r[1]);
}

inline const Quat Quat::operator*(const Quat& quat) const
{
	float32_t qq_w=vgetq_lane_f32(quat.q,3);
	float32x2_t o2=vget_low_f32(q);
	float32x2_t qh=vget_high_f32(q);
	float32x2_t qql=vget_low_f32(quat.q);
	float32x2_t qqh=vget_high_f32(quat.q);
	float32x4_t negqq=vnegq_f32(quat.q);
	float32x2x2_t o3i=vzip_f32(qh,o2);
	float32x4_t q2=vdupq_n_f32(qq_w);
	float32_t negqq_x=vgetq_lane_f32(negqq,0);
	float32x2_t o3=o3i.val[0];
	q2=vsetq_lane_f32(negqq_x,q2,3);
	float32x4_t e1=vmulq_n_f32(quat.q,vgetq_lane_f32(q,3));
	float32x2_t negqql=vget_low_f32(negqq);
	float32x2_t negqqh=vget_high_f32(negqq);
	float32x2x2_t q3l=vzip_f32(qqh,qql);
	float32x2_t o4=vext_f32(o2,qh,1);
	float32x2x2_t q3h=vzip_f32(qql,negqql);
	float32x4_t q3=vcombine_f32(q3l.val[0],q3h.val[1]);
	float32x4_t e2=vmlaq_f32(e1,vcombine_f32(o2,o3),q2);
	float32x2_t q4l=vext_f32(negqql,negqqh,1);
	float32x2x2_t q4hi=vzip_f32(negqql,negqqh);
	float32x4_t q4=vcombine_f32(q4l,q4hi.val[0]);
	float32x4_t e3=vmlaq_f32(e2,vcombine_f32(o4,o2),q3);
	float32x4_t e4=vmlaq_f32(e3,vcombine_f32(o3,o4),q4);
	return Quat(e4);
}

inline Quat& Quat::operator*=(const Quat& quat)
{
    *this=*this*quat;
    return *this;
}

inline const Vector3 rotate(const Quat& quat,const Vector3& vec)
{
	float32x4_t vq=quat.get128(); //qxyzw
	float32x4_t vv=vec.get128(); //vxyz
	float32_t vq_x=vgetq_lane_f32(vq,0); //qx
	float32x2_t vvh=vget_high_f32(vv); //vz
	float32_t vq_w=vgetq_lane_f32(vq,3); //qw
	float32x2_t v1l=vget_low_f32(vv); //vxy
	float32x4_t q1=vdupq_n_f32(vq_w); //q1=wwww
	float32x2x2_t v1hi=vzip_f32(vvh,v1l); //vzx
	float32_t vv_y=vget_lane_f32(v1l,1); //vy
	q1=vsetq_lane_f32(vq_x,q1,3); //q1=wwwx
	float32x2_t vql=vget_low_f32(vq); //qxy
	float32x2_t v2h=vdup_n_f32(vv_y); //vyy
	float32x2_t vqh=vget_high_f32(vq); //qzw
	float32x4_t e1=vmulq_f32(q1,vcombine_f32(v1l,v1hi.val[0])); //q1*v1
	float32x2_t q2l=vext_f32(vql,vqh,1); //qyz
	float32x4_t e2=vmlaq_f32(e1,vcombine_f32(q2l,vql),vcombine_f32(v1hi.val[0],v2h)); //e1+q2*v2
	float32x2_t v3l=vext_f32(v1l,vvh,1); //vyz
	float32x2x2_t q3li=vzip_f32(vqh,vql); //qzx
	float32x2_t nvvh=vneg_f32(vvh); //-vz
	float32x2x2_t v3hi=vzip_f32(v1l,vvh); //vxz
	v3hi=vzip_f32(v3hi.val[0],nvvh); //-vxz
	float32x4_t e3=vmlsq_f32(e2,vcombine_f32(q3li.val[0],q2l),vcombine_f32(v3l,v3hi.val[0])); //e2-q3*v3
	float32_t t_w=vgetq_lane_f32(e3,3); //tw
	float32x4_t e4=vmulq_n_f32(vq,t_w); //tw*qxyz
	float32x2_t tl=vget_low_f32(e3); //txy
	float32x2_t th=vget_high_f32(e3); //tzw
	float32x2_t tyz=vext_f32(tl,th,1); //tyz
	float32x4_t e5=vmlaq_n_f32(e4,e3,vq_w); //e4+txyz*qw
	float32x2x2_t tzxi=vzip_f32(th,tl); //tzx
	float32x4_t e6=vmlsq_f32(e5,vcombine_f32(tyz,tl),vcombine_f32(q3li.val[0],q2l)); //e5-tyzx*qzxy
	float32x4_t e7=vmlaq_f32(e6,vcombine_f32(tzxi.val[0],tyz),vcombine_f32(q2l,vql)); //e6+tzxy*qyzx
	return Vector3(e7);
}

inline const Quat conj(const Quat& quat)
{
	float32x4_t vq=quat.get128();
	float32x4_t qn=vnegq_f32(vq);
	return Quat(vsetq_lane_f32(vgetq_lane_f32(vq,3),qn,3));
}

inline const Quat select(const Quat& quat0,const Quat& quat1,bool select1)
{
	float32x4_t quat0128=quat0.get128();
	float32x4_t quat1128=quat1.get128();
	uint32_t sel=(uint32_t)(-((int32_t)select1));
	uint32x4_t vsel=vdupq_n_u32(sel);
	float32x4_t selres=vbslq_f32(vsel,quat1128,quat0128);
	return Quat(selres);
}

#ifdef _VECTORMATH_DEBUG

inline void print(const Quat& quat)
{
    printf("( %f %f %f %f )\n",quat.getX(),quat.getY(),quat.getZ(),quat.getW());
}

inline void print(const Quat& quat,const char* name)
{
    printf("%s: ( %f %f %f %f )\n",name,quat.getX(),quat.getY(),quat.getZ(),quat.getW());
}

#endif

} // namespace Aos
} // namespace Vectormath

#endif

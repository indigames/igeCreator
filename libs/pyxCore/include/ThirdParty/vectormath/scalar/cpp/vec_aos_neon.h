#ifndef _VECTORMATH_VEC_AOS_NEON_CPP_H
#define _VECTORMATH_VEC_AOS_NEON_CPP_H

#define _VECTORMATH_SLERP_TOL 0.999f

namespace Vectormath {
namespace Aos {

//trigonometric functions
inline float neon_asinf(float x)
{
	//don't try to read. brain may explode
	static const float32_t pi_2=1.57079632679489661923f; //pi/2
	if(x==1.f||x==-1.f) return pi_2*x;
	float32x2_t d0=vdup_n_f32((float32_t)x);
	float32x2_t d4=vdup_n_f32(pi_2);
	float32x2_t d6=d0;
	d0=vabs_f32(d0);

	float32x2_t d5=vdup_n_f32(0.5f);
	d5=vmls_f32(d5,d0,d5);

	float32x2_t d1=d5;
	d5=vrsqrte_f32(d5);
	float32x2_t d2=vmul_f32(d5,d1);
	float32x2_t d3=vrsqrts_f32(d2,d5);
	d5=vmul_f32(d5,d3);
	d2=vmul_f32(d5,d1);
	d3=vrsqrts_f32(d2,d5);
	d5=vmul_f32(d5,d3);

	d1=vrecpe_f32(d5);
	d2=vrecps_f32(d1,d5);
	d1=vmul_f32(d1,d2);
	d2=vrecps_f32(d1,d5);
	d5=vmul_f32(d1,d2);

	d5=vsub_f32(d0,d5);
	d2=vdup_n_f32(0.5f);
	uint32x2_t d3i=vcgt_f32(d0,d2);
	d1=vdup_n_f32(3.f);
	d3i=vshr_n_u32(d3i,31);
	float32x2_t d16=vdup_n_f32(1.f);
	d3=vcvt_f32_u32(d3i);
	d0=vmls_n_f32(d0,d5,vget_lane_f32(d3,0));
	float32x2_t d7=vmul_n_f32(d4,vget_lane_f32(d3,0));
	d16=vmls_n_f32(d16,d1,vget_lane_f32(d3,0));

	d2=vmul_f32(d0,d0);
	static const float32_t poly[4] __attribute__((aligned(16)))={0.105312459675071f,0.169303418571894f,
															 0.051599985887214f,0.999954835104825f};
	float32x4_t d45=vld1q_f32(poly); //=q2
	d3=vmul_f32(d2,d2);
	float32x4_t q0=vmulq_n_f32(d45,vget_lane_f32(d0,0));
	d1=vmla_n_f32(vget_high_f32(q0),vget_low_f32(q0),vget_lane_f32(d2,0));
	d1=vmla_n_f32(d1,d3,vget_lane_f32(d1,0));

	d7=vmla_f32(d7,d1,d16);

	d2=vadd_f32(d7,d7);
	d3i=vclt_f32(d6,vdup_n_f32(0.f));
	d3i=vshr_n_u32(d3i,31);
	d3=vcvt_f32_u32(d3i);
	d7=vmls_n_f32(d7,d2,vget_lane_f32(d3,0));

	return vget_lane_f32(d7,1);
}
	
inline float neon_acosf(float x)
{
	static const float32_t pi_2=1.57079632679489661923f; //pi/2
	return vget_lane_f32(vsub_f32(vdup_n_f32(pi_2),vdup_n_f32(neon_asinf(x))),0);
}

inline float neon_sinf(float x)
{
	static const float32_t pi=3.1415926535897932384626433832795f;
	static const float32_t rng[2] __attribute__((aligned(16)))={2.f/pi,pi/2.f};
	static const float32_t poly[4] __attribute__((aligned(16)))={-0.00018365f,-0.16664831f,
															 0.00830636f,0.99999661f};
	float32x2_t d3=vld1_f32(rng);
	float32x2_t d0=vdup_n_f32(x);
	float32x2_t d1=vabs_f32(d0);

	float32x2_t d2=vmul_n_f32(d1,vget_lane_f32(d3,0));
	uint32x2_t d2i=vcvt_u32_f32(d2);
	uint32x2_t d5i=vdup_n_u32(1);
	float32x2_t d4=vcvt_f32_u32(d2i);
	uint32x2_t d7i=vshr_n_u32(d2i,1);
	d1=vmls_n_f32(d1,d4,vget_lane_f32(d3,1));

	d5i=vand_u32(d2i,d5i);
	uint32x2_t d18i=vclt_f32(d0,vdup_n_f32(0.f));
	float32x2_t d6=vcvt_f32_u32(d5i);
	d1=vmls_n_f32(d1,d6,vget_lane_f32(d3,1));
	d5i=veor_u32(d5i,d7i);
	d2=vmul_f32(d1,d1);

	float32x4_t d1617=vld1q_f32(poly);
	d5i=veor_u32(d5i,d18i);
	d5i=vshl_n_u32(d5i,31);
	d1=vreinterpret_f32_u32(veor_u32(vreinterpret_u32_f32(d1),d5i));

	d3=vmul_f32(d2,d2);
	float32x4_t q0=vmulq_n_f32(d1617,vget_lane_f32(d1,0)); //={d0,d1}
	d1=vmla_n_f32(vget_high_f32(q0),vget_low_f32(q0),vget_lane_f32(d2,0));
	d1=vmla_n_f32(d1,d3,vget_lane_f32(d1,0));

	return vget_lane_f32(d1,1);
}

inline void neon_sincosf(float x,float r[2])
{
	static const float32_t pi=3.1415926535897932384626433832795f;
	static const float32_t rng[2] __attribute__((aligned(16)))={2.f/pi,pi/2.f};
	static const float32_t poly[8] __attribute__((aligned(16)))={-0.00018365f,-0.00018365f,
															 0.00830636f,0.00830636f,
															 -0.16664831f,-0.16664831f,
															 0.99999661f,0.99999661f};

	float32x2_t d1=vdup_n_f32(x);
	float32x2_t d3=vld1_f32(rng);
	float32x2_t d0=vadd_f32(d1,d3);
	d0=vset_lane_f32(vget_lane_f32(d1,0),d0,0);
	d1=vabs_f32(d0);

	float32x2_t d2=vmul_n_f32(d1,vget_lane_f32(d3,0));
	uint32x2_t d2i=vcvt_u32_f32(d2);
	float32x2_t d4=vcvt_f32_u32(d2i);
	d1=vmls_n_f32(d1,d4,vget_lane_f32(d3,1));

	uint32x2_t d4i=vdup_n_u32(1);
	d4i=vand_u32(d4i,d2i);
	float32x2_t d5=vcvt_f32_u32(d4i);
	d1=vmls_n_f32(d1,d5,vget_lane_f32(d3,1));

	uint32x2_t d3i=vshr_n_u32(d2i,1);
	d4i=veor_u32(d4i,d3i);
	d3i=vclt_f32(d0,vdup_n_f32(0.f));
	d4i=veor_u32(d4i,d3i);
	d4i=vshl_n_u32(d4i,31);
	d0=vreinterpret_f32_u32(veor_u32(vreinterpret_u32_f32(d1),d4i));

	float32x4_t d23=vld1q_f32(poly);
	d1=vmul_f32(d0,d0);
	d4=vld1_f32(poly+4);
	d3=vmla_f32(vget_high_f32(d23),vget_low_f32(d23),d1);
	d5=vld1_f32(poly+6);
	d4=vmla_f32(d4,d3,d1);
	d5=vmla_f32(d5,d4,d1);
	d5=vmul_f32(d5,d0);
	vst1_f32((float32_t*)r,d5);
}

inline void neon_vsincosf(float32x4_t x,float32x4_t* ysin,float32x4_t* ycos)
{
	static const float32_t c_cephes_FOPI=1.27323954473516f; //4/pi
	static const float32_t c_minus_cephes_DP1=-0.78515625f;
	static const float32_t c_minus_cephes_DP2=-2.4187564849853515625e-4f;
	static const float32_t c_minus_cephes_DP3=-3.77489497744594108e-8f;
	static const float32_t c_sincof_p0=-1.9515295891e-4f;
	static const float32_t c_sincof_p1=8.3321608736e-3f;
	static const float32_t c_sincof_p2=-1.6666654611e-1f;
	static const float32_t c_coscof_p0=2.443315711809948e-5f;
	static const float32_t c_coscof_p1=-1.388731625493765e-3f;
	static const float32_t c_coscof_p2=4.166664568298827e-2f;
	float32x4_t xmm1,xmm2,xmm3,y;
	uint32x4_t emm2,sign_mask_sin,sign_mask_cos;

	sign_mask_sin=vcltq_f32(x,vdupq_n_f32(0.f));
	x=vabsq_f32(x);

	y=vmulq_f32(x,vdupq_n_f32(c_cephes_FOPI));

	emm2=vcvtq_u32_f32(y);
	emm2=vaddq_u32(emm2,vdupq_n_u32(1));
	emm2=vandq_u32(emm2,vdupq_n_u32(~1));
	y=vcvtq_f32_u32(emm2);

	uint32x4_t poly_mask=vtstq_u32(emm2,vdupq_n_u32(2));

	xmm1=vmulq_n_f32(y,c_minus_cephes_DP1);
	xmm2=vmulq_n_f32(y,c_minus_cephes_DP2);
	xmm3=vmulq_n_f32(y,c_minus_cephes_DP3);
	x=vaddq_f32(x,xmm1);
	x=vaddq_f32(x,xmm2);
	x=vaddq_f32(x,xmm3);

	sign_mask_sin=veorq_u32(sign_mask_sin,vtstq_u32(emm2,vdupq_n_u32(4)));
	sign_mask_cos=vtstq_u32(vsubq_u32(emm2,vdupq_n_u32(2)),vdupq_n_u32(4));

	float32x4_t z=vmulq_f32(x,x),y1,y2;

	y1=vmulq_n_f32(z,c_coscof_p0);
	y2=vmulq_n_f32(z,c_sincof_p0);
	y1=vaddq_f32(y1,vdupq_n_f32(c_coscof_p1));
	y2=vaddq_f32(y2,vdupq_n_f32(c_sincof_p1));
	y1=vmulq_f32(y1,z);
	y2=vmulq_f32(y2,z);
	y1=vaddq_f32(y1,vdupq_n_f32(c_coscof_p2));
	y2=vaddq_f32(y2,vdupq_n_f32(c_sincof_p2));
	y1=vmulq_f32(y1,z);
	y2=vmulq_f32(y2,z);
	y1=vmulq_f32(y1,z);
	y2=vmulq_f32(y2,x);
	y1=vsubq_f32(y1,vmulq_f32(z,vdupq_n_f32(0.5f)));
	y2=vaddq_f32(y2,x);
	y1=vaddq_f32(y1,vdupq_n_f32(1.f));

	float32x4_t ys=vbslq_f32(poly_mask,y1,y2);
	float32x4_t yc=vbslq_f32(poly_mask,y2,y1);
	*ysin=vbslq_f32(sign_mask_sin,vnegq_f32(ys),ys);
	*ycos=vbslq_f32(sign_mask_cos,yc,vnegq_f32(yc));
}

inline float neon_tanf(float x)
{
	static const float32_t pi=3.1415926535897932384626433832795f;
	static const float32_t rng[2] __attribute__((aligned(16)))={2.f/pi,pi/2.f};
	static const float32_t poly[4] __attribute__((aligned(16)))={-0.00018365f,-0.16664831f,
															 0.00830636f,0.99999661f};

	float32x2_t d0=vdup_n_f32(x);
	float32x2_t d1=vabs_f32(d0);

	float32x2_t d3=vld1_f32(rng);
	float32x2_t d2=vmul_n_f32(d1,vget_lane_f32(d3,0));
	uint32x2_t d2i=vcvt_u32_f32(d2);
	float32x2_t d4=vcvt_f32_u32(d2i);
	d1=vmls_n_f32(d1,d4,vget_lane_f32(d3,1));

	uint32x2_t d4i=vdup_n_u32(1);
	d2i=vand_u32(d2i,d4i);
	d2=vcvt_f32_u32(d2i);
	d1=vmls_n_f32(d1,d2,vget_lane_f32(d3,1));

	d4i=vdup_n_u32(0x80000000);
	uint32x2_t d0i=vand_u32(vreinterpret_u32_f32(d0),d4i);
	uint32x2_t d1i=veor_u32(vreinterpret_u32_f32(d1),d0i);

	d1=vreinterpret_f32_u32(d1i);
	d2=vmul_f32(d1,d1);
	float32x4_t d45=vld1q_f32(poly);
	d3=vmul_f32(d2,d2);
	float32x4_t d01=vmulq_n_f32(d45,vget_lane_f32(d1,0));
	d1=vmla_n_f32(vget_high_f32(d01),vget_low_f32(d01),vget_lane_f32(d2,0));
	d1=vmla_n_f32(d1,d3,vget_lane_f32(d1,0));

	d0=vdup_n_f32(1.f);
	d0=vmls_f32(d0,d1,d1);

	d2=d0;
	d0=vrsqrte_f32(d0);
	d3=vmul_f32(d0,d2);
	d4=vrsqrts_f32(d3,d0);
	d0=vmul_f32(d0,d4);
	d3=vmul_f32(d0,d2);
	d4=vrsqrts_f32(d3,d0);
	d0=vmul_f32(d0,d4);

	d0=vmul_f32(d0,d1);

	return vget_lane_f32(d0,1);
}
////////////////////////

inline Vector3::Vector3(const float32x4_t vec)
{
	v=vec;
}

inline const float32x4_t Vector3::get128() const
{
	return v;
}

inline Vector3::Vector3(const Vector3& vec)
{
	v=vec.v;
}

inline Vector3::Vector3(float _x,float _y,float _z)
{
    v=vsetq_lane_f32((float32_t)_x,v,0);
	v=vsetq_lane_f32((float32_t)_y,v,1);
	v=vsetq_lane_f32((float32_t)_z,v,2);
}

inline Vector3::Vector3(const Point3& pnt)
{
	v=pnt.get128();
}

inline Vector3::Vector3(float scalar)
{
	v=vmovq_n_f32((float32_t)scalar);
}

inline const Vector3 Vector3::xAxis()
{
	static const float32_t xaxis[4] __attribute__((aligned(16)))={1.f,0.f,0.f,0.f};
	return Vector3(vld1q_f32(xaxis));
}

inline const Vector3 Vector3::yAxis()
{
	static const float32_t yaxis[4] __attribute__((aligned(16)))={0.f,1.f,0.f,0.f};
	return Vector3(vld1q_f32(yaxis));
}

inline const Vector3 Vector3::zAxis()
{
	static const float32_t zaxis[4] __attribute__((aligned(16)))={0.f,0.f,1.f,0.f};
	return Vector3(vld1q_f32(zaxis));
}

inline const Vector3 lerp(float t,const Vector3& vec0,const Vector3& vec1)
{
    return (vec0+((vec1-vec0)*t));
}

inline const Vector3 slerp(float t,const Vector3& unitVec0,const Vector3& unitVec1)
{
    float recipSinAngle,scale0,scale1,cosAngle,angle;
    cosAngle=dot(unitVec0,unitVec1);
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
    return ((unitVec0*scale0)+(unitVec1*scale1));
}

inline void loadXYZ(Vector3& vec,const float* fptr)
{
    vec=Vector3(fptr[0],fptr[1],fptr[2]);
	//vec=Vector3(vld1q_f32((const float32_t*)fptr)); //fptr align==16 assumed,fptr mem size==4 assumed,dangerous
}

inline void storeXYZ(const Vector3& vec,float* fptr)
{
    fptr[0]=vec.getX();
    fptr[1]=vec.getY();
    fptr[2]=vec.getZ();
	//vst1q_f32((float32_t*)fptr,vec.get128()); //fptr align==16 assumed,fptr mem size==4 assumed,dangerous
}

inline void loadHalfFloats(Vector3& vec,const unsigned short* hfptr)
{
#ifdef SN_TARGET_PSP2
	float16_t vhfptr[4] __attribute__((aligned(16)));
	vhfptr[0]=((float16_t*)hfptr)[0];
	vhfptr[1]=((float16_t*)hfptr)[1];
	vhfptr[2]=((float16_t*)hfptr)[2];
	//vhfptr[3]=(float16_t)0;
	vec=Vector3(vcvt_f32_f16(vld1_f16(vhfptr)));
#else
	union Data32
	{
        unsigned int u32;
        float f32;
    };
    for(int i = 0;i<3;i++)
	{
        unsigned short fp16=hfptr[i];
        unsigned int sign=fp16>>15;
        unsigned int exponent=(fp16>>10)&((1<<5)-1);
        unsigned int mantissa=fp16&((1<<10)-1);
        if(exponent==0)
		{
            //zero
            mantissa=0;
        }
		else if(exponent==31)
		{
            //infinity or nan->infinity
            exponent=255;
			mantissa=0;
        }
		else
		{
            exponent+=127-15;
            mantissa<<=13;
        }
        Data32 d;
        d.u32=(sign<<31)|(exponent<<23)|mantissa;
        vec[i]=d.f32;
    }
#endif
}

inline void storeHalfFloats(const Vector3& vec,unsigned short* hfptr)
{
#ifdef SN_TARGET_PSP2
	float16_t vhfptr[4] __attribute__((aligned(16)));
	vst1_f16(vhfptr,vcvt_f16_f32(vec.get128()));
	hfptr[0]=((unsigned short*)vhfptr)[0];
	hfptr[1]=((unsigned short*)vhfptr)[1];
	hfptr[2]=((unsigned short*)vhfptr)[2];
#else
	union Data32
	{
        unsigned int u32;
        float f32;
    };
    for(int i=0;i<3;i++)
	{
        Data32 d;
        d.f32=vec[i];
        unsigned int sign=d.u32>>31;
        unsigned int exponent=(d.u32>>23)&((1<<8)-1);
        unsigned int mantissa=d.u32&((1<<23)-1);
        if(exponent==0)
		{
            //zero or denorm->zero
            mantissa=0;
        }
		else if(exponent==255&&mantissa!=0)
		{
            //nan->infinity
            exponent=31;
            mantissa=0;
        }
		else if(exponent>=127-15+31)
		{
            //overflow or infinity->infinity
            exponent=31;
            mantissa=0;
        }
		else if(exponent<=127-15)
		{
            //underflow->zero
            exponent=0;
            mantissa=0;
        }
		else
		{
            exponent-=127-15;
            mantissa>>=13;
        }
        hfptr[i]=(unsigned short)((sign<<15)|(exponent<<10)|mantissa);
    }
#endif
}

inline Vector3& Vector3::operator=(const Vector3& vec)
{
	v=vec.v;
    return *this;
}

inline Vector3& Vector3::setX(float _x)
{
	v=vsetq_lane_f32((float32_t)_x,v,0);
    return *this;
}

inline float Vector3::getX() const
{
	return (float)vgetq_lane_f32(v,0);
}

inline Vector3& Vector3::setY(float _y)
{
    v=vsetq_lane_f32((float32_t)_y,v,1);
    return *this;
}

inline float Vector3::getY() const
{
    return (float)vgetq_lane_f32(v,1);
}

inline Vector3& Vector3::setZ(float _z)
{
	v=vsetq_lane_f32((float32_t)_z,v,2);
    return *this;
}

inline float Vector3::getZ() const
{
    return (float)vgetq_lane_f32(v,2);
}

inline Vector3& Vector3::setElem(int idx,float value)
{
#ifdef SN_TARGET_PSP2
    v=vsetq_lane_f32((float32_t)value,v,idx);
#else
    switch (idx)
    {
        case 0:
            v=vsetq_lane_f32((float32_t)value,v,0);
            break;
        case 1:
            v=vsetq_lane_f32((float32_t)value,v,1);
            break;
        case 2:
            v=vsetq_lane_f32((float32_t)value,v,2);
    }
#endif
	return *this;
}

inline float Vector3::getElem(int idx) const
{
#ifdef SN_TARGET_PSP2
	return (float)vgetq_lane_f32(v,idx);
#else
    switch (idx)
    {
        case 0:
            return (float)vgetq_lane_f32(v,0);
            break;
        case 1:
            return (float)vgetq_lane_f32(v,1);
            break;
        case 2:
            return (float)vgetq_lane_f32(v,2);
    }
#endif
}

inline float& Vector3::operator[](int idx)
{ //NEON limitation - cannot return reference or pointer to NEON register. use return value for read only
#ifdef SN_TARGET_PSP2
    float vv=(float)vgetq_lane_f32(v,idx);
#else
    float vv;
    switch(idx)
    {
        case 0:
            vv=(float)vgetq_lane_f32(v,0);
            break;
        case 1:
            vv=(float)vgetq_lane_f32(v,1);
            break;
        case 2:
            vv=(float)vgetq_lane_f32(v,2);
    }
#endif
    return vv;
}

inline float Vector3::operator[](int idx) const
{
#ifdef SN_TARGET_PSP2
	return (float)vgetq_lane_f32(v,idx);
#else
    switch (idx)
    {
        case 0:
            return (float)vgetq_lane_f32(v,0);
            break;
        case 1:
            return (float)vgetq_lane_f32(v,1);
            break;
        case 2:
            return (float)vgetq_lane_f32(v,2);
    }
#endif
}

inline const Vector3 Vector3::operator+(const Vector3& vec) const
{
	return Vector3(vaddq_f32(v,vec.v));
}

inline const Vector3 Vector3::operator-(const Vector3& vec) const
{
	return Vector3(vsubq_f32(v,vec.v));
}

inline const Point3 Vector3::operator+(const Point3& pnt) const
{
	return Point3(vaddq_f32(v,pnt.get128()));
}

inline const Vector3 Vector3::operator*(float scalar) const
{
	return Vector3(vmulq_n_f32(v,(float32_t)scalar));
}

inline Vector3& Vector3::operator+=(const Vector3& vec)
{
    *this=*this+vec;
    return *this;
}

inline Vector3& Vector3::operator-=(const Vector3& vec)
{
    *this=*this-vec;
    return *this;
}

inline Vector3& Vector3::operator*=(float scalar)
{
    *this=*this*scalar;
    return *this;
}

inline const Vector3 Vector3::operator/(float scalar) const
{
	float32x2_t ns=vmov_n_f32((float32_t)scalar);
	float32x2_t rscalar=vrecpe_f32(ns);
	rscalar=vmul_f32(vrecps_f32(ns,rscalar),rscalar);
    return Vector3(vmulq_n_f32(v,vget_lane_f32(rscalar,0)));
}

inline Vector3& Vector3::operator/=(float scalar)
{
    *this=*this/scalar;
    return *this;
}

inline const Vector3 Vector3::operator-() const
{
    return Vector3(vnegq_f32(v));
}

inline const Vector3 operator*(float scalar,const Vector3& vec)
{
    return vec*scalar;
}

inline const Vector3 mulPerElem(const Vector3& vec0,const Vector3& vec1)
{
	return Vector3(vmulq_f32(vec0.get128(),vec1.get128()));
}

inline const Vector3 divPerElem(const Vector3& vec0,const Vector3& vec1)
{
	Vector3 rv1=recipPerElem(vec1);
	return mulPerElem(vec0,rv1);
}

inline const Vector3 recipPerElem(const Vector3& vec)
{
	float32x4_t vec128=vec.get128();
	float32x4_t rvec128=vrecpeq_f32(vec128); //estimate
	rvec128=vmulq_f32(vrecpsq_f32(vec128,rvec128),rvec128); //iteration 1
	//rvec128=vmulq_f32(vrecpsq_f32(vec128,rvec128),rvec128); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
	return Vector3(rvec128);
}

inline const Vector3 sqrtPerElem(const Vector3& vec)
{
	float32x4_t vec128=vec.get128();
	float32x4_t rsqvec128=vrsqrteq_f32(vec128); //estimate
	rsqvec128=vmulq_f32(rsqvec128,vrsqrtsq_f32(vmulq_f32(vec128,rsqvec128),rsqvec128)); //iteration 1
	//rsqvec128=vmulq_f32(rsqvec128,vrsqrtsq_f32(vmulq_f32(vec128,rsqvec128),rsqvec128)); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
    return Vector3(vmulq_f32(vec128,rsqvec128));
}

inline const Vector3 rsqrtPerElem(const Vector3& vec)
{
	float32x4_t vec128=vec.get128();
	float32x4_t rsqvec128=vrsqrteq_f32(vec128); //estimate
	rsqvec128=vmulq_f32(rsqvec128,vrsqrtsq_f32(vmulq_f32(vec128,rsqvec128),rsqvec128)); //iteration 1
	//rsqvec128=vmulq_f32(rsqvec128,vrsqrtsq_f32(vmulq_f32(vec128,rsqvec128),rsqvec128)); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
	return Vector3(rsqvec128);
}

inline const Vector3 absPerElem(const Vector3 & vec )
{
	return Vector3(vabsq_f32(vec.get128()));
}

inline const Vector3 copySignPerElem(const Vector3& vec0,const Vector3& vec1)
{
	uint32x4_t msk=vcltq_f32(vec1.get128(),vdupq_n_f32(0.f));
	float32x4_t absvec0128=vabsq_f32(vec0.get128());
	return Vector3(vbslq_f32(msk,vnegq_f32(absvec0128),absvec0128));
}

inline const Vector3 maxPerElem(const Vector3& vec0,const Vector3& vec1)
{
	return Vector3(vmaxq_f32(vec0.get128(),vec1.get128()));
}

inline float maxElem(const Vector3& vec)
{
	float32x4_t vec128=vec.get128();
	float32x2_t vec128xy=vget_low_f32(vec128);
	float32x2_t vec128zz=vdup_n_f32(vgetq_lane_f32(vec128,2));
	float32x2_t vec64max=vmax_f32(vec128xy,vec128zz);
	float32x2_t vec64max1=vdup_n_f32(vget_lane_f32(vec64max,0));
	float32x2_t vec64max2=vdup_n_f32(vget_lane_f32(vec64max,1));
    return vget_lane_f32(vmax_f32(vec64max1,vec64max2),0);
}

inline const Vector3 minPerElem(const Vector3& vec0,const Vector3& vec1)
{
    return Vector3(vminq_f32(vec0.get128(),vec1.get128()));
}

inline float minElem(const Vector3& vec)
{
	float32x4_t vec128=vec.get128();
	float32x2_t vec128xy=vget_low_f32(vec128);
	float32x2_t vec128zz=vdup_n_f32(vgetq_lane_f32(vec128,2));
	float32x2_t vec64min=vmin_f32(vec128xy,vec128zz);
	float32x2_t vec64min1=vdup_n_f32(vget_lane_f32(vec64min,0));
	float32x2_t vec64min2=vdup_n_f32(vget_lane_f32(vec64min,1));
    return vget_lane_f32(vmin_f32(vec64min1,vec64min2),0);
}

inline float sum(const Vector3& vec)
{
	float32x4_t vec128=vec.get128();
	vec128=vsetq_lane_f32(0.f,vec128,3);
	float32x2_t vec64sum=vadd_f32(vget_low_f32(vec128),vget_high_f32(vec128));
	return vget_lane_f32(vpadd_f32(vec64sum,vec64sum),0);
}

inline float dot(const Vector3& vec0,const Vector3& vec1)
{
	float32x4_t vec0128=vec0.get128();
	float32x4_t vec1128=vec1.get128();
	float32x2_t vec0xy=vget_low_f32(vec0128);
	float32x2_t vec0zz=vdup_n_f32(vgetq_lane_f32(vec0128,2));
	float32x2_t vec1xy=vget_low_f32(vec1128);
	float32x2_t vec1zz=vdup_n_f32(vgetq_lane_f32(vec1128,2));
	float32x2_t vec01xy=vmul_f32(vec0xy,vec1xy);
	float32x2_t vec01xxyy=vpadd_f32(vec01xy,vec01xy);
	float32x2_t vec01xxyyzz=vmla_f32(vec01xxyy,vec0zz,vec1zz);
	return (float)vget_lane_f32(vec01xxyyzz,0);
}

inline float lengthSqr(const Vector3& vec)
{
	float32x4_t vec128=vec.get128();
	float32x2_t vecxy=vget_low_f32(vec128);
	float32x2_t vecz0=vget_high_f32(vec128);
	float32x2_t vecxxyy=vmul_f32(vecxy,vecxy);
	vecxxyy=vpadd_f32(vecxxyy,vecxxyy);
	float32x2_t vecxxyyzz=vmla_f32(vecxxyy,vecz0,vecz0);
	return vget_lane_f32(vecxxyyzz,0);
}

inline float length(const Vector3& vec)
{
	float32x2_t lsq=vdup_n_f32((float32_t)lengthSqr(vec));
	float32x2_t l=vrsqrte_f32(lsq); //estimate
	l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 1
	//l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
    return vget_lane_f32(vmul_f32(lsq,l),0);
}

inline const Vector3 normalize(const Vector3& vec)
{
	float32x4_t vec128=vec.get128();
	float32x2_t vecxy=vget_low_f32(vec128);
	float32x2_t vecz0=vget_high_f32(vec128);
	float32x2_t vecxxyy=vmul_f32(vecxy,vecxy);
	vecxxyy=vpadd_f32(vecxxyy,vecxxyy);
	float32x2_t vecxxyyzz=vmla_f32(vecxxyy,vecz0,vecz0);
	float32x2_t lsq=vdup_n_f32(vget_lane_f32(vecxxyyzz,0));
	float32x2_t l=vrsqrte_f32(lsq); //estimate
	l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 1
	//l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
	return Vector3(vmulq_f32(vec128,vcombine_f32(l,l)));
}

inline const Vector3 cross(const Vector3& vec0, const Vector3& vec1)
{
	float32x4_t vec0128=vec0.get128();
	float32x4_t vec1128=vec1.get128();
	float32x2_t d1=vset_lane_f32(vgetq_lane_f32(vec0128,0),d1,1);
	float32x2_t d0=vext_f32(vget_low_f32(vec0128),vget_high_f32(vec0128),1);
	d1=vset_lane_f32(vgetq_lane_f32(vec0128,2),d1,0);
	float32x2_t d2=vset_lane_f32(vgetq_lane_f32(vec1128,0),d2,1);
	float32x2_t d3=vext_f32(vget_low_f32(vec1128),vget_high_f32(vec1128),1);
	d2=vset_lane_f32(vgetq_lane_f32(vec1128,2),d2,0);
	float32x2_t d4=vmul_f32(d0,d2);
	d4=vmls_f32(d4,d1,d3);
	float32x2_t d5=vmul_n_f32(d3,vget_lane_f32(d1,1));
	d5=vmls_n_f32(d5,d0,vget_lane_f32(d2,1));
	float32x4_t crossres=vcombine_f32(d4,d5);
	return Vector3(crossres);
}

inline const Vector3 select(const Vector3& vec0,const Vector3& vec1,bool select1)
{
	float32x4_t vec0128=vec0.get128();
	float32x4_t vec1128=vec1.get128();
	uint32_t sel=(uint32_t)(-((int32_t)select1));
	uint32x4_t vsel=vdupq_n_u32(sel);
	float32x4_t selres=vbslq_f32(vsel,vec1128,vec0128);
	return Vector3(selres);
}

#ifdef _VECTORMATH_DEBUG

inline void print(const Vector3& vec)
{
    printf("( %f %f %f )\n",vec.getX(),vec.getY(),vec.getZ());
}

inline void print(const Vector3& vec,const char* name)
{
    printf("%s: ( %f %f %f )\n",name,vec.getX(),vec.getY(),vec.getZ());
}

#endif

inline Vector4::Vector4(const float32x4_t vec)
{
	v=vec;
}

inline const float32x4_t Vector4::get128() const
{
	return v;
}

inline Vector4::Vector4(const Vector4& vec)
{
	v=vec.v;
}

inline Vector4::Vector4(float _x,float _y,float _z,float _w)
{
	v=vsetq_lane_f32((float32_t)_x,v,0);
	v=vsetq_lane_f32((float32_t)_y,v,1);
	v=vsetq_lane_f32((float32_t)_z,v,2);
	v=vsetq_lane_f32((float32_t)_w,v,3);
}

inline Vector4::Vector4(const Vector3& xyz,float _w)
{
	v=xyz.get128();
	v=vsetq_lane_f32((float32_t)_w,v,3);
}

inline Vector4::Vector4(const Vector3& vec)
{
    v=vec.get128();
	v=vsetq_lane_f32((float32_t)0.f,v,3);
}

inline Vector4::Vector4(const Point3& pnt)
{
	v=pnt.get128();
	v=vsetq_lane_f32((float32_t)1.f,v,3);
}

inline Vector4::Vector4(const Quat& quat)
{
	v=quat.get128();
}

inline Vector4::Vector4(float scalar)
{
	v=vdupq_n_f32((float32_t)scalar);
}

inline const Vector4 Vector4::xAxis()
{
	static const float32_t xaxis[4] __attribute__((aligned(16)))={1.f,0.f,0.f,0.f};
    return Vector4(vld1q_f32(xaxis));
}

inline const Vector4 Vector4::yAxis()
{
	static const float32_t yaxis[4] __attribute__((aligned(16)))={0.f,1.f,0.f,0.f};
	return Vector4(vld1q_f32(yaxis));
}

inline const Vector4 Vector4::zAxis()
{
	static const float32_t zaxis[4] __attribute__((aligned(16)))={0.f,0.f,1.f,0.f};
	return Vector4(vld1q_f32(zaxis));
}

inline const Vector4 Vector4::wAxis()
{
	static const float32_t waxis[4] __attribute__((aligned(16)))={0.f,0.f,0.f,1.f};
	return Vector4(vld1q_f32(waxis));
}

inline const Vector4 lerp(float t,const Vector4& vec0,const Vector4& vec1)
{
    return (vec0+((vec1-vec0)*t));
}

inline const Vector4 slerp(float t,const Vector4& unitVec0,const Vector4& unitVec1)
{
    float recipSinAngle,scale0,scale1,cosAngle,angle;
    cosAngle=dot(unitVec0,unitVec1);
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
    return ((unitVec0*scale0)+(unitVec1*scale1));
}

inline void loadXYZW(Vector4& vec,const float* fptr)
{
	//vec=Vector4(fptr[0],fptr[1],fptr[2],fptr[3]); //safe way
	vec=Vector4(vld1q_f32((const float32_t*)fptr)); //speedy way,fptr align==16 assumed
}

inline void storeXYZW(const Vector4& vec,float* fptr)
{
    //fptr[0]=vec.getX(); //safe way
    //fptr[1]=vec.getY();
    //fptr[2]=vec.getZ();
    //fptr[3]=vec.getW();
	vst1q_f32((float32_t*)fptr,vec.get128()); //speedy way,fptr align==16 assumed
}

inline void loadHalfFloats(Vector4& vec,const unsigned short* hfptr)
{
#ifdef SN_TARGET_PSP2
	vec=Vector4(vcvt_f32_f16(vld1_f16((const float16_t*)hfptr)));
#else
	union Data32
	{
        unsigned int u32;
        float f32;
    };
    for(int i=0;i<4;i++)
	{
        unsigned short fp16=hfptr[i];
        unsigned int sign=fp16>>15;
        unsigned int exponent=(fp16>>10)&((1<<5)-1);
        unsigned int mantissa=fp16&((1<<10)-1);
        if(exponent==0)
		{
            //zero
            mantissa=0;
        }
		else if(exponent==31)
		{
            //infinity or nan->infinity
            exponent=255;
			mantissa=0;
        }
		else
		{
            exponent+=127-15;
            mantissa<<=13;
        }
        Data32 d;
        d.u32=(sign<<31)|(exponent<<23)|mantissa;
        vec[i]=d.f32;
    }
#endif
}

inline void storeHalfFloats(const Vector4& vec,unsigned short* hfptr)
{
#ifdef SN_TARGET_PSP2
	vst1_f16((float16_t*)hfptr,vcvt_f16_f32(vec.get128()));
#else
	union Data32
	{
        unsigned int u32;
        float f32;
    };
    for(int i=0;i<4;i++)
	{
        Data32 d;
        d.f32=vec[i];
        unsigned int sign=d.u32>>31;
        unsigned int exponent=(d.u32>>23)&((1<<8)-1);
        unsigned int mantissa=d.u32&((1<<23)-1);
        if(exponent==0)
		{
            //zero or denorm->zero
            mantissa=0;
        }
		else if(exponent==255&&mantissa!=0)
		{
            //nan->infinity
            exponent=31;
            mantissa=0;
        }
		else if(exponent>=127-15+31)
		{
            //overflow or infinity->infinity
            exponent=31;
            mantissa=0;
        }
		else if(exponent<=127-15)
		{
            //underflow->zero
            exponent=0;
            mantissa=0;
        }
		else
		{
            exponent-=127-15;
            mantissa>>=13;
        }
        hfptr[i]=(unsigned short)((sign<<15)|(exponent<<10)|mantissa);
    }
#endif
}

inline Vector4& Vector4::operator=(const Vector4& vec)
{
	v=vec.v;
    return *this;
}

inline Vector4& Vector4::setXYZ(const Vector3& vec)
{
    setX(vec.getX());
    setY(vec.getY());
    setZ(vec.getZ());
    return *this;
}

inline const Vector3 Vector4::getXYZ() const
{
    return Vector3(v);
}

inline Vector4& Vector4::setX(float _x)
{
    v=vsetq_lane_f32((float32_t)_x,v,0);
	return *this;
}

inline float Vector4::getX() const
{
    return (float)vgetq_lane_f32(v,0);
}

inline Vector4& Vector4::setY(float _y)
{
	v=vsetq_lane_f32((float32_t)_y,v,1);
    return *this;
}

inline float Vector4::getY() const
{
    return (float)vgetq_lane_f32(v,1);
}

inline Vector4& Vector4::setZ(float _z)
{
	v=vsetq_lane_f32((float32_t)_z,v,2);
    return *this;
}

inline float Vector4::getZ() const
{
    return (float)vgetq_lane_f32(v,2);
}

inline Vector4& Vector4::setW(float _w)
{
	v=vsetq_lane_f32((float32_t)_w,v,3);
    return *this;
}

inline float Vector4::getW() const
{
    return (float)vgetq_lane_f32(v,3);
}

inline Vector4& Vector4::setElem(int idx,float value)
{
#ifdef SN_TARGET_PSP2
	v=vsetq_lane_f32((float32_t)value,v,idx);
#else
    switch(idx)
    {
        case 0:
            v=vsetq_lane_f32((float32_t)value,v,0);
            break;
        case 1:
            v=vsetq_lane_f32((float32_t)value,v,1);
            break;
        case 2:
            v=vsetq_lane_f32((float32_t)value,v,2);
            break;
        case 3:
            v=vsetq_lane_f32((float32_t)value,v,3);
    }
#endif
    return *this;
}

inline float Vector4::getElem(int idx) const
{
#ifdef SN_TARGET_PSP2
    return (float)vgetq_lane_f32(v,idx);
#else
    switch(idx)
    {
        case 0:
            return (float)vgetq_lane_f32(v,0);
            break;
        case 1:
            return (float)vgetq_lane_f32(v,1);
            break;
        case 2:
            return (float)vgetq_lane_f32(v,2);
            break;
        case 3:
            return (float)vgetq_lane_f32(v,3);
    }
#endif
}

inline float& Vector4::operator[](int idx)
{ //NEON limitation - cannot return reference or pointer to NEON register. use return value for read only
#ifdef SN_TARGET_PSP2
    float vv=(float)vgetq_lane_f32(v,idx);
#else
    float vv;
    switch(idx)
    {
        case 0:
            vv=(float)vgetq_lane_f32(v,0);
            break;
        case 1:
            vv=(float)vgetq_lane_f32(v,1);
            break;
        case 2:
            vv=(float)vgetq_lane_f32(v,2);
            break;
        case 3:
            vv=(float)vgetq_lane_f32(v,3);
    }
#endif
	return vv;
}

inline float Vector4::operator[](int idx) const
{
#ifdef SN_TARGET_PSP2
    return (float)vgetq_lane_f32(v,idx);
#else
    switch(idx)
    {
        case 0:
            return (float)vgetq_lane_f32(v,0);
            break;
        case 1:
            return (float)vgetq_lane_f32(v,1);
            break;
        case 2:
            return (float)vgetq_lane_f32(v,2);
            break;
        case 3:
            return (float)vgetq_lane_f32(v,3);
    }
#endif
}

inline const Vector4 Vector4::operator+(const Vector4& vec) const
{
    return Vector4(vaddq_f32(v,vec.v));
}

inline const Vector4 Vector4::operator-(const Vector4& vec) const
{
    return Vector4(vsubq_f32(v,vec.v));
}

inline const Vector4 Vector4::operator*(float scalar) const
{
    return Vector4(vmulq_n_f32(v,(float32_t)scalar));
}

inline Vector4& Vector4::operator+=(const Vector4& vec)
{
    *this=*this+vec;
    return *this;
}

inline Vector4& Vector4::operator-=(const Vector4& vec)
{
    *this=*this-vec;
    return *this;
}

inline Vector4& Vector4::operator*=(float scalar)
{
    *this=*this*scalar;
    return *this;
}

inline const Vector4 Vector4::operator/(float scalar) const
{
	float32x2_t ns=vmov_n_f32((float32_t)scalar);
	float32x2_t rscalar=vrecpe_f32(ns);
	rscalar=vmul_f32(vrecps_f32(ns,rscalar),rscalar);
    return Vector4(vmulq_n_f32(v,vget_lane_f32(rscalar,0)));
}

inline Vector4& Vector4::operator/=(float scalar)
{
    *this=*this/scalar;
    return *this;
}

inline const Vector4 Vector4::operator-() const
{
    return Vector4(vnegq_f32(v));
}

inline const Vector4 operator*(float scalar,const Vector4& vec)
{
    return vec*scalar;
}

inline const Vector4 mulPerElem(const Vector4& vec0,const Vector4& vec1)
{
    return Vector4(vmulq_f32(vec0.get128(),vec1.get128()));
}

inline const Vector4 divPerElem(const Vector4& vec0,const Vector4& vec1)
{
	Vector4 rv1=recipPerElem(vec1);
    return mulPerElem(vec0,rv1);
}

inline const Vector4 recipPerElem(const Vector4& vec)
{
	float32x4_t vec128=vec.get128();
	float32x4_t rvec128=vrecpeq_f32(vec128); //estimate
	rvec128=vmulq_f32(vrecpsq_f32(vec128,rvec128),rvec128); //iteration 1
	//rvec128=vmulq_f32(vrecpsq_f32(vec128,rvec128),rvec128); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
	return Vector4(rvec128);
}

inline const Vector4 sqrtPerElem(const Vector4& vec)
{
	float32x4_t vec128=vec.get128();
	float32x4_t rsqvec128=vrsqrteq_f32(vec128); //estimate
	rsqvec128=vmulq_f32(rsqvec128,vrsqrtsq_f32(vmulq_f32(vec128,rsqvec128),rsqvec128)); //iteration 1
	//rsqvec128=vmulq_f32(rsqvec128,vrsqrtsq_f32(vmulq_f32(vec128,rsqvec128),rsqvec128)); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
    return Vector4(vmulq_f32(vec128,rsqvec128));
}

inline const Vector4 rsqrtPerElem(const Vector4& vec)
{
	float32x4_t vec128=vec.get128();
	float32x4_t rsqvec128=vrsqrteq_f32(vec128); //estimate
	rsqvec128=vmulq_f32(rsqvec128,vrsqrtsq_f32(vmulq_f32(vec128,rsqvec128),rsqvec128)); //iteration 1
	//rsqvec128=vmulq_f32(rsqvec128,vrsqrtsq_f32(vmulq_f32(vec128,rsqvec128),rsqvec128)); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
	return Vector4(rsqvec128);
}

inline const Vector4 absPerElem(const Vector4& vec)
{
    return Vector4(vabsq_f32(vec.get128()));
}

inline const Vector4 copySignPerElem(const Vector4& vec0,const Vector4& vec1)
{
	uint32x4_t msk=vcltq_f32(vec1.get128(),vdupq_n_f32(0.f));
	float32x4_t absvec0128=vabsq_f32(vec0.get128());
    return Vector4(vbslq_f32(msk,vnegq_f32(absvec0128),absvec0128));
}

inline const Vector4 maxPerElem(const Vector4& vec0,const Vector4& vec1)
{
    return Vector4(vmaxq_f32(vec0.get128(),vec1.get128()));
}

inline float maxElem(const Vector4& vec)
{
	float32x4_t vec128=vec.get128();
	float32x2_t vec128xy=vget_low_f32(vec128);
	float32x2_t vec128zw=vget_high_f32(vec128);
	float32x2_t vec64max=vmax_f32(vec128xy,vec128zw);
	float32x2_t vec64max1=vdup_n_f32(vget_lane_f32(vec64max,0));
	float32x2_t vec64max2=vdup_n_f32(vget_lane_f32(vec64max,1));
    return vget_lane_f32(vmax_f32(vec64max1,vec64max2),0);
}

inline const Vector4 minPerElem(const Vector4& vec0,const Vector4& vec1)
{
    return Vector4(vminq_f32(vec0.get128(),vec1.get128()));
}

inline float minElem(const Vector4& vec)
{
	float32x4_t vec128=vec.get128();
	float32x2_t vec128xy=vget_low_f32(vec128);
	float32x2_t vec128zw=vget_high_f32(vec128);
	float32x2_t vec64min=vmin_f32(vec128xy,vec128zw);
	float32x2_t vec64min1=vdup_n_f32(vget_lane_f32(vec64min,0));
	float32x2_t vec64min2=vdup_n_f32(vget_lane_f32(vec64min,1));
    return vget_lane_f32(vmin_f32(vec64min1,vec64min2),0);
}

inline float sum(const Vector4& vec)
{
	float32x4_t vec128=vec.get128();
	float32x2_t vec64sum=vadd_f32(vget_low_f32(vec128),vget_high_f32(vec128));
	return vget_lane_f32(vpadd_f32(vec64sum,vec64sum),0);
}

inline float dot(const Vector4& vec0,const Vector4& vec1)
{
	float32x4_t vec0128=vec0.get128();
	float32x4_t vec1128=vec1.get128();
	float32x2_t r=vmul_f32(vget_high_f32(vec0128),vget_high_f32(vec1128));
	r=vmla_f32(r,vget_low_f32(vec0128),vget_low_f32(vec1128));
	return (float)vget_lane_f32(vpadd_f32(r,r),0);
}

inline float lengthSqr(const Vector4& vec)
{
	float32x4_t vec128=vec.get128();
	float32x2_t vec64h=vget_high_f32(vec128),vec64l=vget_low_f32(vec128);
	float32x2_t r=vmul_f32(vec64h,vec64h);
	r=vmla_f32(r,vec64l,vec64l);
	return vget_lane_f32(vpadd_f32(r,r),0);
}

inline float length(const Vector4& vec)
{
	float32x2_t lsq=vdup_n_f32((float32_t)lengthSqr(vec));
	float32x2_t l=vrsqrte_f32(lsq); //estimate
	l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 1
	//l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
    return vget_lane_f32(vmul_f32(lsq,l),0);
}

inline const Vector4 normalize(const Vector4& vec)
{
	float32x4_t vec128=vec.get128();
	float32x2_t vec64h=vget_high_f32(vec128),vec64l=vget_low_f32(vec128);
	float32x2_t r=vmul_f32(vec64h,vec64h);
	r=vmla_f32(r,vec64l,vec64l);
	float32x2_t lsq=vdup_n_f32(vget_lane_f32(vpadd_f32(r,r),0));
	float32x2_t l=vrsqrte_f32(lsq); //estimate
	l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 1
	//l=vmul_f32(l,vrsqrts_f32(vmul_f32(lsq,l),l)); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
	return Vector4(vmulq_f32(vec128,vcombine_f32(l,l)));
}

inline const Vector4 select(const Vector4& vec0,const Vector4& vec1,bool select1)
{
	float32x4_t vec0128=vec0.get128();
	float32x4_t vec1128=vec1.get128();
	uint32_t sel=(uint32_t)(-((int32_t)select1));
	uint32x4_t vsel=vdupq_n_u32(sel);
	float32x4_t selres=vbslq_f32(vsel,vec1128,vec0128);
	return Vector4(selres);
}

#ifdef _VECTORMATH_DEBUG

inline void print(const Vector4& vec)
{
    printf("( %f %f %f %f )\n",vec.getX(),vec.getY(),vec.getZ(),vec.getW());
}

inline void print(const Vector4& vec,const char* name)
{
    printf("%s: ( %f %f %f %f )\n",name,vec.getX(),vec.getY(),vec.getZ(),vec.getW());
}

#endif

inline Point3::Point3(const float32x4_t pnt)
{
	p=pnt;
}

inline const float32x4_t Point3::get128() const
{
	return p;
}

inline Point3::Point3(const Point3& pnt)
{
	p=pnt.p;
}

inline Point3::Point3(float _x,float _y,float _z)
{
	p=vsetq_lane_f32((float32_t)_x,p,0);
	p=vsetq_lane_f32((float32_t)_y,p,1);
	p=vsetq_lane_f32((float32_t)_z,p,2);
}

inline Point3::Point3(const Vector3& vec)
{
	p=vec.get128();
}

inline Point3::Point3(float scalar)
{
	p=vmovq_n_f32((float32_t)scalar);
}

inline const Point3 lerp(float t,const Point3& pnt0,const Point3& pnt1)
{
    return (pnt0+((pnt1-pnt0)*t));
}

inline void loadXYZ(Point3& pnt,const float* fptr)
{
	//TODO:optimize
    pnt=Point3(fptr[0],fptr[1],fptr[2]);
	//pnt=Point3(vld1q_f32((const float32_t*)fptr)); //fptr align==16 assumed,fptr mem size==4 assumed,dangerous
}

inline void storeXYZ(const Point3& pnt,float* fptr)
{
	//TODO:optimize
    fptr[0]=pnt.getX();
    fptr[1]=pnt.getY();
    fptr[2]=pnt.getZ();
	//vst1q_f32((float32_t*)fptr,pnt.get128()); //fptr align==16 assumed,fptr mem size==4 assumed,dangerous
}

inline void loadHalfFloats(Point3& vec,const unsigned short* hfptr)
{
#ifdef SN_TARGET_PSP2
	float16_t vhfptr[4] __attribute__((aligned(16)));
	vhfptr[0]=((float16_t*)hfptr)[0];
	vhfptr[1]=((float16_t*)hfptr)[1];
	vhfptr[2]=((float16_t*)hfptr)[2];
	//vhfptr[3]=(float16_t)0;
	vec=Point3(vcvt_f32_f16(vld1_f16(vhfptr)));
#else
	union Data32
	{
        unsigned int u32;
        float f32;
    };
    for(int i=0;i<3;i++)
	{
        unsigned short fp16=hfptr[i];
        unsigned int sign=fp16>>15;
        unsigned int exponent=(fp16>>10)&((1<<5)-1);
        unsigned int mantissa=fp16&((1<<10)-1);
        if(exponent==0)
		{
            //zero
            mantissa=0;
        }
		else if(exponent==31)
		{
            //infinity or nan->infinity
            exponent=255;
			mantissa=0;
        }
		else
		{
            exponent+=127-15;
            mantissa<<=13;
        }
        Data32 d;
        d.u32=(sign<<31)|(exponent<<23)|mantissa;
        vec[i]=d.f32;
    }
#endif
}

inline void storeHalfFloats(const Point3& vec,unsigned short* hfptr)
{
#ifdef SN_TARGET_PSP2
	float16_t vhfptr[4] __attribute__((aligned(16)));
	vst1_f16(vhfptr,vcvt_f16_f32(vec.get128()));
	hfptr[0]=((unsigned short*)vhfptr)[0];
	hfptr[1]=((unsigned short*)vhfptr)[1];
	hfptr[2]=((unsigned short*)vhfptr)[2];
#else
	union Data32
	{
        unsigned int u32;
        float f32;
    };
    for(int i=0;i<3;i++)
	{
        Data32 d;
        d.f32=vec[i];
        unsigned int sign=d.u32>>31;
        unsigned int exponent=(d.u32>>23)&((1<<8)-1);
        unsigned int mantissa=d.u32&((1<<23)-1);
        if(exponent==0)
		{
            //zero or denorm->zero
            mantissa=0;
        }
		else if(exponent==255&&mantissa!=0)
		{
            //nan->infinity
            exponent=31;
            mantissa=0;
        }
		else if(exponent>=127-15+31)
		{
            //overflow or infinity->infinity
            exponent=31;
            mantissa=0;
        }
		else if(exponent<=127-15)
		{
            //underflow->zero
            exponent=0;
            mantissa=0;
        }
		else
		{
            exponent-=127-15;
            mantissa>>=13;
        }
        hfptr[i]=(unsigned short)((sign<<15)|(exponent<<10)|mantissa);
    }
#endif
}

inline Point3& Point3::operator=(const Point3& pnt)
{
	p=pnt.p;
    return *this;
}

inline Point3& Point3::setX(float _x)
{
	p=vsetq_lane_f32((float32_t)_x,p,0);
    return *this;
}

inline float Point3::getX() const
{
    return (float)vgetq_lane_f32(p,0);
}

inline Point3& Point3::setY(float _y)
{
	p=vsetq_lane_f32((float32_t)_y,p,1);
    return *this;
}

inline float Point3::getY() const
{
    return (float)vgetq_lane_f32(p,1);
}

inline Point3& Point3::setZ(float _z)
{
	p=vsetq_lane_f32((float32_t)_z,p,2);
    return *this;
}

inline float Point3::getZ() const
{
    return (float)vgetq_lane_f32(p,2);
}

inline Point3& Point3::setElem(int idx,float value)
{
#ifdef SN_TARGET_PSP2
	p=vsetq_lane_f32((float32_t)value,p,idx);
#else
    switch(idx)
    {
        case 0:
            p=vsetq_lane_f32((float32_t)value,p,0);
            break;
        case 1:
            p=vsetq_lane_f32((float32_t)value,p,1);
            break;
        case 2:
            p=vsetq_lane_f32((float32_t)value,p,2);
    }
#endif
    return *this;
}

inline float Point3::getElem(int idx) const
{
#ifdef SN_TARGET_PSP2
    return (float)vgetq_lane_f32(p,idx);
#else
    switch(idx)
    {
        case 0:
            return (float)vgetq_lane_f32(p,0);
            break;
        case 1:
            return (float)vgetq_lane_f32(p,1);
            break;
        case 2:
            return (float)vgetq_lane_f32(p,2);
    }
#endif
}

inline float& Point3::operator[](int idx)
{ //NEON limitation - cannot return reference or pointer to NEON register. use return value for read only
#ifdef SN_TARGET_PSP2
    float vp=(float)vgetq_lane_f32(p,idx);
#else
    float vp;
    switch(idx)
    {
        case 0:
            vp=(float)vgetq_lane_f32(p,0);
            break;
        case 1:
            vp=(float)vgetq_lane_f32(p,1);
            break;
        case 2:
            vp=(float)vgetq_lane_f32(p,2);
    }
#endif
	return vp;
}

inline float Point3::operator[](int idx) const
{
#ifdef SN_TARGET_PSP2
    return (float)vgetq_lane_f32(p,idx);
#else
    switch(idx)
    {
        case 0:
            return (float)vgetq_lane_f32(p,0);
            break;
        case 1:
            return (float)vgetq_lane_f32(p,1);
            break;
        case 2:
            return (float)vgetq_lane_f32(p,2);
    }
#endif
}

inline const Vector3 Point3::operator-(const Point3& pnt) const
{
    return Vector3(vsubq_f32(p,pnt.p));
}

inline const Point3 Point3::operator+(const Vector3& vec) const
{
	return Point3(vaddq_f32(p,vec.get128()));
}

inline const Point3 Point3::operator-(const Vector3& vec) const
{
    return Point3(vsubq_f32(p,vec.get128()));
}

inline Point3& Point3::operator+=(const Vector3& vec)
{
    *this=*this+vec;
    return *this;
}

inline Point3& Point3::operator-=(const Vector3& vec)
{
    *this=*this-vec;
    return *this;
}

inline const Point3 mulPerElem(const Point3& pnt0,const Point3& pnt1)
{
    return Point3(vmulq_f32(pnt0.get128(),pnt1.get128()));
}

inline const Point3 divPerElem(const Point3& pnt0,const Point3& pnt1)
{
	Point3 rp1=recipPerElem(pnt1);
    return Point3(mulPerElem(pnt0,rp1));
}

inline const Point3 recipPerElem(const Point3& pnt)
{
	float32x4_t pnt128=pnt.get128();
	float32x4_t rpnt128=vrecpeq_f32(pnt128); //estimate
	rpnt128=vmulq_f32(vrecpsq_f32(pnt128,rpnt128),rpnt128); //iteration 1
	//rpnt128=vmulq_f32(vrecpsq_f32(pnt128,rpnt128),rpnt128); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
    return Point3(rpnt128);
}

inline const Point3 sqrtPerElem(const Point3& pnt)
{
	float32x4_t pnt128=pnt.get128();
	float32x4_t rsqpnt128=vrsqrteq_f32(pnt128); //estimate
	rsqpnt128=vmulq_f32(rsqpnt128,vrsqrtsq_f32(vmulq_f32(pnt128,rsqpnt128),rsqpnt128)); //iteration 1
	//rsqpnt128=vmulq_f32(rsqpnt128,vrsqrtsq_f32(vmulq_f32(pnt128,rsqpnt128),rsqpnt128)); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
    return Point3(vmulq_f32(pnt128,rsqpnt128));
}

inline const Point3 rsqrtPerElem(const Point3& pnt)
{
	float32x4_t pnt128=pnt.get128();
	float32x4_t rsqpnt128=vrsqrteq_f32(pnt128); //estimate
	rsqpnt128=vmulq_f32(rsqpnt128,vrsqrtsq_f32(vmulq_f32(pnt128,rsqpnt128),rsqpnt128)); //iteration 1
	//rsqpnt128=vmulq_f32(rsqpnt128,vrsqrtsq_f32(vmulq_f32(pnt128,rsqpnt128),rsqpnt128)); //iteration 2
	//iterations 1,2 may be commented out,if precision after estimate is enough
	return Point3(rsqpnt128);
}

inline const Point3 absPerElem(const Point3& pnt)
{
    return Point3(vabsq_f32(pnt.get128()));
}

inline const Point3 copySignPerElem(const Point3& pnt0,const Point3& pnt1)
{
    uint32x4_t msk=vcltq_f32(pnt1.get128(),vdupq_n_f32(0.f));
	float32x4_t abspnt0128=vabsq_f32(pnt0.get128());
	return Point3(vbslq_f32(msk,vnegq_f32(abspnt0128),abspnt0128));
}

inline const Point3 maxPerElem(const Point3& pnt0,const Point3& pnt1)
{
    return Point3(vmaxq_f32(pnt0.get128(),pnt1.get128()));
}

inline float maxElem(const Point3& pnt)
{
	float32x4_t pnt128=pnt.get128();
	float32x2_t pnt128xy=vget_low_f32(pnt128);
	float32x2_t pnt128zz=vdup_n_f32(vgetq_lane_f32(pnt128,2));
	float32x2_t pnt64max=vmax_f32(pnt128xy,pnt128zz);
	float32x2_t pnt64max1=vdup_n_f32(vget_lane_f32(pnt64max,0));
	float32x2_t pnt64max2=vdup_n_f32(vget_lane_f32(pnt64max,1));
    return vget_lane_f32(vmax_f32(pnt64max1,pnt64max2),0);
}

inline const Point3 minPerElem(const Point3& pnt0,const Point3& pnt1)
{
    return Point3(vminq_f32(pnt0.get128(),pnt1.get128()));
}

inline float minElem(const Point3& pnt)
{
	float32x4_t pnt128=pnt.get128();
	float32x2_t pnt128xy=vget_low_f32(pnt128);
	float32x2_t pnt128zz=vdup_n_f32(vgetq_lane_f32(pnt128,2));
	float32x2_t pnt64min=vmin_f32(pnt128xy,pnt128zz);
	float32x2_t pnt64min1=vdup_n_f32(vget_lane_f32(pnt64min,0));
	float32x2_t pnt64min2=vdup_n_f32(vget_lane_f32(pnt64min,1));
    return vget_lane_f32(vmin_f32(pnt64min1,pnt64min2),0);
}

inline float sum(const Point3& pnt)
{
	float32x4_t pnt128=pnt.get128();
	pnt128=vsetq_lane_f32(0.f,pnt128,3);
	float32x2_t pnt64sum=vadd_f32(vget_low_f32(pnt128),vget_high_f32(pnt128));
	return vget_lane_f32(vpadd_f32(pnt64sum,pnt64sum),0);
}

inline const Point3 scale(const Point3& pnt,float scaleVal)
{
	return Point3(vmulq_n_f32(pnt.get128(),(float32_t)scaleVal));
}

inline const Point3 scale(const Point3& pnt,const Vector3& scaleVec)
{
	return Point3(vmulq_f32(pnt.get128(),scaleVec.get128()));
}

inline float projection(const Point3& pnt,const Vector3& unitVec)
{
	float32x4_t pnt128=pnt.get128();
	float32x4_t vec128=unitVec.get128();
	float32x2_t pntxy=vget_low_f32(pnt128);
	float32x2_t pntzz=vdup_n_f32(vgetq_lane_f32(pnt128,2));
	float32x2_t vecxy=vget_low_f32(vec128);
	float32x2_t veczz=vdup_n_f32(vgetq_lane_f32(vec128,2));
	float32x2_t pntvecxy=vmul_f32(pntxy,vecxy);
	float32x2_t pntvecxxyy=vpadd_f32(pntvecxy,pntvecxy);
	float32x2_t pntvecxxyyzz=vmla_f32(pntvecxxyy,pntzz,veczz);
	return vget_lane_f32(pntvecxxyyzz,0);
}

inline float distSqrFromOrigin(const Point3& pnt)
{
    return lengthSqr(Vector3(pnt));
}

inline float distFromOrigin(const Point3& pnt)
{
    return length(Vector3(pnt));
}

inline float distSqr(const Point3& pnt0,const Point3& pnt1)
{
    return lengthSqr(pnt1-pnt0);
}

inline float dist(const Point3& pnt0,const Point3& pnt1)
{
    return length(pnt1-pnt0);
}

inline const Point3 select(const Point3& pnt0,const Point3& pnt1,bool select1)
{
	float32x4_t pnt0128=pnt0.get128();
	float32x4_t pnt1128=pnt1.get128();
	uint32_t sel=(uint32_t)(-((int32_t)select1));
	uint32x4_t vsel=vdupq_n_u32(sel);
	float32x4_t selres=vbslq_f32(vsel,pnt1128,pnt0128);
	return Point3(selres);
}

#ifdef _VECTORMATH_DEBUG

inline void print(const Point3& pnt)
{
    printf("( %f %f %f )\n",pnt.getX(),pnt.getY(),pnt.getZ());
}

inline void print(const Point3& pnt,const char* name)
{
    printf("%s: ( %f %f %f )\n",name,pnt.getX(),pnt.getY(),pnt.getZ());
}

#endif

}
}

#endif

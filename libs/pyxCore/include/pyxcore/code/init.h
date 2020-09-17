#if defined(GLSL)
#ifdef VERSION
VERSION
#endif
#define BLENDWEIGHT
#define TARGET
#define vHPosition gl_Position
#ifdef VP
#ifdef ES2
#define out varying
#define in attribute
#endif
#else //FP
#ifdef ES2
#define in varying
#define oColor gl_FragColor
#define tex2D(s,uv) texture2D(s,uv,MipBias)
#define tex2Dproj texture2DProj
#define texCUBE textureCube
#else
#define tex2D(s,uv) texture(s,uv,MipBias)
#define tex2Dproj textureProj
#define texCUBE textureCube
#endif
#if defined(ALPHA_TEST_ENABLE)
#define FP_ALPHA_TEST
#endif
#endif

#define frac fract
#define float2 vec2
#define float3 vec3
#define float4 vec4
#define float4x4 mat4
#define float3x3 mat3
#define float2x2 mat2
#define FLOAT float
#define FLOAT2 vec2
#define FLOAT3 vec3
#define FLOAT4 vec4
#define FLOAT4x4 mat4
#define FLOAT3x3 mat3
#define FLOAT2x2 mat2

#elif defined CGFX		//Maya CgFx
#define HALFTONE
#define LIGHTCOLOR Diffuse
#define FP
#define VP
#define FX
#undef SKINNING_ENABLE
#ifndef OGL	
#define TANGENT TEXCOORD6
#define BINORMAL TEXCOORD7
#endif
#define FLOAT float
#define FLOAT2 float2
#define FLOAT3 float3
#define FLOAT4 float4
#define FLOAT4x4 float4x4
#define FLOAT3x3 float3x3
#define FLOAT2x2 float2x2
#endif


///////////////////////////////////
#if defined(GLSL)
#define MUL(A,B) ((B)*(A))
#elif defined(DIRECT3D_VERSION)
#define MUL(A,B) mul(B,A)
#else
#define MUL(A,B) mul(A,B)
#endif

#define lerp(A,B,t) ((B*t)+(A*(1.0-t)))

#if defined(GLSL)
#define MAT3(v) float3x3(v)
#else
#define MAT3(v) (float3x3)v
#endif

#if  !(defined(ES2) || defined(ES3))
#define mediump
#define lowp
#endif

#undef HALFTONE	

///////////////////////////////////				
#ifdef SKINNING_ENABLE
#define WORLD_COORDINATE
#if defined(GLSL)
#define int_cast(i) int(i)
#else
#define int_cast(i) i
#endif

#if ( BONE_INFLUENCE == 4 )
#define WEIGHT_FLOAT float4
#define WEIGHT_INT int4
#define ENABLE_WEIGHTS
#define IDX(i) int_cast(idx.i)
#elif ( BONE_INFLUENCE == 3 )
#define WEIGHT_FLOAT float3
#define WEIGHT_INT int3
#define ENABLE_WEIGHTS
#define IDX(i) int_cast(idx.i)
#elif ( BONE_INFLUENCE == 2 )
#define WEIGHT_FLOAT float2
#define WEIGHT_INT int2
#define ENABLE_WEIGHTS
#define IDX(i) int_cast(idx.i)
#elif ( BONE_INFLUENCE == 1 )
#define WEIGHT_FLOAT float
#define WEIGHT_INT int
#define IDX int_cast(idx)
#elif ( BONE_INFLUENCE == 0 )
#define IDX 0
#endif
#endif

#if defined CGFX
#define SEMAMTICS_POSITION :POSITION,
#define SEMAMTICS_NORMAL : NORMAL,
#define SEMAMTICS_TANGENT : TANGENT,
#define SEMAMTICS_BINORMAL : BINORMAL,
#define SEMAMTICS_TEXCOORD0 : TEXCOORD0,
#define SEMAMTICS_TEXCOORD1 : TEXCOORD1,
#define SEMAMTICS_TEXCOORD2 : TEXCOORD2,
#define SEMAMTICS_COLOR0 : COLOR0,
#define SEMAMTICS_BLENDWEIGHT : BLENDWEIGHT,
#define SEMAMTICS_BLENDINDICES : BLENDINDICES,
#define SEMAMTICS_PSIZE : PSIZE,
#else
#define SEMAMTICS_POSITION;
#define SEMAMTICS_NORMAL;
#define SEMAMTICS_TANGENT;
#define SEMAMTICS_BINORMAL;
#define SEMAMTICS_TEXCOORD0;
#define SEMAMTICS_TEXCOORD1;
#define SEMAMTICS_TEXCOORD2;
#define SEMAMTICS_COLOR0;
#define SEMAMTICS_BLENDWEIGHT;
#define SEMAMTICS_BLENDINDICES;
#define SEMAMTICS_PSIZE;
#endif

#define WRAMMODE_REPEAT 0
#define WRAMMODE_CLAMP 1
#define WRAMMODE_MIRROR 2

#define NoMapChannel  0
#define DiffuseMapAlphaChannel 1	
#define DiffuseMapRedChannel 2
#define NormalMapAlphaChannel 3
#define NormalMapRedChannel 4
#define LightMapAlphaChannel 5
#define LightMapRedChannel 6
#define VertexColorRedChanel 7
#define VertexColorAlphaChanel 8
#define OverlayDiffuseMapAlphaChannel 9
#define OverlayDiffuseMapRedChannel 10
#define OverlayNormalMapAlphaChannel 11
#define OverlayNormalMapRedChannel 12

#define AMBIENT_None 0
#define AMBIENT_Ambient 1
#define AMBIENT_Hemisphere 2

#define OPERATION_COL 0
#define OPERATION_ADD 1
#define OPERATION_SUB 2
#define OPERATION_MUL 3

#define LocalSpace 0
#define WorldSpace 1
#define ViewSpace 2
#define ProjectionSpace 3

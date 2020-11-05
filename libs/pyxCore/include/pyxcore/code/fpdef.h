//#ifdef FP

#if defined FX
	#define FX_WidgetSlider(min,max,step,name,def) < string UIWidget = "slider"; float UIMin = min; float UIMax = max; float UIStep = step; string UIName = name; > = def
	#define FX_WidgetRGBA(name, r,g,b,a) < string Object = name; string UIName = name; string UIWidget = "Color"; > = { r,g,b,a }
	#define FX_WidgetRGB(name, r,g,b) < string Object = name; string UIName = name; string UIWidget = "Color"; > = { r,g,b }
	#define FX_WidgetFloat3(name, x,y,z) <string UIWidget = "None"; string UIName = name;> = {x,y,z}
	#define FX_WidgetDirLamp(name, x,y,z) <string Object = "directionallight"; string UIName = name; string Space = "World";> = {x,y,z}
	#define FX_WidgetTexture(file,name,type) <string ResourceName = file; string UIName = name; string ResourceType = type;>
	#define FX_WidgetShadowBuffer <float2 Dimensions = { 1024, 1024 }; string Format = ("r32f"); string UIWidget = "None";>
	#ifndef FX_WidgetSpinner
	#define FX_WidgetSpinner(name,min,max,def) <string UIType = "FloatSpinner"; string UIName = name; float UIMin = min; float UIMax = max;> = def
	#define FX_WorldPos(name,x,y,z,w) < string Object = name; string UIName = name; string Space = "World"; > = { x,y,z,w }
#endif
	#ifdef DIRECT3D_VERSION	
		#define NearestMipmapNearest Point
		#define LinearMipmapLinear Linear
		#define Repeat Wrap
		#define ClampToEdge Clamp
		#define FX_SamplerState(tex, minf,magf,wraps,wrapt) =sampler_state{Texture = <tex>; MinFilter = minf; MipFilter = minf; MagFilter = magf; AddressU = wraps; AddressV = wrapt;}
		#define FX_SamplerStateCUBE(tex, minf,magf,wraps,wrapt,wrapr) =sampler_state{Texture = <tex>; MinFilter = minf; MipFilter = minf; MagFilter = magf; AddressU = wraps; AddressV = wrapt; AddressW = wrapr;}
	#else
		#define FX_SamplerState(tex, minf,magf,wraps,wrapt) =sampler_state{Texture = <tex>; MinFilter = minf; MagFilter = magf; MipMapLodBias = MIPMAP_LOD_BIAS; WrapS = wraps; WrapT = wrapt;}
		#define FX_SamplerStateCUBE(tex, minf,magf,wraps,wrapt,wrapr) =sampler_state{Texture = <tex>; MinFilter = minf; MagFilter = magf; MipMapLodBias = MIPMAP_LOD_BIAS; WrapS = wraps; WrapT = wrapt; WrapR = wrapr;}
	#endif
	#define SEMANTICS_LIGHTCOLOR :LIGHTCOLOR
	#define SEMANTICS_Direction :Direction
	#define SEMANTICS_POWER :POWER
#else
	#define FX_WidgetSlider(min,max,step,name,def)
	#define FX_WidgetRGBA(name, r,g,b,a)
	#define FX_WidgetRGB(name, r,g,b)
	#define FX_WidgetFloat3(name, x,y,z)
	#define FX_WidgetDirLamp(name, x,y,z)
	#define FX_WidgetTexture(file,name,type)
	#define FX_WidgetShadowBuffer
	#ifndef FX_WidgetSpinner
	#define FX_WidgetSpinner(name,min,max,def)
	#endif
	#define FX_SamplerState(tex, minf,magf,wraps,wrapt)
	#define FX_SamplerStateCUBE(tex, minf,magf,wraps,wrapt,wrapr)
	#define SEMANTICS_LIGHTCOLOR
	#define SEMANTICS_Direction
	#define SEMANTICS_POWER
	#define FX_WorldPos(name,x,y,z,w)
#endif

#ifdef POINT_SAMPLING
#define COLORSAMPLER_MIN_FILTER NearestMipmapNearest
#define COLORSAMPLER_MAG_FILTER Point
#else
#define COLORSAMPLER_MIN_FILTER LinearMipmapLinear
#define COLORSAMPLER_MAG_FILTER Linear
#endif

#if defined(ES2) || defined(ES3)
precision mediump float;
#endif

#ifdef HALFTONE	
	//uniform FLOAT tone
	//#ifndef TARGET
	//	 = 1.0
	//#endif
	//;
#endif

#ifdef FP_ALPHA_TEST
	uniform FLOAT AlphaRef;
#endif


#ifdef LOCAL_INTENSITY
	uniform FLOAT LocalIntensity FX_WidgetSlider(0.0, 100.0, 0.1, "Local Intensity", 1.0)
#endif


#if (NUM_DIR_LAMP >= 1)
	uniform FLOAT4 DirLamp01Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("DirLamp01Color", 0, 0, 0, 1);
#ifndef TARGET
	uniform FLOAT DirLamp01Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "DirLamp01 Intensity", 1.0);
#endif
	uniform FLOAT3 DirLamp01Dir SEMANTICS_Direction FX_WidgetDirLamp("DirLamp01Direction", 0.0, 0.0, 1.0);
#endif
#if (NUM_DIR_LAMP >= 2)
	uniform FLOAT4 DirLamp02Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("DirLamp02Color", 0, 0, 0, 1);
#ifndef TARGET
	uniform FLOAT DirLamp02Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "DirLamp02 Intensity", 1.0);
#endif
	uniform FLOAT3 DirLamp02Dir SEMANTICS_Direction FX_WidgetDirLamp("DirLamp02Direction", 0.0, 0.0, 1.0);
#endif
#if (NUM_DIR_LAMP >= 3)
	uniform FLOAT4 DirLamp03Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("DirLamp03Color", 0, 0, 0, 1);
#ifndef TARGET
	uniform FLOAT DirLamp03Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "DirLamp03 Intensity", 1.0);
#endif
	uniform FLOAT3 DirLamp03Dir SEMANTICS_Direction FX_WidgetDirLamp("DirLamp03Direction", 0.0, 0.0, 1.0);
#endif


#if(NUM_POINT_LAMP >= 1)
	uniform FLOAT4 PointLamp01Pos FX_WorldPos("PointLamp01Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp01Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp01Intensity", 1.0);
	uniform FLOAT PointLamp01Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp01Range", 1000.0);
#else
#define PointLamp01Range PointLamp01Pos.w
#endif
	uniform FLOAT4 PointLamp01Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("PointLamp01Color", 0, 0, 0, 1);
#endif
#if(NUM_POINT_LAMP >= 2)
	uniform FLOAT4 PointLamp02Pos FX_WorldPos("PointLamp02Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp02Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp02Intensity", 1.0);
	uniform FLOAT PointLamp02Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp02Range", 1000.0);
#else
#define PointLamp02Range PointLamp02Pos.w
#endif
	uniform FLOAT4 PointLamp02Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("PointLamp02Color", 0, 0, 0, 1);
#endif
#if(NUM_POINT_LAMP >= 3)
	uniform FLOAT4 PointLamp03Pos FX_WorldPos("PointLamp03Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp03Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp03Intensity", 1.0);
	uniform FLOAT PointLamp03Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp03Range", 1000.0);
#else
#define PointLamp03Range PointLamp03Pos.w
#endif
	uniform FLOAT4 PointLamp03Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("PointLamp03Color", 0, 0, 0, 1);
#endif
#if(NUM_POINT_LAMP >= 4)
	uniform FLOAT4 PointLamp04Pos FX_WorldPos("PointLamp04Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp04Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp04Intensity", 1.0);
	uniform FLOAT PointLamp04Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp04Range", 1000.0);
#else
#define PointLamp04Range PointLamp04Pos.w
#endif
	uniform FLOAT4 PointLamp04Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("PointLamp04Color", 0, 0, 0, 1);
#endif
#if(NUM_POINT_LAMP >= 5)
	uniform FLOAT4 PointLamp05Pos FX_WorldPos("PointLamp05Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp05Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp05Intensity", 1.0);
	uniform FLOAT PointLamp05Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp05Range", 1000.0);
#else
#define PointLamp05Range PointLamp05Pos.w
#endif
	uniform FLOAT4 PointLamp05Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("PointLamp05Color", 0, 0, 0, 1);
#endif
#if(NUM_POINT_LAMP >= 6)
	uniform FLOAT4 PointLamp06Pos FX_WorldPos("PointLamp06Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp06Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp06Intensity", 1.0);
	uniform FLOAT PointLamp06Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp06Range", 1000.0);
#else
#define PointLamp06Range PointLamp06Pos.w
#endif
	uniform FLOAT4 PointLamp06Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("PointLamp06Color", 0, 0, 0, 1);
#endif
#if(NUM_POINT_LAMP >= 7)
	uniform FLOAT4 PointLamp07Pos FX_WorldPos("PointLamp07Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp07Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp07Intensity", 1.0);
	uniform FLOAT PointLamp07Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp07Range", 1000.0);
#else
#define PointLamp07Range PointLamp07Pos.w
#endif
	uniform FLOAT4 PointLamp07Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("PointLamp07Color", 0, 0, 0, 1);
#endif


#if(NUM_SPOT_LAMP >= 1)
	uniform FLOAT4 SpotLamp01Pos FX_WorldPos("SpotLamp01Pos", 0.0, 0.0, 0.0, 100.0);
	uniform FLOAT4 SpotLamp01Dir SEMANTICS_Direction FX_WidgetDirLamp("SpotLamp01Direction", 0.0, 0.0, 1.0);
	uniform FLOAT4 SpotLamp01Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("SpotLamp01Color", 0, 0, 0, 1);
#ifndef TARGET
	uniform FLOAT SpotLamp01Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "SpotLamp01Intensity", 1.0);
	uniform FLOAT SpotLamp01Range FX_WidgetSlider(1.0, 1000.0, 1.0, "SpotLamp01Range", 1000.0);
	uniform FLOAT SpotLamp01Angle FX_WidgetSlider(1.0, 90.0, 1.0, "SpotLamp01Angle", 30.0);
#else
	#define SpotLamp01Range SpotLamp01Pos.w
	#define SpotLamp01Angle SpotLamp01Dir.w
#endif
#endif
#if(NUM_SPOT_LAMP >= 2)
	uniform FLOAT4 SpotLamp02Pos FX_WorldPos("SpotLamp02Pos", 0.0, 0.0, 0.0, 100.0);
	uniform FLOAT4 SpotLamp02Dir SEMANTICS_Direction FX_WidgetDirLamp("SpotLamp02Direction", 0.0, 0.0, 1.0);
	uniform FLOAT4 SpotLamp02Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("SpotLamp02Color", 0, 0, 0, 1);
#ifndef TARGET
	uniform FLOAT SpotLamp02Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "SpotLamp02Intensity", 1.0);
	uniform FLOAT SpotLamp02Range FX_WidgetSlider(1.0, 1000.0, 1.0, "SpotLamp02Range", 1000.0);
	uniform FLOAT SpotLamp02Angle FX_WidgetSlider(1.0, 90.0, 1.0, "SpotLamp02Angle", 30.0);
#else
#define SpotLamp02Range SpotLamp02Pos.w
#define SpotLamp02Angle SpotLamp02Dir.w
#endif
#endif
#if(NUM_SPOT_LAMP >= 3)
	uniform FLOAT4 SpotLamp03Pos FX_WorldPos("SpotLamp03Pos", 0.0, 0.0, 0.0, 100.0);
	uniform FLOAT4 SpotLamp03Dir SEMANTICS_Direction FX_WidgetDirLamp("SpotLamp03Direction", 0.0, 0.0, 1.0);
	uniform FLOAT4 SpotLamp03Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("SpotLamp03Color", 0, 0, 0, 1);
#ifndef TARGET
	uniform FLOAT SpotLamp03Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "SpotLamp03Intensity", 1.0);
	uniform FLOAT SpotLamp03Range FX_WidgetSlider(1.0, 1000.0, 1.0, "SpotLamp03Range", 1000.0);
	uniform FLOAT SpotLamp03Angle FX_WidgetSlider(1.0, 90.0, 1.0, "SpotLamp03Angle", 30.0);
#else
#define SpotLamp03Range SpotLamp03Pos.w
#define SpotLamp03Angle SpotLamp03Dir.w
#endif
#endif
#if(NUM_SPOT_LAMP >= 4)
	uniform FLOAT4 SpotLamp04Pos FX_WorldPos("SpotLamp04Pos", 0.0, 0.0, 0.0, 100.0);
	uniform FLOAT4 SpotLamp04Dir SEMANTICS_Direction FX_WidgetDirLamp("SpotLamp04Direction", 0.0, 0.0, 1.0);
	uniform FLOAT4 SpotLamp04Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("SpotLamp04Color", 0, 0, 0, 1);
#ifndef TARGET
	uniform FLOAT SpotLamp04Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "SpotLamp04Intensity", 1.0);
	uniform FLOAT SpotLamp04Range FX_WidgetSlider(1.0, 1000.0, 1.0, "SpotLamp04Range", 1000.0);
	uniform FLOAT SpotLamp04Angle FX_WidgetSlider(1.0, 90.0, 1.0, "SpotLamp04Angle", 30.0);
#else
#define SpotLamp04Range SpotLamp04Pos.w
#define SpotLamp04Angle SpotLamp04Dir.w
#endif
#endif
#if(NUM_SPOT_LAMP >= 5)
	uniform FLOAT4 SpotLamp05Pos FX_WorldPos("SpotLamp05Pos", 0.0, 0.0, 0.0, 100.0);
	uniform FLOAT4 SpotLamp05Dir SEMANTICS_Direction FX_WidgetDirLamp("SpotLamp05Direction", 0.0, 0.0, 1.0);
	uniform FLOAT4 SpotLamp05Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("SpotLamp05Color", 0, 0, 0, 1);
#ifndef TARGET
	uniform FLOAT SpotLamp05Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "SpotLamp05Intensity", 1.0);
	uniform FLOAT SpotLamp05Range FX_WidgetSlider(1.0, 1000.0, 1.0, "SpotLamp05Range", 1000.0);
	uniform FLOAT SpotLamp05Angle FX_WidgetSlider(1.0, 90.0, 1.0, "SpotLamp05Angle", 30.0);
#else
#define SpotLamp05Range SpotLamp05Pos.w
#define SpotLamp05Angle SpotLamp05Dir.w
#endif
#endif
#if(NUM_SPOT_LAMP >= 6)
	uniform FLOAT4 SpotLamp06Pos FX_WorldPos("SpotLamp06Pos", 0.0, 0.0, 0.0, 100.0);
	uniform FLOAT4 SpotLamp06Dir SEMANTICS_Direction FX_WidgetDirLamp("SpotLamp06Direction", 0.0, 0.0, 1.0);
	uniform FLOAT4 SpotLamp06Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("SpotLamp06Color", 0, 0, 0, 1);
#ifndef TARGET
	uniform FLOAT SpotLamp06Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "SpotLamp06Intensity", 1.0);
	uniform FLOAT SpotLamp06Range FX_WidgetSlider(1.0, 1000.0, 1.0, "SpotLamp06Range", 1000.0);
	uniform FLOAT SpotLamp06Angle FX_WidgetSlider(1.0, 90.0, 1.0, "SpotLamp06Angle", 30.0);
#else
#define SpotLamp06Range SpotLamp06Pos.w
#define SpotLamp06Angle SpotLamp06Dir.w
#endif
#endif
#if(NUM_SPOT_LAMP >= 7)
	uniform FLOAT4 SpotLamp07Pos FX_WorldPos("SpotLamp07Pos", 0.0, 0.0, 0.0, 100.0);
	uniform FLOAT4 SpotLamp07Dir SEMANTICS_Direction FX_WidgetDirLamp("SpotLamp07Direction", 0.0, 0.0, 1.0);
	uniform FLOAT4 SpotLamp07Color SEMANTICS_LIGHTCOLOR FX_WidgetRGBA("SpotLamp07Color", 0, 0, 0, 1);
#ifndef TARGET
	uniform FLOAT SpotLamp07Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "SpotLamp07Intensity", 1.0);
	uniform FLOAT SpotLamp07Range FX_WidgetSlider(1.0, 1000.0, 1.0, "SpotLamp07Range", 1000.0);
	uniform FLOAT SpotLamp07Angle FX_WidgetSlider(1.0, 90.0, 1.0, "SpotLamp07Angle", 30.0);
#else
#define SpotLamp07Range SpotLamp07Pos.w
#define SpotLamp07Angle SpotLamp07Dir.w
#endif
#endif
	
///////Ambient Light//////////////////////////
#if (AMBIENT_TYPE > AMBIENT_None)
	uniform FLOAT3 AmbientColor FX_WidgetFloat3("AmbientColor", 0.1, 0.2, 0.5);
	#ifndef TARGET
	uniform FLOAT AmbientIntensity FX_WidgetSlider(0.0, 100.0, 0.1, "Ambient Intensity", 1.0);
	#endif
	#if (AMBIENT_TYPE == AMBIENT_Hemisphere)
		uniform FLOAT3 GroundColor FX_WidgetFloat3("GroundColor", 0.5, 0.2, 0.0);
		uniform FLOAT3 SkyDir FX_WidgetFloat3("SunDirection", 0.0, 1.0, 0.0);
	#endif
#endif

//////// COLOR ///////////////////////////////
#ifdef CGFX
#define MIPMAP_LOD_BIAS -2.0
#endif

#ifdef COLOR_SAMPLER_ENABLE
#ifdef FX
uniform texture ColorTexture : DiffuseMap FX_WidgetTexture("default_color.dds", "Diffuse Texture", "2D");
#endif
uniform sampler2D ColorSampler FX_SamplerState(ColorTexture, COLORSAMPLER_MIN_FILTER, COLORSAMPLER_MAG_FILTER, COLOR_SAMPLER_WrapS, COLOR_SAMPLER_WrapT);
#endif

//////////////// diffuse //////////////////////
#ifdef DIFFUSE_COLOR_ENABLE
	#ifdef TARGET
		uniform mediump FLOAT4 DiffuseColor;
	#else
		uniform FLOAT3 DiffuseColor FX_WidgetRGB("DiffuseColor", 1.0, 1.0, 1.0);
		#ifdef ALPHA_BLENDING_ENABLE
		uniform FLOAT DiffuseAlpha FX_WidgetSlider(0.0, 1.0, 0.05, "Diffuse Alpha", 1.0);
		#endif
	#endif
#endif


//////////////// Normal //////////////////////
#ifdef NORMAL_SAMPLER_ENABLE
#ifdef FX
uniform texture NormalTexture FX_WidgetTexture("default_bump_normal.dds", "Normal Texture", "2D");
#endif
uniform sampler2D NormalSampler FX_SamplerState(NormalTexture, LinearMipmapLinear, Linear, NORMAL_SAMPLER_WrapS, NORMAL_SAMPLER_WrapT);
#endif

#ifdef NORMAL_MAP_ENABLE
uniform FLOAT Bumpiness FX_WidgetSlider(0.0, 10.0, 0.01, "Bumpiness", 1.0);
#endif

//////////////// Specular ////////////////////
#ifdef SPECULAR_ENABLE
#ifndef SPECULAR_MAP_ENABLE
uniform mediump FLOAT SpecularStrength FX_WidgetSlider(0.0, 5.0, 0.05, "Specular Strength", 1.0);
#endif
#ifndef CLUT_LIGHT_ENABLE
uniform mediump FLOAT SpecularPower FX_WidgetSlider(1.0, 256.0, 1.0, "Specular Power", 55.0);
#endif
#endif

//////////////// Reflection  /////////////////
#ifdef REFLECTION_ENABLE

#ifdef ENV_SPHERE
	#ifdef FX
	uniform texture EnvsTexture FX_WidgetTexture("Default_sphere.dds", "Sphere Env Texture", "2D");
	#endif
	uniform sampler2D EnvsSampler FX_SamplerState(EnvsTexture, Linear, Linear, ClampToEdge, ClampToEdge);
#else
	#ifdef FX
	uniform texture EnvTexture : ENVIRONMENT FX_WidgetTexture("default_reflection.dds", "Env Texture", "Cube");
	#endif
	uniform samplerCUBE EnvSampler FX_SamplerStateCUBE(EnvTexture, LinearMipmapLinear, Linear, ClampToEdge, ClampToEdge, ClampToEdge);
#endif

uniform FLOAT ReflectionStrength FX_WidgetSlider(0.0, 2.0, 0.05, "Reflection Strength", 0.3);

#ifdef FRESNEL_ENABLE
uniform FLOAT FresnelMin FX_WidgetSlider(0.0, 1.0, 0.05, "Fresnel Reflection Scale", 0.05);
uniform FLOAT FresnelExp SEMANTICS_POWER FX_WidgetSlider(0.0, 5.0, 0.05, "Fresnel Exponent", 3.5);
#endif
#endif

//////////////// VELVET //////////////////////
#ifdef VELVET_ENABLE
uniform FLOAT3 SurfaceColor FX_WidgetRGB("VelvetColor", 0.5, 0.5, 0.5);
uniform FLOAT3 FuzzySpecColor FX_WidgetRGB("VelvetFuzzyColor", 0.7, 0.7, 0.7);
uniform FLOAT3 SubColor FX_WidgetRGB("VelvetUnderColor", 0.2, 0.2, 1.0);
uniform FLOAT RollOff FX_WidgetSlider(0.0, 10.0, 0.05, "VelvetEdgeRolloff", 0.3);
uniform FLOAT VelvetStrength FX_WidgetSlider(0.0, 1.0, 0.05, "VelvetPower", 0.3);
#endif

///////////// Light Map //////////////////////
#ifdef LIGHT_SAMPLER_ENABLE
#ifdef FX
uniform texture LightTexture : DiffuseMap FX_WidgetTexture("default_lightmap.dds", "Light-Map Texture", "2D");
#endif
uniform sampler2D LightSampler FX_SamplerState(LightTexture, LinearMipmapLinear, Linear, LIGHT_SAMPLER_WrapS, LIGHT_SAMPLER_WrapT);
#endif

#ifdef LIGHT_MAP_ENABLE
uniform FLOAT LightMapPower FX_WidgetSlider(0.0, 10.0, 0.01, "Light Map Power", 1.0);
uniform FLOAT LightMapStrength FX_WidgetSlider(0.0, 1.0, 0.01, "Light Map Strength", 1.0);
#endif

///////////// Overlay Color Map //////////////
#ifdef OVERLAY_COLOR_SAMPLER_ENABLE
#ifdef FX
uniform texture OverlayColorTexture FX_WidgetTexture("Default_overlay_color.dds", "Overlay Color Texture", "2D");
#endif
uniform sampler2D OverlayColorSampler FX_SamplerState(OverlayColorTexture, LinearMipmapLinear, Linear, OVERLAY_COLOR_SAMPLER_WrapS, OVERLAY_COLOR_SAMPLER_WrapT);
#endif

///////////// Overlay Normal Map /////////////
#ifdef OVERLAY_NORMAL_SAMPLER_ENABLE
#ifdef FX
uniform texture OverlayNormalTexture FX_WidgetTexture("Default_overlay_normal.dds", "Overlay Normal Texture", "2D");
#endif
uniform sampler2D OverlayNormalSampler FX_SamplerState(OverlayNormalTexture, LinearMipmapLinear, Linear, OVERLAY_NORMAL_SAMPLER_WrapS, OVERLAY_NORMAL_SAMPLER_WrapT);
#endif

#ifdef OVERLAY_NORMAL_MAP_ENABLE
uniform FLOAT OverlayBumpiness FX_WidgetSlider(0.0, 10.0, 0.01, "Overlay Bumpiness", 1.0);
#endif

#ifdef OVERLAY_STRENGTH_ENABLE
uniform FLOAT OverlayStrength FX_WidgetSlider(0.0, 1.0, 0.01, "Overlay Strength", 1.0);
#endif

///////////// Color look up table. ///////////
#ifdef CLUT_MAP_ENABLE
#ifdef FX
uniform texture CLUTTexture FX_WidgetTexture("Default_CLUT.dds", "CLUT Texture", "2D");
#endif
uniform sampler2D CLUTSampler FX_SamplerState(CLUTTexture, Linear, Linear, ClampToEdge, ClampToEdge);
#endif

//////////////// Glass Map ///////////////////
#ifdef GLASS_MAP_ENABLE
#ifdef FX
uniform texture BackBufferTexture FX_WidgetTexture("default_color.dds", "Back Buffer", "2D");
#endif
uniform sampler2D BackBufferSampler FX_SamplerState(BackBufferTexture, Linear, Linear, ClampToEdge, ClampToEdge);
uniform FLOAT GlassStrength FX_WidgetSlider(0.0, 1.0, 0.01, "Glass Strength", 1.0);
uniform FLOAT Curvature FX_WidgetSlider(0.0, 2.0, 0.01, "Glass Curvature", 1.0);
#endif

//////////////// Parallax ////////////////////
#ifdef PARALLAX_MAP_ENABLE
uniform FLOAT ParallaxBiasX FX_WidgetSpinner("Parallax Height", -0.1, 0.1, 0.035);
uniform FLOAT ParallaxBiasY FX_WidgetSpinner("Parallax Scale", -0.1, 0.1, -0.03);
#endif

#ifdef OL_PARALLAX_MAP_ENABLE
uniform FLOAT OlParallaxBiasX FX_WidgetSpinner("Overlay map Parallax Height", -0.1, 0.1, 0.035);
uniform FLOAT OlParallaxBiasY FX_WidgetSpinner("Overlay map Parallax Scale", -0.1, 0.1, -0.03);
#endif

//////////////// Distorton ///////////////////
#if defined(DISTORTION) || defined(NORMAL_DISTORTION)
uniform FLOAT DistortionStrength FX_WidgetSpinner("Distortion Strength", -5.0, 5.0, 1.0);
#endif

//////////////// Fog /////////////////////////
#ifdef FOG
uniform FLOAT3 FogColor FX_WidgetRGB("Fog Color", 1.0, 1.0, 1.0);
#endif

//////////////// SHADOW //////////////////////
#if defined(RECEIVE_SHADOW_ENABLE)
#ifdef FX
	uniform texture ShadowTexture : RENDERCOLORTARGET FX_WidgetShadowBuffer;
	#define  Radius  0.3f
	float2   g_Offset[5] = {
		{+Radius / SHADOW_SIZE, +Radius / SHADOW_SIZE},
		{-Radius / SHADOW_SIZE, -Radius / SHADOW_SIZE},
		{+Radius / SHADOW_SIZE, -Radius / SHADOW_SIZE},
		{-Radius / SHADOW_SIZE, +Radius / SHADOW_SIZE},
		{0, 0}
	};
	float ShadowDensity = 0.0;
#endif				
	uniform sampler2D ShadowSampler FX_SamplerState(ShadowTexture, Linear, Linear, ClampToEdge, ClampToEdge);
	uniform FLOAT ShadowBias FX_WidgetSlider(0.0, 0.1, 0.001, "Shadow Bias", 0.005);
#endif

#ifdef SHADOW_COLOR_ENABLE
	uniform FLOAT4 ShadowColor FX_WidgetRGBA("Shadow Color", 0.0, 0.0, 0.0, 0.2);
#endif


//////////////// COVERMAP ////////////////////
#ifdef COVER_SAMPLER_ENABLE	
#ifdef FX
uniform texture CoverTexture FX_WidgetTexture("Default_Cover.dds", "Cover Texture", "2D");
#endif
uniform sampler2D CoverSampler FX_SamplerState(CoverTexture, Linear, Linear, ClampToEdge, ClampToEdge);
uniform FLOAT CoverAddition FX_WidgetSlider(0.0, 1.0, 0.01, "Cover Addition", 0.0);
#endif

/////////////////// Mip Bias /////////////////
#if defined(SAMPLER_ENABLE) && defined(GLSL)
uniform FLOAT MipBias;
#endif	

/////////////////// Sepia ////////////////////
#ifdef SEPIA
uniform FLOAT SepiaStrength FX_WidgetSpinner("Sepia Strength", 0.0, 1.0, 0.0);
#endif

///////////////// Saturation /////////////////
#ifdef SATURATION
uniform FLOAT Saturation FX_WidgetSpinner("Saturation", 0.0, 1.0, 0.0);
#endif

///////////////// InnerGrow //////////////////
#ifdef GLOW_A
#ifdef GLOW_A_MAP
#ifdef FX
uniform texture GlowACLUTTexture FX_WidgetTexture("Default_CLUT.dds", "GlowA CLUT Texture", "2D");
#endif
uniform sampler2D GlowACLUTSampler FX_SamplerState(GlowACLUTTexture, Linear, Linear, ClampToEdge, ClampToEdge);
#else
uniform FLOAT3 InnerGrowAColor FX_WidgetRGB("Inner Grow A Color", 1.0, 1.0, 1.0);

#ifdef TARGET
uniform mediump FLOAT3 InnerGrowAValue;
#define InnerGrowAPower InnerGrowAValue.x
#define InnerGrowAStrength InnerGrowAValue.y
#define InnerGrowALimit InnerGrowAValue.z
#else
uniform FLOAT InnerGrowAPower FX_WidgetSlider(0.0, 128.0, 0.1, "Inner Grow A Power", 1.0);
uniform FLOAT InnerGrowAStrength FX_WidgetSlider(0.0, 10.0, 0.05, "Inner Grow A Strength", 1.0);
uniform FLOAT InnerGrowALimit FX_WidgetSlider(0.0, 1.0, 0.05, "Inner Grow A Limit", 1.0);
#endif
#endif

#ifdef GLOW_B
#ifdef GLOW_B_MAP
#ifdef FX
uniform texture GlowBCLUTTexture FX_WidgetTexture("Default_CLUT.dds", "GlowB CLUT Texture", "2D");
#endif
uniform sampler2D GlowBCLUTSampler FX_SamplerState(GlowBCLUTTexture, Linear, Linear, ClampToEdge, ClampToEdge);
#else
uniform FLOAT3 InnerGrowBColor FX_WidgetRGB("Inner Grow B Color", 1.0, 1.0, 1.0);
#ifdef TARGET
uniform mediump FLOAT3 InnerGrowBValue;
#define InnerGrowBPower InnerGrowBValue.x
#define InnerGrowBStrength InnerGrowBValue.y
#define InnerGrowBLimit InnerGrowBValue.z
#else
uniform FLOAT InnerGrowBPower InnerGrowAPower FX_WidgetSlider(0.0, 128.0, 0.1, "Inner Grow B Power", 1.0);
uniform FLOAT InnerGrowBStrength FX_WidgetSlider(0.0, 10.0, 0.05, "Inner Grow B Strength", 1.0);
uniform FLOAT InnerGrowBLimit FX_WidgetSlider(0.0, 1.0, 0.05, "Inner Grow B Limit", 1.0);
#endif
#endif
#endif
#endif

//#endif	//end of FP	

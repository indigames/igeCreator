//#ifdef VP

#if defined CGFX
#define FX_WidgetNone < string UIWidget = "None"; >
#define FX_WidgetFloat(name, def) <string UIWidget = "None"; string UIName = name;> = def
#define FX_WidgetFloat3(name, x,y,z) <string UIWidget = "None"; string UIName = name;> = {x,y,z}
#define FX_WidgetSlider(min,max,step,name,def) < string UIWidget = "slider"; float UIMin = min; float UIMax = max; float UIStep = step; string UIName = name; > = def
#define FX_WidgetSpinner(name,min,max,def) <string UIType = "FloatSpinner"; float UIMin = min; float UIMax = max; string UIName = name;> = def
#define FX_WorldPos(name,x,y,z,w) < string Object = name; string UIName = name; string Space = "World"; > = { x,y,z,w }
#define SEMANTICS_ViewProjection : ViewProjection
#define SEMANTICS_ViewInverse : ViewInverse
#define SEMANTICS_View : View
#define SEMANTICS_World : World
#define SEMANTICS_WorldInverse : WorldInverse
#define SEMANTICS_viewportpixelsize : viewportpixelsize
#define SEMANTICS_TIME : TIME
#define SEMANTICS_CAMERAPOSITION : CAMERAPOSITION
#define SEMANTICS_ViewInverse : ViewInverse
#else
#define FX_WidgetNone
#define FX_WidgetFloat(name, def)
#define FX_WidgetFloat3(name, x,y,z)
#define FX_WidgetSlider(min,max,step,name,def)
#define FX_WidgetSpinner(min,max,name,def)
#define FX_WorldPos(name,x,y,z,w)
#define SEMANTICS_ViewProjection
#define SEMANTICS_ViewInverse
#define SEMANTICS_View
#define SEMANTICS_World
#define SEMANTICS_WorldInverse
#define SEMANTICS_viewportpixelsize
#define SEMANTICS_TIME
#define SEMANTICS_CAMERAPOSITION
#define SEMANTICS_ViewInverse
#endif

#ifndef LIGHT_VIEW_TRANSFORM
uniform float4x4 ViewProj SEMANTICS_ViewProjection FX_WidgetNone;
#endif

#ifdef VIEW_INVERSE_MATRIX
uniform float4x4 ViewInverse SEMANTICS_ViewInverse FX_WidgetNone;
#define VIEW_INV
#endif

#ifdef VIEW_MATRIX
uniform float4x4 View SEMANTICS_View FX_WidgetNone;
#endif

#ifndef WORLD_COORDINATE
uniform float4x4 World SEMANTICS_World FX_WidgetNone;
#ifdef USE_NOM
	uniform float4x4 WorldInverse SEMANTICS_WorldInverse FX_WidgetNone;
#endif
#endif

#ifdef POINT_PARTICLE_SHADER
	uniform float2 ViewportPixelSize SEMANTICS_viewportpixelsize;
#endif

#if (defined(LIGHT_VIEW_POSITION) || defined(LIGHT_VIEW_TRANSFORM) || defined(USE_SHDW))
	uniform float4x4 LightViewProj;
#endif

//////////////// UV scroll ///////////////////
#ifdef TIME_ENABLE
	uniform float Time SEMANTICS_TIME;
#endif

#ifdef SCROLL_SET0
#ifdef TARGET
	uniform float2 ScrollSpeedSet0;
#else
	uniform float ScrollSpeedSet0U FX_WidgetSlider(-5.0, 5.0, 0.01, "U0 scroll speed", 1.0);
	uniform float ScrollSpeedSet0V FX_WidgetSlider(-5.0, 5.0, 0.01, "V0 scroll speed", 1.0);
#define ScrollSpeedSet0 float2(ScrollSpeedSet0U,ScrollSpeedSet0V)
#endif
#endif
#ifdef SCROLL_SET1
#ifdef TARGET
	uniform float2 ScrollSpeedSet1;
#else
	uniform float ScrollSpeedSet1U FX_WidgetSlider(-5.0, 5.0, 0.01, "U1 scroll speed", 1.0);
	uniform float ScrollSpeedSet1V FX_WidgetSlider(-5.0, 5.0, 0.01, "V1 scroll speed", 1.0);
	#define ScrollSpeedSet1 float2(ScrollSpeedSet1U,ScrollSpeedSet1V)
#endif
#endif
#ifdef SCROLL_SET2
#ifdef TARGET
	uniform float2 ScrollSpeedSet2;
#else
	uniform float ScrollSpeedSet2U FX_WidgetSlider(-5.0, 5.0, 0.01, "U2 scroll speed", 1.0);
	uniform float ScrollSpeedSet2V FX_WidgetSlider(-5.0, 5.0, 0.01, "V2 scroll speed", 1.0);
	#define ScrollSpeedSet2 float2(ScrollSpeedSet2U,ScrollSpeedSet2V)
#endif
#endif

/////////////// UV rotation //////////////////
#ifdef ROTATION_SET0
	uniform float RotationSet0 FX_WidgetSlider(-12.5664, 12.5664, 0.005, "UV0 rotation", 0.0);
#endif
#ifdef ROTATION_SET1
	uniform float RotationSet1 FX_WidgetSlider(-12.5664, 12.5664, 0.005, "UV1 rotation", 0.0);
#endif
#ifdef ROTATION_SET2
	uniform float RotationSet2 FX_WidgetSlider(-12.5664, 12.5664, 0.005, "UV2 rotation", 0.0);
#endif

//////////////// UV offset ///////////////////
#ifdef OFFSET_SET0
#ifdef TARGET
	uniform float2 OffsetSet0;
#define OffsetSet0U OffsetSet0.x
#define OffsetSet0V OffsetSet0.y
#else
	uniform float OffsetSet0U FX_WidgetSlider(-10.0, 10.0, 0.005, "U0 offset", 0.0);
	uniform float OffsetSet0V FX_WidgetSlider(-10.0, 10.0, 0.005, "V0 offset", 0.0);
#define OffsetSet0 float2(OffsetSet0U,OffsetSet0V)
#endif
#endif

#ifdef OFFSET_SET1
#ifdef TARGET
	uniform float2 OffsetSet1;
	#define OffsetSet1U OffsetSet1.x
	#define OffsetSet1V OffsetSet1.y
#else
	uniform float OffsetSet1U FX_WidgetSlider(-10.0, 10.0, 0.005, "U1 offset", 0.0);
	uniform float OffsetSet1V FX_WidgetSlider(-10.0, 10.0, 0.005, "V1 offset", 0.0);
	#define OffsetSet1 float2(OffsetSet1U,OffsetSet1V)

#endif
#endif
#ifdef OFFSET_SET2
#ifdef TARGET
	uniform float2 OffsetSet2;
#define OffsetSet2U OffsetSet2.x
#define OffsetSet2V OffsetSet2.y
#else
	uniform float OffsetSet2U FX_WidgetSlider(-10.0, 10.0, 0.005, "U2 offset", 0.0);
	uniform float OffsetSet2V FX_WidgetSlider(-10.0, 10.0, 0.005, "V2 offset", 0.0);
	#define OffsetSet2 float2(OffsetSet2U,OffsetSet2V)
#endif
#endif

///////////////// UV scale ///////////////////
#ifdef SCALE_SET0
#ifdef TARGET
	uniform float2 ScaleSet0;
#define ScaleSet0U ScaleSet0.x
#define ScaleSet0V ScaleSet0.y
#else
	uniform float ScaleSet0U FX_WidgetSlider(0.0, 10.0, 0.005, "U0 scale", 1.0);
	uniform float ScaleSet0V FX_WidgetSlider(0.0, 10.0, 0.005, "V0 scale", 1.0);
#define ScaleSet0 float2(ScaleSet0U,ScaleSet0V)
#endif
#endif

#ifdef SCALE_SET1
#ifdef TARGET
	uniform float2 ScaleSet1;
#define ScaleSet1U ScaleSet1.x
#define ScaleSet1V ScaleSet1.y
#else
	uniform float ScaleSet1U FX_WidgetSlider(0.0, 10.0, 0.005, "U1 scale", 1.0);
	uniform float ScaleSet1V FX_WidgetSlider(0.0, 10.0, 0.005, "V1 scale", 1.0);
	#define ScaleSet1 float2(ScaleSet1U,ScaleSet1V)
#endif
#endif
#ifdef SCALE_SET2
#ifdef TARGET
	uniform float2 ScaleSet2;
#define ScaleSet2U ScaleSet2.x
#define ScaleSet2V ScaleSet2.y
#else
	uniform float ScaleSet2U FX_WidgetSlider(0.0, 10.0, 0.005, "U2 scale", 1.0);
	uniform float ScaleSet2V FX_WidgetSlider(0.0, 10.0, 0.005, "V2 scale", 1.0);
	#define ScaleSet2 float2(ScaleSet2U,ScaleSet2V)
#endif
#endif

//////////////// Z bias //////////////////////
#ifdef ZBIAS
	uniform FLOAT ZBias FX_WidgetSpinner(-1.0f, 1.0f, "Z Bias", 0.0);
#endif

//////////////// Fog /////////////////////////
#ifdef FOG
#ifdef TARGET
	uniform float3 FogParams;
	#define FogParamA FogParams.x
	#define FogParamB FogParams.y
	#define FogAlpha FogParams.z
#else
	uniform float FogNear FX_WidgetSlider(0.0, 10000.0, 1.0, "Fog Near", 100.0);
	uniform float FogFar FX_WidgetSlider(0.0, 10000.0, 1.0, "Fog Far", 300.0);
	uniform float FogAlpha FX_WidgetSlider(0.0, 1.0, 0.05, "Fog Alpha", 1.0);
#endif
#endif

#ifdef USE_VIEW
#ifdef TARGET
	uniform float3 CameraPosition SEMANTICS_CAMERAPOSITION;
#else
#ifndef VIEW_INV
	uniform float4x4 ViewInverse SEMANTICS_ViewInverse FX_WidgetNone;
#endif
#endif
#endif

///////Point Light////////////////////////////
#if(NUM_POINT_LAMP >=1)
	uniform FLOAT4 PointLamp01Pos FX_WorldPos("PointLamp01Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp01Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp01Intensity", 1.0);
	uniform FLOAT PointLamp01Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp01Range", 1000.0);
#else
	#define PointLamp01Range PointLamp01Pos.w
#endif
#endif
#if(NUM_POINT_LAMP >= 2)
	uniform FLOAT4 PointLamp02Pos FX_WorldPos("PointLamp02Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp02Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp02Intensity", 1.0);
	uniform FLOAT PointLamp02Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp02Range", 1000.0);
#else
#define PointLamp02Range PointLamp02Pos.w
#endif
#endif
#if(NUM_POINT_LAMP >= 3)
	uniform FLOAT4 PointLamp03Pos FX_WorldPos("PointLamp03Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp03Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp03Intensity", 1.0);
	uniform FLOAT PointLamp03Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp03Range", 1000.0);
#else
#define PointLamp03Range PointLamp03Pos.w
#endif
#endif
#if(NUM_POINT_LAMP >= 4)
	uniform FLOAT4 PointLamp04Pos FX_WorldPos("PointLamp04Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp04Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp04Intensity", 1.0);
	uniform FLOAT PointLamp04Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp04Range", 1000.0);
#else
#define PointLamp04Range PointLamp04Pos.w
#endif
#endif
#if(NUM_POINT_LAMP >= 5)
	uniform FLOAT4 PointLamp05Pos FX_WorldPos("PointLamp05Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp05Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp05Intensity", 1.0);
	uniform FLOAT PointLamp05Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp05Range", 1000.0);
#else
#define PointLamp05Range PointLamp05Pos.w
#endif
#endif
#if(NUM_POINT_LAMP >= 6)
	uniform FLOAT4 PointLamp06Pos FX_WorldPos("PointLamp06Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp06Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp06Intensity", 1.0);
	uniform FLOAT PointLamp06Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp06Range", 1000.0);
#else
#define PointLamp06Range PointLamp06Pos.w
#endif
#endif
#if(NUM_POINT_LAMP >= 7)
	uniform FLOAT4 PointLamp07Pos FX_WorldPos("PointLamp07Pos", 0.0, 0.0, 0.0, 100.0);
#ifndef TARGET
	uniform FLOAT PointLamp07Intensity FX_WidgetSlider(0.0, 100.0, 0.1, "PointLamp07Intensity", 1.0);
	uniform FLOAT PointLamp07Range FX_WidgetSlider(1.0, 1000.0, 1.0, "PointLamp07Range", 1000.0);
#else
#define PointLamp07Range PointLamp07Pos.w
#endif
#endif

//////////////// Glass Map ///////////////////
#ifdef GLASS_MAP_ENABLE
	uniform FLOAT UpsideDown FX_WidgetFloat("Glass up side down", 0.0);
#endif

//////////////// COVERMAP ////////////////////
#ifdef COVER_MAP_ENABLE	
#ifdef TARGET
	uniform float2 CoverScale;
#else
	uniform float CoverScaleH FX_WidgetSlider(-5.0, 5.0, 0.01, "Cover Scale H", 1.0);
	uniform float CoverScaleV FX_WidgetSlider(-5.0, 5.0, 0.01, "Cover Scale V", 1.0);
	#define CoverScale float2(CoverScaleH,CoverScaleV)
#endif
#ifdef TARGET
	uniform float2 CoverOffset;
#else
	uniform float CoverOffsetX FX_WidgetFloat("Cover Offset X", 0.0);
	uniform float CoverOffsetY FX_WidgetFloat("Cover Offset Y", 0.0);
	#define CoverOffset float2(CoverOffsetX,CoverOffsetY) 
#endif
#endif

///////////////////// Fat ////////////////////
#ifdef FAT
	uniform float Fat FX_WidgetSpinner(-2.0f, 2.0f, "Fat", 0.0)
#define USE_IN_NORMAL
#endif

///////////////ViewOffset ////////////////////
#ifdef VIEW_OFFSET
		uniform float3 ViewOffset FX_WidgetFloat3("View Offset", 0.0, 0.0, 0.0);
#endif

#ifdef USE_NOM
#define USE_IN_NORMAL
#endif

#ifdef SKINNING_ENABLE
uniform float4 MatrixPalette[150 * 4];
#endif


//#endif


//#ifdef FP
#ifdef FLIP_TEXTURE_Y
#define ONE_MINUS 1.0 -
#else
#define ONE_MINUS
#endif


#ifdef PARALLAX_MAP_ENABLE
#define ADD_PARALLAX +parallax
#else
#define ADD_PARALLAX
#endif

#ifdef OL_PARALLAX_MAP_ENABLE
#define ADD_OLPARALLAX (+olparallax)
#else
#define ADD_OLPARALLAX
#endif

#ifdef NORMAL_DISTORTION
	#define ADD_NOM_DISTORTION +distortion.xy
#else 
	#define ADD_NOM_DISTORTION
	#ifdef DISTORTION
		#define ADD_DISTORTION +distortion.xy
	#else 
		#define ADD_DISTORTION
	#endif
#endif



#ifdef CGFX
float3 EulerToVector(float3 euler)
{
	float radX = euler.x / 180.0 * 3.1416;
	float radY = euler.y / 180.0 * 3.1416;
	float radZ = euler.z / 180.0 * 3.1416;
	float cosX = cos(radX);
	float sinX = sin(radX);
	float cosY = cos(radY);
	float sinY = sin(radY);
	float cosZ = cos(radZ);
	float sinZ = sin(radZ);

	float4x4 rotX = {
		1    ,   0  ,     0,0,
		0    ,cosX  , -sinX,0,
		0    ,sinX  ,  cosX,0,
		0,0,0,1 };

	float4x4 rotY = {
		cosY , 0    , sinY ,0,
		0    , 1    ,    0 ,0,
		-sinY, 0    , cosY ,0,
		0,0,0,1 };

	float4x4 rotZ = {
		cosZ , -sinZ, 0    ,0,
		sinZ ,  cosZ, 0    ,0,
		0   ,0      , 1    ,0,
		0,0,0,1 };

	float4x4 rotmat = mul(mul(rotZ, rotY), rotX);
	return (MUL(rotmat, float4(0, 0, 1, 1))).xyz;
}
#else
#define EulerToVector(euler) euler
#endif

#ifdef CGFX
void std_PS(
#endif
#ifdef POINT_PARTICLE_SHADER
	in lowp FLOAT vPointSize SEMAMTICS_PSIZE
#endif
#ifdef VERTEX_PAINT_ENABLE
	in lowp FLOAT4  vColor SEMAMTICS_COLOR0
#endif
#ifdef USE_TEX0
	in mediump TEX0_VEC vTexcoord0 TEXCOORD_TEX0
#endif
#ifdef USE_TEX1
	in mediump FLOAT2 vTexcoord1 TEXCOORD_TEX1
#endif
#ifdef USE_TEX2
	in mediump FLOAT2 vTexcoord2 TEXCOORD_TEX2
#endif
#ifdef USE_WPOS
	in mediump FLOAT4 vWorldPosition TEXCOORD_WPOS
#endif
#ifdef USE_LVPOS
	in mediump FLOAT4 vLViewPosition TEXCOORD_LVPOS
#endif
#ifdef USE_COUV
	in mediump FLOAT2 vCoverUV TEXCOORD_COUV
#endif
#ifdef USE_NOM
	in lowp NOM_VEC vNormal TEXCOORD_NOM
#endif
#ifdef USE_TAN
	in lowp FLOAT3 vTangent TEXCOORD_TAN
#endif
#ifdef USE_BIN
	in lowp FLOAT3 vBinormal TEXCOORD_BIN
#endif
#ifdef USE_VIEW
	in mediump FLOAT3 vView TEXCOORD_VIEW
#endif
#ifdef USE_VIEW2
	in mediump FLOAT3 vView2 TEXCOORD_VIEW2
#endif
#ifdef USE_SPOS
	in mediump FLOAT4 vScreenPos TEXCOORD_SPOS
#endif
#ifdef USE_SHDW
	in mediump FLOAT4 vShadowMapCoord TEXCOORD_SHDW
#endif

#ifdef USE_FOG
	in lowp FLOAT vFogFactor TEXCOORD_FOG
#endif
#ifdef CGFX
	out FLOAT4 oColor : COLOR)
#else
#ifndef ES2
	out FLOAT4 oColor;
#endif
	void main()
#endif
{
#ifdef SPECCONTRIB
	FLOAT4 specContrib = FLOAT4(0, 0, 0, 0);
#endif

#ifdef USE_NOM
	FLOAT3 Nn = vNormal.xyz;
#endif

	//PARALLAX
#ifdef LOCAL_SPACE_VIEW
	
#ifdef CGFX
	#define PARALLAXMAT\\
		FLOAT3x3(vTangent.x, vBinormal.x, -Nn.x,\\
		vTangent.y, vBinormal.y, -Nn.y,\\
		vTangent.z, vBinormal.z, -Nn.z)
#else
	#define PARALLAXMAT\\
		FLOAT3x3(vTangent.x, vBinormal.x, Nn.x,\\
		vTangent.y, vBinormal.y, Nn.y,\\
		vTangent.z, vBinormal.z, Nn.z)
#endif
	FLOAT3 vv = mul(vView, PARALLAXMAT);
#endif

#ifdef PARALLAX_MAP_ENABLE
	FLOAT2 parallax = (
#if (HEIGHT_MAP_CHANNEL == NormalMapAlphaChannel)
#ifdef NMAP_UV
		tex2D(NormalSampler, NMAP_UV).a
#else
		tex2D(NormalSampler, NMAPA_UV).a
#endif
#else
#ifdef CMAP_UV
		tex2D(ColorSampler, CMAP_UV).a
#else
		tex2D(ColorSampler, CMAPA_UV).a
#endif
#endif
		* ParallaxBiasX + ParallaxBiasY) * vv.xy;
#ifdef FLIP_TEXTURE_Y
		parallax.y = 1.0 - parallax.y;
#endif
#endif

#ifdef OL_PARALLAX_MAP_ENABLE
	FLOAT2 olparallax = (
#if (OL_HEIGHT_MAP_CHANNEL == NormalMapAlphaChannel)
#ifdef ONMAP_UV
		tex2D(OverlayNormalSampler, ONMAP_UV).a
#else
		tex2D(OverlayNormalSampler, ONMAPA_UV).a
#endif
#else
#ifdef OCMAP_UV
		tex2D(OverlayColorSampler, OCMAP_UV).a
#else
		tex2D(OverlayColorSampler, OCMAPA_UV).a
#endif
#endif
		* OlParallaxBiasX + OlParallaxBiasY) * vv.xy;
#ifdef FLIP_TEXTURE_Y
	olparallax.y = 1.0 - olparallax.y;
#endif
#endif

#ifdef OVERLAY_NORMAL_SAMPLER_ENABLE
	FLOAT4 overlayNormalRGBA;
	#ifdef ONMAP_UV
		overlayNormalRGBA = tex2D(OverlayNormalSampler, ONMAP_UV ADD_OLPARALLAX);
	#endif
	#ifdef ONMAPC_UV
		overlayNormalRGBA.rgb = tex2D(OverlayNormalSampler, ONMAPC_UV ADD_OLPARALLAX).rgb;
	#endif
	#ifdef ONMAPA_UV
		overlayNormalRGBA.a = tex2D(OverlayNormalSampler, ONMAPA_UV ADD_OLPARALLAX).a;
	#endif
#endif

#ifdef NORMAL_DISTORTION
	FLOAT3 distortion = ((overlayNormalRGBA.rgb) - FLOAT3(0.5, 0.5, 0.5)) * DistortionStrength;
#define _DISTORTION
#endif

#ifdef DISTORTION
#ifdef DISTORTION_SOURCE_OVERRAY
#ifndef _DISTORTION
	FLOAT3 distortion = ((overlayNormalRGBA.rgb) - FLOAT3(0.5, 0.5, 0.5)) * DistortionStrength;
#endif
#endif
#endif

#ifdef NORMAL_SAMPLER_ENABLE
	FLOAT4 normalRGBA;
	#ifdef NMAP_UV
		normalRGBA = tex2D(NormalSampler, NMAP_UV ADD_PARALLAX ADD_NOM_DISTORTION);
	#endif
	#ifdef NMAPC_UV
		normalRGBA.rgb = tex2D(NormalSampler, NMAPC_UV  ADD_PARALLAX ADD_NOM_DISTORTION).rgb;
	#endif
	#ifdef NMAPA_UV
		normalRGBA.a = tex2D(NormalSampler, NMAPA_UV ADD_PARALLAX ADD_NOM_DISTORTION).a;
	#endif
#endif

#ifdef DISTORTION
#ifdef _DISTORTION
	distortion = ((normalRGBA.rgb) - FLOAT3(0.5, 0.5, 0.5)) * DistortionStrength;
#else
	FLOAT3 distortion = ((normalRGBA.rgb) - FLOAT3(0.5, 0.5, 0.5)) * DistortionStrength;
#endif
#endif	

#ifdef COLOR_SAMPLER_ENABLE
	FLOAT4 colorRGBA;
#ifdef CMAP_UV
colorRGBA = tex2D(ColorSampler, CMAP_UV ADD_PARALLAX ADD_DISTORTION);
#ifdef FP_ALPHA_TEST
#ifndef LATE_DISCARD
if (colorRGBA.w < AlphaRef) discard;
#endif
#endif
#endif
#ifdef CMAPC_UV
	colorRGBA.rgb = tex2D(ColorSampler, CMAPC_UV ADD_PARALLAX ADD_DISTORTION).rgb;
#endif
#ifdef CMAPA_UV
#ifdef DISCARD_COLOR_MAP_RGB
	colorRGBA.a = tex2D(ColorSampler, CMAPA_UV ADD_PARALLAX ADD_DISTORTION).r;
	colorRGBA.rgb = FLOAT3(1.0, 1.0, 1.0);
#else
	colorRGBA.a = tex2D(ColorSampler, CMAPA_UV ADD_PARALLAX ADD_DISTORTION).a;
#endif
#endif
#endif

#ifdef LIGHT_SAMPLER_ENABLE
	FLOAT4 lightRGBA;
#ifdef LMAP_UV
	lightRGBA = tex2D(LightSampler, LMAP_UV);
#endif
#ifdef LMAPC_UV
	lightRGBA.rgb = tex2D(LightSampler, LMAPC_UV).rgb;
#endif
#ifdef LMAPA_UV
	lightRGBA.a = tex2D(LightSampler, LMAPA_UV).a;
#endif
#endif

#ifdef OVERLAY_COLOR_SAMPLER_ENABLE
	FLOAT4 overlayColorRGBA;
#ifdef OCMAP_UV
	overlayColorRGBA = tex2D(OverlayColorSampler, OCMAP_UV ADD_OLPARALLAX);
#endif
#ifdef OCMAPC_UV
	overlayColorRGBA.rgb = tex2D(OverlayColorSampler, OCMAPC_UV ADD_OLPARALLAX).rgb;
#endif
#ifdef OCMAPA_UV
	overlayColorRGBA.a = tex2D(OverlayColorSampler, OCMAPA_UV ADD_OLPARALLAX).a;
#endif
#endif

#ifdef OVERLAY_STRENGTH_ENABLE
	FLOAT overlayStlength = OverlayStrength;
#if(OVERLAY_TRANSPARENT_MAP_CHANNEL == DiffuseMapAlphaChannel)
	overlayStlength *= colorRGBA.a;
#elif(OVERLAY_TRANSPARENT_MAP_CHANNEL == DiffuseMapRedChannel)
	overlayStlength *= colorRGBA.r;
#elif(OVERLAY_TRANSPARENT_MAP_CHANNEL == NormalMapAlphaChannel)
	overlayStlength *= normalRGBA.a;
#elif(OVERLAY_TRANSPARENT_MAP_CHANNEL == NormalMapRedChannel)
	overlayStlength *= normalRGBA.r;
#elif(OVERLAY_TRANSPARENT_MAP_CHANNEL == LightMapAlphaChannel)
	overlayStlength *= lightRGBA.a;
#elif(OVERLAY_TRANSPARENT_MAP_CHANNEL == LightMapRedChannel)
	overlayStlength *= lightRGBA.r;
#elif(OVERLAY_TRANSPARENT_MAP_CHANNEL == OverlayDiffuseMapAlphaChannel)
	overlayStlength *= overlayColorRGBA.a;
#elif(OVERLAY_TRANSPARENT_MAP_CHANNEL == OverlayDiffuseMapRedChannel)
	overlayStlength *= overlayColorRGBA.r;
#elif(OVERLAY_TRANSPARENT_MAP_CHANNEL == OverlayNormalMapAlphaChannel)
	overlayStlength *= overlayNormalRGBA.a;
#elif(OVERLAY_TRANSPARENT_MAP_CHANNEL == OverlayNormalMapRedChannel)
	overlayStlength *= overlayNormalRGBA.r;
#endif
#ifdef OVERLAY_VERTEX_ALPHA_ENABLE
	overlayStlength *= vColor.a;
#endif
#endif

#ifdef NORMAL_MAP_ENABLE
#ifdef OVERLAY_STRENGTH_ENABLE
	FLOAT3 bump = (normalRGBA.rgb - FLOAT3(0.5, 0.5, 0.5))*((1.0 - overlayStlength)*Bumpiness);
#else
	FLOAT3 bump = (normalRGBA.rgb - FLOAT3(0.5, 0.5, 0.5))*Bumpiness;
#endif
	Nn = Nn + bump.x*vTangent + bump.y*vBinormal;
#endif

#ifdef OVERLAY_NORMAL_MAP_ENABLE
#ifdef OVERLAY_STRENGTH_ENABLE
	FLOAT3 olbump = (overlayNormalRGBA.rgb - FLOAT3(0.5, 0.5, 0.5))*(OverlayBumpiness*overlayStlength);
#else
	FLOAT3 olbump = (overlayNormalRGBA.rgb - FLOAT3(0.5, 0.5, 0.5))*OverlayBumpiness;
#endif
	Nn = Nn + olbump.x*vTangent + olbump.y*vBinormal;
#endif

#ifdef USE_NOM	
	Nn = normalize(Nn);
#endif	

	FLOAT4 diffuseColor;
#ifdef COLOR_MAP_ENABLE	
	diffuseColor = colorRGBA;
#else
	#ifdef DIFFUSE_COLOR_ENABLE
		#ifdef TARGET
			diffuseColor = DiffuseColor;
		#else
			#ifdef ALPHA_BLENDING_ENABLE
				diffuseColor = FLOAT4(DiffuseColor.rgb, DiffuseAlpha);
			#else
				diffuseColor = FLOAT4(DiffuseColor.rgb, 1.0);
			#endif
		#endif
	#endif
#endif

#ifdef MAKE_SHADOW_SHADER
#ifdef LIGHT_VIEW_POSITION
	FLOAT z = vLViewPosition.z / vLViewPosition.w;
	diffuseColor = FLOAT4(z, z, z, 1.0);
#else
	diffuseColor = FLOAT4(ShadowColor.w, 0.0, 0.0, 1.0);
#endif
#endif

#if(TRANSPARENT_MAP_CHANNEL == DiffuseMapAlphaChannel)
#ifndef COLOR_MAP_ENABLE	
	diffuseColor.a = colorRGBA.a;
#endif	
#elif(TRANSPARENT_MAP_CHANNEL == DiffuseMapRedChannel)
#ifndef COLOR_MAP_ENABLE	
	iffuseColor.a = colorRGBA.r;
#endif	
#elif(TRANSPARENT_MAP_CHANNEL == NormalMapAlphaChannel)
	iffuseColor.a = normalRGBA.a;
#elif(TRANSPARENT_MAP_CHANNEL == NormalMapRedChannel)
	iffuseColor.a = normalRGBA.r;
#elif(TRANSPARENT_MAP_CHANNEL == LightMapAlphaChannel)
	iffuseColor.a = lightRGBA.a;
#elif(TRANSPARENT_MAP_CHANNEL == LightMapRedChannel)
	iffuseColor.a = lightRGBA.r;
#elif(TRANSPARENT_MAP_CHANNEL == OverlayDiffuseMapAlphaChannel)
	iffuseColor.a = overlayColorRGBA.a;
#elif(TRANSPARENT_MAP_CHANNEL == OverlayDiffuseMapRedChannel)
	iffuseColor.a = overlayColorRGBA.r;
#elif(TRANSPARENT_MAP_CHANNEL == OverlayNormalMapAlphaChannel)
	iffuseColor.a = overlayNormalRGBA.a;
#elif(TRANSPARENT_MAP_CHANNEL == OverlayNormalMapRedChannel)
	iffuseColor.a = overlayNormalRGBA.r;
#endif

#ifdef OVERLAY_COLOR_MAP_ENABLE
#ifdef OVERLAY_COLOR_MAP_ADD
	diffuseColor.rgb = diffuseColor.rgb + overlayColorRGBA.rgb * overlayStlength;
#else
	diffuseColor.rgb = diffuseColor.rgb * (1.0 - overlayStlength) + overlayColorRGBA.rgb * overlayStlength;
#endif
#endif

#ifdef VERTEX_DIFFUSE_COLOR_ENABLE
#ifdef VERTEX_ALPHA_ENABLE
	diffuseColor *= vColor;
#else            
	diffuseColor.rgb *= vColor.rgb;
#endif
#endif

#ifdef COLOR_MAP_ENABLE

#if (DIFFUSE_STR_MAP_CHANNEL == DiffuseMapAlphaChannel)
	#define diffusePower colorRGBA.a
#elif (DIFFUSE_STR_MAP_CHANNEL == NormalMapAlphaChannel)
	#define diffusePower normalRGBA.a
#elif (DIFFUSE_STR_MAP_CHANNEL == LightMapAlphaChannel)
	#define diffusePower lightRGBA.a
#elif (DIFFUSE_STR_MAP_CHANNEL == LightMapRedChannel)
	#define diffusePower lightRGBA.r
#else //(DIFFUSE_STR_MAP_CHANNEL == NormalMapRedChannel)
	#define diffusePower normalRGBA.r
#endif


#ifdef DIFFUSE_COLOR_ENABLE
#ifdef TARGET
#ifdef DIFFUSE_STR_MAP_ENABLE
	diffuseColor *= (FLOAT4(1.0, 1.0, 1.0, 1.0) + (DiffuseColor - FLOAT4(1.0, 1.0, 1.0, 1.0))*diffusePower);
#else
	diffuseColor *= DiffuseColor;
#endif
#else
#ifdef DIFFUSE_STR_MAP_ENABLE
	diffuseColor.rgb *= (FLOAT3(1.0, 1.0, 1.0) + (DiffuseColor.rgb - FLOAT3(1.0, 1.0, 1.0))*diffusePower);
#else
	diffuseColor.rgb *= DiffuseColor.rgb;
#endif

#ifdef ALPHA_BLENDING_ENABLE
#ifdef DIFFUSE_STR_MAP_ENABLE
	diffuseColor.a *= (1.0 + (DiffuseAlpha - 1.0)*diffusePower);
#else
	diffuseColor.a *= DiffuseAlpha;
#endif
#endif
#endif
#endif

#endif

#ifdef LIGHT_MAP_ENABLE
	diffuseColor.rgb *= lerp(FLOAT3(1.0, 1.0, 1.0), lightRGBA.rgb * LightMapPower, LightMapStrength);
#endif


#ifdef SPECULAR_ENABLE
#ifdef SPECULAR_MAP_ENABLE
	#if (SPECULAR_MAP_CHANNEL == NormalMapAlphaChannel)
		FLOAT spStr = colorRGBA.a;
	#elif (SPECULAR_MAP_CHANNEL == VertexColorRedChanel)
		FLOAT spStr = vColor.r;
	#elif (SPECULAR_MAP_CHANNEL == VertexColorAlphaChanel)
		FLOAT spStr = vColor.a;
	#else
		FLOAT spStr = normalRGBA.a;
	#endif
#else
	FLOAT spStr = SpecularStrength;
#endif


#ifdef OVERLAY_SPECULAR_MAP_ENABLE
#if (OVERLAY_SPECULAR_MAP_CHANNEL == OverlayNormalMapAlphaChannel)
	spStr = spStr * (1.0 - overlayStlength) + overlayNormalRGBA.w * overlayStlength;
#else
	spStr = spStr * (1.0 - overlayStlength) + overlayDiffuseColor.a * overlayStlength;
#endif
#endif
#endif

#ifndef NOLIGHT

	FLOAT3 diffTmp;
	FLOAT3 diffContrib = FLOAT3(0, 0, 0);
	FLOAT3 Ln;
	FLOAT3 Lc;
	FLOAT l;
	FLOAT ldn;
	FLOAT attenuation;

#ifdef SPECULAR_ENABLE
	FLOAT3 Hn;
	#ifdef CLUT_LIGHT_ENABLE
		FLOAT2 clut;
		FLOAT4 lightPow;
	#else
		FLOAT d;
		FLOAT pw;
		FLOAT specPow;
	#endif
	FLOAT3 specTmp;
#else
	#ifdef CLUT_LIGHT_ENABLE
		FLOAT2 clut;
	#endif
#endif

#ifdef VELVET_ENABLE
	FLOAT subLamb;
	FLOAT3 subContrib;
	#ifdef SPECULAR_ENABLE
		FLOAT vdn;
		FLOAT3 vecColor;
	#endif
#endif
#endif

////// Dir Lamp
#ifndef TARGET
	#define LC_MUL_DirLampIntensity(i) Lc *= DirLamp0##i##Intensity;
#else
	#define LC_MUL_DirLampIntensity(i)
#endif

#ifdef LOCAL_INTENSITY
	#define Lc_MUL_LocalIntensity Lc *= LocalIntensity;
#else
	#define Lc_MUL_LocalIntensity
#endif

#ifndef CLUT_LIGHT_ENABLE
	#define MAX_LDN ldn = max(0.0, ldn);
#else
	#define MAX_LDN
#endif

#ifdef SPECULAR_ENABLE
	#ifdef CLUT_LIGHT_ENABLE	
		#define DIR_LAMP_CALC\\
			Hn = normalize(vView + Ln);\\
			clut = FLOAT2(ldn*0.495 + 0.5, (ONE_MINUS dot(Hn, Nn))*0.98 + 0.01);\\
			lampPow = tex2D(CLUTSampler, clut);\\
			diffTmp = lampPow.rgb * Lc;\\
			specTmp = lampPow.a * spStr * diffTmp;
	#else
		#define DIR_LAMP_CALC\\
			Hn = normalize(vView + Ln);\\
			d = dot(Hn, Nn);\\
			pw = max(d / (SpecularPower *(1.0 - d) + d), 0.0);\\
			specPow = step(0.0, ldn) * pw;\\
			diffTmp = ldn * Lc;\\
			specTmp = specPow * spStr * diffTmp;
	#endif
#else
	#ifdef CLUT_LIGHT_ENABLE
		#define DIR_LAMP_CALC\\
			clut = FLOAT2(ldn*0.495 + 0.5, 0.5);\\
			diffTmp = tex2D(CLUTSampler, clut).xyz * Lc;
	#else
		#define DIR_LAMP_CALC diffTmp = ldn * Lc;
	#endif
#endif


#ifdef VELVET_ENABLE
	#ifdef SPECULAR_ENABLE
		#define DIR_LAMP_VELVET_SPECULAR_CALC\\
			vdn = 1.0 - dot(vView, Nn);\\
			vecColor = FLOAT3(vdn, vdn, vdn);\\
			specTmp += (vecColor*FuzzySpecColor.rgb).xyz*VelvetStrength;
	#else
		#define DIR_LAMP_VELVET_SPECULAR_CALC
	#endif

	#define DIR_LAMP_VELVET_CALC\\
		diffTmp *= SurfaceColor.rgb;\\
		subLamb = smoothstep(-RollOff, 1.0, ldn) - smoothstep(0.0, 1.0, ldn);\\
		subLamb = max(0.0, subLamb);\\
		subContrib = subLamb * SubColor.rgb;\\
		diffTmp += subContrib * VelvetStrength;\\
		DIR_LAMP_VELVET_SPECULAR_CALC
#else
	#define DIR_LAMP_VELVET_CALC
#endif

#ifdef SPECULAR_ENABLE
	#define specContrib_ADD_specTmp specContrib.rgb += specTmp;
#else
	#define specContrib_ADD_specTmp
#endif

#define DIR_LAMP_IMPL(i)\\
	Ln = EulerToVector(DirLamp0##i##Dir);\\
	Lc = DirLamp0##i##Color.rgb;\\
	LC_MUL_DirLampIntensity(i)\\
	Lc_MUL_LocalIntensity\\
	ldn = dot(Ln, Nn);\\
	MAX_LDN\\
	DIR_LAMP_CALC\\
	DIR_LAMP_VELVET_CALC\\
	diffContrib += diffTmp;\\
	specContrib_ADD_specTmp

#if (NUM_DIR_LAMP >= 1)
DIR_LAMP_IMPL(1)
#endif
#if (NUM_DIR_LAMP >= 2)
DIR_LAMP_IMPL(2)
#endif
#if (NUM_DIR_LAMP >= 3)
DIR_LAMP_IMPL(3)
#endif

////////Point Lamp
#ifndef TARGET
#define Lc_MUL_PointLampIntensity(i) Lc *= PointLamp0##i##Intensity;
#else
#define Lc_MUL_PointLampIntensity(i)
#endif

#ifdef SPECULAR_ENABLE
#ifdef CLUT_LIGHT_ENABLE
#define POINT_LAMP_CALC\\ 
Hn = normalize(vView + Ln); \\
clut = FLOAT2(ldn * 0.495 + 0.5, (ONE_MINUS dot(Hn, Nn)) * 0.98 + 0.01); \\
lampPow = tex2D(CLUTSampler, clut); \\
diffTmp = lampPow.rgb * Lc; \\
specTmp = lampPow.a * spStr * diffTmp;
#else
#define POINT_LAMP_CALC\\
			Hn = normalize(vView + Ln);\\
			d = dot(Hn, Nn);\\
			pw = max(d / (SpecularPower *(1.0 - d) + d), 0.0);\\
			specPow = step(0.0, ldn) * pw;\\
			diffTmp = ldn * Lc;\\
			specTmp = specPow * spStr * diffTmp;
#endif
#else
#ifdef CLUT_LIGHT_ENABLE
#define POINT_LAMP_CALC\\
			clut = FLOAT2(ldn*0.495 + 0.5, 0.5);\\
			diffTmp = tex2D(CLUTSampler, clut).xyz * Lc;
#else
#define POINT_LAMP_CALC diffTmp = ldn * Lc;
#endif
#endif

#ifdef VELVET_ENABLE
#ifdef SPECULAR_ENABLE
#define POINT_LAMP_VELVET_SPECULAR_CALC\\
			vdn = 1.0 - dot(vView, Nn);\\
			vecColor = vdn.xxx;\\
			specTmp += (vecColor*FuzzySpecColor.rgb).xyz*VelvetStrength;
#else
#define POINT_LAMP_VELVET_SPECULAR_CALC
#endif

#define POINT_LAMP_VELVET_CALC\\
		diffTmp *= SurfaceColor.rgb;\\
		subLamb = smoothstep(-RollOff, 1.0, ldn) - smoothstep(0.0, 1.0, ldn);\\
		subLamb = max(0.0, subLamb);\\
		subContrib = subLamb * SubColor.rgb;\\
		diffTmp += subContrib * VelvetStrength;\\
		POINT_LAMP_VELVET_SPECULAR_CALC
#else
#define POINT_LAMP_VELVET_CALC
#endif

#define POINT_LAMP_IMPL(i)\\
	Ln = PointLamp0##i##Pos.xyz - vWorldPosition.xyz; \\
	l = length(Ln); \\
	Ln /= l; \\
	attenuation = max(PointLamp0##i##Range - l, 0.0) / PointLamp0##i##Range; \\
	Lc = PointLamp0##i##Color.rgb * attenuation;\\
	Lc_MUL_PointLampIntensity(i)\\
	Lc_MUL_LocalIntensity\\
	ldn = dot(Ln, Nn);\\
	MAX_LDN\\
	POINT_LAMP_CALC\\
	POINT_LAMP_VELVET_CALC\\
	diffContrib += diffTmp;\\
	specContrib_ADD_specTmp

#if(NUM_POINT_LAMP >= 1)
	POINT_LAMP_IMPL(1)
#endif
#if(NUM_POINT_LAMP >= 2)
	POINT_LAMP_IMPL(2)
#endif
#if(NUM_POINT_LAMP >= 3)
	POINT_LAMP_IMPL(3)
#endif
#if(NUM_POINT_LAMP >= 4)
	POINT_LAMP_IMPL(4)
#endif
#if(NUM_POINT_LAMP >= 5)
	POINT_LAMP_IMPL(5)
#endif
#if(NUM_POINT_LAMP >= 6)
	POINT_LAMP_IMPL(6)
#endif
#if(NUM_POINT_LAMP >= 7)
	POINT_LAMP_IMPL(7)
#endif


////////Spot Lamp
#ifndef TARGET
#define Lc_MUL_SpotLampIntensity(i) Lc *= SpotLamp0##i##Intensity;
#else
#define Lc_MUL_SpotLampIntensity(i)
#endif

#define SPOT_LAMP_IMPL(i)\\
	Ln = SpotLamp0##i##Pos.xyz - vWorldPosition.xyz; \\
	l = length(Ln); \\
	Ln /= l; \\
	attenuation = max(SpotLamp0##i##Range - l, 0.0) / SpotLamp0##i##Range; \\
	ldn = dot(-Ln, SpotLamp0##i##Dir.xyz); \\
	attenuation *= (ldn < SpotLamp0##i##Angle) ? 0.0 : smoothstep(SpotLamp0##i##Angle, min(SpotLamp0##i##Angle+0.04,1.0), ldn); \\
	Lc = SpotLamp0##i##Color.rgb * attenuation; \\
	Lc_MUL_SpotLampIntensity(i)\\
	Lc_MUL_LocalIntensity\\
	ldn = dot(Ln, Nn);\\
	MAX_LDN\\
	POINT_LAMP_CALC\\
	POINT_LAMP_VELVET_CALC\\
	diffContrib += diffTmp;\\
	specContrib_ADD_specTmp

#if(NUM_SPOT_LAMP >= 1)
SPOT_LAMP_IMPL(1)
#endif
#if(NUM_SPOT_LAMP >= 2)
SPOT_LAMP_IMPL(2)
#endif
#if(NUM_SPOT_LAMP >= 3)
SPOT_LAMP_IMPL(3)
#endif
#if(NUM_SPOT_LAMP >= 4)
SPOT_LAMP_IMPL(4)
#endif
#if(NUM_SPOT_LAMP >= 5)
SPOT_LAMP_IMPL(5)
#endif
#if(NUM_SPOT_LAMP >= 6)
SPOT_LAMP_IMPL(6)
#endif
#if(NUM_SPOT_LAMP >= 7)
SPOT_LAMP_IMPL(7)
#endif




#ifndef NOLIGHT
	#if (DIFFUSE_LIGHT_MAP_CHANNEL == DiffuseMapAlphaChannel)
		diffContrib *= colorRGBA.a;
	#elif (DIFFUSE_LIGHT_MAP_CHANNEL == NormalMapAlphaChannel)
		diffContrib *= normalRGBA.a;
	#elif (DIFFUSE_LIGHT_MAP_CHANNEL == LightMapAlphaChannel)
		diffContrib *= lightRGBA.a;
	#elif (DIFFUSE_LIGHT_MAP_CHANNEL == NormalMapRedChannel)
		diffContrib *= normalRGBA.r;
	#elif (DIFFUSE_LIGHT_MAP_CHANNEL == LightMapRedChannel)
		diffContrib *= lightRGBA.r;
	#endif
#endif

#ifdef REFLECTION_ENABLE
#ifdef SPECULAR_MAP_ENABLE
	#if (SPECULAR_MAP_CHANNEL == NormalMapAlphaChannel)
		FLOAT rfStr = normalRGBA.w;
	#else
		FLOAT rfStr = diffuseColor.a;
	#endif
	rfStr *= ReflectionStrength;
#else
	FLOAT rfStr = ReflectionStrength;
#endif

#ifdef FRESNEL_ENABLE
	FLOAT KrMin = (rfStr * FresnelMin);
	FLOAT InvFrExp = (1.0 / FresnelExp);
	FLOAT ft = pow(abs(dot(Nn, vView)), InvFrExp);
	rfStr = lerp(rfStr, KrMin, ft);
#endif

	FLOAT3 reflVect = reflect(-vView, -Nn);

#ifdef DISTORTION
	reflVect += distortion;
	reflVect = normalize(reflVect);
#endif

#ifdef ENV_SPHERE
	FLOAT m = 2.0 * sqrt(reflVect.x*reflVect.x + reflVect.y*reflVect.y + (reflVect.z + 1.0)*(reflVect.z + 1.0));
	float2 envuv = reflVect / m + FLOAT2(0.5, 0.5);
	specContrib += tex2D(EnvsSampler, envuv) * rfStr;
#else
	specContrib += texCUBE(EnvSampler, reflVect) *rfStr;
#endif
#endif


#ifdef GLOW_A
#ifdef GLOW_OFFSET
	FLOAT glow = 1.0 - max(0.0, dot(vView2, Nn));
#else
	FLOAT glow = 1.0 - max(0.0, dot(vView, Nn));
#endif
#ifdef GLOW_A_MAP
#ifdef GLOW_A_SUB
	specContrib.rgb -= tex2D(GlowACLUTSampler, FLOAT2(glow*0.5 + 0.5, 0.0)).rgb;
#else
	specContrib.rgb += tex2D(GlowACLUTSampler, FLOAT2(glow*0.5 + 0.5, 0.0)).rgb;
#endif
#else
	FLOAT glowPow = max(glow / (InnerGrowAPower *(1.0 - glow) + glow), 0.0) * InnerGrowAStrength;
	glowPow = min(1.0, glowPow*step(InnerGrowALimit, glowPow));
#ifdef GLOW_A_SUB
	glowPow = glowPow * -1.0;
#endif
	specContrib.rgb += InnerGrowAColor * glowPow;
#endif

#ifdef GLOW_B
#ifdef GLOW_B_MAP
#ifdef GLOW_B_SUB
	specContrib.rgb -= tex2D(GlowBCLUTSampler, FLOAT2(glow*0.5 + 0.5, 0.0)).rgb;
#else
	specContrib.rgb += tex2D(GlowBCLUTSampler, FLOAT2(glow*0.5 + 0.5, 0.0)).rgb;
#endif
#else
	glowPow = max(glow / (InnerGrowBPower *(1.0 - glow) + glow), 0.0) * InnerGrowBStrength;
	glowPow = glowPow * step(InnerGrowBLimit, glowPow);
#ifdef GLOW_B_SUB
	glowPow = glowPow * -1.0;
#endif
	specContrib.rgb += InnerGrowBColor * glowPow;
#endif
#endif
#endif


#ifdef GLASS_MAP_ENABLE
	mediump FLOAT4 glassPos = vScreenPos;
	glassPos.xy += (vView.xy - Nn.xy)*(Curvature*vScreenPos.w);
	diffuseColor.rgb = mix(diffuseColor.rgb, tex2Dproj(BackBufferSampler, glassPos.xyw).rgb, GlassStrength);
#endif



#ifdef RECEIVE_SHADOW_ENABLE
	FLOAT obscure = 1.0;
	#define USE_OBSCURE

#ifdef MAYA	
	FLOAT ourdepth = ((vShadowMapCoord.z / vShadowMapCoord.w) + 1.0) * 0.5;
	ourdepth = min(0.999, ourdepth);
	for (int i = 0; i < 5; i++)
	{
		FLOAT2 t = vShadowMapCoord.xy + g_Offset[i];
		FLOAT shadow_map = tex2D(ShadowSampler, t).x;
		obscure -= ((shadow_map > ourdepth) ? 0 : ShadowDensity);
	}
#endif

#ifdef TARGET
	obscure = tex2Dproj(ShadowSampler, vShadowMapCoord.xyw).x;
	#ifdef LIGHT_VIEW_POSITION
		obscure = ((obscure * vLViewPosition.w) < (vLViewPosition.z - ShadowBias)) ? ShadowColor.w : 1.0;
	#endif
#endif
#endif


#ifdef AMBIENT_OCCLUSION_ENABLE
#ifndef USE_OBSCURE
#define USE_OBSCURE
	FLOAT obscure = 1.0;
#endif

#if (AMBIENT_OCCLUSION_CHANNEL==NormalMapAlphaChannel)
	obscure *= normalRGBA.w;
#elif (AMBIENT_OCCLUSION_CHANNEL==DiffuseMapAlphaChannel)
	obscure *= colorRGBA.w;
#elif (AMBIENT_OCCLUSION_CHANNEL==VertexColorRedChanel)
	obscure *= vColor.r;
#else
	obscure *= vColor.a;
#endif
#endif

#ifdef USE_OBSCURE
	FLOAT3 obscureColor = lerp(ShadowColor.xyz, FLOAT3(1.0, 1.0, 1.0), obscure);
#endif


#if (AMBIENT_TYPE > AMBIENT_None )
#if (AMBIENT_TYPE == AMBIENT_Hemisphere)
#ifdef CGFX
	FLOAT hst = (dot(Nn, normalize(SkyDir)) + 1.0)*0.5;
#else
	FLOAT hst = (dot(Nn, SkyDir.rgb) + 1.0)*0.5;
#endif
	FLOAT3 ambient = lerp(GroundColor, AmbientColor, hst);
#else
	FLOAT3 ambient = AmbientColor;
#endif

#ifndef TARGET
	ambient *= AmbientIntensity;
#endif

#ifdef NOLIGHT
	diffuseColor.rgb *=ambient;
#else
	#ifdef USE_OBSCURE	
		#if	(LIGHT_CALC == OPERATION_ADD)
			diffuseColor.rgb += ((diffContrib + ambient) * obscureColor);
		#elif (LIGHT_CALC == OPERATION_SUB)
			diffuseColor.rgb -= ((diffContrib + ambient) * obscureColor);
		#else
			diffuseColor.rgb *= ((diffContrib + ambient) * obscureColor);
		#endif
	#else
		#if (LIGHT_CALC == OPERATION_ADD)
			diffuseColor.rgb += (diffContrib + ambient);
		#elif (LIGHT_CALC == OPERATION_SUB)
			diffuseColor.rgb -= (diffContrib + ambient);
		#else
			diffuseColor.rgb *= (diffContrib + ambient);
		#endif
	#endif
#endif
#else
#ifdef NOLIGHT
#ifdef USE_OBSCURE
	diffuseColor.rgb *= obscureColor;
#endif
#else
#ifdef USE_OBSCURE
#if	(LIGHT_CALC == OPERATION_ADD)
	diffuseColor.rgb += (diffContrib*obscureColor);
#elif	(LIGHT_CALC == OPERATION_SUB)
	diffuseColor.rgb -= (diffContrib*obscureColor);
#else
	diffuseColor.rgb *= (diffContrib*obscureColor);
#endif
#else
#if	(LIGHT_CALC == OPERATION_ADD)
	diffuseColor.rgb += diffContrib;
#elif (LIGHT_CALC == OPERATION_SUB)
	diffuseColor.rgb -= diffContrib;
#else
	diffuseColor.rgb *= diffContrib;
#endif
#endif
#endif
#endif

#ifdef SPECCONTRIB
#ifdef USE_OBSCURE
	diffuseColor.rgb += specContrib.rgb*obscureColor;
#else
	diffuseColor.rgb += specContrib.rgb;
#endif
#ifdef REFLECTION_ALPHA
	diffuseColor.a *= specContrib.a;
#endif
#endif


#ifdef SEPIA
	float3 RGB2Y = { 0.29900, 0.58700, 0.11400 };
	float3x3 YCbCr2RGB = { {1.0f,  0.00000f, +1.40200f},
			{1.0f, -0.34414f, -0.71414f},
			{1.0f,  1.77200f,  0.00000f}
	};
	float3 YCbCr = float3(dot(diffuseColor.rgb, RGB2Y), -0.200, 0.100);
	float3 sepiaCol = mul(YCbCr2RGB, YCbCr);
	diffuseColor.rgb = lerp(sepiaCol, diffuseColor.rgb, SepiaStrength);
#endif

#ifdef SATURATION
	float mono = dot(diffuseColor.rgb, FLOAT3(0.29900, 0.58700, 0.11400));
	diffuseColor.rgb = lerp(FLOAT3(mono, mono, mono), diffuseColor.rgb, Saturation);
#endif


#ifdef FOG
	float ff = vFogFactor;
#ifdef FOG_MAP
#if (FOG_MAP_CHANNEL == NormalMapAlphaChannel)
	ff *= normalRGBA.a;
#else
	ff *= colorRGBA.a;
#endif

#endif
	diffuseColor.rgb = FogColor * ff + diffuseColor.rgb * (1.0 - ff);
#endif

#ifdef COVER_MAP_ENABLE	
	FLOAT4 cover = tex2D(CoverSampler, vCoverUV);
	diffuseColor.rgb *= min(1.0, cover.r + cover.a + CoverAddition);
#endif

#ifdef FP_ALPHA_TEST
#ifdef LATE_DISCARD
	if (diffuseColor.w < AlphaRef) discard;
#endif
#endif

#ifdef HALFTONE	
	//diffuseColor.rgb *= tone;	
#endif

oColor = diffuseColor;
}


//#endif

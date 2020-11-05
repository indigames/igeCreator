//#ifdef VP

#ifdef CGFX
void std_VS (
#endif
in float4 Position SEMAMTICS_POSITION
#ifdef USE_IN_NORMAL
in float4 Normal SEMAMTICS_NORMAL
#endif
#ifdef NORMAL_MAP_ENABLE
in float4  Tangent SEMAMTICS_TANGENT
#ifndef CALC_BINORMAL
in float4 Binormal SEMAMTICS_BINORMAL
#endif
#endif
#ifdef USE_TEX0
#ifndef PROJ_MAP0
in float2  Texcoord0 SEMAMTICS_TEXCOORD0
#endif
#endif
#ifdef USE_TEX1
#ifndef PROJ_MAP1
in float2  Texcoord1 SEMAMTICS_TEXCOORD1
#endif
#endif
#ifdef USE_TEX2
#ifndef PROJ_MAP2
in float2  Texcoord2 SEMAMTICS_TEXCOORD2
#endif
#endif
#ifdef VERTEX_COLOR_ENABLE
in float4  Color SEMAMTICS_COLOR0
#endif

#ifdef ENABLE_WEIGHTS
in WEIGHT_FLOAT Indices SEMAMTICS_BLENDINDICES
in WEIGHT_FLOAT Weights SEMAMTICS_BLENDWEIGHT
#else
#if( BONE_INFLUENCE == 1 )
in WEIGHT_FLOAT Indices SEMAMTICS_BLENDINDICES
#endif
#endif

#ifdef POINT_PARTICLE_SHADER				
in float PointSize SEMAMTICS_PSIZE
#endif

#ifdef POINT_PARTICLE_SHADER
out lowp float vPointSize SEMAMTICS_PSIZE
#endif
#ifdef VERTEX_PAINT_ENABLE
out lowp float4  vColor SEMAMTICS_COLOR0
#endif
#ifdef USE_TEX0
out mediump tex0_vec vTexcoord0 TEXCOORD_TEX0
#endif
#ifdef USE_TEX1
out mediump float2 vTexcoord1 TEXCOORD_TEX1
#endif
#ifdef USE_TEX2
out mediump float2 vTexcoord2 TEXCOORD_TEX2
#endif
#ifdef USE_WPOS
out mediump float4 vWorldPosition TEXCOORD_WPOS
#endif
#ifdef USE_LVPOS
out mediump float4 vLViewPosition TEXCOORD_LVPOS
#endif
#ifdef USE_COUV
out mediump float2 vCoverUV TEXCOORD_COUV
#endif
#ifdef USE_NOM
out lowp nom_vec vNormal TEXCOORD_NOM
#endif
#ifdef USE_TAN
out lowp float3 vTangent TEXCOORD_TAN
#endif
#ifdef USE_BIN
out lowp float3 vBinormal TEXCOORD_BIN
#endif
#ifdef USE_VIEW
out mediump float3 vView TEXCOORD_VIEW
#endif
#ifdef USE_VIEW2
out mediump float3 vView2 TEXCOORD_VIEW2
#endif
#ifdef USE_SPOS
out mediump float4 vScreenPos TEXCOORD_SPOS
#endif
#ifdef USE_SHDW
out mediump float4 vShadowMapCoord TEXCOORD_SHDW
#endif
#ifdef USE_FOG
out lowp float vFogFactor TEXCOORD_FOG
#endif

#ifdef CGFX
out float4 vHPosition : POSITION)
#else
void main()
#endif
{
	float4 TmpPosition;

	#ifdef USE_NOM
	float3 TmpNormal;
	#endif
	#ifdef NORMAL_MAP_ENABLE
	float3 TmpTangent;
	float3 TmpBinormal;
	#endif

	#ifndef BILLBOARD
		#ifdef FAT
			#ifdef FAT_PAINT
				#define FAT_VALUE Fat*Color.r
			#else
				#define FAT_VALUE Fat
			#endif
			#define FAT_CALC(p,n)  p.xyz = n.xyz*FAT_VALUE
		#else
			#define FAT_CALC(p,n)
		#endif

		#define MulMatrixPalettePos(a,i,c) \\
				jointMatrix[0] = MatrixPalette[ i + 0 ];\\
				jointMatrix[1] = MatrixPalette[ i + 1 ];\\
				jointMatrix[2] = MatrixPalette[ i + 2 ];\\
				a = MUL(jointMatrix,c)

		#ifdef SKINNING_ENABLE
			float4x4 jointMatrix;
			jointMatrix[3] = float4(0.0, 0.0, 0.0, 1.0);
		#ifndef ENABLE_WEIGHTS	
			float4 pos = Position;
			FAT_CALC(pos, Normal);
			#if ( BONE_INFLUENCE == 1 )
					float idx = Indices*3.0;
			#endif
			MulMatrixPalettePos(TmpPosition, IDX, pos);
		#ifdef USE_NOM
			jointMatrix[0].w = jointMatrix[1].w = jointMatrix[2].w = 0.0;
			TmpNormal = MUL(jointMatrix, Normal).xyz;
		#ifdef NORMAL_MAP_ENABLE	
			TmpTangent = MUL(jointMatrix, Tangent).xyz;
		#ifndef CALC_BINORMAL	
			TmpBinormal = MUL(jointMatrix, Binormal).xyz;
		#endif
		#endif
		#endif
		#else
			float4 pos = Position;
			FAT_CALC(pos, Normal);
				float4 tmp;
			WEIGHT_FLOAT idx = Indices*3.0;

		#if( BONE_INFLUENCE >= 1 )
			MulMatrixPalettePos(tmp, IDX(x), pos);
			TmpPosition = tmp * Weights.x;
		#ifdef USE_NOM	
			jointMatrix[0].w = jointMatrix[1].w = jointMatrix[2].w = 0.0;
			tmp.xyz = MUL(jointMatrix, Normal).xyz;
			TmpNormal.xyz = tmp.xyz*Weights.x;
		#ifdef NORMAL_MAP_ENABLE	
			tmp.xyz = MUL(jointMatrix, Tangent).xyz;
			TmpTangent.xyz = tmp.xyz*Weights.x;
		#ifndef CALC_BINORMAL	
			tmp.xyz = MUL(jointMatrix, Binormal).xyz;
			TmpBinormal.xyz = tmp.xyz*Weights.x;
		#endif	
		#endif	
		#endif	
		#endif	

		#if( BONE_INFLUENCE >= 2 )
			MulMatrixPalettePos(tmp, IDX(y), pos);
			TmpPosition += tmp * Weights.y;
		#ifdef USE_NOM	
			jointMatrix[0].w = jointMatrix[1].w = jointMatrix[2].w = 0.0;
			tmp.xyz = MUL(jointMatrix, Normal).xyz;
			TmpNormal.xyz += tmp.xyz*Weights.y;
		#ifdef NORMAL_MAP_ENABLE	
			tmp.xyz = MUL(jointMatrix, Tangent).xyz;
			TmpTangent.xyz += tmp.xyz*Weights.y;
		#ifndef CALC_BINORMAL				
			tmp.xyz = MUL(jointMatrix, Binormal).xyz;
			TmpBinormal.xyz += tmp.xyz*Weights.y;
		#endif
		#endif
		#endif
		#endif

		#if( BONE_INFLUENCE >= 3 )
			MulMatrixPalettePos(tmp, IDX(z), pos);
			TmpPosition += tmp * Weights.z;
		#ifdef USE_NOM				
			jointMatrix[0].w = jointMatrix[1].w = jointMatrix[2].w = 0.0;
			tmp.xyz = MUL(jointMatrix, Normal).xyz;
			TmpNormal.xyz += tmp.xyz*Weights.z;
		#ifdef NORMAL_MAP_ENABLE    				
			tmp.xyz = MUL(jointMatrix, Tangent).xyz;
			TmpTangent.xyz += tmp.xyz*Weights.z;
		#ifndef CALC_BINORMAL				
			tmp.xyz = MUL(jointMatrix, Binormal).xyz;
			TmpBinormal.xyz += tmp.xyz*Weights.z;
		#endif
		#endif
		#endif	
		#endif

		#if( BONE_INFLUENCE >= 4 )
			MulMatrixPalettePos(tmp, IDX(w), pos);
			TmpPosition += tmp * Weights.w;
		#ifdef USE_NOM				
			jointMatrix[0].w = jointMatrix[1].w = jointMatrix[2].w = 0.0;
			tmp.xyz = MUL(jointMatrix, Normal).xyz;
			TmpNormal.xyz += tmp.xyz*Weights.w;
		#ifdef NORMAL_MAP_ENABLE    				
			tmp.xyz = MUL(jointMatrix, Tangent).xyz;
			TmpTangent.xyz += tmp.xyz*Weights.w;
		#ifndef CALC_BINORMAL				
			tmp.xyz = MUL(jointMatrix, Binormal).xyz;
			TmpBinormal.xyz += tmp.xyz*Weights.w;
		#endif
		#endif
		#endif
		#endif
		#endif

		#ifdef USE_NOM
			TmpNormal.xyz = normalize(TmpNormal.xyz);
		#ifdef NORMAL_MAP_ENABLE
		#ifdef CALC_BINORMAL
			TmpTangent.xyz = normalize(Tangent.xyz - Normal.xyz * dot(Normal.xyz, Tangent.xyz)) * Tangent.w;
			TmpBinormal.xyz = cross(Normal.xyz, Tangent.xyz) * Tangent.w;
		#else
			TmpTangent.xyz = normalize(TmpTangent.xyz);
			TmpBinormal.xyz = normalize(TmpBinormal.xyz);
		#endif
		#endif
		#endif
		#else
			TmpPosition = Position;
			FAT_CALC(TmpPosition, Normal);

		#ifdef USE_NOM
				TmpNormal = Normal.xyz;
		#ifdef NORMAL_MAP_ENABLE
		#ifdef CALC_BINORMAL
			TmpTangent.xyz = normalize(Tangent.xyz - Normal.xyz * dot(Normal.xyz, Tangent.xyz)) * Tangent.w;
			TmpBinormal.xyz = cross(Normal.xyz, Tangent.xyz) * Tangent.w;
		#else
			TmpTangent = Tangent.xyz;
			TmpBinormal = Binormal.xyz;
		#endif
		#endif
		#endif
		#endif

		#ifdef WORLD_COORDINATE
		#ifdef USE_NOM
			vNormal.xyz = TmpNormal.xyz;
		#endif
		#ifdef NORMAL_MAP_ENABLE
			vTangent = TmpTangent.xyz;
			vBinormal = TmpBinormal.xyz;
		#endif
		#else
			TmpPosition = MUL(World, TmpPosition);
		#ifdef USE_NOM
			vNormal.xyz = normalize(MUL(TmpNormal.xyz, MAT3(WorldInverse)));
		#endif
		#ifdef NORMAL_MAP_ENABLE
			vTangent = normalize(MUL(TmpTangent.xyz, MAT3(WorldInverse)));
			vBinormal = normalize(MUL(TmpBinormal.xyz, MAT3(WorldInverse)));
		#endif
		#endif
	#else
		#ifdef YLOCK
			float3x3 camPos;
			camPos[2] = normalize(float3(ViewInverse[2].x, 0.0, ViewInverse[2].z));
			camPos[0] = normalize(cross(float3(0.0, 1.0, 0.0), camPos[2]));
			camPos[1] = cross(camPos[2], camPos[0]);
			#define VIEWINV camPos
		#else
			#define VIEWINV MAT3(ViewInverse)
		#endif

		float4 InPos;
		#ifdef FAT
			float4 pos = Position;
			pos.xyz += Normal.xyz*Fat;
			InPos.xyz = MUL(VIEWINV, pos.xyz);
		#else
			InPos.xyz = MUL(VIEWINV, Position.xyz);
		#endif

		InPos.w = 1.0;
		#ifdef USE_NOM
			float3  InNom = MUL(VIEWINV, Normal.xyz);
			#ifdef NORMAL_MAP_ENABLE
				float3  InTan = MUL(VIEWINV, Tangent.xyz);
				#ifndef CALC_BINORMAL
					float3  InBin = MUL(VIEWINV, Binormal.xyz);
				#endif
			#endif
		#endif

		float3 wscale;
		float3 wtrans;

		#define MakeBillboardTransform(w0,w1,w2) wscale = float3(length(float3(w0.x, w1.x, w2.x)), length(float3(w0.y, w1.y, w2.y)), length(float3(w0.z, w1.z, w2.z)));	wtrans = float3(w0.w, w1.w, w2.w)

		#ifdef SKINNING_ENABLE
		#ifndef ENABLE_WEIGHTS
		float4 pos = InPos;
		#ifdef ONE_INDEX
		float idx = Indices*3.0;
		#endif
		MakeBillboardTransform(MatrixPalette[IDX + 0], MatrixPalette[IDX + 1], MatrixPalette[IDX + 2]);
		TmpPosition.xyz = pos.xyz*wscale + wtrans;
		TmpPosition.w = 1.0;
		#else
		float3 tmp;
		WEIGHT_FLOAT idx = Indices * 3.0;
		#if( BONE_INFLUENCE >= 1 )
		MakeBillboardTransform(MatrixPalette[IDX(x) + 0], MatrixPalette[IDX(x) + 1], MatrixPalette[IDX(x) + 2]);
		tmp.xyz = InPos.xyz*wscale + wtrans;
		TmpPosition.xyz = tmp * Weights.x;
		TmpPosition.w = 1.0;
		#endif
		#if( BONE_INFLUENCE >= 2 )
		MakeBillboardTransform(MatrixPalette[IDX(y) + 0], MatrixPalette[IDX(y) + 1], MatrixPalette[IDX(y) + 2]);
		tmp.xyz = InPos.xyz*wscale + wtrans;
		TmpPosition.xyz += tmp * Weights.y;
		#endif
		#if( BONE_INFLUENCE >= 3 )
		MakeBillboardTransform(MatrixPalette[IDX(z) + 0], MatrixPalette[IDX(z) + 1], MatrixPalette[IDX(z) + 2]);
		tmp.xyz = InPos.xyz*wscale + wtrans;
		TmpPosition.xyz += tmp * Weights.z;
		#endif
		#if( BONE_INFLUENCE >= 4 )
		MakeBillboardTransform(MatrixPalette[IDX(w) + 0], MatrixPalette[IDX(w) + 1], MatrixPalette[IDX(w) + 2]);
		tmp.xyz = InPos.xyz*wscale + wtrans;
		TmpPosition.xyz += tmp * Weights.w;
		#endif
		#endif

		#ifdef USE_NOM
		TmpNormal.xyz = normalize(InNom.xyz);
		#ifdef NORMAL_MAP_ENABLE
		#ifdef CALC_BINORMAL
		TmpTangent.xyz = normalize(InTan.xyz - InNom.xyz * dot(InNom.xyz, InTan.xyz)) * Tangent.w;
		TmpBinormal.xyz = cross(InNom.xyz, InTan.xyz) * Tangent.w;
		#else
		TmpTangent.xyz = normalize(InTan.xyz);
		TmpBinormal.xyz = normalize(InBin.xyz);
		#endif
		#endif
		#endif
		#else
		TmpPosition = InPos;
		#ifdef USE_NOM
		TmpNormal = InNom;
		#ifdef NORMAL_MAP_ENABLE
		#ifdef CALC_BINORMAL
		TmpTangent.xyz = normalize(InTan.xyz - InNom.xyz * dot(InNom.xyz, InTan.xyz)) * Tangent.w;
		TmpBinormal.xyz = cross(InNom.xyz, InTan.xyz) * Tangent.w;
		#else
		TmpTangent = InTan;
		TmpBinormal = InBin;
		#endif
		#endif
		#endif
		#endif

		#ifdef WORLD_COORDINATE
		#ifdef USE_NOM
		vNormal.xyz = TmpNormal.xyz;
		#endif
		#ifdef NORMAL_MAP_ENABLE
		vTangent = TmpTangent.xyz;
		vBinormal = TmpBinormal.xyz;
		#endif
		#else
		MakeBillboardTransform(World[0], World[1], World[2]);
		TmpPosition.xyz = TmpPosition.xyz*wscale + wtrans;
		TmpPosition.w = 1.0;
		#ifdef USE_NOM
		vNormal.xyz = normalize(TmpNormal.xyz);
		#endif
		#ifdef NORMAL_MAP_ENABLE
		vTangent = normalize(TmpTangent.xyz);
		vBinormal = normalize(TmpBinormal.xyz);
		#endif
		#endif
	#endif

	#ifdef USE_WPOS
		vWorldPosition = TmpPosition;
	#endif

	#ifdef LIGHT_VIEW_POSITION
		vLViewPosition = MUL(LightViewProj, TmpPosition);
	#endif

	#ifdef LIGHT_VIEW_TRANSFORM
		#ifdef LIGHT_VIEW_POSITION
			vHPosition = vLViewPosition;
		#else
			vHPosition = MUL(LightViewProj, TmpPosition);
		#endif
	#else
		#ifdef NO_VIEW_PROJ
			vHPosition = float4(TmpPosition.xyz, 1.0);
		#else
			vHPosition = MUL(ViewProj, TmpPosition);
		#endif
	#endif

	#ifdef LIGHT_VIEW_POSITION
			vLViewPosition.z = min(1.0, vLViewPosition.z);
	#endif

	#ifdef USE_TEX0	
		#ifdef PROJ_MAP0
			vTexcoord0.xy = vHPosition.xy/vHPosition.w;
		#else
			vTexcoord0.xy = Texcoord0;
		#endif

		#ifdef SCROLL_SET0
		vTexcoord0.xy += frac(ScrollSpeedSet0 * Time);	
		#endif	
						
		#ifdef ROTATION_SET0
		float c = cos(RotationSet0);
		float s = sin(RotationSet0);
		vTexcoord0.xy = float2(vTexcoord0.x * c - vTexcoord0.y * s, vTexcoord0.x * s + vTexcoord0.y * c);
		#endif

		#ifdef OFFSET_SET0
		vTexcoord0.xy = vTexcoord0.xy + OffsetSet0;
		#endif

		#ifdef SCALE_SET0
		vTexcoord0.xy = vTexcoord0.xy * ScaleSet0;
		#endif

		#ifdef FLIP_TEXTURE_Y
		vTexcoord0.y = 1.0 - vTexcoord0.y;
		#endif
	#endif

	#ifdef USE_TEX1
		#ifdef PROJ_MAP1
		vTexcoord1.xy = vHPosition.xy/vHPosition.w;
		#else
		vTexcoord1 = Texcoord1.xy.xy;
		#endif

		#ifdef SCROLL_SET1
		vTexcoord1.xy += frac(ScrollSpeedSet1 * Time);	
		#endif

		#ifdef ROTATION_SET1
		float c = cos(RotationSet1);
		float s = sin(RotationSet1);
		vTexcoord1.xy = float2(vTexcoord1.x * c - vTexcoord1.y * s, vTexcoord1.x * s + vTexcoord1.y * c);
		#endif

		#ifdef OFFSET_SET1
		vTexcoord1.xy = vTexcoord1.xy + OffsetSet1;
		#endif

		#ifdef SCALE_SET1
		vTexcoord1.xy = vTexcoord1.xy * ScaleSet1;
		#endif

		#ifdef FLIP_TEXTURE_Y
		vTexcoord1.y = 1.0 - vTexcoord1.y;
		#endif
	#endif

	#ifdef USE_TEX2
		#ifdef PROJ_MAP2
		vTexcoord2.xy = vHPosition.xy/vHPosition.w;
		#else
		vTexcoord2 = Texcoord2.xy;
		#endif

		#ifdef SCROLL_SET2
		vTexcoord2.xy += frac(ScrollSpeedSet2 * Time);
		#endif

		#ifdef ROTATION_SET2
		float c = cos(RotationSet2);
		float s = sin(RotationSet2);
		vTexcoord2.xy = float2(vTexcoord2.x * c - vTexcoord2.y * s, vTexcoord2.x * s + vTexcoord2.y * c);
		#endif

		#ifdef OFFSET_SET2
		vTexcoord2.xy = vTexcoord2.xy + OffsetSet2;
		#endif

		#ifdef SCALE_SET
		vTexcoord2.xy = vTexcoord2.xy * ScaleSet2;
		#endif

		#ifdef FLIP_TEXTURE_Y
		vTexcoord2.y = 1.0 - vTexcoord2.y;
		#endif
	#endif

	#ifdef VERTEX_PAINT_ENABLE
	vColor = Color;
	#endif

	#ifdef POINT_PARTICLE_SHADER
	float De = sqrt(ViewInverse._14 * ViewInverse._14 +
		ViewInverse._24 * ViewInverse._24 +
		ViewInverse._34 * ViewInverse._34);
	float A = 0.0;
	float B = 0.0;
	float C = 1.0;
	vPointSize = ViewportPixelSize.y * PointSize * sqrt(1.0 / (A + B * De + C * (De * De)));
	#endif


	#ifdef USE_COUV	
	#if (COVER_MAP_SPACE==WorldSpace)
	#ifdef FLIP_TEXTURE_Y
	vCoverUV = ((float2(TmpPosition.x, -TmpPosition.z)/TmpPosition.w) * CoverScale + CoverOffset) * 0.5 - 0.5;	
	#else
	vCoverUV = ((float2(TmpPosition.x, TmpPosition.z)/TmpPosition.w) * CoverScale + CoverOffset) * 0.5 - 0.5;	
	#endif
	#else
	#ifdef FLIP_TEXTURE_Y
	vCoverUV = (float2(Position.x, -Position.z) * CoverScale + CoverOffset) * 0.5 - 0.5;
	#else
	vCoverUV = (float2(Position.x, Position.z) * CoverScale + CoverOffset) * 0.5 - 0.5;	
	#endif
	#endif
	#endif

	#ifdef USE_SPOS	
	vScreenPos = vHPosition;	
	vScreenPos.xy = vScreenPos.xy * 0.5 + (0.5*vHPosition.w);	
	#endif

	#ifdef RECEIVE_SHADOW_ENABLE
	vShadowMapCoord = MUL(LightViewProj, float4(TmpPosition.xyz,1.0));
	vShadowMapCoord.x *= 0.5;
	#ifdef FLIP_TEXTURE_Y
	vShadowMapCoord.y *= -0.5;
	#else
	vShadowMapCoord.y *= 0.5;
	#endif
	vShadowMapCoord.xy += vShadowMapCoord.w * 0.5;
	#endif

	#ifdef FOG
	#ifndef TARGET
	float FogParamA = FogFar / (FogFar - FogNear);
	float FogParamB = -1.0f / (FogFar - FogNear);
	#endif
	#ifdef HEIGHT_FOG
	vFogFactor = 1.0 - (FogParamA + TmpPosition.y * FogParamB);
	#else
	vFogFactor = 1.0 - (FogParamA + vHPosition.w * FogParamB);
	#endif
	vFogFactor = clamp(vFogFactor,0.0,1.0)*FogAlpha;
	#endif

	#ifdef USE_VIEW
		#ifdef TARGET
		vView = normalize(CameraPosition - TmpPosition.xyz);
		#else
		#ifdef DIRECT3D_VERSION
		vView = normalize(ViewInverse[3].xyz - TmpPosition.xyz);
		#else
		vView = normalize(float3(ViewInverse[0].w,ViewInverse[1].w,ViewInverse[2].w) - TmpPosition.xyz);
		#endif
		#endif
		#ifdef USE_VIEW2
		vView2 = normalize(vView + mul(ViewOffset, (float3x3)View));
		#endif
	#endif

	#ifdef ZBIAS
	vHPosition.z = vHPosition.z + ZBias*0.3;
	#endif
}

//#endif

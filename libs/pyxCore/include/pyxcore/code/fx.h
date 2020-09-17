//#ifdef FX
#ifdef DIRECT3D_VERSION				
technique Main < string Script = "Pass=p0;"; >
{
	pass p0 < string Script = "Draw=geometry;"; >
	{
		ZEnable = Z_ENABLE;
		ZFunc = LessEqual;
		ZWriteEnable = Z_WRITE_ENABLE;

		#ifdef ALPHA_BLEND_STATE_ENABLE				
			AlphaBlendEnable = true;
			#if (ALPHA_BLEND_OP == OPERATION_MUL)
				BlendOp = 0;
				SrcBlend = SrcAlpha;
				DestBlend = InvSrcAlpha;
			#elif (ALPHA_BLEND_OP == OPERATION_ADD)
				BlendOp = Add;
				SrcBlend = SrcColor;
				DestBlend = One;
			#elif (ALPHA_BLEND_OP == OPERATION_SUB)
				BlendOp = ReverseSubtract;
				SrcBlend = SrcColor;
				DestBlend = One;
			#elif (ALPHA_BLEND_OP == OPERATION_COL)
				BlendOp = Add;
				SrcBlend = Zero;
				DestBlend = SrcColor;
			#endif
		#endif
		
		#ifdef ALPHA_TEST_ENABLE
			AlphaTestEnable = true;
			#ifdef POINT_SAMPLING
				AlphaRef = 128;
			#else
				AlphaRef = 10;
			#endif
			AlphaFunc = Greater;
		#else
			AlphaTestEnable = false;
		#endif

		#ifdef DOUBLE_SIDE				
			CullMode = None;
		#else				
			#ifdef XBOX
				CullMode = CCW;
			#else
				CullMode = CW;
			#endif				
		#endif
		VertexShader = compile vs_3_0 std_VS();
		PixelShader = compile ps_3_0 std_PS();
	}
}
#else
technique Main < string Script = "Pass=p0;"; >
{
	pass p0 < string Script = "Draw=geometry;"; >
	{
		DepthTestEnable = Z_ENABLE;
		DepthFunc = LEqual;
		DepthMask = Z_WRITE_ENABLE;

		#ifdef COLOR_MASK_ENABLE	
			ColorMask = float4(false, false, false, false);
		#endif

		#ifdef STENCIL_ENABLE	
			StencilTestEnable = true;
			StencilFunc = float3(Always, 3, 3);
			StencilOp = float3(Keep, Keep, Keep);
			StencilMask = 0;
		#endif

		#ifdef ALPHA_BLEND_STATE_ENABLE				
			BlendEnable = true;
			#if (ALPHA_BLEND_OP == OPERATION_MUL)
				BlendEquation = Add;
				BlendFunc = float2(SrcAlpha, OneMinusSrcAlpha);
			#elif (ALPHA_BLEND_OP == OPERATION_ADD)
				BlendEquation = Add;
				BlendFunc = float2(SrcAlpha, One);
			#elif (ALPHA_BLEND_OP == OPERATION_SUB)
				BlendEquation = ReverseSubtract;
				BlendFunc = float2(SrcAlpha, One);
			#elif (ALPHA_BLEND_OP == OPERATION_COL)
				BlendEquation = Add;
				BlendFunc = float2(Zero, SrcColor);
			#endif
		#endif

		#ifdef ALPHA_TEST_ENABLE			
				AlphaTestEnable = true;
				AlphaFunc = float2(Greater, 0.5);
		#else			
				AlphaTestEnable = false;
		#endif

		#ifdef DOUBLE_SIDE				
				CullFaceEnable = false;
		#else				
				CullFaceEnable = true;
		#endif				

		#ifdef FRONT_FACE_CULLING				
				CullFace = Front
		#endif				

		#ifdef POINT_PARTICLE_SHADER				
				PointSpriteEnable = true;
				VertexProgramPointSizeEnable = true;
		#endif				

		VertexProgram = compile vp40 std_VS();
		FragmentProgram = compile fp40 std_PS();
	}
}
#endif	
//#endif				

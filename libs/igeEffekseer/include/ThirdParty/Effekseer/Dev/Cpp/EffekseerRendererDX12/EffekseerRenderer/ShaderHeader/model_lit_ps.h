#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//
//   fxc /Zpc /Tps_4_0 /Emain /Fh ShaderHeader/model_lit_ps.h
//    Shader/model_lit_ps.fx
//
//
// Buffer Definitions: 
//
// cbuffer VS_ConstantBuffer
// {
//
//   float4 _79_fLightDirection;        // Offset:    0 Size:    16
//   float4 _79_fLightColor;            // Offset:   16 Size:    16
//   float4 _79_fLightAmbient;          // Offset:   32 Size:    16
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim Slot Elements
// ------------------------------ ---------- ------- ----------- ---- --------
// g_colorSampler                    sampler      NA          NA    0        1
// g_normalSampler                   sampler      NA          NA    1        1
// g_colorTexture                    texture  float4          2d    0        1
// g_normalTexture                   texture  float4          2d    1        1
// VS_ConstantBuffer                 cbuffer      NA          NA    1        1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// TEXCOORD                 0   xy          0     NONE  float   xy  
// TEXCOORD                 1   xyz         1     NONE  float   xyz 
// TEXCOORD                 2   xyz         2     NONE  float   xyz 
// TEXCOORD                 3   xyz         3     NONE  float   xyz 
// TEXCOORD                 4   xyzw        4     NONE  float   xyzw
// SV_Position              0   xyzw        5      POS  float       
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_Target                0   xyzw        0   TARGET  float   xyzw
//
ps_4_0
dcl_constantbuffer cb1[3], immediateIndexed
dcl_sampler s0, mode_default
dcl_sampler s1, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_resource_texture2d (float,float,float,float) t1
dcl_input_ps linear v0.xy
dcl_input_ps linear v1.xyz
dcl_input_ps linear v2.xyz
dcl_input_ps linear v3.xyz
dcl_input_ps linear v4.xyzw
dcl_output o0.xyzw
dcl_temps 3
sample r0.xyzw, v0.xyxx, t0.xyzw, s0
mul r0.xyzw, r0.xyzw, v4.xyzw
eq r1.x, r0.w, l(0.000000)
discard r1.x
sample r1.xyzw, v0.xyxx, t1.xyzw, s1
add r1.xyz, r1.xyzx, l(-0.500000, -0.500000, -0.500000, 0.000000)
add r1.xyz, r1.xyzx, r1.xyzx
mul r2.xyz, r1.yyyy, v2.xyzx
mad r1.xyw, r1.xxxx, v3.xyxz, r2.xyxz
mad r1.xyz, r1.zzzz, v1.xyzx, r1.xywx
dp3 r1.w, r1.xyzx, r1.xyzx
rsq r1.w, r1.w
mul r1.xyz, r1.wwww, r1.xyzx
dp3 r1.x, cb1[0].xyzx, r1.xyzx
max r1.x, r1.x, l(0.000000)
mad r1.xyz, cb1[1].xyzx, r1.xxxx, cb1[2].xyzx
mul o0.xyz, r0.xyzx, r1.xyzx
mov o0.w, r0.w
ret 
// Approximately 19 instruction slots used
#endif

const BYTE g_main[] =
{
     68,  88,  66,  67,  51, 137, 
    172, 217, 136,  63,   0,  37, 
    253, 181, 207, 122, 158,  43, 
    192,  95,   1,   0,   0,   0, 
     76,   6,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     32,   2,   0,   0, 216,   2, 
      0,   0,  12,   3,   0,   0, 
    208,   5,   0,   0,  82,  68, 
     69,  70, 228,   1,   0,   0, 
      1,   0,   0,   0,  12,   1, 
      0,   0,   5,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    255, 255,  16,   1,   0,   0, 
    178,   1,   0,   0, 188,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    203,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 219,   0,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     13,   0,   0,   0, 234,   0, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  13,   0,   0,   0, 
    250,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 103,  95,  99, 111, 
    108, 111, 114,  83,  97, 109, 
    112, 108, 101, 114,   0, 103, 
     95, 110, 111, 114, 109,  97, 
    108,  83,  97, 109, 112, 108, 
    101, 114,   0, 103,  95,  99, 
    111, 108, 111, 114,  84, 101, 
    120, 116, 117, 114, 101,   0, 
    103,  95, 110, 111, 114, 109, 
     97, 108,  84, 101, 120, 116, 
    117, 114, 101,   0,  86,  83, 
     95,  67, 111, 110, 115, 116, 
     97, 110, 116,  66, 117, 102, 
    102, 101, 114,   0, 250,   0, 
      0,   0,   3,   0,   0,   0, 
     36,   1,   0,   0,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 108,   1, 
      0,   0,   0,   0,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0, 128,   1,   0,   0, 
      0,   0,   0,   0, 144,   1, 
      0,   0,  16,   0,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0, 128,   1,   0,   0, 
      0,   0,   0,   0, 160,   1, 
      0,   0,  32,   0,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0, 128,   1,   0,   0, 
      0,   0,   0,   0,  95,  55, 
     57,  95, 102,  76, 105, 103, 
    104, 116,  68, 105, 114, 101, 
     99, 116, 105, 111, 110,   0, 
      1,   0,   3,   0,   1,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  95,  55, 
     57,  95, 102,  76, 105, 103, 
    104, 116,  67, 111, 108, 111, 
    114,   0,  95,  55,  57,  95, 
    102,  76, 105, 103, 104, 116, 
     65, 109,  98, 105, 101, 110, 
    116,   0,  77, 105,  99, 114, 
    111, 115, 111, 102, 116,  32, 
     40,  82,  41,  32,  72,  76, 
     83,  76,  32,  83, 104,  97, 
    100, 101, 114,  32,  67, 111, 
    109, 112, 105, 108, 101, 114, 
     32,  57,  46,  50,  57,  46, 
     57,  53,  50,  46,  51,  49, 
     49,  49,   0, 171,  73,  83, 
     71,  78, 176,   0,   0,   0, 
      6,   0,   0,   0,   8,   0, 
      0,   0, 152,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   3,   3, 
      0,   0, 152,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   7,   7, 
      0,   0, 152,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,   7,   7, 
      0,   0, 152,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,   7,   7, 
      0,   0, 152,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      4,   0,   0,   0,  15,  15, 
      0,   0, 161,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      5,   0,   0,   0,  15,   0, 
      0,   0,  84,  69,  88,  67, 
     79,  79,  82,  68,   0,  83, 
     86,  95,  80, 111, 115, 105, 
    116, 105, 111, 110,   0, 171, 
    171, 171,  79,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  84,  97, 114, 
    103, 101, 116,   0, 171, 171, 
     83,  72,  68,  82, 188,   2, 
      0,   0,  64,   0,   0,   0, 
    175,   0,   0,   0,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   1,   0, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   0,   0, 
      0,   0,  85,  85,   0,   0, 
     88,  24,   0,   4,   0, 112, 
     16,   0,   1,   0,   0,   0, 
     85,  85,   0,   0,  98,  16, 
      0,   3,  50,  16,  16,   0, 
      0,   0,   0,   0,  98,  16, 
      0,   3, 114,  16,  16,   0, 
      1,   0,   0,   0,  98,  16, 
      0,   3, 114,  16,  16,   0, 
      2,   0,   0,   0,  98,  16, 
      0,   3, 114,  16,  16,   0, 
      3,   0,   0,   0,  98,  16, 
      0,   3, 242,  16,  16,   0, 
      4,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      0,   0,   0,   0, 104,   0, 
      0,   2,   3,   0,   0,   0, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  16,  16,   0,   0,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   7, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  30,  16,   0, 
      4,   0,   0,   0,  24,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  13,   0,   4,   3, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70,  16,  16,   0, 
      0,   0,   0,   0,  70, 126, 
     16,   0,   1,   0,   0,   0, 
      0,  96,  16,   0,   1,   0, 
      0,   0,   0,   0,   0,  10, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0, 191, 
      0,   0,   0, 191,   0,   0, 
      0, 191,   0,   0,   0,   0, 
      0,   0,   0,   7, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      2,   0,   0,   0,  86,   5, 
     16,   0,   1,   0,   0,   0, 
     70,  18,  16,   0,   2,   0, 
      0,   0,  50,   0,   0,   9, 
    178,   0,  16,   0,   1,   0, 
      0,   0,   6,   0,  16,   0, 
      1,   0,   0,   0,  70,  24, 
     16,   0,   3,   0,   0,   0, 
     70,   8,  16,   0,   2,   0, 
      0,   0,  50,   0,   0,   9, 
    114,   0,  16,   0,   1,   0, 
      0,   0, 166,  10,  16,   0, 
      1,   0,   0,   0,  70,  18, 
     16,   0,   1,   0,   0,   0, 
     70,   3,  16,   0,   1,   0, 
      0,   0,  16,   0,   0,   7, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     68,   0,   0,   5, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   1,   0, 
      0,   0, 246,  15,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     16,   0,   0,   8,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     70, 130,  32,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  52,   0,   0,   7, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     50,   0,   0,  11, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70, 130,  32,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      6,   0,  16,   0,   1,   0, 
      0,   0,  70, 130,  32,   0, 
      1,   0,   0,   0,   2,   0, 
      0,   0,  56,   0,   0,   7, 
    114,  32,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     54,   0,   0,   5, 130,  32, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  62,   0,   0,   1, 
     83,  84,  65,  84, 116,   0, 
      0,   0,  19,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
     11,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0
};

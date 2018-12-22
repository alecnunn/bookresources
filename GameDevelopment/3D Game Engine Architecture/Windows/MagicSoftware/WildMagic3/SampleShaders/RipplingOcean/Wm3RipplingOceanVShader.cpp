// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3RipplingOceanVShader.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,RipplingOceanVShader,VertexShader);
WM3_IMPLEMENT_STREAM(RipplingOceanVShader);
WM3_IMPLEMENT_DEFAULT_STREAM(RipplingOceanVShader,VertexShader);
WM3_IMPLEMENT_DEFAULT_NAME_ID(RipplingOceanVShader,VertexShader);

//----------------------------------------------------------------------------
RipplingOceanVShader::RipplingOceanVShader ()
{
    // shader program (load the type of the current renderer)
    m_acProgram = ms_aacProgram[Renderer::GetType()];

    // shader constants
    int iType, iOption, iReg, iRegQuantity;
    ShaderConstant* pkConst;

    // model-to-view * projection matrix (4x4)
    iType = ShaderConstant::SC_TRANSFORM_MVP;
    iOption = ShaderConstant::SCO_MATRIX;
    iReg = 0;
    iRegQuantity = 4;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    m_kConstant.Append(pkConst);

    // camera position vector (x,y,z,1)
    iType = ShaderConstant::SC_CAMERA_POSITION;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 4;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    m_kConstant.Append(pkConst);

    // user defined vector (register 5)
    // Constants
    iType = ShaderConstant::SC_USER_DEFINED;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 5;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    m_kConstant.Append(pkConst);

    // user defined vector (register 6)
    // LightDir
    iType = ShaderConstant::SC_USER_DEFINED;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 6;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    m_kConstant.Append(pkConst);

    // user defined vector (register 7)
    // WaveDirX
    iType = ShaderConstant::SC_USER_DEFINED;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 7;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    m_kConstant.Append(pkConst);

    // user defined vector (register 8)
    // WaveDirY
    iType = ShaderConstant::SC_USER_DEFINED;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 8;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    m_kConstant.Append(pkConst);

    // user defined vector (register 9)
    // WaveSpeed
    iType = ShaderConstant::SC_USER_DEFINED;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 9;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    m_kConstant.Append(pkConst);

    // user defined vector (register 10)
    // WaveOffset
    iType = ShaderConstant::SC_USER_DEFINED;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 10;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    m_kConstant.Append(pkConst);

    // user defined vector (register 11)
    // WaveHeight
    iType = ShaderConstant::SC_USER_DEFINED;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 11;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    m_kConstant.Append(pkConst);

    // user defined vector (register 12)
    // BumpSpeed
    iType = ShaderConstant::SC_USER_DEFINED;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 12;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    m_kConstant.Append(pkConst);

    // numerical constant vector (register 14)
    iType = ShaderConstant::SC_NUMERICAL_CONSTANT;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 14;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    pkConst->SetRegisterData(0,0.000000f,0.500000f,1.00000f,0.000000f);
    m_kConstant.Append(pkConst);

    // numerical constant vector (register 15)
    iType = ShaderConstant::SC_NUMERICAL_CONSTANT;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 15;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    pkConst->SetRegisterData(0,0.250000f,-9.00000f,0.750000f,0.159155f);
    m_kConstant.Append(pkConst);

    // numerical constant vector (register 16)
    iType = ShaderConstant::SC_NUMERICAL_CONSTANT;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 16;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    pkConst->SetRegisterData(0,24.9808f,-24.9808f,-60.1458f,60.1458f);
    m_kConstant.Append(pkConst);

    // numerical constant vector (register 17)
    iType = ShaderConstant::SC_NUMERICAL_CONSTANT;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 17;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    pkConst->SetRegisterData(0,85.4538f,-85.4538f,-64.9393f,64.9393f);
    m_kConstant.Append(pkConst);

    // numerical constant vector (register 18)
    iType = ShaderConstant::SC_NUMERICAL_CONSTANT;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 18;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    pkConst->SetRegisterData(0,19.7392f,-19.7392f,-1.00000f,1.00000f);
    m_kConstant.Append(pkConst);

    // numerical constant vector (register 13)
    iType = ShaderConstant::SC_NUMERICAL_CONSTANT;
    iOption = ShaderConstant::SCO_NONE;
    iReg = 13;
    iRegQuantity = 1;
    pkConst = new ShaderConstant(iType,iOption,iReg,iRegQuantity);
    pkConst->SetRegisterData(0,3.14159f,0.500000f,6.28319f,2.00000f);
    m_kConstant.Append(pkConst);

}
//----------------------------------------------------------------------------
RipplingOceanVShader::~RipplingOceanVShader ()
{
}
//----------------------------------------------------------------------------
const char* RipplingOceanVShader::ms_aacProgram[Renderer::RT_QUANTITY] =
{
"!!ARBvp1.0\n"
"# ARB_vertex_program generated by NVIDIA Cg compiler\n"
"# cgc version 1.1.0003, build date Mar  4 2003  12:32:10\n"
"# command line args: -q -profile arbvp1 -entry vmain\n"
"# nv30vp backend compiling 'vmain' program\n"
"PARAM c14 = { 0, 0.5, 1, 0 };\n"
"PARAM c15 = { 0.25, -9, 0.75, 0.159155 };\n"
"PARAM c16 = { 24.9808, -24.9808, -60.1458, 60.1458 };\n"
"PARAM c17 = { 85.4538, -85.4538, -64.9394, 64.9394 };\n"
"PARAM c18 = { 19.7392, -19.7392, -1, 1 };\n"
"PARAM c13 = { 3.14159, 0.5, 6.28319, 2 };\n"
"#vendor NVIDIA Corporation\n"
"#version 1.0.02\n"
"#profile arbvp1\n"
"#program vmain\n"
"#semantic vmain.Wm3TransformMVP\n"
"#semantic vmain.Wm3CameraPosition\n"
"#semantic vmain.kConstants\n"
"#semantic vmain.kLightDir\n"
"#semantic vmain.kWaveDirX\n"
"#semantic vmain.kWaveDirY\n"
"#semantic vmain.kWaveSpeed\n"
"#semantic vmain.kWaveOffset\n"
"#semantic vmain.kWaveHeight\n"
"#semantic vmain.kBumpSpeed\n"
"#var float4 i_f4Position : $vin.POSITION : POSITION : 0 : 1\n"
"#var float4 i_f4Normal : $vin.NORMAL : NORMAL : 1 : 1\n"
"#var float3 i_f3Tangent : $vin.COLOR : COLOR : 2 : 1\n"
"#var float2 i_f2Tex : $vin.TEXCOORD0 : TEXCOORD0 : 3 : 1\n"
"#var float4 o_f4Position : $vout.POSITION : POSITION : 4 : 1\n"
"#var float2 o_f2Tex0 : $vout.TEXCOORD0 : TEXCOORD0 : 5 : 1\n"
"#var float2 o_f2Tex1 : $vout.TEXCOORD1 : TEXCOORD1 : 6 : 1\n"
"#var float3 o_f3View : $vout.TEXCOORD2 : TEXCOORD2 : 7 : 1\n"
"#var float3 o_f3Tangent : $vout.TEXCOORD3 : TEXCOORD3 : 8 : 1\n"
"#var float3 o_f3Binormal : $vout.TEXCOORD4 : TEXCOORD4 : 9 : 1\n"
"#var float3 o_f3Normal : $vout.TEXCOORD5 : TEXCOORD5 : 10 : 1\n"
"#var float4 o_f4LightDir : $vout.TEXCOORD6 : TEXCOORD6 : 11 : 1\n"
"#var float4x4 Wm3TransformMVP :  : c[0], 4 : 12 : 1\n"
"#var float3 Wm3CameraPosition :  : c[4] : 13 : 1\n"
"#var float4 kConstants :  : c[5] : 14 : 1\n"
"#var float3 kLightDir :  : c[6] : 15 : 1\n"
"#var float4 kWaveDirX :  : c[7] : 16 : 1\n"
"#var float4 kWaveDirY :  : c[8] : 17 : 1\n"
"#var float4 kWaveSpeed :  : c[9] : 18 : 1\n"
"#var float4 kWaveOffset :  : c[10] : 19 : 1\n"
"#var float4 kWaveHeight :  : c[11] : 20 : 1\n"
"#var float4 kBumpSpeed :  : c[12] : 21 : 1\n"
"TEMP R0, R1, R2, R3, R4, R5, R6, R7, R8, R9;\n"
"ATTRIB v24 = vertex.texcoord[0];\n"
"ATTRIB v19 = vertex.color;\n"
"ATTRIB v18 = vertex.normal;\n"
"ATTRIB v16 = vertex.position;\n"
"PARAM c6 = program.local[6];\n"
"PARAM c12 = program.local[12];\n"
"PARAM c4 = program.local[4];\n"
"PARAM c0[4] = { program.local[0..3] };\n"
"PARAM c11 = program.local[11];\n"
"PARAM c10 = program.local[10];\n"
"PARAM c9 = program.local[9];\n"
"PARAM c8 = program.local[8];\n"
"PARAM c7 = program.local[7];\n"
"PARAM c5 = program.local[5];\n"
"	MOV R6, c5;\n"
"	MUL R0, v24.y, c8;\n"
"	MAD R0, v24.x, c7, R0;\n"
"	MAD R0, c9, R6.w, R0;\n"
"	ADD R0, R0, c10;\n"
"	FRC R0, R0;\n"
"	MOV R5.yzw, c13;\n"
"	ADD R0, R0, -R5.y;\n"
"	MUL R1, R0, R5.z;\n"
"	MUL R3.zw, c15.w, R1.x;\n"
"	ADD R3.xy, R3.z, -c15.x;\n"
"	FRC R3.y, R3.x;\n"
"	MOV R4.xyz, c14;\n"
"	ADD R0.xyz, R4.xyzx, -R3.y;\n"
"	MUL R0, R0.xyzx, R0.xyzx;\n"
"	SLT R2.x, R3.y, c15.x;\n"
"	SGE R7.xyw, R3.y, c15.yzyy;\n"
"	MOV R2.yz, R7.xxyw;\n"
"	DP3 R2.y, R2.xyzx, c18.zwzz;\n"
"	MAD R7.xyz, c16.xyxx, R0.xyzx, c16.zwzz;\n"
"	MAD R7.xyz, R7.xyzx, R0.xyzx, c17.xyxx;\n"
"	MAD R7.xyz, R7.xyzx, R0.xyzx, c17.zwzz;\n"
"	MAD R7.xyz, R7.xyzx, R0.xyzx, c18.xyxx;\n"
"	MAD R7.xyz, R7.xyzx, R0.xyzx, c18.zwzz;\n"
"	DP3 R0.x, R7.xyzx, -R2.xyzx;\n"
"	MUL R3.xy, c15.w, R1.y;\n"
"	ADD R8.xy, R3.x, -c15.x;\n"
"	FRC R8.y, R8.x;\n"
"	ADD R2.xyz, R4.xyzx, -R8.y;\n"
"	MUL R7.xyz, R2.xyzx, R2.xyzx;\n"
"	SLT R2.x, R8.y, c15.x;\n"
"	SGE R8.xyw, R8.y, c15.yzyy;\n"
"	MOV R2.yz, R8.xxyw;\n"
"	DP3 R2.y, R2.xyzx, c18.zwzz;\n"
"	MAD R8.xyz, c16.xyxx, R7.xyzx, c16.zwzz;\n"
"	MAD R8.xyz, R8.xyzx, R7.xyzx, c17.xyxx;\n"
"	MAD R8.xyz, R8.xyzx, R7.xyzx, c17.zwzz;\n"
"	MAD R8.xyz, R8.xyzx, R7.xyzx, c18.xyxx;\n"
"	MAD R8.xyz, R8.xyzx, R7.xyzx, c18.zwzz;\n"
"	DP3 R7.x, R8.xyzx, -R2.xyzx;\n"
"	MOV R0.y, R7.x;\n"
"	MUL R2.zw, c15.w, R1.z;\n"
"	ADD R2.xy, R2.z, -c15.x;\n"
"	FRC R2.y, R2.x;\n"
"	ADD R7.xyz, R4.xyzx, -R2.y;\n"
"	MUL R8.xyz, R7.xyzx, R7.xyzx;\n"
"	SLT R7.x, R2.y, c15.x;\n"
"	SGE R9.xyw, R2.y, c15.yzyy;\n"
"	MOV R7.yz, R9.xxyw;\n"
"	DP3 R7.y, R7.xyzx, c18.zwzz;\n"
"	MAD R9.xyz, c16.xyxx, R8.xyzx, c16.zwzz;\n"
"	MAD R9.xyz, R9.xyzx, R8.xyzx, c17.xyxx;\n"
"	MAD R9.xyz, R9.xyzx, R8.xyzx, c17.zwzz;\n"
"	MAD R9.xyz, R9.xyzx, R8.xyzx, c18.xyxx;\n"
"	MAD R9.xyz, R9.xyzx, R8.xyzx, c18.zwzz;\n"
"	DP3 R8.x, R9.xyzx, -R7.xyzx;\n"
"	MOV R0.z, R8.x;\n"
"	MUL R2.xy, c15.w, R1.w;\n"
"	ADD R8.xy, R2.x, -c15.x;\n"
"	FRC R8.y, R8.x;\n"
"	ADD R1.xyz, R4.xyzx, -R8.y;\n"
"	MUL R7.xyz, R1.xyzx, R1.xyzx;\n"
"	SLT R1.x, R8.y, c15.x;\n"
"	SGE R8.xyw, R8.y, c15.yzyy;\n"
"	MOV R1.yz, R8.xxyw;\n"
"	DP3 R1.y, R1.xyzx, c18.zwzz;\n"
"	MAD R8.xyz, c16.xyxx, R7.xyzx, c16.zwzz;\n"
"	MAD R8.xyz, R8.xyzx, R7.xyzx, c17.xyxx;\n"
"	MAD R8.xyz, R8.xyzx, R7.xyzx, c17.zwzz;\n"
"	MAD R8.xyz, R8.xyzx, R7.xyzx, c18.xyxx;\n"
"	MAD R8.xyz, R8.xyzx, R7.xyzx, c18.zwzz;\n"
"	DP3 R7.x, R8.xyzx, -R1.xyzx;\n"
"	MOV R0.w, R7.x;\n"
"	DP4 R0.x, R0, c11;\n"
"	MOV R1, v18;\n"
"	MAD R0, R0.x, R1, v16;\n"
"	DPH result.position.x, R0, c0[0];\n"
"	DPH result.position.y, R0, c0[1];\n"
"	DPH result.position.z, R0, c0[2];\n"
"	DPH result.position.w, R0, c0[3];\n"
"	FRC R3.w, R3.z;\n"
"	ADD R0.xyz, R4.xyzx, -R3.w;\n"
"	MUL R0, R0.xyzx, R0.xyzx;\n"
"	SLT R7.x, R3.w, c15.x;\n"
"	SGE R8.xyw, R3.w, c15.yzyy;\n"
"	MOV R7.yz, R8.xxyw;\n"
"	DP3 R7.y, R7.xyzx, c18.zwzz;\n"
"	MAD R8.xyz, c16.xyxx, R0.xyzx, c16.zwzz;\n"
"	MAD R8.xyz, R8.xyzx, R0.xyzx, c17.xyxx;\n"
"	MAD R8.xyz, R8.xyzx, R0.xyzx, c17.zwzz;\n"
"	MAD R8.xyz, R8.xyzx, R0.xyzx, c18.xyxx;\n"
"	MAD R8.xyz, R8.xyzx, R0.xyzx, c18.zwzz;\n"
"	DP3 R0.x, R8.xyzx, -R7.xyzx;\n"
"	FRC R3.y, R3.x;\n"
"	ADD R7.xyz, R4.xyzx, -R3.y;\n"
"	MUL R8.xyz, R7.xyzx, R7.xyzx;\n"
"	SLT R7.x, R3.y, c15.x;\n"
"	SGE R3.xyw, R3.y, c15.yzyy;\n"
"	MOV R7.yz, R3.xxyw;\n"
"	DP3 R7.y, R7.xyzx, c18.zwzz;\n"
"	MAD R3.xyz, c16.xyxx, R8.xyzx, c16.zwzz;\n"
"	MAD R3.xyz, R3.xyzx, R8.xyzx, c17.xyxx;\n"
"	MAD R3.xyz, R3.xyzx, R8.xyzx, c17.zwzz;\n"
"	MAD R3.xyz, R3.xyzx, R8.xyzx, c18.xyxx;\n"
"	MAD R3.xyz, R3.xyzx, R8.xyzx, c18.zwzz;\n"
"	DP3 R8.x, R3.xyzx, -R7.xyzx;\n"
"	MOV R0.y, R8.x;\n"
"	FRC R2.w, R2.z;\n"
"	ADD R3.xyz, R4.xyzx, -R2.w;\n"
"	MUL R7.xyz, R3.xyzx, R3.xyzx;\n"
"	SLT R3.x, R2.w, c15.x;\n"
"	SGE R8.xyw, R2.w, c15.yzyy;\n"
"	MOV R3.yz, R8.xxyw;\n"
"	DP3 R3.y, R3.xyzx, c18.zwzz;\n"
"	MAD R8.xyz, c16.xyxx, R7.xyzx, c16.zwzz;\n"
"	MAD R8.xyz, R8.xyzx, R7.xyzx, c17.xyxx;\n"
"	MAD R8.xyz, R8.xyzx, R7.xyzx, c17.zwzz;\n"
"	MAD R8.xyz, R8.xyzx, R7.xyzx, c18.xyxx;\n"
"	MAD R8.xyz, R8.xyzx, R7.xyzx, c18.zwzz;\n"
"	DP3 R7.x, R8.xyzx, -R3.xyzx;\n"
"	MOV R0.z, R7.x;\n"
"	FRC R2.y, R2.x;\n"
"	ADD R3.xyz, R4.xyzx, -R2.y;\n"
"	MUL R7.xyz, R3.xyzx, R3.xyzx;\n"
"	SLT R3.x, R2.y, c15.x;\n"
"	SGE R2.xyw, R2.y, c15.yzyy;\n"
"	MOV R3.yz, R2.xxyw;\n"
"	DP3 R3.y, R3.xyzx, c18.zwzz;\n"
"	MAD R2.xyz, c16.xyxx, R7.xyzx, c16.zwzz;\n"
"	MAD R2.xyz, R2.xyzx, R7.xyzx, c17.xyxx;\n"
"	MAD R2.xyz, R2.xyzx, R7.xyzx, c17.zwzz;\n"
"	MAD R2.xyz, R2.xyzx, R7.xyzx, c18.xyxx;\n"
"	MAD R2.xyz, R2.xyzx, R7.xyzx, c18.zwzz;\n"
"	DP3 R7.x, R2.xyzx, -R3.xyzx;\n"
"	MOV R0.w, R7.x;\n"
"	MUL R2, R0, c11;\n"
"	DP4 R0.x, R2, c7;\n"
"	DP4 R0.w, R2, c8;\n"
"	MOV R0.x, -R0.x;\n"
"	MOV R0.yz, -R0.w;\n"
"	MUL R0.yzw, R0.xxyz, R6.x;\n"
"	ADD R1.xy, R1.xyxx, R0.yzyy;\n"
"	DP3 R0.x, R1.xyzx, R1.xyzx;\n"
"	RSQ R0.x, R0.x;\n"
"	MUL R2.xyz, R0.x, R1.xyzx;\n"
"	RCP R0.x, R5.w;\n"
"	MAD result.texcoord[5].xyz, R2.xyzx, R0.x, R5.y;\n"
"	MAD R1.xyz, v19.xyzx, R5.w, -R4.z;\n"
"	ADD R1.z, R1.z, -R0.w;\n"
"	DP3 R0.y, R1.xyzx, R1.xyzx;\n"
"	RSQ R0.y, R0.y;\n"
"	MUL R1.xyz, R0.y, R1.xyzx;\n"
"	MAD result.texcoord[3].xyz, R1.xyzx, R0.x, R5.y;\n"
"	MUL R0.yzw, R2.zzxy, R1.yyzx;\n"
"	MAD R1.xyz, R2.yzxy, R1.zxyz, -R0.yzwy;\n"
"	DP3 R0.y, R1.xyzx, R1.xyzx;\n"
"	RSQ R0.y, R0.y;\n"
"	MUL R0.yzw, R0.y, R1.xxyz;\n"
"	MAD result.texcoord[4].xyz, R0.yzwy, R0.x, R5.y;\n"
"	MOV R2.xyz, R0;\n"
"	ADD R1.xyz, R2.xyzx, -c4.xyzx;\n"
"	DP3 R0.y, R1.xyzx, R1.xyzx;\n"
"	RSQ R0.y, R0.y;\n"
"	MUL R0.yzw, R0.y, R1.xxyz;\n"
"	MAD result.texcoord[2].xyz, R0.yzwy, R0.x, R5.y;\n"
"	MUL R0.yz, R6.z, v24.xxyx;\n"
"	MAD result.texcoord[0].xy, c12.xyxx, R6.w, R0.yzyy;\n"
"	MAD R1, c12.wzww, R6.w, R0.yzyy;\n"
"	MOV result.texcoord[1].xy, R1.yxzw;\n"
"	MAD result.texcoord[6].xyz, c6.xyzx, R0.x, R5.y;\n"
"	MOV result.texcoord[6].w, R6.y;\n"
"END\n"
"# 172 instructions\n"
"# 10 temp registers\n"
"# End of program\n"
,
"vs_2_0\n"
"// DX9 Vertex shader generated by NVIDIA Cg compiler\n"
"// cgc version 1.1.0003, build date Mar  4 2003  12:32:10\n"
"// command line args: -q -profile vs_2_0 -entry vmain -profileopts dcls -strict\n"
"// nv30vp backend compiling 'vmain' program\n"
"def c14, 0, 0.5, 1, 0\n"
"def c15, 0.25, -9, 0.75, 0.159155\n"
"def c16, 24.9808, -24.9808, -60.1458, 60.1458\n"
"def c17, 85.4538, -85.4538, -64.9394, 64.9394\n"
"def c18, 19.7392, -19.7392, -1, 1\n"
"def c13, 3.14159, 0.5, 6.28319, 2\n"
"//vendor NVIDIA Corporation\n"
"//version 1.0.02\n"
"//profile vs_2_0\n"
"//program vmain\n"
"//semantic vmain.Wm3TransformMVP\n"
"//semantic vmain.Wm3CameraPosition\n"
"//semantic vmain.kConstants\n"
"//semantic vmain.kLightDir\n"
"//semantic vmain.kWaveDirX\n"
"//semantic vmain.kWaveDirY\n"
"//semantic vmain.kWaveSpeed\n"
"//semantic vmain.kWaveOffset\n"
"//semantic vmain.kWaveHeight\n"
"//semantic vmain.kBumpSpeed\n"
"//var float4 i_f4Position : $vin.POSITION : POSITION : 0 : 1\n"
"//var float4 i_f4Normal : $vin.NORMAL : NORMAL : 1 : 1\n"
"//var float3 i_f3Tangent : $vin.COLOR : COLOR : 2 : 1\n"
"//var float2 i_f2Tex : $vin.TEXCOORD0 : TEXCOORD0 : 3 : 1\n"
"//var float4 o_f4Position : $vout.POSITION : POSITION : 4 : 1\n"
"//var float2 o_f2Tex0 : $vout.TEXCOORD0 : TEXCOORD0 : 5 : 1\n"
"//var float2 o_f2Tex1 : $vout.TEXCOORD1 : TEXCOORD1 : 6 : 1\n"
"//var float3 o_f3View : $vout.TEXCOORD2 : TEXCOORD2 : 7 : 1\n"
"//var float3 o_f3Tangent : $vout.TEXCOORD3 : TEXCOORD3 : 8 : 1\n"
"//var float3 o_f3Binormal : $vout.TEXCOORD4 : TEXCOORD4 : 9 : 1\n"
"//var float3 o_f3Normal : $vout.TEXCOORD5 : TEXCOORD5 : 10 : 1\n"
"//var float4 o_f4LightDir : $vout.TEXCOORD6 : TEXCOORD6 : 11 : 1\n"
"//var float4x4 Wm3TransformMVP :  : c[0], 4 : 12 : 1\n"
"//var float3 Wm3CameraPosition :  : c[4] : 13 : 1\n"
"//var float4 kConstants :  : c[5] : 14 : 1\n"
"//var float3 kLightDir :  : c[6] : 15 : 1\n"
"//var float4 kWaveDirX :  : c[7] : 16 : 1\n"
"//var float4 kWaveDirY :  : c[8] : 17 : 1\n"
"//var float4 kWaveSpeed :  : c[9] : 18 : 1\n"
"//var float4 kWaveOffset :  : c[10] : 19 : 1\n"
"//var float4 kWaveHeight :  : c[11] : 20 : 1\n"
"//var float4 kBumpSpeed :  : c[12] : 21 : 1\n"
"//const c[14] = 0 0.5 1 0\n"
"//const c[15] = 0.25 -9 0.75 0.1591549\n"
"//const c[16] = 24.9808 -24.9808 -60.14581 60.14581\n"
"//const c[17] = 85.45379 -85.45379 -64.93935 64.93935\n"
"//const c[18] = 19.73921 -19.73921 -1 1\n"
"//const c[13] = 3.141593 0.5 6.283185 2\n"
"dcl_texcoord0 v3\n"
"dcl_color v2\n"
"dcl_normal v1\n"
"dcl_position v0\n"
"	mov r7, c5\n"
"	mul r0, v3.y, c8\n"
"	mad r0, v3.x, c7, r0\n"
"	mad r0, c9, r7.w, r0\n"
"	add r0, r0, c10\n"
"	frc r0, r0\n"
"	mov r6.yzw, c13\n"
"	add r0, r0, -r6.y\n"
"	mul r1, r0, r6.z\n"
"	mul r4.zw, c15.w, r1.x\n"
"	add r3.xy, r4.z, -c15.x\n"
"	frc r3.y, r3.x\n"
"	mov r5.xyz, c14\n"
"	add r0.xyz, r5.xyz, -r3.y\n"
"	mul r0, r0.xyz, r0.xyz\n"
"	slt r2.x, r3.y, c15.x\n"
"	sge r3.xyw, r3.y, c15.yz\n"
"	mov r2.yz, r3.xxyw\n"
"	dp3 r2.y, r2.xyz, c18.zwz\n"
"	mad r3.xyz, c16.xyx, r0.xyz, c16.zwz\n"
"	mad r3.xyz, r3.xyz, r0.xyz, c17.xyx\n"
"	mad r3.xyz, r3.xyz, r0.xyz, c17.zwz\n"
"	mad r3.xyz, r3.xyz, r0.xyz, c18.xyx\n"
"	mad r3.xyz, r3.xyz, r0.xyz, c18.zwz\n"
"	dp3 r0.x, r3.xyz, -r2.xyz\n"
"	mul r4.xy, c15.w, r1.y\n"
"	add r8.xy, r4.x, -c15.x\n"
"	frc r8.y, r8.x\n"
"	add r2.xyz, r5.xyz, -r8.y\n"
"	mul r3.xyz, r2.xyz, r2.xyz\n"
"	slt r2.x, r8.y, c15.x\n"
"	sge r8.xyw, r8.y, c15.yz\n"
"	mov r2.yz, r8.xxyw\n"
"	dp3 r2.y, r2.xyz, c18.zwz\n"
"	mad r8.xyz, c16.xyx, r3.xyz, c16.zwz\n"
"	mad r8.xyz, r8.xyz, r3.xyz, c17.xyx\n"
"	mad r8.xyz, r8.xyz, r3.xyz, c17.zwz\n"
"	mad r8.xyz, r8.xyz, r3.xyz, c18.xyx\n"
"	mad r8.xyz, r8.xyz, r3.xyz, c18.zwz\n"
"	dp3 r3.x, r8.xyz, -r2.xyz\n"
"	mov r0.y, r3.x\n"
"	mul r3.zw, c15.w, r1.z\n"
"	add r3.xy, r3.z, -c15.x\n"
"	frc r3.y, r3.x\n"
"	add r2.xyz, r5.xyz, -r3.y\n"
"	mul r8.xyz, r2.xyz, r2.xyz\n"
"	slt r2.x, r3.y, c15.x\n"
"	sge r9.xyw, r3.y, c15.yz\n"
"	mov r2.yz, r9.xxyw\n"
"	dp3 r2.y, r2.xyz, c18.zwz\n"
"	mad r9.xyz, c16.xyx, r8.xyz, c16.zwz\n"
"	mad r9.xyz, r9.xyz, r8.xyz, c17.xyx\n"
"	mad r9.xyz, r9.xyz, r8.xyz, c17.zwz\n"
"	mad r9.xyz, r9.xyz, r8.xyz, c18.xyx\n"
"	mad r9.xyz, r9.xyz, r8.xyz, c18.zwz\n"
"	dp3 r8.x, r9.xyz, -r2.xyz\n"
"	mov r0.z, r8.x\n"
"	mul r3.xy, c15.w, r1.w\n"
"	add r8.xy, r3.x, -c15.x\n"
"	frc r8.y, r8.x\n"
"	add r1.xyz, r5.xyz, -r8.y\n"
"	mul r2.xyz, r1.xyz, r1.xyz\n"
"	slt r1.x, r8.y, c15.x\n"
"	sge r8.xyw, r8.y, c15.yz\n"
"	mov r1.yz, r8.xxyw\n"
"	dp3 r1.y, r1.xyz, c18.zwz\n"
"	mad r8.xyz, c16.xyx, r2.xyz, c16.zwz\n"
"	mad r8.xyz, r8.xyz, r2.xyz, c17.xyx\n"
"	mad r8.xyz, r8.xyz, r2.xyz, c17.zwz\n"
"	mad r8.xyz, r8.xyz, r2.xyz, c18.xyx\n"
"	mad r8.xyz, r8.xyz, r2.xyz, c18.zwz\n"
"	dp3 r2.x, r8.xyz, -r1.xyz\n"
"	mov r0.w, r2.x\n"
"	dp4 r0.x, r0, c11\n"
"	mov r2, v1\n"
"	mad r1.xyz, r0.x, r2, v0\n"
"	mov r1.w, r5.zxxz\n"
"	dp4 oPos.x, c0, r1\n"
"	dp4 oPos.y, c1, r1\n"
"	dp4 oPos.z, c2, r1\n"
"	dp4 oPos.w, c3, r1\n"
"	frc r4.w, r4.z\n"
"	add r0.xyz, r5.xyz, -r4.w\n"
"	mul r0, r0.xyz, r0.xyz\n"
"	slt r8.x, r4.w, c15.x\n"
"	sge r9.xyw, r4.w, c15.yz\n"
"	mov r8.yz, r9.xxyw\n"
"	dp3 r8.y, r8.xyz, c18.zwz\n"
"	mad r9.xyz, c16.xyx, r0.xyz, c16.zwz\n"
"	mad r9.xyz, r9.xyz, r0.xyz, c17.xyx\n"
"	mad r9.xyz, r9.xyz, r0.xyz, c17.zwz\n"
"	mad r9.xyz, r9.xyz, r0.xyz, c18.xyx\n"
"	mad r9.xyz, r9.xyz, r0.xyz, c18.zwz\n"
"	dp3 r0.x, r9.xyz, -r8.xyz\n"
"	frc r4.y, r4.x\n"
"	add r8.xyz, r5.xyz, -r4.y\n"
"	mul r9.xyz, r8.xyz, r8.xyz\n"
"	slt r8.x, r4.y, c15.x\n"
"	sge r4.xyw, r4.y, c15.yz\n"
"	mov r8.yz, r4.xxyw\n"
"	dp3 r8.y, r8.xyz, c18.zwz\n"
"	mad r4.xyz, c16.xyx, r9.xyz, c16.zwz\n"
"	mad r4.xyz, r4.xyz, r9.xyz, c17.xyx\n"
"	mad r4.xyz, r4.xyz, r9.xyz, c17.zwz\n"
"	mad r4.xyz, r4.xyz, r9.xyz, c18.xyx\n"
"	mad r4.xyz, r4.xyz, r9.xyz, c18.zwz\n"
"	dp3 r9.x, r4.xyz, -r8.xyz\n"
"	mov r0.y, r9.x\n"
"	frc r3.w, r3.z\n"
"	add r4.xyz, r5.xyz, -r3.w\n"
"	mul r8.xyz, r4.xyz, r4.xyz\n"
"	slt r4.x, r3.w, c15.x\n"
"	sge r9.xyw, r3.w, c15.yz\n"
"	mov r4.yz, r9.xxyw\n"
"	dp3 r4.y, r4.xyz, c18.zwz\n"
"	mad r9.xyz, c16.xyx, r8.xyz, c16.zwz\n"
"	mad r9.xyz, r9.xyz, r8.xyz, c17.xyx\n"
"	mad r9.xyz, r9.xyz, r8.xyz, c17.zwz\n"
"	mad r9.xyz, r9.xyz, r8.xyz, c18.xyx\n"
"	mad r9.xyz, r9.xyz, r8.xyz, c18.zwz\n"
"	dp3 r8.x, r9.xyz, -r4.xyz\n"
"	mov r0.z, r8.x\n"
"	frc r3.y, r3.x\n"
"	add r4.xyz, r5.xyz, -r3.y\n"
"	mul r8.xyz, r4.xyz, r4.xyz\n"
"	slt r4.x, r3.y, c15.x\n"
"	sge r3.xyw, r3.y, c15.yz\n"
"	mov r4.yz, r3.xxyw\n"
"	dp3 r4.y, r4.xyz, c18.zwz\n"
"	mad r3.xyz, c16.xyx, r8.xyz, c16.zwz\n"
"	mad r3.xyz, r3.xyz, r8.xyz, c17.xyx\n"
"	mad r3.xyz, r3.xyz, r8.xyz, c17.zwz\n"
"	mad r3.xyz, r3.xyz, r8.xyz, c18.xyx\n"
"	mad r3.xyz, r3.xyz, r8.xyz, c18.zwz\n"
"	dp3 r8.x, r3.xyz, -r4.xyz\n"
"	mov r0.w, r8.x\n"
"	mul r3, r0, c11\n"
"	dp4 r0.x, r3, c7\n"
"	dp4 r0.w, r3, c8\n"
"	mov r0.x, -r0.x\n"
"	mov r0.yz, -r0.w\n"
"	mul r0.yzw, r0.xxyz, r7.x\n"
"	add r2.xy, r2.xy, r0.yz\n"
"	dp3 r0.x, r2.xyz, r2.xyz\n"
"	rsq r0.x, r0.x\n"
"	mul r3.xyz, r0.x, r2.xyz\n"
"	rcp r0.x, r6.w\n"
"	mad oT5.xyz, r3.xyz, r0.x, r6.y\n"
"	mad r2.xyz, v2.xyz, r6.w, -r5.z\n"
"	add r2.z, r2.z, -r0.w\n"
"	dp3 r0.y, r2.xyz, r2.xyz\n"
"	rsq r0.y, r0.y\n"
"	mul r2.xyz, r0.y, r2.xyz\n"
"	mad oT3.xyz, r2.xyz, r0.x, r6.y\n"
"	mul r0.yzw, r3.zzxy, r2.yyzx\n"
"	mad r2.xyz, r3.yzx, r2.zxy, -r0.yzw\n"
"	dp3 r0.y, r2.xyz, r2.xyz\n"
"	rsq r0.y, r0.y\n"
"	mul r0.yzw, r0.y, r2.xxyz\n"
"	mad oT4.xyz, r0.yzw, r0.x, r6.y\n"
"	add r1.xyz, r1.xyz, -c4.xyz\n"
"	dp3 r0.y, r1.xyz, r1.xyz\n"
"	rsq r0.y, r0.y\n"
"	mul r0.yzw, r0.y, r1.xxyz\n"
"	mad oT2.xyz, r0.yzw, r0.x, r6.y\n"
"	mul r0.yz, r7.z, v3.xxy\n"
"	mad oT0.xy, c12.xy, r7.w, r0.yz\n"
"	mad r1, c12.wz, r7.w, r0.yz\n"
"	mov oT1.xy, r1.yxzw\n"
"	mad oT6.xyz, c6.xyz, r0.x, r6.y\n"
"	mov oT6.w, r7.y\n"
"// 171 instructions\n"
"// 10 temp registers\n"
"// End of program\n"
};

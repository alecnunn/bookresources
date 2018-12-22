/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
float s1 = 1.0f / texture_resolution_x;  // one texel width
float t1 = 1.0f / texture_resolution_y;  // one texel height
float s_off =  s1 / 2.0f;     // to sample texel center
float t_off =  t1 / 2.0f;

// s,t,r,q offsets for 4 nearest neighbors (bilinear or point)
float offset_a1[4] = { -s1  + s_off, 0.0f  + t_off, 0.0f, 0.0f};
float offset_a2[4] = {  s1  + s_off, 0.0f  + t_off, 0.0f, 0.0f};
float offset_a3[4] = { 0.0f + s_off,  t1   + t_off, 0.0f, 0.0f};
float offset_a4[4] = { 0.0f + s_off, -t1   + t_off, 0.0f, 0.0f};

// s,t,r,q offsets for 8 surrounding neighbors (use bilinear)
float offset_b1[4] = {  s1/2.0f + s_off,  t1      + t_off, 0.0f, 0.0f};
float offset_b2[4] = { -s1      + s_off,  t1/2.0f + t_off, 0.0f, 0.0f};
float offset_b3[4] = { -s1/2.0f + s_off, -t1      + t_off, 0.0f, 0.0f};
float offset_b4[4] = {  s1      + s_off, -t1/2.0f + t_off, 0.0f, 0.0f};

SetVShaderConstants((TO_BASE .. T3_BASE) + SET_A, offset_a1 .. offset_a4);
SetVShaderConstants((T0_BASE .. T3_BASE) + SET_B, offset_b1 .. offset_b4);
SetVShaderConstants( OFFSET_TO_USE,  use_a ? SET_A : SET_B );
RenderFullCoverageQuad();

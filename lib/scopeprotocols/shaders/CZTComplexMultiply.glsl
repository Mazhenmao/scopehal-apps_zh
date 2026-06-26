/***********************************************************************************************************************
*                                                                                                                      *
* libscopeprotocols                                                                                                    *
*                                                                                                                      *
* Copyright (c) 2012-2026 Andrew D. Zonenberg and contributors                                                         *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
***********************************************************************************************************************/

#version 430
#pragma shader_stage(compute)

layout(std430, binding=0) restrict readonly buffer buf_a
{
	float a[];
};

layout(std430, binding=1) restrict readonly buffer buf_b
{
	float b[];
};

layout(std430, binding=2) restrict writeonly buffer buf_out
{
	float outbuf[];
};

layout(std430, push_constant) uniform constants
{
	uint npoints;
};

layout(local_size_x=64, local_size_y=1, local_size_z=1) in;

void main()
{
	uint i = gl_GlobalInvocationID.x;
	if(i >= npoints)
		return;

	float ar = a[2*i];
	float ai = a[2*i + 1];
	float br = b[2*i];
	float bi = b[2*i + 1];

	outbuf[2*i] = (ar * br) - (ai * bi);
	outbuf[2*i + 1] = (ar * bi) + (ai * br);
}

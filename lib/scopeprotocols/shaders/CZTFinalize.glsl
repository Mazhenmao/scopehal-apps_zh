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

layout(std430, binding=0) restrict readonly buffer buf_din
{
	float din[];
};

layout(std430, binding=1) restrict writeonly buffer buf_dout
{
	float dout[];
};

layout(std430, push_constant) uniform constants
{
	uint npoints;
	float theta;
	float powerScale;
};

layout(local_size_x=64, local_size_y=1, local_size_z=1) in;

void main()
{
	uint k = gl_GlobalInvocationID.x;
	if(k >= npoints)
		return;

	float phase = -theta * k * k / 2;
	float c = cos(phase);
	float s = sin(phase);

	float pr = din[2*k];
	float pi = din[2*k + 1];
	float re = (pr * c) - (pi * s);
	float im = (pr * s) + (pi * c);
	float power = max((re*re + im*im) * powerScale, 1.17549435e-38);

	dout[k] = 10 * log(power) / log(10);
}

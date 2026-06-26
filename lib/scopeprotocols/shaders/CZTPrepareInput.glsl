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
	uint fftlen;
	uint samplesUsed;
	uint inputStep;
	uint inputLength;
	uint window;
	float startPhaseStep;
	float theta;
};

layout(local_size_x=64, local_size_y=1, local_size_z=1) in;

float GetWindowCoefficient(uint i)
{
	if(samplesUsed < 2)
		return 1;

	float x = 6.283185307179586 * i / (samplesUsed - 1);
	if(window == 3)
		return 0.35875 - 0.48829*cos(x) + 0.14128*cos(2*x) - 0.01168*cos(3*x);
	else if(window == 1)
		return 0.5 - 0.5*cos(x);
	else if(window == 2)
		return 0.54 - 0.46*cos(x);
	else
		return 1;
}

void main()
{
	uint i = gl_GlobalInvocationID.x;
	if(i >= fftlen)
		return;

	if(i >= samplesUsed)
	{
		dout[2*i] = 0;
		dout[2*i + 1] = 0;
		return;
	}

	uint n = min(i * inputStep, inputLength - 1);
	float phase = -(startPhaseStep * i) - (theta * i * i / 2);
	float value = din[n] * GetWindowCoefficient(i);

	dout[2*i] = value * cos(phase);
	dout[2*i + 1] = value * sin(phase);
}

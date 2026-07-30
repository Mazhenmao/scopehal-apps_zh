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

layout(std430, binding=1) restrict readonly buffer buf_kernel
{
	float kernel[];
};

layout(std430, binding=2) restrict writeonly buffer buf_dout
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
	uint kernelRadius;
	float mixerPhaseStep;
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

	uint base = min(i * inputStep, inputLength - 1);
	int firstDelta = max(-int(kernelRadius), -int(base));
	int lastDelta = min(int(kernelRadius), int(inputLength - 1 - base));
	uint firstSample = uint(int(base) + firstDelta);

	// Mix the requested RF band to complex baseband before low-pass filtering and decimation. This prevents high
	// frequency content outside the requested CZT span from folding into the result.
	float mixerPhase = -mixerPhaseStep * firstSample;
	vec2 oscillator = vec2(cos(mixerPhase), sin(mixerPhase));
	vec2 oscillatorStep = vec2(cos(mixerPhaseStep), -sin(mixerPhaseStep));
	vec2 filtered = vec2(0);
	float coefficientSum = 0;
	for(int delta=firstDelta; delta<=lastDelta; delta++)
	{
		float coefficient = kernel[delta + int(kernelRadius)];
		float sampleValue = din[int(base) + delta];
		filtered += sampleValue * coefficient * oscillator;
		coefficientSum += coefficient;
		oscillator = vec2(
			oscillator.x*oscillatorStep.x - oscillator.y*oscillatorStep.y,
			oscillator.x*oscillatorStep.y + oscillator.y*oscillatorStep.x);
	}
	if(abs(coefficientSum) > 1e-12)
		filtered /= coefficientSum;

	float phase = -(startPhaseStep * i) - (theta * i * i / 2);
	float windowCoefficient = GetWindowCoefficient(i);
	float c = cos(phase);
	float s = sin(phase);

	dout[2*i] = windowCoefficient * (filtered.x*c - filtered.y*s);
	dout[2*i + 1] = windowCoefficient * (filtered.x*s + filtered.y*c);
}

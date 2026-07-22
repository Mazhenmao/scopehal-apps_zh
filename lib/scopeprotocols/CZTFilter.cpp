/***********************************************************************************************************************
*                                                                                                                      *
* libscopeprotocols                                                                                                    *
*                                                                                                                      *
* Copyright (c) 2012-2026 Andrew D. Zonenberg and contributors                                                         *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
***********************************************************************************************************************/

#include "../scopehal/scopehal.h"
#include "CZTFilter.h"

#include <complex>
#include <cfloat>

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

CZTFilter::CZTFilter(const string& color)
	: PeakDetectionFilter(color, CAT_RF)
	, m_centerFrequency(m_parameters["中心频率"])
	, m_bandwidth(m_parameters["带宽"])
	, m_outputPoints(m_parameters["输出点数"])
	, m_maxInputSamples(m_parameters["最大输入点数"])
	, m_window(m_parameters["窗函数"])
	, m_prepareInputPipeline("shaders/CZTPrepareInput.spv", 2, sizeof(CZTPrepareInputArgs))
	, m_complexMultiplyPipeline("shaders/CZTComplexMultiply.spv", 3, sizeof(CZTComplexMultiplyArgs))
	, m_finalizePipeline("shaders/CZTFinalize.spv", 2, sizeof(CZTFinalizeArgs))
{
	m_xAxisUnit = Unit(Unit::UNIT_MICROHZ);
	AddStream(Unit(Unit::UNIT_DBM), "data", Stream::STREAM_TYPE_ANALOG);

	CreateInput<InputConstraintStreamType>("din", Stream::STREAM_TYPE_ANALOG);

	m_centerFrequency = FilterParameter(FilterParameter::TYPE_FLOAT, Unit(Unit::UNIT_HZ));
	m_centerFrequency.SetFloatVal(50e6);

	m_bandwidth = FilterParameter(FilterParameter::TYPE_FLOAT, Unit(Unit::UNIT_HZ));
	m_bandwidth.SetFloatVal(100e6);

	// FFT-accelerated CZT can use a larger default output than the old direct CPU path.
	m_outputPoints = FilterParameter(FilterParameter::TYPE_INT, Unit(Unit::UNIT_COUNTS));
	m_outputPoints.SetIntVal(1024);

	// Keep a configurable input cap so very large captures do not allocate huge FFT buffers by default.
	m_maxInputSamples = FilterParameter(FilterParameter::TYPE_INT, Unit(Unit::UNIT_SAMPLEDEPTH));
	m_maxInputSamples.SetIntVal(65536);

	m_window = FilterParameter(FilterParameter::TYPE_ENUM, Unit(Unit::UNIT_COUNTS));
	m_window.AddEnumValue("Blackman-Harris", WINDOW_BLACKMAN_HARRIS);
	m_window.AddEnumValue("Hamming", WINDOW_HAMMING);
	m_window.AddEnumValue("Hann", WINDOW_HANN);
	m_window.AddEnumValue("Rectangular", WINDOW_RECTANGULAR);
	m_window.SetIntVal(WINDOW_HAMMING);
}

CZTFilter::~CZTFilter()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Accessors

string CZTFilter::GetProtocolName()
{
	return "CZT";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Actual decoder logic

float CZTFilter::GetWindowCoefficient(WindowFunction window, size_t i, size_t npoints) const
{
	if(npoints < 2)
		return 1;

	float x = 2 * M_PI * i / (npoints - 1);
	switch(window)
	{
		case WINDOW_BLACKMAN_HARRIS:
			return 0.35875f - 0.48829f*cos(x) + 0.14128f*cos(2*x) - 0.01168f*cos(3*x);

		case WINDOW_HANN:
			return 0.5f - 0.5f*cos(x);

		case WINDOW_HAMMING:
			return 0.54f - 0.46f*cos(x);

		default:
		case WINDOW_RECTANGULAR:
			return 1;
	}
}

float CZTFilter::GetWindowGainCorrection(WindowFunction window) const
{
	switch(window)
	{
		case WINDOW_HAMMING:
			return 1.862f;

		case WINDOW_HANN:
			return 2.013f;

		case WINDOW_BLACKMAN_HARRIS:
			return 2.805f;

		default:
		case WINDOW_RECTANGULAR:
			return 1;
	}
}

void CZTFilter::Refresh(
	[[maybe_unused]] vk::raii::CommandBuffer& cmdBuf,
	[[maybe_unused]] shared_ptr<QueueHandle> queue)
{
	#ifdef HAVE_NVTX
		nvtx3::scoped_range nrange("CZTFilter::Refresh");
	#endif

	ClearMessages();
	auto din = dynamic_cast<UniformAnalogWaveform*>(GetInputWaveform(0));
	if(!din)
	{
		if(!GetInput(0))
			AddErrorMessage("Missing inputs", "No signal input connected");
		else if(!GetInputWaveform(0))
			AddErrorMessage("Missing inputs", "No waveform available at input");
		else
			AddErrorMessage("Invalid inputs", "Expect a uniform analog input");

		SetData(nullptr, 0);
		return;
	}

	size_t npoints = din->size();
	if(npoints < 2)
	{
		AddErrorMessage("Invalid input", "Input waveform is too short");
		SetData(nullptr, 0);
		return;
	}

	int64_t fs_per_sample = din->m_timescale;
	double sample_hz = FS_PER_SECOND / fs_per_sample;
	double nyquist = sample_hz / 2;

	double center = m_centerFrequency.GetFloatVal();
	double bandwidth = m_bandwidth.GetFloatVal();
	if(bandwidth < 0)
		bandwidth = -bandwidth;

	double fstart = center - (bandwidth / 2);
	double fstop = center + (bandwidth / 2);
	fstart = max(fstart, 0.0);
	fstop = min(fstop, nyquist);
	center = (fstart + fstop) / 2;
	bandwidth = fstop - fstart;

	int64_t noutsRaw = m_outputPoints.GetIntVal();
	if(noutsRaw < 2)
	{
		AddErrorMessage("Invalid configuration", "Output point count must be at least 2");
		SetData(nullptr, 0);
		return;
	}

	size_t nouts = static_cast<size_t>(noutsRaw);
	constexpr size_t maxOutputPoints = 1048576;
	nouts = min(nouts, maxOutputPoints);

	int64_t maxInputSamplesRaw = m_maxInputSamples.GetIntVal();
	if(maxInputSamplesRaw < 2)
	{
		AddErrorMessage("Invalid configuration", "Max input sample count must be at least 2");
		SetData(nullptr, 0);
		return;
	}

	size_t maxInputSamples = static_cast<size_t>(maxInputSamplesRaw);
	size_t inputStep = max<size_t>(1, (npoints + maxInputSamples - 1) / maxInputSamples);
	size_t samplesUsed = (npoints + inputStep - 1) / inputStep;
	constexpr size_t maxFftLen = 4 * 1024 * 1024;
	bool maxInputSamplesClamped = false;
	if(samplesUsed + 1 > maxFftLen)
	{
		inputStep = (npoints + maxFftLen - 2) / (maxFftLen - 1);
		samplesUsed = (npoints + inputStep - 1) / inputStep;
		maxInputSamplesClamped = true;
	}

	size_t maxNoutsForFft = maxFftLen - samplesUsed + 1;
	nouts = min(nouts, maxNoutsForFft);
	if(nouts < 2)
	{
		AddErrorMessage("Invalid configuration", "Not enough FFT workspace for CZT output");
		SetData(nullptr, 0);
		return;
	}

	if(fstart == fstop)
	{
		AddErrorMessage("Invalid configuration", "Start and stop frequencies are equal");
		SetData(nullptr, 0);
		return;
	}

	auto window = static_cast<WindowFunction>(m_window.GetIntVal());
	size_t fftlen = next_pow2(samplesUsed + nouts - 1);
	while(fftlen > maxFftLen && nouts > 2)
	{
		nouts = max<size_t>(2, nouts / 2);
		fftlen = next_pow2(samplesUsed + nouts - 1);
	}

	// Keep the parameter editor synchronized with the effective values after automatic clamping.
	if(m_centerFrequency.GetFloatVal() != center)
		m_centerFrequency.SetFloatVal(center);
	if(m_bandwidth.GetFloatVal() != bandwidth)
		m_bandwidth.SetFloatVal(bandwidth);
	if(m_outputPoints.GetIntVal() != static_cast<int64_t>(nouts))
		m_outputPoints.SetIntVal(nouts);
	if(maxInputSamplesClamped && (m_maxInputSamples.GetIntVal() != static_cast<int64_t>(samplesUsed)))
		m_maxInputSamples.SetIntVal(samplesUsed);

	double bin_hz = (fstop - fstart) / (nouts - 1);
	int64_t bin_uhz = round(bin_hz * 1e6);
	if(bin_uhz == 0)
	{
		AddErrorMessage("Invalid configuration", "Frequency step is too small");
		SetData(nullptr, 0);
		return;
	}

	m_xAxisUnit = Unit(Unit::UNIT_MICROHZ);
	SetYAxisUnits(Unit(Unit::UNIT_DBM), 0);

	auto cap = SetupEmptyUniformAnalogOutputWaveform(din, 0);
	cap->m_triggerPhase = round(fstart * 1e6);
	cap->m_timescale = bin_uhz;
	cap->Resize(nouts);

	if(!m_forwardPlan || (m_forwardPlan->size() != fftlen))
	{
		m_forwardPlan = make_unique<VulkanFFTPlan>(
			fftlen,
			fftlen,
			VulkanFFTPlan::DIRECTION_FORWARD,
			1,
			VulkanFFTPlan::TYPE_COMPLEX);
		m_reversePlan = make_unique<VulkanFFTPlan>(
			fftlen,
			fftlen,
			VulkanFFTPlan::DIRECTION_REVERSE,
			1,
			VulkanFFTPlan::TYPE_COMPLEX);
		m_chirpKernelFftValid = false;
	}

	m_chirpInput.SetCpuAccessHint(AcceleratorBuffer<float>::HINT_NEVER);
	m_chirpInput.SetGpuAccessHint(AcceleratorBuffer<float>::HINT_LIKELY);
	m_chirpKernel.SetCpuAccessHint(AcceleratorBuffer<float>::HINT_LIKELY);
	m_chirpKernel.SetGpuAccessHint(AcceleratorBuffer<float>::HINT_LIKELY);
	m_chirpInputFft.SetCpuAccessHint(AcceleratorBuffer<float>::HINT_NEVER);
	m_chirpInputFft.SetGpuAccessHint(AcceleratorBuffer<float>::HINT_LIKELY);
	m_chirpKernelFft.SetCpuAccessHint(AcceleratorBuffer<float>::HINT_NEVER);
	m_chirpKernelFft.SetGpuAccessHint(AcceleratorBuffer<float>::HINT_LIKELY);
	m_chirpProduct.SetCpuAccessHint(AcceleratorBuffer<float>::HINT_NEVER);
	m_chirpProduct.SetGpuAccessHint(AcceleratorBuffer<float>::HINT_LIKELY);
	m_chirpIfft.SetCpuAccessHint(AcceleratorBuffer<float>::HINT_NEVER);
	m_chirpIfft.SetGpuAccessHint(AcceleratorBuffer<float>::HINT_LIKELY);

	m_chirpInput.resize(2 * fftlen);
	m_chirpKernel.resize(2 * fftlen);
	m_chirpInputFft.resize(2 * fftlen);
	m_chirpKernelFft.resize(2 * fftlen);
	m_chirpProduct.resize(2 * fftlen);
	m_chirpIfft.resize(2 * fftlen);

	constexpr double two_pi = 2 * M_PI;
	double effectiveSampleHz = sample_hz / inputStep;
	double startPhaseStep = two_pi * fstart / effectiveSampleHz;
	double theta = two_pi * bin_hz / effectiveSampleHz;

	size_t kernelSpan = max(samplesUsed, nouts);
	bool kernelFftDirty =
		!m_chirpKernelFftValid ||
		(m_cachedKernelFftLen != fftlen) ||
		(m_cachedKernelSpan != kernelSpan) ||
		(fabs(m_cachedKernelTheta - theta) > 1e-15);
	if(kernelFftDirty)
	{
		m_chirpKernel.PrepareForCpuAccess();
		fill(m_chirpKernel.begin(), m_chirpKernel.end(), 0);

		// The chirp convolution kernel only changes when the CZT geometry changes.
		for(size_t i=0; i<kernelSpan; i++)
		{
			double phase = theta * i * i / 2;
			float re = cos(phase);
			float im = sin(phase);

			m_chirpKernel[2*i] = re;
			m_chirpKernel[2*i + 1] = im;

			if(i != 0)
			{
				size_t neg = fftlen - i;
				m_chirpKernel[2*neg] = re;
				m_chirpKernel[2*neg + 1] = im;
			}
		}
		m_chirpKernel.MarkModifiedFromCpu();
	}
	float scale = sqrt(2.0f) / samplesUsed;
	scale *= GetWindowGainCorrection(window);
	scale /= fftlen;
	const float impedance = 50;

	cmdBuf.begin({});
	CZTPrepareInputArgs prepareArgs;
	prepareArgs.fftlen = fftlen;
	prepareArgs.samplesUsed = samplesUsed;
	prepareArgs.inputStep = inputStep;
	prepareArgs.inputLength = npoints;
	prepareArgs.window = window;
	prepareArgs.startPhaseStep = startPhaseStep;
	prepareArgs.theta = theta;
	m_prepareInputPipeline.BindBufferNonblocking(0, din->m_samples, cmdBuf);
	m_prepareInputPipeline.BindBufferNonblocking(1, m_chirpInput, cmdBuf, true);
	m_prepareInputPipeline.Dispatch(cmdBuf, prepareArgs, GetComputeBlockCount(fftlen, 64));
	m_prepareInputPipeline.AddComputeMemoryBarrier(cmdBuf);
	m_chirpInput.MarkModifiedFromGpu();

	m_forwardPlan->AppendForward(m_chirpInput, m_chirpInputFft, cmdBuf);
	if(kernelFftDirty)
		m_forwardPlan->AppendForward(m_chirpKernel, m_chirpKernelFft, cmdBuf);

	CZTComplexMultiplyArgs mulArgs;
	mulArgs.npoints = fftlen;
	m_complexMultiplyPipeline.BindBufferNonblocking(0, m_chirpInputFft, cmdBuf);
	m_complexMultiplyPipeline.BindBufferNonblocking(1, m_chirpKernelFft, cmdBuf);
	m_complexMultiplyPipeline.BindBufferNonblocking(2, m_chirpProduct, cmdBuf, true);
	m_complexMultiplyPipeline.Dispatch(cmdBuf, mulArgs, GetComputeBlockCount(fftlen, 64));
	m_complexMultiplyPipeline.AddComputeMemoryBarrier(cmdBuf);
	m_chirpProduct.MarkModifiedFromGpu();

	m_reversePlan->AppendReverse(m_chirpProduct, m_chirpIfft, cmdBuf);

	CZTFinalizeArgs finalArgs;
	finalArgs.npoints = nouts;
	finalArgs.theta = theta;
	finalArgs.powerScale = scale * scale / impedance;
	m_finalizePipeline.BindBufferNonblocking(0, m_chirpIfft, cmdBuf);
	m_finalizePipeline.BindBufferNonblocking(1, cap->m_samples, cmdBuf, true);
	m_finalizePipeline.Dispatch(cmdBuf, finalArgs, GetComputeBlockCount(nouts, 64));
	m_finalizePipeline.AddComputeMemoryBarrier(cmdBuf);

	cmdBuf.end();
	queue->SubmitAndBlock(cmdBuf);
	cap->MarkModifiedFromGpu();
	if(kernelFftDirty)
	{
		m_chirpKernelFftValid = true;
		m_cachedKernelFftLen = fftlen;
		m_cachedKernelSpan = kernelSpan;
		m_cachedKernelTheta = theta;
	}

	// Peak count defaults to zero; users can enable peak search from filter parameters when needed.
	FindPeaks(cap, cmdBuf, queue);
}

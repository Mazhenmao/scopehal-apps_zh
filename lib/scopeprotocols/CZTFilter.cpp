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
	, m_prepareInputPipeline("shaders/CZTPrepareInput.spv", 3, sizeof(CZTPrepareInputArgs))
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

	m_antialiasKernel.SetCpuAccessHint(AcceleratorBuffer<float>::HINT_LIKELY);
	m_antialiasKernel.SetGpuAccessHint(AcceleratorBuffer<float>::HINT_LIKELY);
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

/**
	@brief Finds peaks in a CZT waveform without allowing endpoint interpolation

	The shared peak detector can evaluate the final bin and then read one sample beyond the waveform during quadratic
	interpolation. CZT uses a local implementation so its endpoint handling can be fixed without changing shared code.
 */
void CZTFilter::FindCztPeaks(UniformAnalogWaveform* cap)
{
	int64_t maxPeaks = m_numpeaks.GetIntVal();
	size_t nouts = cap ? cap->size() : 0;
	if( (maxPeaks <= 0) || (nouts < 3) )
	{
		m_peaks.clear();
		return;
	}

	int64_t binSize = std::abs(cap->m_timescale);
	if(binSize == 0)
	{
		m_peaks.clear();
		return;
	}

	cap->PrepareForCpuAccess();

	// CZT waveform offsets are stored in microhertz, while Peak Window is configured in hertz.
	double searchWindowUhz = std::abs(static_cast<double>(m_peakwindow.GetFloatVal())) * 1e6;
	double searchBinsRaw = ceil(searchWindowUhz / binSize);
	size_t searchRadius = 1;
	if(std::isfinite(searchBinsRaw))
	{
		double radius = max(1.0, searchBinsRaw / 2);
		searchRadius = static_cast<size_t>(min(radius, static_cast<double>(nouts - 2)));
	}

	vector<Peak> peaks;
	for(size_t i=1; (i + 1)<nouts; i++)
	{
		float target = cap->m_samples[i];
		if(!std::isfinite(target))
			continue;

		size_t left = (i > searchRadius) ? (i - searchRadius) : 1;
		size_t right = i + min(searchRadius, nouts - 2 - i);
		bool isPeak = true;
		for(size_t j=left; j<=right; j++)
		{
			if(i == j)
				continue;

			float neighbor = cap->m_samples[j];
			if(std::isfinite(neighbor) && (neighbor >= target))
			{
				if(j > i)
					i = j - 1;
				isPeak = false;
				break;
			}
		}
		if(!isPeak)
			continue;

		float alpha = cap->m_samples[i-1];
		float beta = cap->m_samples[i];
		float gamma = cap->m_samples[i+1];
		if(!std::isfinite(alpha) || !std::isfinite(beta) || !std::isfinite(gamma))
			continue;

		float denominator = alpha - 2*beta + gamma;
		float offset = 0;
		if(std::isfinite(denominator) && (std::abs(denominator) > 1e-20f))
			offset = std::clamp(0.5f * (alpha - gamma) / denominator, -1.0f, 1.0f);

		int64_t peakLocation =
			(i * cap->m_timescale) +
			static_cast<int64_t>(round(offset * cap->m_timescale)) +
			cap->m_triggerPhase;
		float interpolatedMagnitude = beta - 0.25f * (alpha - gamma) * offset;

		float halfMagnitudeTarget = interpolatedMagnitude - 3;
		size_t halfMagnitudeLeft = i;
		size_t halfMagnitudeRight = i;
		for(size_t j=i; ; j--)
		{
			if(cap->m_samples[j] <= halfMagnitudeTarget)
			{
				halfMagnitudeLeft = j;
				break;
			}
			if(j == 0)
				break;
		}
		for(size_t j=i; j<nouts; j++)
		{
			if(cap->m_samples[j] <= halfMagnitudeTarget)
			{
				halfMagnitudeRight = j;
				break;
			}
		}

		float fwhm = (halfMagnitudeRight - halfMagnitudeLeft) * binSize;
		peaks.emplace_back(peakLocation, interpolatedMagnitude, fwhm);
		i += searchRadius - 1;
	}

	sort(peaks.rbegin(), peaks.rend(), less<Peak>());
	m_peaks.clear();
	size_t outputCount = min(static_cast<size_t>(maxPeaks), peaks.size());
	m_peaks.insert(m_peaks.end(), peaks.begin(), peaks.begin() + outputCount);
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
	size_t requestedInputStep = inputStep;

	// Preserve the complete requested band after complex downconversion. Keeping the post-decimation sample rate at
	// least twice the bandwidth also leaves a transition band for the antialiasing low-pass filter.
	size_t maxStepForBandwidth = 1;
	if(bandwidth > 0)
	{
		double maxStepRaw = floor(sample_hz / (2 * bandwidth));
		if(maxStepRaw >= 1)
			maxStepForBandwidth = static_cast<size_t>(min(maxStepRaw, static_cast<double>(npoints)));
	}
	inputStep = min(inputStep, maxStepForBandwidth);
	size_t samplesUsed = (npoints + inputStep - 1) / inputStep;
	constexpr size_t maxFftLen = 4 * 1024 * 1024;
	bool maxInputSamplesClamped = (inputStep != requestedInputStep);
	if(samplesUsed + 1 > maxFftLen)
	{
		size_t workspaceStep = (npoints + maxFftLen - 2) / (maxFftLen - 1);
		if(workspaceStep > maxStepForBandwidth)
		{
			AddErrorMessage(
				"Invalid configuration",
				"Input record is too large to decimate without aliasing at the requested CZT bandwidth");
			SetData(nullptr, 0);
			return;
		}

		inputStep = workspaceStep;
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

	// Generate a unity-gain Hamming-windowed sinc low-pass filter for this decimation ratio. The cutoff is the new
	// Nyquist frequency and the target band is kept inside half of that frequency by maxStepForBandwidth above.
	if(m_cachedAntialiasInputStep != inputStep)
	{
		size_t kernelRadius = (inputStep > 1) ? (4 * inputStep) : 0;
		size_t kernelSize = 2 * kernelRadius + 1;
		m_antialiasKernel.PrepareForCpuAccess();
		m_antialiasKernel.resize(kernelSize);

		double coefficientSum = 0;
		double cutoff = 0.5 / inputStep;
		for(size_t i=0; i<kernelSize; i++)
		{
			int64_t delta = static_cast<int64_t>(i) - static_cast<int64_t>(kernelRadius);
			double coefficient = 1;
			if(kernelRadius > 0)
			{
				double ideal = (delta == 0) ?
					(2 * cutoff) :
					(sin(2 * M_PI * cutoff * delta) / (M_PI * delta));
				double windowCoefficient = 0.54 + 0.46*cos(M_PI * delta / kernelRadius);
				coefficient = ideal * windowCoefficient;
			}

			m_antialiasKernel[i] = coefficient;
			coefficientSum += coefficient;
		}

		for(auto& coefficient : m_antialiasKernel)
			coefficient /= coefficientSum;
		m_antialiasKernel.MarkModifiedFromCpu();
		m_cachedAntialiasInputStep = inputStep;
	}

	constexpr double two_pi = 2 * M_PI;
	double effectiveSampleHz = sample_hz / inputStep;
	double mixerPhaseStep = two_pi * center / sample_hz;
	double startPhaseStep = two_pi * (fstart - center) / effectiveSampleHz;
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
	prepareArgs.kernelRadius = static_cast<uint32_t>((m_antialiasKernel.size() - 1) / 2);
	prepareArgs.mixerPhaseStep = mixerPhaseStep;
	prepareArgs.startPhaseStep = startPhaseStep;
	prepareArgs.theta = theta;
	m_prepareInputPipeline.BindBufferNonblocking(0, din->m_samples, cmdBuf);
	m_prepareInputPipeline.BindBufferNonblocking(1, m_antialiasKernel, cmdBuf);
	m_prepareInputPipeline.BindBufferNonblocking(2, m_chirpInput, cmdBuf, true);
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

	// Peak count defaults to zero; users can enable the CZT-specific endpoint-safe search when needed.
	FindCztPeaks(cap);
}

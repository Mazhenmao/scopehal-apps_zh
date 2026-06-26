/***********************************************************************************************************************
*                                                                                                                      *
* libscopeprotocols                                                                                                    *
*                                                                                                                      *
* Copyright (c) 2012-2026 Andrew D. Zonenberg and contributors                                                         *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
***********************************************************************************************************************/

/**
	@file
	@brief Declaration of CZTFilter
 */
#ifndef CZTFilter_h
#define CZTFilter_h

#include "VulkanFFTPlan.h"

struct CZTComplexMultiplyArgs
{
	uint32_t npoints;
};

struct CZTPrepareInputArgs
{
	uint32_t fftlen;
	uint32_t samplesUsed;
	uint32_t inputStep;
	uint32_t inputLength;
	uint32_t window;
	float startPhaseStep;
	float theta;
};

struct CZTFinalizeArgs
{
	uint32_t npoints;
	float theta;
	float powerScale;
};

/**
	@brief Chirp-Z Transform based narrow-band frequency analysis
 */
class CZTFilter : public PeakDetectionFilter
{
public:
	CZTFilter(const std::string& color);
	virtual ~CZTFilter();

	virtual void Refresh(vk::raii::CommandBuffer& cmdBuf, std::shared_ptr<QueueHandle> queue) override;

	static std::string GetProtocolName();

	enum WindowFunction
	{
		WINDOW_RECTANGULAR,
		WINDOW_HANN,
		WINDOW_HAMMING,
		WINDOW_BLACKMAN_HARRIS
	};

	PROTOCOL_DECODER_INITPROC(CZTFilter)

protected:
	float GetWindowCoefficient(WindowFunction window, size_t i, size_t npoints) const;
	float GetWindowGainCorrection(WindowFunction window) const;

	FilterParameter& m_centerFrequency;
	FilterParameter& m_bandwidth;
	FilterParameter& m_outputPoints;
	FilterParameter& m_maxInputSamples;
	FilterParameter& m_window;

	AcceleratorBuffer<float> m_chirpInput;
	AcceleratorBuffer<float> m_chirpKernel;
	AcceleratorBuffer<float> m_chirpInputFft;
	AcceleratorBuffer<float> m_chirpKernelFft;
	AcceleratorBuffer<float> m_chirpProduct;
	AcceleratorBuffer<float> m_chirpIfft;

	ComputePipeline m_prepareInputPipeline;
	ComputePipeline m_complexMultiplyPipeline;
	ComputePipeline m_finalizePipeline;

	std::unique_ptr<VulkanFFTPlan> m_forwardPlan;
	std::unique_ptr<VulkanFFTPlan> m_reversePlan;

	///@brief Cache state for the chirp convolution kernel FFT, which is independent of waveform sample values
	bool m_chirpKernelFftValid = false;
	size_t m_cachedKernelFftLen = 0;
	size_t m_cachedKernelSpan = 0;
	double m_cachedKernelTheta = 0;
};

#endif

/***********************************************************************************************************************
*                                                                                                                      *
* libscopeprotocols                                                                                                    *
*                                                                                                                      *
* Copyright (c) 2012-2026 Andrew D. Zonenberg and contributors                                                         *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the     *
* following conditions are met:                                                                                        *
*                                                                                                                      *
*    * Redistributions of source code must retain the above copyright notice, this list of conditions, and the         *
*      following disclaimer.                                                                                           *
*                                                                                                                      *
*    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       *
*      following disclaimer in the documentation and/or other materials provided with the distribution.                *
*                                                                                                                      *
*    * Neither the name of the author nor the names of any contributors may be used to endorse or promote products     *
*      derived from this software without specific prior written permission.                                           *
*                                                                                                                      *
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   *
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL *
* THE AUTHORS BE HELD LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES        *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR       *
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
* POSSIBILITY OF SUCH DAMAGE.                                                                                          *
*                                                                                                                      *
***********************************************************************************************************************/

/**
	@file
	@author Andrew D. Zonenberg
	@brief Declaration of FIRFilter
 */
#ifndef FIRFilter_h
#define FIRFilter_h

/**
	@brief Performs an arbitrary FIR filter with tap delay equal to the sample rate
 */
class FIRFilter : public Filter
{
public:
	FIRFilter(const std::string& color);

	virtual void Refresh(vk::raii::CommandBuffer& cmdBuf, std::shared_ptr<QueueHandle> queue) override;

	static std::string GetProtocolName();
	virtual void SetDefaultName() override;

	PROTOCOL_DECODER_INITPROC(FIRFilter)

	void DoFilterKernel(
		vk::raii::CommandBuffer& cmdBuf,
		std::shared_ptr<QueueHandle> queue,
		UniformAnalogWaveform* din,
		UniformAnalogWaveform* cap);

	FIRFilterType GetFilterType()
	{ return m_filterType.GetEnumVal<FIRFilterType>(); }

	void SetFilterType(FIRFilterType type)
	{ m_filterType.SetIntVal(type); }

	void SetFreqLow(float freq)
	{ m_freqLow.SetFloatVal(freq); }

	void SetFreqHigh(float freq)
	{ m_freqHigh.SetFloatVal(freq); }

	AcceleratorBuffer<float>& GetCoefficients()
	{ return m_coefficients; }

protected:

	void CalculateFilterCoefficients(float fa, float fb, float stopbandAtten, FIRFilterType type)
	{ CalculateFIRCoefficients(fa, fb, stopbandAtten, type, m_coefficients); }

	/**
		@brief 判断当前滤波器配置是否真的发生变化

		只有在滤波器类型、归一化截止频率、阻带衰减或tap数变化时，
		才需要重新生成FIR系数，避免在连续刷新时反复占满单个CPU核心。
	 */
	bool CoefficientsNeedUpdate(float flo, float fhi, float atten, FIRFilterType type, size_t filterlen) const;

	FilterParameter& m_filterType;
	FilterParameter& m_filterLength;
	FilterParameter& m_stopbandAtten;
	FilterParameter& m_freqLow;
	FilterParameter& m_freqHigh;

	ComputePipeline m_computePipeline;

	AcceleratorBuffer<float> m_coefficients;

	///@brief 缓存上一次生成FIR系数时使用的关键参数，减少重复设计滤波器的CPU开销
	bool m_coefficientsValid = false;
	float m_cachedFlo = 0;
	float m_cachedFhi = 0;
	float m_cachedStopbandAtten = 0;
	size_t m_cachedFilterLen = 0;
	FIRFilterType m_cachedFilterType = FILTER_TYPE_LOWPASS;
};

#endif

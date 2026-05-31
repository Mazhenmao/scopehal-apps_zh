/***********************************************************************************************************************
*                                                                                                                      *
* ngscopeclient                                                                                                        *
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
	@brief Implementation of PowerSupplyDialog
 */

#include "ngscopeclient.h"
#include "PowerSupplyDialog.h"

using namespace std;
using namespace std::chrono_literals;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

PowerSupplyDialog::PowerSupplyDialog(
	shared_ptr<SCPIPowerSupply> psu,
	shared_ptr<PowerSupplyState> state,
	Session* session)
	: Dialog(
		string("Power Supply: ") + psu->m_nickname,
		string("Power Supply: ") + psu->m_nickname,
		ImVec2(500, 400), session)
	, m_masterEnable(psu->GetMasterPowerEnable())
	, m_tstart(GetTime())
	, m_psu(psu)
	, m_state(state)
{
	AsyncLoadState();
}

PowerSupplyDialog::~PowerSupplyDialog()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rendering

void PowerSupplyDialog::RefreshFromHardware()
{
	//pull settings again
	AsyncLoadState();
}

void PowerSupplyDialog::AsyncLoadState()
{
	//Clear existing state (if any) and allocate space for new state
	m_channelUIState.clear();
	m_channelUIState.resize(m_psu->GetChannelCount());

	//Do the async load
	m_futureUIState.clear();
	shared_ptr<SCPIPowerSupply> psu = m_psu;
	for(size_t i=0; i<m_psu->GetChannelCount(); i++)
	{
		//Add placeholders for non-power channels
		//TODO: can we avoid spawning a thread here pointlessly?
		if( (m_psu->GetInstrumentTypesForChannel(i) & Instrument::INST_PSU) == 0)
			m_futureUIState.push_back(async(launch::async, [/*psu, i*/]{ PowerSupplyChannelUIState dummy; return dummy; }));

		//Actual power channels get async load
		else
			m_futureUIState.push_back(async(launch::async, [psu, i]{ return PowerSupplyChannelUIState(psu, i); }));
	}
}

bool PowerSupplyDialog::DoRender()
{
	//Device information
	if(ImGui::CollapsingHeader("信息"))
	{
		ImGui::BeginDisabled();

			auto name = m_psu->GetName();
			auto vendor = m_psu->GetVendor();
			auto serial = m_psu->GetSerial();
			auto driver = m_psu->GetDriverName();
			auto transport = m_psu->GetTransport();
			auto tname = transport->GetName();
			auto tstring = transport->GetConnectionString();

			ImGui::InputText("制造商", &vendor[0], vendor.size());
			ImGui::InputText("型号", &name[0], name.size());
			ImGui::InputText("序列号", &serial[0], serial.size());
			ImGui::InputText("驱动", &driver[0], driver.size());
			ImGui::InputText("传输", &tname[0], tname.size());
			ImGui::InputText("路径", &tstring[0], tstring.size());

		ImGui::EndDisabled();
	}

	//Top level settings
	if(m_psu->SupportsMasterOutputSwitching())
	{
		if(ImGui::CollapsingHeader("全局", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if(ImGui::Checkbox("输出使能", &m_masterEnable))
				m_psu->SetMasterPowerEnable(m_masterEnable);

			HelpMarker("顶层输出启用，用于控制 PSU 的所有输出。\n\n它相当于与各通道输出启用串联的第二级开关。");
		}
	}

	//Grab asynchronously loaded channel state if it's ready
	if(m_futureUIState.size())
	{
		bool allDone = true;
		for(size_t i=0; i<m_futureUIState.size(); i++)
		{
			//Already loaded? No action needed
			if(m_channelUIState[i].m_setVoltage != "")
				continue;

			//Not ready? Keep waiting
			if(m_futureUIState[i].wait_for(0s) != future_status::ready)
			{
				allDone = false;
				continue;
			}

			//Ready, process it
			m_channelUIState[i] = m_futureUIState[i].get();
		}

		if(allDone)
			m_futureUIState.clear();
	}

	auto t = GetTime() - m_tstart;

	//Per channel settings
	for(size_t i=0; i<m_psu->GetChannelCount(); i++)
	{
		//Skip non-power channels
		if( (m_psu->GetInstrumentTypesForChannel(i) & Instrument::INST_PSU) == 0)
			continue;

		ChannelSettings(i, m_state->m_channelVoltage[i].load(), m_state->m_channelCurrent[i].load(), t);
	}

	return true;
}

/**
	@brief A single channel's settings

	@param i		Channel index
	@param v		Most recently observed voltage
	@param a		Most recently observed current
	@param etime	Elapsed time for animation
 */
void PowerSupplyDialog::ChannelSettings(int i, float v, float a, float etime)
{
	float valueWidth = 100;

	auto chname = m_psu->GetChannel(i)->GetDisplayName();

	Unit volts(Unit::UNIT_VOLTS);
	Unit amps(Unit::UNIT_AMPS);
	Unit fs(Unit::UNIT_FS);

	if(ImGui::CollapsingHeader(chname.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::PushID(chname.c_str());

		bool shdn = m_state->m_channelFuseTripped[i].load();
		bool cc = m_state->m_channelConstantCurrent[i].load();

		if(m_psu->SupportsIndividualOutputSwitching())
		{
			if(ImGui::Checkbox("输出使能", &m_channelUIState[i].m_outputEnabled))
			{
				m_psu->SetPowerChannelActive(i, m_channelUIState[i].m_outputEnabled);
				// Tell intrument thread that the PSU state has to be updated
				m_state->m_needsUpdate[i] = true;
			}
			if(shdn)
			{
				//TODO: preference for configuring this?
				float alpha = fabs(sin(etime*M_PI))*0.5 + 0.5;

				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1*alpha, 0, 0, 1*alpha));
				ImGui::TextUnformatted("过载关断");
				ImGui::PopStyleColor();
				Tooltip("已触发过流关断。\n\n清除负载上的故障后，关闭并重新打开输出以复位。");
			}
			HelpMarker("打开或关闭此通道的供电");
		}

		//Advanced features (not available with all PSUs)
		bool ocp = m_psu->SupportsOvercurrentShutdown();
		bool ss = m_psu->SupportsSoftStart();
		if(ocp || ss)
		{
			if(ImGui::TreeNode("高级"))
			{
				if(ocp)
				{
					if(ImGui::Checkbox("过流关断", &m_channelUIState[i].m_overcurrentShutdownEnabled))
					{
						m_psu->SetPowerOvercurrentShutdownEnabled(i, m_channelUIState[i].m_overcurrentShutdownEnabled);
						// Tell intrument thread that the PSU state has to be updated
						m_state->m_needsUpdate[i] = true;
					}
					HelpMarker("启用后，通道在过流时会关断，而不是切换到恒流模式。\n\n过流关断触发后，必须先禁用再重新启用该通道，才能恢复对负载供电。");
				}

				if(ss)
				{
					if(ImGui::Checkbox("软启动", &m_channelUIState[i].m_softStartEnabled))
					{
						m_psu->SetSoftStartEnabled(i, m_channelUIState[i].m_softStartEnabled);
						// Tell intrument thread that the PSU state has to be updated
						m_state->m_needsUpdate[i] = true;
					}

					HelpMarker("有意限制输出上升时间，以降低驱动容性负载时的浪涌电流。");

					ImGui::SetNextItemWidth(valueWidth);
					if(UnitInputWithExplicitApply(
						"Ramp time", m_channelUIState[i].m_setSSRamp, m_channelUIState[i].m_committedSSRamp, fs))
					{
						m_psu->SetSoftStartRampTime(i, m_channelUIState[i].m_committedSSRamp);
						// Tell intrument thread that the PSU state has to be updated
						m_state->m_needsUpdate[i] = true;
					}
					HelpMarker("使用软启动时从关闭到开启状态的过渡时间\n\n点击应用前，更改不会发送到硬件。\n\n注意：部分仪器（例如 R&S HMC804x）在更改斜坡时间时会关闭输出。");
				}

				ImGui::TreePop();
			}
		}

		if(m_psu->SupportsVoltageCurrentControl(i))
		{
			//Set points for channels
			ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
			if(ImGui::TreeNode("设定值"))
			{
				ImGui::SetNextItemWidth(valueWidth);
				if(UnitInputWithExplicitApply(
					"Voltage", m_channelUIState[i].m_setVoltage, m_channelUIState[i].m_committedSetVoltage, volts))
				{
					m_psu->SetPowerVoltage(i, m_channelUIState[i].m_committedSetVoltage);
					// Tell intrument thread that the PSU state has to be updated
					m_state->m_needsUpdate[i] = true;
				}
				HelpMarker("提供给负载的目标电压。\n\n点击应用前，更改不会发送到硬件。");

				ImGui::SetNextItemWidth(valueWidth);
				if(UnitInputWithExplicitApply(
					"Current", m_channelUIState[i].m_setCurrent, m_channelUIState[i].m_committedSetCurrent, amps))
				{
					m_psu->SetPowerCurrent(i, m_channelUIState[i].m_committedSetCurrent);
					// Tell intrument thread that the PSU state has to be updated
					m_state->m_needsUpdate[i] = true;
				}
				HelpMarker("提供给负载的最大电流。\n\n点击应用前，更改不会发送到硬件。");

				ImGui::TreePop();
			}

			//Actual values of channels
			ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
			if(ImGui::TreeNode("测量值"))
			{
				ImGui::BeginDisabled();
					ImGui::SetNextItemWidth(valueWidth);
					auto svolts = volts.PrettyPrint(v);
					ImGui::InputText("Voltage###VMeasured", &svolts);
				ImGui::EndDisabled();

				if(!cc && m_channelUIState[i].m_outputEnabled && !shdn)
				{
					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
					ImGui::TextUnformatted("CV");
					ImGui::PopStyleColor();
					Tooltip("通道正在恒压模式下工作");
				}
				HelpMarker("电源正在输出的测量电压");

				ImGui::BeginDisabled();
					ImGui::SetNextItemWidth(valueWidth);
					auto scurr = amps.PrettyPrint(a);
					ImGui::InputText("Current###IMeasured", &scurr);
				ImGui::EndDisabled();

				if(cc && m_channelUIState[i].m_outputEnabled && !shdn)
				{
					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 0, 0, 1));
					ImGui::TextUnformatted("CC");
					Tooltip("通道正在恒流模式下工作");
					ImGui::PopStyleColor();
				}

				HelpMarker("电源正在输出的测量电流");

				ImGui::TreePop();
			}
		}

		ImGui::PopID();
	}
}

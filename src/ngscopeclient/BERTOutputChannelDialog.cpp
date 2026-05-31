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
	@brief Implementation of BERTOutputChannelDialog
 */

#include "ngscopeclient.h"
#include "BERTOutputChannelDialog.h"
#include <imgui_node_editor.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

BERTOutputChannelDialog::BERTOutputChannelDialog(BERTOutputChannel* chan, bool graphEditorMode)
	: EmbeddableDialog(
		chan->GetHwname(),
		string("Channel properties: ") + chan->GetHwname(),
		ImVec2(300, 400),
		nullptr,
		graphEditorMode)
	, m_channel(chan)
{
	m_committedDisplayName = m_channel->GetDisplayName();
	m_displayName = m_committedDisplayName;

	//Color
	auto color = ColorFromString(m_channel->m_displaycolor);
	m_color[0] = ((color >> IM_COL32_R_SHIFT) & 0xff) / 255.0f;
	m_color[1] = ((color >> IM_COL32_G_SHIFT) & 0xff) / 255.0f;
	m_color[2] = ((color >> IM_COL32_B_SHIFT) & 0xff) / 255.0f;

	m_invert = chan->GetInvert();
	m_enable = chan->GetEnable();

	m_precursor = chan->GetPreCursor();
	m_postcursor = chan->GetPostCursor();

	//Transmit pattern
	BERT::Pattern pat = chan->GetPattern();
	m_patternValues = chan->GetAvailablePatterns();
	m_patternIndex = 0;
	for(size_t i=0; i<m_patternValues.size(); i++)
	{
		auto p = m_patternValues[i];
		m_patternNames.push_back(chan->GetBERT()->GetPatternName(p));
		if(p == pat)
			m_patternIndex = i;
	}

	//Drive strength
	float drive = chan->GetDriveStrength();
	m_driveValues = chan->GetAvailableDriveStrengths();
	m_driveIndex = 0;
	Unit volts(Unit::UNIT_VOLTS);
	for(size_t i=0; i<m_driveValues.size(); i++)
	{
		auto p = m_driveValues[i];
		m_driveNames.push_back(volts.PrettyPrint(p));
		if( fabs(p - drive) < 0.01)
			m_driveIndex = i;
	}

	//Data rate
	auto currentRate = chan->GetDataRate();
	m_dataRateIndex = 0;
	m_dataRates = chan->GetBERT()->GetAvailableDataRates();
	Unit bps(Unit::UNIT_BITRATE);
	m_dataRateNames.clear();
	for(size_t i=0; i<m_dataRates.size(); i++)
	{
		auto rate = m_dataRates[i];
		if(rate == currentRate)
			m_dataRateIndex = i;

		m_dataRateNames.push_back(bps.PrettyPrint(rate));
	}
}

BERTOutputChannelDialog::~BERTOutputChannelDialog()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rendering

/**
	@brief Renders the dialog and handles UI events

	@return		True if we should continue showing the dialog
				False if it's been closed
 */
bool BERTOutputChannelDialog::DoRender()
{
	//Flags for a header that should be open by default EXCEPT in the graph editor
	ImGuiTreeNodeFlags defaultOpenFlags = m_graphEditorMode ? 0 : ImGuiTreeNodeFlags_DefaultOpen;

	float width = 10 * ImGui::GetFontSize();

	auto bert = m_channel->GetBERT();
	if(ImGui::CollapsingHeader("信息"))
	{
		//Scope info
		if(bert)
		{
			auto nickname = bert->m_nickname;
			auto hwname = m_channel->GetHwname();
			auto index = to_string(m_channel->GetIndex() + 1);	//use one based index for display

			ImGui::BeginDisabled();
				ImGui::SetNextItemWidth(width);
				ImGui::InputText("仪器", &nickname);
			ImGui::EndDisabled();
			HelpMarker("测量此通道的仪器");

			ImGui::BeginDisabled();
				ImGui::SetNextItemWidth(width);
				ImGui::InputText("硬件通道", &index);
			ImGui::EndDisabled();
			HelpMarker("仪器前面板上的物理通道编号（从 1 开始）");

			ImGui::BeginDisabled();
				ImGui::SetNextItemWidth(width);
				ImGui::InputText("硬件名称", &hwname);
			ImGui::EndDisabled();
			HelpMarker("通道的硬件名称（仪器 API 使用的名称）");
		}
	}

	//All channels have display settings
	if(ImGui::CollapsingHeader("显示", defaultOpenFlags))
	{
		ImGui::SetNextItemWidth(width);
		if(TextInputWithImplicitApply("自定义名称", m_displayName, m_committedDisplayName))
			m_channel->SetDisplayName(m_committedDisplayName);

		HelpMarker("通道显示名称");

		if(ImGui::ColorEdit3(
			"Color",
			m_color,
			ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Uint8))
		{
			char tmp[32];
			snprintf(tmp, sizeof(tmp), "#%02x%02x%02x",
				static_cast<int>(round(m_color[0] * 255)),
				static_cast<int>(round(m_color[1] * 255)),
				static_cast<int>(round(m_color[2] * 255)));
			m_channel->m_displaycolor = tmp;
		}
	}

	if(ImGui::CollapsingHeader("码型发生器", defaultOpenFlags))
	{
		ImGui::SetNextItemWidth(width);
		if(Dialog::Combo("Pattern", m_patternNames, m_patternIndex))
			m_channel->SetPattern(m_patternValues[m_patternIndex]);

		if(!m_channel->GetBERT()->IsCustomPatternPerChannel())
			HelpMarker("从此端口输出的码型。\n注意，所有处于“自定义”模式的端口共用同一个码型发生器");
		else
			HelpMarker("从此端口输出的码型。");
	}

	if(ImGui::CollapsingHeader("PHY 控制", defaultOpenFlags))
	{
		ImGui::SetNextItemWidth(width);
		if(ImGui::Checkbox("启用", &m_enable))
			m_channel->Enable(m_enable);
		HelpMarker("启用输出驱动器");

		ImGui::SetNextItemWidth(width);
		if(ImGui::Checkbox("反相", &m_invert))
			m_channel->SetInvert(m_invert);
		HelpMarker("反转输出极性");

		ImGui::SetNextItemWidth(width);
		if(Dialog::Combo("Swing", m_driveNames, m_driveIndex))
			m_channel->SetDriveStrength(m_driveValues[m_driveIndex]);
		HelpMarker("输出峰峰值摆幅（无加重）");

		if(ImGui::SliderFloat("前游标", &m_precursor, 0.0, 1.0, "%.2f", ImGuiSliderFlags_AlwaysClamp))
			m_channel->SetPreCursor(m_precursor);
		HelpMarker("前游标 FFE 抽头值");

		if(ImGui::SliderFloat("后游标", &m_postcursor, 0.0, 1.0, "%.2f", ImGuiSliderFlags_AlwaysClamp))
			m_channel->SetPostCursor(m_postcursor);
		HelpMarker("后游标 FFE 抽头值");

	}

	if(m_channel->GetBERT()->IsDataRatePerChannel())
	{
		if(ImGui::CollapsingHeader("时基", defaultOpenFlags))
		{
			ImGui::SetNextItemWidth(width);
			if(Dialog::Combo("Data Rate", m_dataRateNames, m_dataRateIndex))
				m_channel->SetDataRate(m_dataRates[m_dataRateIndex]);
			HelpMarker("此发送端口的 PHY 信号速率");
		}
	}

	return true;
}

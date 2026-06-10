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
	@brief Implementation of BaseChannelPropertiesDialog
 */

#include "ngscopeclient.h"
#include "BaseChannelPropertiesDialog.h"
#include <imgui_node_editor.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

BaseChannelPropertiesDialog::BaseChannelPropertiesDialog(
	InstrumentChannel* chan,
	MainWindow* parent,
	bool graphEditorMode)
	: EmbeddableDialog(
		chan->GetHwname(),
		string("Channel properties: ") + chan->GetHwname(),
		ImVec2(300, 400),
		parent,
		graphEditorMode)
{
	CreateInput("chan");
	SetInput(0, StreamDescriptor(chan, 0));
}

BaseChannelPropertiesDialog::~BaseChannelPropertiesDialog()
{
}

bool BaseChannelPropertiesDialog::DoRender()
{
	//If our channel has been disconnected (e.g. by the node being deleted) stop
	auto chan = GetChannel();
	if(!chan)
		return false;

	auto f = dynamic_cast<Filter*>(chan);

	//TODO
	auto ochan = dynamic_cast<OscilloscopeChannel*>(chan);
	Oscilloscope* scope = nullptr;
	if(ochan)
		scope = ochan->GetScope();

	float width = 10 * ImGui::GetFontSize();

	ImGui::PushID("info");
	if(ImGui::CollapsingHeader("信息"))
	{
		//Scope info
		if(scope)
		{
			auto nickname = scope->m_nickname;
			auto hwname = chan->GetHwname();
			auto index = to_string(chan->GetIndex() + 1);	//use one based index for display

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

		//Filter info
		if(f)
		{
			string fname = f->GetProtocolDisplayName();

			ImGui::BeginDisabled();
				ImGui::SetNextItemWidth(width);
				ImGui::InputText("滤波器类型", &fname);
			ImGui::EndDisabled();
			HelpMarker("滤波器对象类型");
		}
	}
	ImGui::PopID();

	return true;
}

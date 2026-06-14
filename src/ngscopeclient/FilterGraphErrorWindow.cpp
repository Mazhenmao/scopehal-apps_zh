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
	@brief Implementation of FilterGraphErrorWindow
 */

#include "ngscopeclient.h"
#include "FilterGraphErrorWindow.h"
#include "Session.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

FilterGraphErrorWindow::FilterGraphErrorWindow(Session* session)
	: Dialog("Errors", "FilterGraphErrors", ImVec2(300, 400), session)
	, m_firstRun(true)
	, m_emptyErrorFrames(0)
{
}

FilterGraphErrorWindow::~FilterGraphErrorWindow()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rendering

bool FilterGraphErrorWindow::Render()
{
	//Refresh list of errors
	auto& nodes = Filter::GetAllInstances();
	m_nodesWithErrors.clear();
	for(auto node : nodes)
	{
		if(node->HasErrors())
			m_nodesWithErrors.emplace(node);
	}

	//Show error window on first run, or if we have errors
	if(!m_nodesWithErrors.empty())
	{
		m_open = true;
		m_emptyErrorFrames = 0;
	}
	else if(m_firstRun)
	{
		m_open = true;
		m_firstRun = false;
	}
	else
	{
		// 滤波器刷新时会短暂 ClearErrors()，下一步才重新 AddErrorMessage()。
		// 连续几帧都没有错误后再自动关闭，避免未接输入滤波器导致错误窗口反复开关闪烁。
		if(m_open && (m_emptyErrorFrames < 3))
			m_emptyErrorFrames ++;
		else
			m_open = false;
	}

	return Dialog::Render();
}

/**
	@brief Renders the dialog and handles UI events

	@return		True if we should continue showing the dialog
				False if it's been closed
 */
bool FilterGraphErrorWindow::DoRender()
{
	// 防抖期间窗口可能仍保持打开，但当前错误列表已经为空。
	// 直接返回，避免删除滤波器后继续访问上一帧保存的节点指针。
	if(m_nodesWithErrors.empty())
		return true;

	const ImGuiTableFlags flags =
		ImGuiTableFlags_Resizable |
		ImGuiTableFlags_BordersOuter |
		ImGuiTableFlags_BordersV |
		ImGuiTableFlags_ScrollY |
		ImGuiTableFlags_RowBg |
		ImGuiTableFlags_SizingFixedFit;

	auto width = ImGui::GetFontSize();
	if(ImGui::BeginTable("table", 2, flags))
	{
		ImGui::TableSetupScrollFreeze(0, 1); //Header row does not scroll

		ImGui::TableSetupColumn("通道", ImGuiTableColumnFlags_WidthFixed, 12*width);
		ImGui::TableSetupColumn("错误", ImGuiTableColumnFlags_WidthStretch, 0);
		ImGui::TableHeadersRow();

		for(auto f : m_nodesWithErrors)
		{
			auto messages = explode(f->GetErrorLog(), '\n');
			for(auto& m : messages)
			{
				//remove bullet and space
				string s = m.substr(m.find(' ') + 1);

				ImGui::TableNextRow(ImGuiTableRowFlags_None);
				ImGui::TableSetColumnIndex(0);
				ImGui::TextUnformatted(f->GetDisplayName().c_str());
				ImGui::TableSetColumnIndex(1);
				ImGui::TextUnformatted(s.c_str());
			}
		}

		ImGui::EndTable();
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UI event handlers

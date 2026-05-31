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
	@brief Implementation of NotesDialog
 */

#include "ngscopeclient.h"
#include "NotesDialog.h"
#include "MainWindow.h"
#include <imgui_markdown.h>

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

NotesDialog::NotesDialog(MainWindow* parent)
	: Dialog("实验记录", "实验记录", ImVec2(800, 400), nullptr, parent)
{
}

NotesDialog::~NotesDialog()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rendering

/**
	@brief Renders the dialog and handles UI events

	@return		True if we should continue showing the dialog
				False if it's been closed
 */
bool NotesDialog::DoRender()
{
	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	if (ImGui::BeginTabBar("NotesFile", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("设置记录"))
		{
			SetupNotes();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("常规记录"))
		{
			GeneralNotes();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	return true;
}

void NotesDialog::SetupNotes()
{
	ImGui::TextWrapped(
	 "详细描述你的实验搭建方式，确保能据此核对接线是否正确。支持有限的 Markdown 语法。\n\n重新加载会话时会显示这些备注，方便你在修改硬件配置前，确认所有仪器通道已正确连接。"
		);

	MarkdownEditor(m_parent->GetSession().m_setupNotes);
}

void NotesDialog::GeneralNotes()
{
	ImGui::TextWrapped(
	  "在此记录测试备注，支持有限 Markdown 语法。"
		);

	MarkdownEditor(m_parent->GetSession().m_generalNotes);
}

/**
	@brief Displays a split view with a Markdown editor and viewer
 */
void NotesDialog::MarkdownEditor(string& str)
{
	//Table with one col for live view and one for editor
	static ImGuiTableFlags flags =
		ImGuiTableFlags_Resizable |
		ImGuiTableFlags_BordersOuter |
		ImGuiTableFlags_BordersV |
		ImGuiTableFlags_ScrollY |
		ImGuiTableFlags_SizingStretchSame;
	if(ImGui::BeginTable("setupnotes", 2, flags, ImGui::GetContentRegionAvail() ))
	{
		ImGui::TableNextRow(ImGuiTableRowFlags_None);

		//Editor
		ImGui::TableSetColumnIndex(0);
		ImGui::InputTextMultiline("###设置记录", &str, ImGui::GetContentRegionAvail());

		//Render the markdown
		ImGui::TableSetColumnIndex(1);
		ImGui::Markdown(str.c_str(), str.length(), m_parent->GetMarkdownConfig());

		ImGui::EndTable();
	}
}

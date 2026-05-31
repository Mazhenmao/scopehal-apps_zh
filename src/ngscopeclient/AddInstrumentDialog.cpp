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
	@brief Implementation of AddInstrumentDialog
 */

#include "ngscopeclient.h"
#include "AddInstrumentDialog.h"
#include "MainWindow.h"

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Construction / destruction

AddInstrumentDialog::AddInstrumentDialog(
	const string& title,
	const string& nickname,
	Session* session,
	MainWindow* parent,
	const string& driverType,
	const std::string& driver,
	const std::string& transport,
	const std::string& path)
	: Dialog(
		title,
		string("AddInstrument") + to_string_hex(reinterpret_cast<uintptr_t>(this)),
		ImVec2(600, 200),
		session,
		parent)
	, m_nickname(nickname)
	, m_selectedDriver(0)
	, m_selectedTransport(0)
	, m_selectedTransportType(SCPITransportType::TRANSPORT_HID)
	, m_selectedEndpoint(0)
	, m_selectedModel(0)
	, m_path(path)
{
	SCPITransport::EnumTransports(m_transports);
	m_supportedTransports.insert(m_transports.begin(), m_transports.end());
	m_pathEdited = !m_path.empty();
	m_defaultNickname = nickname;
	m_originalNickname = nickname;
	m_nicknameEdited = false;

	m_drivers = session->GetDriverNamesForType(driverType);
	if(!driver.empty())
	{
		int i = 0;
		for(auto& driverName : m_drivers)
		{
			if(driverName == driver)
			{
				m_selectedDriver = i;
				break;
			}
			i++;
		}
	}
	// Update combo now to have the right list of transports according to the selected driver
	UpdateCombos();

	if(!transport.empty())
	{
		int i = 0;
		for(auto& transportName : m_transports)
		{
			if(transportName == transport)
			{
				m_selectedTransport = i;
				break;
			}
			i++;
		}
	}
	// Update again to setup path and nickenae
	UpdateCombos();
}

AddInstrumentDialog::~AddInstrumentDialog()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Rendering

/**
	@brief Renders the dialog and handles UI events

	@return		True if we should continue showing the dialog
				False if it's been closed
 */
bool AddInstrumentDialog::DoRender()
{
	//Get the tutorial wizard and see if we're on the "connect to scope" page
	auto tutorial = m_parent->GetTutorialWizard();
	if(tutorial && (tutorial->GetCurrentStep() != TutorialWizard::TUTORIAL_02_CONNECT) )
		tutorial = nullptr;

	if(ImGui::InputText("自定义名称", &m_nickname))
		m_nicknameEdited = !(m_nickname.empty() || (m_nickname == m_defaultNickname));
	HelpMarker(
		"为此仪器设置文本昵称，以便区分多台相似设备。\n"
		"\n"
		"该昵称会显示在最近使用仪器列表中，也会在多仪器配置中用于区分通道名称等。");

	bool dropdownOpen = false;
	if(Combo("驱动", m_drivers, m_selectedDriver,&dropdownOpen))
	{
		m_selectedModel = 0;
		m_selectedTransport = 0;
		UpdateCombos();
	}
	HelpMarker(
		"选择要使用的仪器驱动。\n"
		"\n"
		"通常一个驱动会支持某个厂商同一类型的所有硬件（例如 Siglent 示波器），"
		"但如果同一厂商有多个软件栈差异很大的产品线，也可能需要在多个驱动中选择。\n"
		"\n"
		"具体仪器应使用哪个驱动，请查阅用户手册。");

	//Show speech bubble for tutorial
	bool showedBubble = false;
	if(tutorial && (m_drivers[m_selectedDriver] != "demo") && !dropdownOpen )
	{
		auto pos = ImGui::GetCursorScreenPos();
		ImVec2 anchorPos(pos.x + 10*ImGui::GetFontSize(), pos.y);
		tutorial->DrawSpeechBubble(anchorPos, ImGuiDir_Up, "选择 \"demo\" 驱动");
		showedBubble = true;
	}
	else if(dropdownOpen)	//suppress further bubbles if dropdown is active
		showedBubble = true;

	if(m_models.size() > 1)
	{	// Only show model combo if there is more than one model
		if(Combo("型号", m_models, m_selectedModel))
			UpdateCombos();
		HelpMarker(
			"选择仪器型号。\n"
			"\n"
			"所选驱动支持该厂商的多个型号，"
			"选择型号后会自动调整仪器昵称和连接字符串。");
	}

	if(Combo("传输", m_transports, m_selectedTransport, &dropdownOpen))
		UpdateCombos();

	HelpMarker(
		"选择计算机与仪器之间连接使用的 SCPI 传输方式。\n"
		"\n"
		"该设置控制远程控制命令和波形数据如何在计算机与仪器之间传输（USB、Ethernet、GPIB 等）。\n"
		"\n"
		"注意，由于不同仪器差异很大，基于 TCP/IP 的传输方式有四种：\n",
			{
				"lan：通过 TCP socket 传输无帧格式的原始 SCPI",
				"lxi: LXI VXI-11",
				"twinlan：SCPI 文本控制命令和原始二进制波形使用独立 socket。\n"
				"通常配合桥接服务器连接 USB 仪器（Digilent、DreamSourceLabs、Pico）。",
				"vicp: Teledyne LeCroy Virtual Instrument Control Protocol"
			}
		);

	//Show speech bubble for tutorial
	if(tutorial && (m_transports[m_selectedTransport] != "null") && !dropdownOpen && !showedBubble)
	{
		auto pos = ImGui::GetCursorScreenPos();
		ImVec2 anchorPos(pos.x + 10*ImGui::GetFontSize(), pos.y);
		tutorial->DrawSpeechBubble(anchorPos, ImGuiDir_Up, "选择 \"null\" 传输方式");
		showedBubble = true;
	}
	else if(dropdownOpen)	//suppress further bubbles if dropdown is active
		showedBubble = true;

	if(!m_endpoints.empty())
	{	// Endpoint discovery available: create endpoint combo
		if(Combo("端点", m_endpointNames, m_selectedEndpoint, &dropdownOpen))
		{
			UpdatePath();
		}
		HelpMarker("从列表中选择传输端点，也可以手动编辑路径。");
		ImGui::SameLine();
		if(ImGui::Button("鉄?"))
		{
			UpdateCombos();
		}
	}
	if(ImGui::InputText("路径", &m_path))
		m_pathEdited = !(m_path.empty() || (m_path == m_defaultPath));
	HelpMarker(
		"指定如何通过当前传输方式连接到仪器。\n",
			{
				"GPIB：板卡索引和主地址（0:7）",
				"TCP/IP 传输：IP 或主机名 : 端口（localhost:5025）。\n"
				"注意 twinlan 需要两个端口号（localhost:5025:5026），分别用于 SCPI 和数据端口。",
				"UART：设备路径和波特率（/dev/ttyUSB0:9600，COM1）。未指定时默认 115200。",
				"USBTMC：Linux 设备路径（/dev/usbtmcX）",
				"USB-HID：设备 vendor id、product id（以及可选序列号）：<vendorId(hex)>:<productId(hex)>:<serialNumber>（例如：2e3c:af01）"
			}
		);

	if(ImGui::Button("添加"))
	{
		if(m_nickname.empty())
		{
			ShowErrorPopup(
				"昵称错误",
				"昵称不能为空");
		}
		else
		{
			auto transport = MakeTransport();
			if(transport)
			{
				if(DoConnect(transport))
				{
					if(tutorial)
						tutorial->AdvanceToNextStep();

					return false;
				}
			}
		}
	}

	if(tutorial && !dropdownOpen && !showedBubble)
	{
		auto pos = ImGui::GetCursorScreenPos();
		ImVec2 anchorPos(pos.x + 2*ImGui::GetFontSize(), pos.y);
		tutorial->DrawSpeechBubble(anchorPos, ImGuiDir_Up, "将示波器添加到会话");
		//showedBubble = true;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UI event handlers

/**
	@brief Create and return a new transport with the specified path
 */
SCPITransport* AddInstrumentDialog::MakeTransport()
{
	//Create the transport
	auto transport = SCPITransport::CreateTransport(m_transports[m_selectedTransport], m_path);
	if(transport == nullptr)
	{
		ShowErrorPopup(
			"传输错误",
			"无法创建类型为 \"" + m_transports[m_selectedTransport] + "\" 的传输");
		return nullptr;
	}

	//Make sure we connected OK
	if(!transport->IsConnected())
	{
		delete transport;
		ShowErrorPopup("连接错误", "无法连接到 \"" + m_path + "\"");
		return nullptr;
	}

	return transport;
}

bool AddInstrumentDialog::DoConnect(SCPITransport* transport)
{
	return m_session->CreateAndAddInstrument(m_drivers[m_selectedDriver], transport, m_nickname);
}

void AddInstrumentDialog::UpdatePath()
{
	if(m_selectedTransportType == SCPITransportType::TRANSPORT_HID)
	{	// Special handling for HID transport: replace the whole path with endpoint value
		m_path = m_endpoints[m_selectedEndpoint].path;
	}
	else
	{
		size_t pos = m_path.find(':');
		string suffix = (pos == std::string::npos) ? "" : m_path.substr(pos);
		m_path = m_endpoints[m_selectedEndpoint].path + suffix;
	}
}

void AddInstrumentDialog::UpdateCombos()
{
	// Update transoport list according to selected driver an connection string according to transport
	string driver = m_drivers[m_selectedDriver];
	auto supportedModels = SCPIInstrument::GetSupportedModels(driver);
	m_endpoints.clear();
	m_endpointNames.clear();
	if(!supportedModels.empty())
	{
		m_models.clear();
		m_transports.clear();
		int modelIndex = 0;
		auto selectedModel = supportedModels[0];
		// Model list
		for(auto& model : supportedModels)
		{
			m_models.push_back(model.modelName);
			if(modelIndex == m_selectedModel)
			{
				selectedModel = model;
			}
			modelIndex++;
		}
		// Nick name
		if(!m_nicknameEdited)
		{
			m_nickname = selectedModel.modelName;
			m_defaultNickname = m_nickname;
		}

		// Transport list
		int transportIndex = 0;
		for(auto& transport : selectedModel.supportedTransports)
		{
			string transportName = to_string(transport.transportType);
			// Prepare transport type for default value
			if(transportIndex == 0) m_selectedTransportType = transport.transportType;
			if(m_supportedTransports.find(transportName) != m_supportedTransports.end())
			{
				m_transports.push_back(transportName);
				if(transportIndex == m_selectedTransport)
				{
					m_selectedTransportType = transport.transportType;
					if(!m_pathEdited)
					{
						m_path = transport.connectionString;
						m_defaultPath = m_path;
					}
				}
				transportIndex++;
			}
		}
		if(m_selectedTransport >= (int)m_transports.size())
		{
			m_selectedTransport = 0;
			if(!m_pathEdited)
				m_path = "";
		}
		// Update endpoint list
		auto endpoints = SCPITransport::EnumEndpoints(m_transports[m_selectedTransport]);
		int endpointIndex = 0;
		for(auto& endpoint : endpoints)
		{
			m_endpoints.push_back(endpoint);
			m_endpointNames.push_back(endpoint.path + " ("+ endpoint.description +")");
			if(m_selectedTransportType == SCPITransportType::TRANSPORT_HID && (endpoint.path.rfind(m_path) == 0))
			{	// Special handling for HID : select the endpoint matching the path provided by the driver
				m_selectedEndpoint = endpointIndex;
			}
			endpointIndex++;
		}
		if(m_selectedEndpoint >= (int)m_endpoints.size())
		{
			m_selectedEndpoint = 0;
		}
		if(m_endpoints.size()>0)
			UpdatePath();
	}
	else
	{	// Supported transports not provided => add all transports
		m_transports.clear();
		m_models.clear();
		if(!m_nicknameEdited)
			m_nickname = m_originalNickname;
		SCPITransport::EnumTransports(m_transports);
		if(!m_pathEdited)
			m_path = "";
	}
}

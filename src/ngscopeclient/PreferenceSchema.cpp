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

#include "PreferenceManager.h"
#include "PreferenceTypes.h"
#include "ngscopeclient.h"

PreferenceManager PreferenceManager::m_instance;

void PreferenceManager::InitializeDefaults()
{
	//Make sure search paths are valid before we try finding fonts
	//(this can get called from global constructors before DriverStaticInit is called)
	InitializeSearchPaths();

	auto& appearance = this->m_treeRoot.AddCategory("Appearance", "外观");

		auto& consts = appearance.AddCategory("Constellations", "星座图");
			consts.AddPreference(
				Preference::ThemedColor("point_color", ColorFromString("#ff0000ff"), ColorFromString("#ff0000ff"))
				.Label("点颜色")
				.Description("标称星座点的颜色"));

		auto& cursors = appearance.AddCategory("Cursors", "光标");
			cursors.AddPreference(
				Preference::ThemedColor("cursor_1_color", ColorFromString("#808000"), ColorFromString("#ffff00"))
				.Label("光标 #1 颜色")
				.Description("左侧或顶部光标的颜色"));
			cursors.AddPreference(
				Preference::ThemedColor("cursor_2_color", ColorFromString("#ff8000"), ColorFromString("#ff8000"))
				.Label("光标 #2 颜色")
				.Description("右侧或底部光标的颜色"));
			cursors.AddPreference(
				Preference::ThemedColor("cursor_fill_color", ColorFromString("#ffff0040"), ColorFromString("#ffff0040"))
				.Label("光标填充颜色")
				.Description("两个光标之间填充区域的颜色"));
			/*cursors.AddPreference(
				Preference::Color("cursor_fill_text_color", ColorFromString("#ffff00"))
				.Label("光标填充文本颜色")
				.Description("光标之间绘制的带内功率及其他文本的颜色"));*/
			cursors.AddPreference(
				Preference::Font("label_font", FontDescription(FindDataFile("fonts/DejaVuSans.ttf"), 13))
				.Label("标签字体")
				.Description("光标标签使用的字体"));
			cursors.AddPreference(
				Preference::ThemedColor("marker_color", ColorFromString("#ff00a0"), ColorFromString("#ff00a0"))
				.Label("标记颜色")
				.Description("标记的颜色"));
			cursors.AddPreference(
				Preference::ThemedColor("hover_color", ColorFromString("#00000080"), ColorFromString("#ffffff80"))
				.Label("悬停颜色")
				.Description("鼠标悬停数据包指示器的颜色"));

		auto& decodes = appearance.AddCategory("Decodes", "协议解码");
			decodes.AddPreference(
				Preference::Font("protocol_font", FontDescription(FindDataFile("fonts/DejaVuSans.ttf"), 13))
				.Label("协议字体")
				.Description("协议解码叠加文本使用的字体"));
			/*decodes.AddPreference(
				Preference::Color("address_color", ColorFromString("#ffff00"))
				.Label("地址颜色")
				.Description("寄存器/内存地址的颜色"));
			decodes.AddPreference(
				Preference::Color("checksum_bad_color", ColorFromString("#ff0000"))
				.Label("校验和/CRC 颜色（错误）")
				.Description("错误校验和/CRC 的颜色"));
			decodes.AddPreference(
				Preference::Color("checksum_ok_color", ColorFromString("#00ff00"))
				.Label("校验和/CRC 颜色（正确）")
				.Description("正确校验和/CRC 的颜色"));
			decodes.AddPreference(
				Preference::Color("control_color", ColorFromString("#c000a0"))
				.Label("控制颜色")
				.Description("控制事件的颜色"));
			decodes.AddPreference(
				Preference::Color("data_color", ColorFromString("#336699"))
				.Label("数据颜色")
				.Description("通用协议数据字节的颜色"));
			decodes.AddPreference(
				Preference::Color("error_color", ColorFromString("#ff0000"))
				.Label("错误颜色")
				.Description("格式错误数据或错误条件的颜色"));
			decodes.AddPreference(
				Preference::Color("idle_color", ColorFromString("#404040"))
				.Label("空闲颜色")
				.Description("有效数据之间空闲序列的颜色"));
			decodes.AddPreference(
				Preference::Color("preamble_color", ColorFromString("#808080"))
				.Label("前导码颜色")
				.Description("前导码、同步字节和其他固定头部数据的颜色"));*/

		auto& eye = appearance.AddCategory("Eye Patterns", "眼图");
			eye.AddPreference(
				Preference::ThemedColor("border_color_pass", ColorFromString("#00ff00ff"), ColorFromString("#00ff00ff"))
				.Label("边框颜色（通过）")
				.Description("没有违规或违规可接受时，绘制模板多边形边框的颜色"));
			eye.AddPreference(
				Preference::ThemedColor("border_color_fail", ColorFromString("#ff0000ff"), ColorFromString("#ff0000ff"))
				.Label("边框颜色（失败）")
				.Description("存在不可接受违规时，绘制模板多边形边框的颜色"));
			eye.AddPreference(
				Preference::ThemedColor("mask_color", ColorFromString("#0000ff80"), ColorFromString("#0000ff80"))
				.Label("模板颜色")
				.Description("绘制模板叠加层的颜色"));

		auto& file = appearance.AddCategory("File Browser", "文件浏览器");
			file.AddPreference(
				Preference::Enum("dialogmode", BROWSER_NATIVE)
					.Label("非全屏对话框样式")
					.Description(
						"选择非全屏模式下，用于文件加载与保存的文件浏览器。\n\n"

						"ngscopeclient 处于全屏模式时无法使用系统原生文件浏览器，\n"
						"因此全屏状态下将始终使用 ImGui 内置文件浏览器。"
						)
					.EnumValue("ImGui", "ImGui", BROWSER_IMGUI)
					.EnumValue("Native", "原生", BROWSER_NATIVE)
					.EnumValue("KDialog", "KDialog", BROWSER_KDIALOG)
				);

		auto& graph = appearance.AddCategory("Filter Graph", "滤波器图");
			graph.AddPreference(
				Preference::Font("header_font", FontDescription(FindDataFile("fonts/DejaVuSans.ttf"), 15))
				.Label("标题字体")
				.Description("滤波器/通道名称的字体"));
			graph.AddPreference(
				Preference::ThemedColor("background_color", ColorFromString("#c0c0c0c8"), ColorFromString("#3c3c46c8"))
				.Label("背景颜色")
				.Description("滤波器图形画布颜色"));
			graph.AddPreference(
				Preference::ThemedColor("grid_color", ColorFromString("#78787828"), ColorFromString("#78787828"))
				.Label("网格颜色")
				.Description("滤波器图形画布上网格线的颜色"));
			graph.AddPreference(
				Preference::ThemedColor("node_bg_color", ColorFromString("#a0a0a0c8"), ColorFromString("#202020c8"))
				.Label("节点背景颜色")
				.Description("框图节点颜色"));
			graph.AddPreference(
				Preference::ThemedColor("node_border_color", ColorFromString("#00000060"), ColorFromString("#ffffff60"))
				.Label("节点边框颜色")
				.Description("框图节点轮廓颜色"));
			graph.AddPreference(
				Preference::ThemedColor("node_border_hovered_color", ColorFromString("#0d81ffff"), ColorFromString("#50b0ffff"))
				.Label("悬停节点边框颜色")
				.Description("鼠标悬停时框图节点的轮廓颜色"));
			graph.AddPreference(
				Preference::ThemedColor("node_border_selected_color", ColorFromString("#ff9d04ff"), ColorFromString("#ffb032ff"))
				.Label("选中节点边框颜色")
				.Description("节点被选中时框图节点的轮廓颜色"));
			graph.AddPreference(
				Preference::ThemedColor("node_select_rect", ColorFromString("#0582ff40"), ColorFromString("#0582ff40"))
				.Label("节点选择框填充颜色")
				.Description("选择框的填充颜色"));
			graph.AddPreference(
				Preference::ThemedColor("node_select_border", ColorFromString("#0582ff80"), ColorFromString("#0582ff80"))
				.Label("节点选择框边框")
				.Description("选择框的边框颜色"));
			graph.AddPreference(
				Preference::ThemedColor("edge_select_rect", ColorFromString("#0582ff40"), ColorFromString("#0582ff40"))
				.Label("连线选择框填充颜色")
				.Description("选择框的填充颜色"));
			graph.AddPreference(
				Preference::ThemedColor("edge_select_border", ColorFromString("#0582ff80"), ColorFromString("#0582ff80"))
				.Label("连线选择框边框")
				.Description("选择框的边框颜色"));
			graph.AddPreference(
				Preference::ThemedColor("edge_hovered_border", ColorFromString("#32b0ffff"), ColorFromString("#32b0ffff"))
				.Label("悬停连线外框")
				.Description("鼠标悬停时框图连线的轮廓颜色"));
			graph.AddPreference(
				Preference::ThemedColor("edge_selected_border", ColorFromString("#ffb032ff"), ColorFromString("#ffb032ff"))
				.Label("选中连线外框")
				.Description("连线被选中时框图连线的轮廓颜色"));
			graph.AddPreference(
				Preference::ThemedColor("edge_highlight_border", ColorFromString("#cc6900ff"), ColorFromString("#cc6900ff"))
				.Label("高亮连线外框")
				.Description("连线被高亮时框图连线的轮廓颜色"));
			graph.AddPreference(
				Preference::ThemedColor("pin_rect", ColorFromString("#0280D164"), ColorFromString("#3cb4ff64"))
				.Label("端口矩形底色")
				.Description("被高亮的节点端口背景矩形颜色"));
			graph.AddPreference(
				Preference::ThemedColor("pin_rect_border", ColorFromString("#3cb4ff80"), ColorFromString("#3cb4ff80"))
				.Label("端口矩形边框")
				.Description("被高亮的节点端口轮廓颜色"));
			graph.AddPreference(
				Preference::ThemedColor("edge_flow", ColorFromString("#ff8040ff"), ColorFromString("#ff8040ff"))
				.Label("连线流动光效颜色")
				.Description("连线上数据流流动光晕的颜色"));
			graph.AddPreference(
				Preference::ThemedColor("edge_flow_marker", ColorFromString("#ff8040ff"), ColorFromString("#ff8040ff"))
				.Label("连线流动标记颜色")
				.Description("连线上数据流流动标记点的颜色"));
			graph.AddPreference(
				Preference::ThemedColor("edge_color", ColorFromString("#000000ff"), ColorFromString("#ffffffff"))
				.Label("连线颜色")
				.Description("框图连线的颜色"));
			graph.AddPreference(
				Preference::ThemedColor("group_bg", ColorFromString("#00000030"), ColorFromString("#000000a0"))
				.Label("分组背景")
				.Description("节点分组背景矩形的颜色"));
			graph.AddPreference(
				Preference::ThemedColor("group_border", ColorFromString("#00000020"), ColorFromString("#ffffff20"))
				.Label("分组边框")
				.Description("节点分组的边框颜色"));

			graph.AddPreference(
				Preference::ThemedColor("valid_link_color", ColorFromString("#00ff00"), ColorFromString("#00ff00"))
				.Label("有效连接颜色")
				.Description("表示潜在连接路径有效的颜色"));
			graph.AddPreference(
				Preference::ThemedColor("invalid_link_color", ColorFromString("#ff0000"), ColorFromString("#ff0000"))
				.Label("无效连接颜色")
				.Description("表示潜在连接路径无效的颜色"));
			graph.AddPreference(
				Preference::ThemedColor("infobubble_color", ColorFromString("#a0a0a0"), ColorFromString("#404040"))
				.Label("信息气泡颜色")
				.Description("图表节点上方信息气泡的颜色"));
			graph.AddPreference(
				Preference::Font("icon_caption_font", FontDescription(FindDataFile("fonts/DejaVuSans.ttf"), 13))
				.Label("图标字体")
				.Description("图标标题的字体"));
			graph.AddPreference(
				Preference::ThemedColor("icon_caption_color", ColorFromString("#000000"), ColorFromString("#ffffff"))
				.Label("图标颜色")
				.Description("图标标题颜色"));

			graph.AddPreference(
				Preference::ThemedColor("error_outline_color", ColorFromString("#ff0000"), ColorFromString("#ff0000"))
				.Label("错误轮廓颜色")
				.Description("带错误图节点的轮廓颜色"));
			graph.AddPreference(
				Preference::ThemedColor("warning_outline_color", ColorFromString("#ffff00"), ColorFromString("#ffff00"))
				.Label("警告轮廓颜色")
				.Description("含警告图节点轮廓颜色"));
			graph.AddPreference(
				Preference::ThemedColor("notice_outline_color", ColorFromString("#00ff00"), ColorFromString("#00ff00"))
				.Label("提示轮廓颜色")
				.Description("含提示信息图节点轮廓颜色"));
			graph.AddPreference(
					Preference::ThemedColor("verbose_outline_color", ColorFromString("#a0a0ff"), ColorFromString("#a0a0ff"))
					.Label("详细轮廓颜色r")
					.Description("用于以详细消息勾勒图节点的颜色"));
			graph.AddPreference(
					Preference::ThemedColor("debug_outline_color", ColorFromString("#a0a0a0"), ColorFromString("#a0a0a0"))
					.Label("调试轮廓颜色")
					.Description("用于高亮显示调试消息的图节点颜色"));

		auto& ahelp = appearance.AddCategory("Help", "帮助");
			ahelp.AddPreference(
				Preference::ThemedColor("bubble_outline_color", ColorFromString("#00ff00"), ColorFromString("#00ff00"))
				.Label("气泡轮廓颜色")
				.Description("教程气泡轮廓的颜色"));

			ahelp.AddPreference(
				Preference::ThemedColor("bubble_fill_color", ColorFromString("#e0e0e0e0"), ColorFromString("#202020e0"))
				.Label("气泡填充颜色")
				.Description("教程气泡填充的颜色"));

		auto& stream = appearance.AddCategory("Stream Browser", "流浏览器");
			stream.AddPreference(
				Preference::Enum("numeric_value_display", NUMERIC_DISPLAY_MONO_FONT)
					.Label("数值显示")
					.Description(
						"选择数字万用表 (DMM) 与电源单元 (PSU) 节点的数值显示方式\n"
						"- 控制台字体：使用常规首选项中定义的控制台字体（等宽字体）。\n"
						"- 七段数码管：使用七段数码管样式显示。\n"
						"- 默认字体：使用常规首选项中定义的默认字体（比例字体）。\n"
						)
					.EnumValue("Console font", "控制台字体", NUMERIC_DISPLAY_MONO_FONT)
					.EnumValue("7 segment", "七段数码管字体", NUMERIC_DISPLAY_7SEGMENT)
					.EnumValue("Default font", "默认字体", NUMERIC_DISPLAY_DEFAULT_FONT)
				);
			stream.AddPreference(
				Preference::Bool("show_block_border", true)
				.Label("显示块边框")
				.Description("在流浏览器块周围显示可视边框（例如通道属性）"));
			stream.AddPreference(
				Preference::Real("instrument_badge_latch_duration", 0.4)
				.Label("仪器徽章保持时间（秒）")
				.Description("仪器徽章保持显示的持续时间（用于防止闪烁）。"));
			stream.AddPreference(
				Preference::ThemedColor("download_wait_badge_color", ColorFromString("#CC4C4C"), ColorFromString("#CC4C4C"))
				.Label("下载等待徽章颜色")
				.Description("下载“等待”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("download_progress_badge_color", ColorFromString("#B3B44D"), ColorFromString("#B3B44D"))
				.Label("下载进度徽章颜色")
				.Description("下载“进行中”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("download_finished_badge_color", ColorFromString("#4CCC4C"), ColorFromString("#4CCC4C"))
				.Label("下载完成徽章颜色")
				.Description("下载“已完成”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("download_active_badge_color", ColorFromString("#4CCC4C"), ColorFromString("#4CCC4C"))
				.Label("下载活动徽章颜色")
				.Description("下载“活动”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("trigger_armed_badge_color", ColorFromString("#4CCC4C"), ColorFromString("#4CCC4C"))
				.Label("触发已就绪徽章颜色")
				.Description("触发“已就绪”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("trigger_stopped_badge_color", ColorFromString("#CC4C4C"), ColorFromString("#CC4C4C"))
				.Label("触发已停止徽章颜色")
				.Description("触发“已停止”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("trigger_triggered_badge_color", ColorFromString("#B3B44D"), ColorFromString("#B3B44D"))
				.Label("已触发徽章颜色")
				.Description("触发“已触发”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("trigger_busy_badge_color", ColorFromString("#CC4C4C"), ColorFromString("#CC4C4C"))
				.Label("触发忙碌徽章颜色")
				.Description("触发“忙碌”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("trigger_auto_badge_color", ColorFromString("#4CCC4C"), ColorFromString("#4CCC4C"))
				.Label("自动触发徽章颜色")
				.Description("触发“自动”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("instrument_disabled_badge_color", ColorFromString("#666666"), ColorFromString("#666666"))
				.Label("仪器已禁用徽章颜色")
				.Description("仪器“已禁用”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("instrument_offline_badge_color", ColorFromString("#CC4C4C"), ColorFromString("#CC4C4C"))
				.Label("仪器离线徽章颜色")
				.Description("仪器“离线”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("instrument_on_badge_color", ColorFromString("#4CCC4C"), ColorFromString("#4CCC4C"))
				.Label("仪器开启徽章颜色")
				.Description("仪器“开启”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("instrument_partial_badge_color", ColorFromString("#E2CD23FF"), ColorFromString("#E2CD23FF"))
				.Label("仪器部分开启徽章颜色")
				.Description("仪器“部分开启”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("instrument_off_badge_color", ColorFromString("#EBEB00FF"), ColorFromString("#808000ff"))
				.Label("仪器关闭徽章颜色")
				.Description("仪器“关闭”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("psu_cv_badge_color", ColorFromString("#4CCC4C"), ColorFromString("#4CCC4C"))
				.Label("PSU CV 徽章颜色")
				.Description("PSU “CV”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("psu_cc_badge_color", ColorFromString("#CC4C4C"), ColorFromString("#CC4C4C"))
				.Label("PSU CC 徽章颜色")
				.Description("PSU “CC”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("psu_set_label_color", ColorFromString("#FFFF00"), ColorFromString("#FFFF00"))
				.Label("PSU 设定标签颜色")
				.Description("PSU “设定”标签的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("psu_meas_label_color", ColorFromString("#00C100"), ColorFromString("#00C100"))
				.Label("PSU 测量标签颜色")
				.Description("PSU “测量”标签的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("psu_7_segment_color", ColorFromString("#B2FFFF"), ColorFromString("#B2FFFF"))
				.Label("PSU 7 段数码管显示颜色")
				.Description("PSU 7 段数码管样式显示的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("awg_hiz_badge_color", ColorFromString("#666600"), ColorFromString("#666600"))
				.Label("函数发生器 HI-Z 徽章颜色")
				.Description("函数发生器“HI-Z”徽章的颜色"));
			stream.AddPreference(
				Preference::ThemedColor("awg_50ohms_badge_color", ColorFromString("#B54C85"), ColorFromString("#B54C85"))
				.Label("函数发生器 50 欧姆徽章颜色")
				.Description("函数发生器“50Ohm”徽章的颜色"));

		auto& general = appearance.AddCategory("General", "常规");
			general.AddPreference(
				Preference::Enum("theme", THEME_DARK)
					.Label("GUI 主题")
					.Description("GUI 控件的配色方案")
					.EnumValue("Light", "浅色", THEME_LIGHT)
					.EnumValue("Dark", "深色", THEME_DARK)
					.EnumValue("Classic", "经典", THEME_CLASSIC)
				);
			general.AddPreference(
				Preference::Font("default_font", FontDescription(FindDataFile("fonts/DejaVuSans.ttf"), 14))
				.Label("默认字体")
				.Description("大多数 GUI 元素使用的字体"));
			general.AddPreference(
				Preference::Font("title_font", FontDescription(FindDataFile("fonts/DejaVuSans-Bold.ttf"), 13))
				.Label("标题字体")
				.Description("报告或向导标题使用的字体"));
			general.AddPreference(
				Preference::Font("console_font", FontDescription(FindDataFile("fonts/DejaVuSansMono.ttf"), 13))
				.Label("控制台字体")
				.Description("SCPI 控制台、日志查看器以及流浏览器中 PSU/DMM 数值使用的字体"));
			general.AddPreference(
				Preference::ThemedColor("apply_button_color", ColorFromString("#4CCC4C"), ColorFromString("#4CCC4C"))
				.Label("应用按钮颜色")
				.Description("应用数值按钮的颜色"));

		auto& graphs = appearance.AddCategory("Graphs", "图表");
			graphs.AddPreference(
				Preference::ThemedColor("bottom_color", ColorFromString("#e0e0e0ff"), ColorFromString("#000000ff"))
				.Label("背景底部颜色")
				.Description("波形图背景渐变底部的颜色"));
			graphs.AddPreference(
				Preference::ThemedColor("top_color", ColorFromString("#ffffffff"), ColorFromString("#202020ff"))
				.Label("背景顶部颜色")
				.Description("波形图背景渐变顶部的颜色"));
			graphs.AddPreference(
				Preference::ThemedColor("grid_centerline_color", ColorFromString("#c0c0c0ff"), ColorFromString("#c0c0c0ff"))
				.Label("网格中心线颜色")
				.Description("Y=0 网格线的颜色"));
			graphs.AddPreference(
				Preference::ThemedColor("grid_color", ColorFromString("#c0c0c040"), ColorFromString("#c0c0c040"))
				.Label("网格颜色")
				.Description("网格线的颜色"));
			graphs.AddPreference(
				Preference::Real("grid_centerline_width", 1)
				.Label("坐标轴宽度")
				.Description("Y=0 网格线的宽度"));
			graphs.AddPreference(
				Preference::Real("grid_width", 1)
				.Label("网格线宽度")
				.Description("网格线宽度"));
			graphs.AddPreference(
				Preference::ThemedColor("y_axis_text_color", ColorFromString("#000000ff"), ColorFromString("#ffffffff"))
				.Label("Y 轴文本颜色")
				.Description("Y 轴文本的颜色"));
			graphs.AddPreference(
				Preference::Font("y_axis_font", FontDescription(FindDataFile("fonts/DejaVuSans.ttf"), 13))
					.Label("Y 轴字体")
					.Description("Y 轴文本使用的字体"));

		auto& logs = appearance.AddCategory("Log Viewer", "日志查看器");
			logs.AddPreference(
				Preference::ThemedColor("error_color", ColorFromString("#c00000"), ColorFromString("#800000"))
				.Label("错误颜色")
				.Description("错误级别日志消息的背景颜色"));

			logs.AddPreference(
				Preference::ThemedColor("warning_color", ColorFromString("#c0c000"), ColorFromString("#404000"))
				.Label("警告颜色")
				.Description("警告级别日志消息的背景颜色"));

		auto& markdown = appearance.AddCategory("Markdown", "Markdown");

			markdown.AddPreference(
				Preference::Font("heading_1_font", FontDescription(FindDataFile("fonts/DejaVuSans-Bold.ttf"), 20))
					.Label("一级标题字体")
					.Description("Markdown 一级标题使用的字体"));

			markdown.AddPreference(
				Preference::Font("heading_2_font", FontDescription(FindDataFile("fonts/DejaVuSans-Bold.ttf"), 16))
					.Label("二级标题字体")
					.Description("Markdown 二级标题使用的字体"));

			markdown.AddPreference(
				Preference::Font("heading_3_font", FontDescription(FindDataFile("fonts/DejaVuSans-Bold.ttf"), 14))
					.Label("三级标题字体")
					.Description("Markdown 三级标题使用的字体"));

		auto& peaks = appearance.AddCategory("Peaks", "峰值");
			/*peaks.AddPreference(
				Preference::Color("peak_outline_color", ColorFromString("#009900"))
				.Label("轮廓颜色")
				.Description("峰值标签轮廓的颜色"));*/
			peaks.AddPreference(
				Preference::ThemedColor("peak_text_color", ColorFromString("#000000"), ColorFromString("#ffffff"))
				.Label("文本颜色")
				.Description("峰值标签文本的颜色"));
			/*peaks.AddPreference(
				Preference::Color("peak_background_color", ColorFromString("#000000"))
				.Label("背景颜色")
				.Description("峰值标签背景的颜色"));
				*/
			peaks.AddPreference(
				Preference::Font("label_font", FontDescription(FindDataFile("fonts/DejaVuSans.ttf"), 13))
					.Label("标签字体")
					.Description("峰值标签使用的字体"));


		auto& proto = appearance.AddCategory("Protocol Analyzer", "协议分析器");
			/*proto.AddPreference(
				Preference::Color("command_color", ColorFromString("#600050"))
				.Label("命令颜色")
				.Description("执行命令的数据包颜色"));
			proto.AddPreference(
				Preference::Color("control_color", ColorFromString("#808000"))
				.Label("控制颜色")
				.Description("具有控制功能的数据包颜色"));
			proto.AddPreference(
				Preference::Color("data_read_color", ColorFromString("#336699"))
				.Label("数据读取颜色")
				.Description("从外设读取信息的数据包颜色"));
			proto.AddPreference(
				Preference::Color("data_write_color", ColorFromString("#339966"))
				.Label("数据写入颜色")
				.Description("向外设写入信息的数据包颜色"));
			proto.AddPreference(
				Preference::Color("error_color", ColorFromString("#800000"))
				.Label("错误颜色")
				.Description("格式错误或表示错误条件的数据包颜色"));
			proto.AddPreference(
				Preference::Color("status_color", ColorFromString("#000080"))
				.Label("状态颜色")
				.Description("传递状态信息的数据包颜色"));
			proto.AddPreference(
				Preference::Color("default_color", ColorFromString("#101010"))
				.Label("默认颜色")
				.Description("不属于其他类别的数据包颜色"));*/
			proto.AddPreference(
				Preference::Font("data_font", FontDescription(FindDataFile("fonts/DejaVuSansMono.ttf"), 13))
					.Label("数据字体")
					.Description("数据包十六进制转储使用的字体"));

		auto& timeline = appearance.AddCategory("Timeline", "时间线");
			timeline.AddPreference(
				Preference::ThemedColor("axis_color", ColorFromString("#000000"), ColorFromString("#ffffff"))
				.Label("坐标轴颜色")
				.Description("X 轴线和刻度线的颜色"));
			timeline.AddPreference(
				Preference::ThemedColor("text_color", ColorFromString("#000000"), ColorFromString("#ffffff"))
				.Label("文本颜色")
				.Description("X 轴文本标签的颜色"));
			timeline.AddPreference(
				Preference::ThemedColor("trigger_bar_color", ColorFromString("#00000040"), ColorFromString("#ffffff40"))
				.Label("触发条颜色")
				.Description("拖动触发位置时显示的垂直位置线颜色"));
			timeline.AddPreference(
				Preference::Font("x_axis_font", FontDescription(FindDataFile("fonts/DejaVuSans.ttf"), 15))
				.Label("X 轴字体")
				.Description("X 轴文本使用的字体"));

		auto& toolbar = appearance.AddCategory("Toolbar", "工具栏");
			toolbar.AddPreference(
				Preference::Enum("toolbar_height", 2)
					.Label("工具栏高度")
					.Description("工具栏高度，以字体大小的倍数为单位")
					.EnumValue("2x", 2)
					.EnumValue("4x", 4)
				);
			toolbar.AddPreference(
				Preference::Enum("icon_theme", ICON_THEME_DARK)
					.Label("图标主题")
					.Description("工具栏主题")
					.EnumValue("Dark", ICON_THEME_DARK)
					.EnumValue("Light", ICON_THEME_LIGHT)
				);

		auto& windows = appearance.AddCategory("Windowing", "窗口");
			windows.AddPreference(
				Preference::Enum("viewport_mode", VIEWPORT_ENABLE)
					.Label("视窗模式")
					.Description(
						"指定 GUI 库是否允许创建多个顶层窗口,\n"
						"或是强制所有子窗口（菜单、对话框、工具提示等）都限制在\n"
						"应用程序窗口的边界之内。\n"
						"\n"
						"默认采用多窗口模式，但如果你在使用 Linux 平铺式窗口管理器时遇到问题，\n"
						"切换为单窗口模式可能会获得更好的使用体验。\n"
						"\n"
						"修改此项设置后，需要重启 ngscopeclient 才会生效。"
						)
					.EnumValue("Multi window", "多窗口", VIEWPORT_ENABLE)
					.EnumValue("Single window", "单窗口", VIEWPORT_DISABLE)
				);
			windows.AddPreference(
				Preference::Enum("startup_mode", STARTUP_MODE_WINDOWED)
					.Label("窗口启动模式")
					.Description(
						"指定 ngscopeclient 启动时窗口的打开方式。\n"
						"\n"
						"默认为窗口模式：程序以固定 1280×720 大小的窗口启动。\n"
						"其他可选选项：\n"
						" - 最大化：窗口在主显示器上最大化显示\n"
						" - 上次状态：恢复为上一次退出时的窗口位置与尺寸\n"
						)
					.EnumValue("Windowed", STARTUP_MODE_WINDOWED)
					.EnumValue("Maximized", STARTUP_MODE_MAXIMIZED)
					.EnumValue("Last State", STARTUP_MODE_LAST_STATE)
				);
		auto& windowStartup = appearance.AddCategory("Startup", "启动");
			windowStartup.AddPreference(Preference::Bool("startup_fullscreen", false).Invisible());
			windowStartup.AddPreference(Preference::Bool("startup_maximized", false).Invisible());
			windowStartup.AddPreference(Preference::Int ("startup_pos_x", 0).Invisible());
			windowStartup.AddPreference(Preference::Int ("startup_pos_y", 0).Invisible());
			windowStartup.AddPreference(Preference::Int ("startup_size_width", 0).Invisible());
			windowStartup.AddPreference(Preference::Int ("startup_size_height", 0).Invisible());
			windowStartup.AddPreference(Preference::String ("monitor_name", "").Invisible());
			windowStartup.AddPreference(Preference::Int ("monitor_width", 0).Invisible());
			windowStartup.AddPreference(Preference::Int ("monitor_height", 0).Invisible());

	auto& drivers = this->m_treeRoot.AddCategory("Drivers", "驱动");
		auto& dgeneral = drivers.AddCategory("General", "常规");
			dgeneral.AddPreference(
				Preference::Enum("headless_startup", HEADLESS_STARTUP_C1_ONLY)
				.Label("无界面示波器默认状态")
				.Description(
				"选择连接至 PC、且自身不带前面板显示屏的示波器，\n"
				"默认启用的通道集合。"
				)
				.EnumValue("All non-MSO channels", HEADLESS_STARTUP_ALL_NON_MSO)
				.EnumValue("Channel 1 only", HEADLESS_STARTUP_C1_ONLY) );

		auto& rigol = drivers.AddCategory("Rigol DHO", "Rigol DHO");
			rigol.AddPreference(
				Preference::Enum("data_width", WIDTH_AUTO)
					.Label("数据宽度")
					.Description(
					"从仪器下载采样数据时使用的数据位宽。\n\n"
					"即便仪器搭载 12 位 ADC，选用 8 位而非 16 位数据格式，"
					"可以获得更快的波形刷新速率。\n\n"
					"如果希望优先保证数据精度，而非刷新速度，请选择 16 位模式。\n\n"
					"该设置将在下次建立仪器连接时生效；"
					"已经处于活动状态的会话不会更新传输格式。"
						)
					.EnumValue("Auto", WIDTH_AUTO)
					.EnumValue("8 bits", WIDTH_8_BITS)
					.EnumValue("16 bits", WIDTH_16_BITS)
				);

		auto& rsrtb = drivers.AddCategory("RohdeSchwarz RTB", "RohdeSchwarz RTB");
			rsrtb.AddPreference(
				Preference::Enum("data_width", WIDTH_16_BITS)
					.Label("数据宽度")
					.Description(
					"从仪器下载采样数据时使用的数据位宽。\n\n"
					"即便仪器配备 10 位 ADC，使用 8 位而非 16 位数据格式，"
					"能够提升波形刷新速率。\n\n"
					"如果希望优先保证数据精度，而非刷新速度，请选择 16 位模式。\n\n"
					"该设置会在下次建立仪器连接时生效；"
					"已处于运行中的会话不会更新传输格式。"
						)
					.EnumValue("8 bits", WIDTH_8_BITS)
					.EnumValue("16 bits", WIDTH_16_BITS)
				);

		auto& siglent = drivers.AddCategory("Siglent SDS HD", "Siglent SDS HD");
			siglent.AddPreference(
				Preference::Enum("data_width", WIDTH_AUTO)
					.Label("数据宽度")
					.Description(
					"从仪器下载采样数据时使用的数据位宽。\n\n"
					"即便仪器搭载 12 位 ADC，使用 8 位而非 16 位数据格式，"
					"波形刷新速率可提升约 10%。\n\n"
					"如果你希望优先保证数据精度，而非刷新速度，请选择 16 位模式。\n\n"
					"该设置将在下次建立仪器连接时生效；"
					"已处于活动状态的会话不会更新传输格式。"
						)
					.EnumValue("Auto", WIDTH_AUTO)
					.EnumValue("8 bits", WIDTH_8_BITS)
					.EnumValue("16 bits", WIDTH_16_BITS)
				);

		auto& lecroy = drivers.AddCategory("Teledyne LeCroy", "Teledyne LeCroy");
			lecroy.AddPreference(
				Preference::Bool("force_16bit", true)
				.Label("强制 16 位模式")
				.Description(
					"从仪器下载采样数据时强制使用 16 位整数格式。\n\n"
					"即便仪器仅配备 8 位 ADC，由于内部平坦度校正与校准流程，"
					"示波器内部的数据表示会带有额外有效位。\n\n"
					"关闭该选项时，搭载 8 位 ADC 的仪器将采用 8 位整数格式下载采样点。"
					"此举可小幅提升每秒波形处理性能，但会增大量化噪声，"
					"还可能在眼图中出现水平 “条纹” 伪影。\n\n"
					"该设置对 ADC 位数大于 8 位的仪器（HDO、WaveSurfer HD、WaveRunner HD、WavePro HD）无效，"
					"这类仪器始终使用 16 位传输格式。\n\n"
					"修改此设置后，将在下次建立仪器连接时生效；"
					"已建立的活动会话不会更新传输格式。"
				));

	auto& files = this->m_treeRoot.AddCategory("Files", "文件");
		files.AddPreference(
			Preference::Int("max_recent_files", 10)
			.Label("最近文件最大数量")
			.Description("历史记录中保存的最近 .scopesession 文件路径最大数量")
			.Unit(Unit::UNIT_COUNTS));

	auto& help = this->m_treeRoot.AddCategory("Help", "帮助");
		auto& wizards = help.AddCategory("Wizards", "向导");
			wizards.AddPreference(
				Preference::Bool("first_run_wizard", true)
				.Label("首次运行向导")
				.Description("启用首次运行教程向导"));

	auto& misc = this->m_treeRoot.AddCategory("Miscellaneous", "杂项");
		auto& menus = misc.AddCategory("Menus", "菜单");
			menus.AddPreference(
				Preference::Int("recent_instrument_count", 20)
				.Label("最近使用仪器数量")
				.Description("显示的最近使用仪器数量"));

	auto& pwr = this->m_treeRoot.AddCategory("Power", "电源/功率");
		auto& events = pwr.AddCategory("Events", "事件");
			events.AddPreference(
				Preference::Enum("event_driven_ui", 0)
					.Label("事件循环模式")
					.Description(
						"指定主事件循环的运行方式。\n"
						"\n"
						"在**性能模式**下，事件循环以与显示器刷新率锁定的恒定速率运行。\n"
						"该模式下界面最为流畅，波形刷新能力最强，\n"
						"但持续重绘会增加功耗。\n"
						"\n"
						"在**省电模式**下，事件循环会进入阻塞状态，直至发生 GUI 事件（按键、鼠标移动等），\n"
						"或是到达用户设定的超时时间。该模式画面更新会出现卡顿，\n"
						"但可让 CPU 大部分时间处于空闲状态，以此节省功耗。"
						)
					.EnumValue("Performance", 0)
					.EnumValue("Power", 1)
				);
			events.AddPreference(
				Preference::Real("polling_timeout", FS_PER_SECOND / 4)
				.Label("轮询超时")
				.Unit(Unit::UNIT_FS)
				.Description(
					"省电优化模式下事件循环的轮询超时时间。\n\n"
					"超时数值越大，功耗越低，但画面更新速度也会变慢。\n")
				);


	/*
	auto& privacy = this->m_treeRoot.AddCategory("Privacy", "隐私");
		 privacy.AddPreference(
			Preference::Bool("redact_serial_in_title", false)
			.Label("在标题栏隐藏序列号")
			.Description(
				"在窗口标题栏中对仪器序列号做部分隐藏处理。\n\n"
				"该功能便于你分享截图，而不会泄露仪器序列号。"
			));*/
}

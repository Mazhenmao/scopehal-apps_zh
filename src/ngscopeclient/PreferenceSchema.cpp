/***********************************************************************************************************************
*                                                                                                                      *
* ngscopeclient                                                                                                        *
*                                                                                                                      *
* Copyright (c) 2012-2026 Andrew D. Zonenberg                                                                          *
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
	auto& appearance = this->m_treeRoot.AddCategory("Appearance", "外观");

		auto& consts = appearance.AddCategory("Constellations", "星座图");
			consts.AddPreference(
				Preference::Color("point_color", ColorFromString("#ff0000ff"))
				.Label("点颜色")
				.Description("标称星座点的颜色"));

		auto& cursors = appearance.AddCategory("Cursors", "光标");
			cursors.AddPreference(
				Preference::Color("cursor_1_color", ColorFromString("#ffff00"))
				.Label("光标 #1 颜色")
				.Description("左侧或顶部光标的颜色"));
			cursors.AddPreference(
				Preference::Color("cursor_2_color", ColorFromString("#ff8000"))
				.Label("光标 #2 颜色")
				.Description("右侧或底部光标的颜色"));
			cursors.AddPreference(
				Preference::Color("cursor_fill_color", ColorFromString("#ffff0040"))
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
				Preference::Color("marker_color", ColorFromString("#ff00a0"))
				.Label("标记颜色")
				.Description("标记的颜色"));
			cursors.AddPreference(
				Preference::Color("hover_color", ColorFromString("#ffffff80"))
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
				Preference::Color("border_color_pass", ColorFromString("#00ff00ff"))
				.Label("边框颜色（通过）")
				.Description("没有违规或违规可接受时，绘制模板多边形边框的颜色"));
			eye.AddPreference(
				Preference::Color("border_color_fail", ColorFromString("#ff0000ff"))
				.Label("边框颜色（失败）")
				.Description("存在不可接受违规时，绘制模板多边形边框的颜色"));
			eye.AddPreference(
				Preference::Color("mask_color", ColorFromString("#0000ff80"))
				.Label("模板颜色")
				.Description("绘制模板叠加层的颜色"));

		auto& file = appearance.AddCategory("File Browser", "文件浏览器");
			file.AddPreference(
				Preference::Enum("dialogmode", BROWSER_NATIVE)
					.Label("非全屏对话框样式")
					.Description("选择非全屏模式下加载和保存文件时使用的文件浏览器。\n\nngscopeclient 处于全屏模式时无法使用原生文件浏览器，\n因此全屏时始终使用 ImGui 浏览器。")
					.EnumValue("ImGui", BROWSER_IMGUI)
					.EnumValue("Native", "原生", BROWSER_NATIVE)
					.EnumValue("KDialog", BROWSER_KDIALOG)
				);

		auto& graph = appearance.AddCategory("Filter Graph", "滤波器图");
			graph.AddPreference(
				Preference::Font("header_font", FontDescription(FindDataFile("fonts/DejaVuSans.ttf"), 15))
				.Label("标题字体")
				.Description("滤波器/通道名称的字体"));
			graph.AddPreference(
				Preference::Color("header_text_color", ColorFromString("#000000"))
				.Label("标题文本颜色")
				.Description("滤波器/通道名称的颜色"));
			graph.AddPreference(
				Preference::Color("valid_link_color", ColorFromString("#00ff00"))
				.Label("有效连接颜色")
				.Description("表示潜在连接路径有效的颜色"));
			graph.AddPreference(
				Preference::Color("invalid_link_color", ColorFromString("#ff0000"))
				.Label("无效连接颜色")
				.Description("表示潜在连接路径无效的颜色"));
			graph.AddPreference(
				Preference::Color("infobubble_color", ColorFromString("#404040"))
				.Label("信息气泡颜色")
				.Description("图节点上方信息气泡的颜色"));
			graph.AddPreference(
				Preference::Color("error_outline_color", ColorFromString("#ff0000"))
				.Label("错误轮廓颜色")
				.Description("带错误图节点的轮廓颜色"));
			graph.AddPreference(
				Preference::Font("icon_caption_font", FontDescription(FindDataFile("fonts/DejaVuSans.ttf"), 13))
				.Label("图标字体")
				.Description("图标标题的字体"));
			graph.AddPreference(
				Preference::Color("icon_caption_color", ColorFromString("#ffffff"))
				.Label("图标颜色")
				.Description("图标标题的颜色"));

		auto& ahelp = appearance.AddCategory("Help", "帮助");
			ahelp.AddPreference(
				Preference::Color("bubble_outline_color", ColorFromString("#00ff00"))
				.Label("气泡轮廓颜色")
				.Description("教程气泡轮廓的颜色"));

			ahelp.AddPreference(
				Preference::Color("bubble_fill_color", ColorFromString("#202020e0"))
				.Label("气泡填充颜色")
				.Description("教程气泡填充的颜色"));

		auto& stream = appearance.AddCategory("Stream Browser", "流浏览器");
			stream.AddPreference(
				Preference::Enum("numeric_value_display", NUMERIC_DISPLAY_MONO_FONT)
					.Label("数值显示")
					.Description("选择 DMM 和 PSU 节点的数值显示方式。\n- 控制台字体：使用“常规”偏好设置中定义的控制台字体（等宽）。\n- 7 段数码管：使用 7 段数码管样式显示。\n- 默认字体：使用“常规”偏好设置中定义的默认字体（比例字体）。\n")
					.EnumValue("Console font", "控制台字体", NUMERIC_DISPLAY_MONO_FONT)
					.EnumValue("7 segment", "7 段数码管", NUMERIC_DISPLAY_7SEGMENT)
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
				Preference::Color("download_wait_badge_color", ColorFromString("#CC4C4C"))
				.Label("下载等待徽章颜色")
				.Description("下载“等待”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("download_progress_badge_color", ColorFromString("#B3B44D"))
				.Label("下载进度徽章颜色")
				.Description("下载“进行中”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("download_finished_badge_color", ColorFromString("#4CCC4C"))
				.Label("下载完成徽章颜色")
				.Description("下载“已完成”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("download_active_badge_color", ColorFromString("#4CCC4C"))
				.Label("下载活动徽章颜色")
				.Description("下载“活动”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("trigger_armed_badge_color", ColorFromString("#4CCC4C"))
				.Label("触发已就绪徽章颜色")
				.Description("触发“已就绪”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("trigger_stopped_badge_color", ColorFromString("#CC4C4C"))
				.Label("触发已停止徽章颜色")
				.Description("触发“已停止”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("trigger_triggered_badge_color", ColorFromString("#B3B44D"))
				.Label("已触发徽章颜色")
				.Description("触发“已触发”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("trigger_busy_badge_color", ColorFromString("#CC4C4C"))
				.Label("触发忙碌徽章颜色")
				.Description("触发“忙碌”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("trigger_auto_badge_color", ColorFromString("#4CCC4C"))
				.Label("自动触发徽章颜色")
				.Description("触发“自动”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("instrument_disabled_badge_color", ColorFromString("#666666"))
				.Label("仪器已禁用徽章颜色")
				.Description("仪器“已禁用”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("instrument_offline_badge_color", ColorFromString("#CC4C4C"))
				.Label("仪器离线徽章颜色")
				.Description("仪器“离线”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("instrument_on_badge_color", ColorFromString("#4CCC4C"))
				.Label("仪器开启徽章颜色")
				.Description("仪器“开启”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("instrument_partial_badge_color", ColorFromString("#E2CD23FF"))
				.Label("仪器部分开启徽章颜色")
				.Description("仪器“部分开启”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("instrument_off_badge_color", ColorFromString("#808000ff"))
				.Label("仪器关闭徽章颜色")
				.Description("仪器“关闭”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("psu_cv_badge_color", ColorFromString("#4CCC4C"))
				.Label("PSU CV 徽章颜色")
				.Description("PSU “CV”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("psu_cc_badge_color", ColorFromString("#CC4C4C"))
				.Label("PSU CC 徽章颜色")
				.Description("PSU “CC”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("psu_set_label_color", ColorFromString("#FFFF00"))
				.Label("PSU 设定标签颜色")
				.Description("PSU “设定”标签的颜色"));
			stream.AddPreference(
				Preference::Color("psu_meas_label_color", ColorFromString("#00C100"))
				.Label("PSU 测量标签颜色")
				.Description("PSU “测量”标签的颜色"));
			stream.AddPreference(
				Preference::Color("psu_7_segment_color", ColorFromString("#B2FFFF"))
				.Label("PSU 7 段数码管显示颜色")
				.Description("PSU 7 段数码管样式显示的颜色"));
			stream.AddPreference(
				Preference::Color("awg_hiz_badge_color", ColorFromString("#666600"))
				.Label("函数发生器 HI-Z 徽章颜色")
				.Description("函数发生器“HI-Z”徽章的颜色"));
			stream.AddPreference(
				Preference::Color("awg_50ohms_badge_color", ColorFromString("#B54C85"))
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
				Preference::Color("apply_button_color", ColorFromString("#4CCC4C"))
				.Label("应用按钮颜色")
				.Description("应用数值按钮的颜色"));

		auto& graphs = appearance.AddCategory("Graphs", "图表");
			graphs.AddPreference(
				Preference::Color("bottom_color", ColorFromString("#000000ff"))
				.Label("背景底部颜色")
				.Description("波形图背景渐变底部的颜色"));
			graphs.AddPreference(
				Preference::Color("top_color", ColorFromString("#202020ff"))
				.Label("背景顶部颜色")
				.Description("波形图背景渐变顶部的颜色"));
			graphs.AddPreference(
				Preference::Color("grid_centerline_color", ColorFromString("#c0c0c0ff"))
				.Label("网格中心线颜色")
				.Description("Y=0 网格线的颜色"));
			graphs.AddPreference(
				Preference::Color("grid_color", ColorFromString("#c0c0c040"))
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
				Preference::Color("y_axis_text_color", ColorFromString("#ffffffff"))
				.Label("Y 轴文本颜色")
				.Description("Y 轴文本的颜色"));
			graphs.AddPreference(
				Preference::Font("y_axis_font", FontDescription(FindDataFile("fonts/DejaVuSans.ttf"), 13))
					.Label("Y 轴字体")
					.Description("Y 轴文本使用的字体"));

		auto& logs = appearance.AddCategory("Log Viewer", "日志查看器");
			logs.AddPreference(
				Preference::Color("error_color", ColorFromString("#800000"))
				.Label("错误颜色")
				.Description("错误级别日志消息的背景颜色"));

			logs.AddPreference(
				Preference::Color("warning_color", ColorFromString("#404000"))
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
				Preference::Color("peak_text_color", ColorFromString("#ffffff"))
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
				Preference::Color("axis_color", ColorFromString("#ffffff"))
				.Label("坐标轴颜色")
				.Description("X 轴线和刻度线的颜色"));
			timeline.AddPreference(
				Preference::Color("text_color", ColorFromString("#ffffff"))
				.Label("文本颜色")
				.Description("X 轴文本标签的颜色"));
			timeline.AddPreference(
				Preference::Color("trigger_bar_color", ColorFromString("#ffffff40"))
				.Label("触发条颜色")
				.Description("拖动触发位置时显示的垂直位置线颜色"));
			timeline.AddPreference(
				Preference::Font("x_axis_font", FontDescription(FindDataFile("fonts/DejaVuSans.ttf"), 15))
				.Label("X 轴字体")
				.Description("X 轴文本使用的字体"));

		/*auto& waveforms = appearance.AddCategory("Waveforms", "波形");
			waveforms.AddPreference(
				Preference::Real("persist_decay_rate", 0.9)
				.Label("余辉衰减率（0 = 无，1 = 无限）")
				.Description("余辉波形的衰减率。")
				.Unit(Unit::UNIT_COUNTS));
		*/
		auto& windows = appearance.AddCategory("Windowing", "窗口");
			windows.AddPreference(
				Preference::Enum("viewport_mode", VIEWPORT_ENABLE)
					.Label("视口模式")
					.Description("指定 GUI 库是否允许创建多个顶层窗口，\n或者强制所有子窗口（菜单、对话框、工具提示等）保持在\n应用程序窗口边界内。\n\n默认使用多窗口模式；如果 Linux 平铺窗口管理器出现问题，\n使用单窗口模式可能体验更好。\n\n更改此设置后，需要重启 ngscopeclient 才会生效。")
					.EnumValue("Multi window", "多窗口", VIEWPORT_ENABLE)
					.EnumValue("Single window", "单窗口", VIEWPORT_DISABLE)
				);
			windows.AddPreference(
				Preference::Enum("startup_mode", STARTUP_MODE_WINDOWED)
					.Label("窗口启动模式")
					.Description("指定 Ngscopeclient 启动时窗口的打开方式。\n\n默认是窗口模式：应用程序以固定的 1280x720 窗口启动。\n其他选项包括：\n - 最大化：窗口在主屏幕上最大化，\n - 上次状态：恢复到上次的位置和大小。\n")
					.EnumValue("Windowed", "窗口模式", STARTUP_MODE_WINDOWED)
					.EnumValue("Maximized", "最大化", STARTUP_MODE_MAXIMIZED)
					.EnumValue("Last State", "上次状态", STARTUP_MODE_LAST_STATE)
				);
		auto& windowStartup = appearance.AddCategory("Startup", "启动");
			windowStartup.AddPreference(Preference::Bool("startup_fullscreen", false).Invisible());
			windowStartup.AddPreference(Preference::Bool("startup_maximized", false).Invisible());
			windowStartup.AddPreference(Preference::Int ("startup_pos_x", 0).Invisible());
			windowStartup.AddPreference(Preference::Int ("startup_pos_y", 0).Invisible());
			windowStartup.AddPreference(Preference::Int ("startup_size_width", 0).Invisible());
			windowStartup.AddPreference(Preference::Int ("startup_size_heigth", 0).Invisible());
			windowStartup.AddPreference(Preference::String ("monitor_name", "").Invisible());
			windowStartup.AddPreference(Preference::Int ("monitor_width", 0).Invisible());
			windowStartup.AddPreference(Preference::Int ("monitor_heigth", 0).Invisible());

	auto& drivers = this->m_treeRoot.AddCategory("Drivers", "驱动");
		auto& dgeneral = drivers.AddCategory("General", "常规");
			dgeneral.AddPreference(
				Preference::Enum("headless_startup", HEADLESS_STARTUP_C1_ONLY)
				.Label("无屏示波器默认状态")
				.Description("选择连接到 PC 且没有前面板显示的示波器默认启用的通道集合。")
				.EnumValue("All non-MSO channels", "所有非 MSO 通道", HEADLESS_STARTUP_ALL_NON_MSO)
				.EnumValue("Channel 1 only", "仅通道 1", HEADLESS_STARTUP_C1_ONLY) );

		auto& rigol = drivers.AddCategory("Rigol DHO", "Rigol DHO");
			rigol.AddPreference(
				Preference::Enum("data_width", WIDTH_AUTO)
					.Label("数据宽度")
					.Description("从仪器下载采样数据时使用的数据宽度。\n\n即使仪器具有 12 位 ADC，使用 8 位而不是 16 位数据格式也可以获得更快的波形刷新率。\n\n如果更重视数据精度而不是刷新率，请选择 16 位模式。\n\n更改此设置会在下次打开仪器连接时生效；活动会话的传输格式不会更新。")
					.EnumValue("Auto", "自动", WIDTH_AUTO)
					.EnumValue("8 bits", "8 位", WIDTH_8_BITS)
					.EnumValue("16 bits", "16 位", WIDTH_16_BITS)
				);

		auto& rsrtb = drivers.AddCategory("RohdeSchwarz RTB", "RohdeSchwarz RTB");
			rsrtb.AddPreference(
				Preference::Enum("data_width", WIDTH_16_BITS)
					.Label("数据宽度")
					.Description("从仪器下载采样数据时使用的数据宽度。\n\n即使仪器具有 10 位 ADC，使用 8 位而不是 16 位数据格式也可以获得更快的波形刷新率。\n\n如果更重视数据精度而不是刷新率，请选择 16 位模式。\n\n更改此设置会在下次打开仪器连接时生效；活动会话的传输格式不会更新。")
					.EnumValue("8 bits", "8 位", WIDTH_8_BITS)
					.EnumValue("16 bits", "16 位", WIDTH_16_BITS)
				);

		auto& siglent = drivers.AddCategory("Siglent SDS HD", "Siglent SDS HD");
			siglent.AddPreference(
				Preference::Enum("data_width", WIDTH_AUTO)
					.Label("数据宽度")
					.Description("从仪器下载采样数据时使用的数据宽度。\n\n即使仪器具有 12 位 ADC，使用 8 位而不是 16 位数据格式也可以获得约 10% 更快的波形刷新率。\n\n如果更重视数据精度而不是刷新率，请选择 16 位模式。\n\n更改此设置会在下次打开仪器连接时生效；活动会话的传输格式不会更新。")
					.EnumValue("Auto", "自动", WIDTH_AUTO)
					.EnumValue("8 bits", "8 位", WIDTH_8_BITS)
					.EnumValue("16 bits", "16 位", WIDTH_16_BITS)
				);

		auto& lecroy = drivers.AddCategory("Teledyne LeCroy", "Teledyne LeCroy");
			lecroy.AddPreference(
				Preference::Bool("force_16bit", true)
				.Label("强制 16 位模式")
				.Description("从仪器下载采样数据时强制使用 16 位整数格式。\n\n即使仪器只有 8 位 ADC，由于内部平坦度校正和校准步骤，示波器内部数据表示也具有额外的有效位。\n\n禁用此设置时，具有 8 位 ADC 的仪器将使用 8 位整数格式下载采样。这会略微提升每秒波形数性能，但会增加量化噪声，并可能在眼图中产生水平“条纹”伪影。\n\n此设置对 ADC 位数大于 8 位的仪器（HDO、WaveSurfer HD、WaveRunner HD、WavePro HD）无效，这些仪器始终使用 16 位传输格式。\n\n更改此设置会在下次打开仪器连接时生效；活动会话的传输格式不会更新。"));

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
					.Description("指定主事件循环的运行方式。\n\n性能模式下，事件循环以锁定到显示刷新率的恒定速度运行。这样可以获得最流畅的 GUI 和最高的波形更新率，但持续重绘会增加功耗。\n\n省电模式下，事件循环会阻塞，直到发生 GUI 事件（按键、鼠标移动等）或用户指定的超时时间到期。这会使显示更新不那么平滑，但让 CPU 大部分时间保持空闲以节省电力。")
					.EnumValue("Performance", "性能", 0)
					.EnumValue("Power", "省电", 1)
				);
			events.AddPreference(
				Preference::Real("polling_timeout", FS_PER_SECOND / 4)
				.Label("轮询超时")
				.Unit(Unit::UNIT_FS)
				.Description("省电优化模式下事件循环的轮询超时。\n\n更长的超时时间会降低功耗，但也会减慢显示更新。\n")
				);


	/*
	auto& privacy = this->m_treeRoot.AddCategory("Privacy", "隐私");
		 privacy.AddPreference(
			Preference::Bool("redact_serial_in_title", false)
			.Label("在标题栏隐藏序列号")
			.Description("在窗口标题栏中部分隐藏仪器序列号。\n\n这样可以在共享截图时不暴露序列号。"));*/
}

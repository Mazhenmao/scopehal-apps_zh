/***********************************************************************************************************************
*                                                                                                                      *
* ngscopeclient                                                                                                        *
*                                                                                                                      *
* Copyright (c) 2012-2026 Andrew D. Zonenberg and contributors                                                         *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
***********************************************************************************************************************/

#include "Localization.h"

#include <unordered_map>

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Simplified Chinese UI localization

static const unordered_map<string, string>& GetTranslations()
{
	static const unordered_map<string, string> translations =
	{
		{"File", "文件"},
		{"Open Online...", "在线打开..."},
		{"Open Offline...", "离线打开..."},
		{"Open Online", "在线打开"},
		{"Open Offline", "离线打开"},
		{"Save", "保存"},
		{"Save As...", "另存为..."},
		{"Close", "关闭"},
		{"Exit", "退出"},
		{"Recent Files", "最近文件"},
		{"View", "视图"},
		{"Fullscreen", "全屏显示"},
		{"Persistence Setup", "余辉设置"},
		{"Add", "添加"},
		{"Add ", "添加 "},
		{"Update ", "更新 "},
		{"BERT", "误码率测试仪"},
		{"Function Generator", "函数信号发生器"},
		{"Load", "电子负载"},
		{"Misc", "其他"},
		{"Multimeter", "万用表"},
		{"Oscilloscope", "示波器"},
		{"RF Generator", "射频信号源"},
		{"SDR", "软件无线电"},
		{"Spectrometer", "光谱仪"},
		{"VNA", "矢量网络分析仪"},
		{"Add an oscilloscope to your session", "向会话添加一台示波器"},
		{"Connect...", "连接..."},
		{"Channels", "通道"},
		{"Import", "导入"},
		{"Generate", "生成"},
		{"Setup", "设置"},
		{"Manage Instruments...", "管理仪器..."},
		{"Manage Instruments", "管理仪器"},
		{"Trigger Groups", "触发组"},
		{"All instruments in a trigger group are synchronized and trigger in lock-step.\n"
		 "The root instrument of a trigger group must have a trigger-out port.\n"
		 "All instruments in a trigger group should be connected to a common reference clock to avoid skew.", 
		 "触发组内所有仪器保持同步，并以锁步方式触发。\n"
		 "触发组的主仪器必须具备触发输出端口。\n"
		 "触发组内所有仪器应连接到同一参考时钟，以避免时序偏移。"},
		{"Trigger...", "触发..."},
		{"Trigger", "触发"},
		{"Preferences...", "首选项..."},
		{"Window", "窗口"},
		{"Lab Notes", "实验记录"},
		{"Describe your experimental setup in sufficient detail that you could verify it's wired correctly. "
		 "Limited Markdown syntax is supported.\n\n"
		 "These notes will be displayed when re-loading the session so you can confirm all instrument channels "
		 "are connected correctly before making any changes to hardware configuration.",
		 "详细描述你的实验搭建方式，确保能据此核对接线是否正确。支持有限的 Markdown 语法。\n\n"
		 "重新加载会话时会显示这些备注，方便你在修改硬件配置前，确认所有仪器通道已正确连接。"},
		 {"Take notes on your testing here. Limited Markdown syntax is supported.", "在此记录测试备注，支持有限 Markdown 语法。"},
		{"Log Viewer", "日志查看器"},
		{"Measurements", "测量"},
		{"Performance Metrics", "性能指标"},
		{"Rendering", "渲染"},
		{"Filter graph", "滤波器图"},
		{"Filter Graph Editor", "滤波器编辑器"},
		{"Waveform Group ", "波形组 "},
		{"Acquisition", "采集"},
		{"Buffers", "缓存"},
		{"Memory", "存储器"},
		{"Memory Analysis", "内存分析"},
		{"History", "历史"},
		{"Filter Graph", "滤波器图"},
		{"Stream Browser", "流浏览器"},
		{"Filter Palette", "滤波器面板"},
		{"Category", "分类"},
		{"New Workspace", "新建工作区"},
		{"Analyzer", "分析仪"},
		{"Power Supply", "电源"},
		{"SCPI Console", "SCPI 控制台"},
		{"Debug", "调试"},
		{"ImGui Demo", "ImGui 演示"},
		{"Memory Leaker", "内存泄漏测试"},
		{"Hardware Flags", "硬件标志"},
		{"Help", "帮助"},
		{"Tutorial...", "教程..."},
		{"About...", "关于..."},
		{"OK", "确定"},
		{"Cancel", "取消"},
		{"Apply", "应用"},
		{"Delete", "删除"},
		{"Autofit", "自动适配"},
		{"Cursors", "游标"},
		{"X axis", "X 轴"},
		{"Y axis", "Y 轴"},
		{"None", "无"},
		{"Single", "单游标"},
		{"Dual", "双游标"},
		{"Add Marker", "添加标记"},
		{"Color ramp", "颜色映射"},
		{"Persistence", "余辉"},
		{"Trend", "趋势"},
		{"Summary", "摘要"},
		{"Back", "上一步"},
		{"<< Back", "<< 上一步"},
		{"Finish", "完成"},
		{"Continue >>", "继续 >>"},
		{"Auto", "自动"},
		{"PLL Lock", "PLL 锁定"},
		{"Invert", "反相"},
		{"Auto Zero", "自动调零"},
		{"Enable", "启用"},
		{"Lock", "锁定"},
		{"Horz Bathtub", "水平浴盆图"},
		{"Eye", "眼图"},
		{"Mask file", "掩码文件"},
		{"Clock Source", "时钟源"},
		{"Sampling mode", "采样模式"},
		{"Voltage:", "电压:"},
		{"Current:", "电流:"},
		{"Waveform:", "波形:"},
		{"Drag %s", "拖动 %s"},
		{"Realtime BER: %s\nEye BER: %s", "实时 BER: %s\n眼图 BER: %s"},
		{"BER: %s", "BER: %s"},
		{"Changing input buffer settings will also affect the following channels:", "更改输入缓冲区设置也会影响以下通道:"},
		{"Estimated %s", "预计 %s"},
		{"(drag stream here)", "(将流拖到此处)"},
		{"All", "全部"},
		{"Reconnect", "重新连接"},
		{"Load Offline", "离线加载"},
		{"Abort", "中止"},
		{"Proceed", "继续"},
		{"I have reviewed the instrument configuration and confirmed it will not cause damage.", "我已检查仪器配置，并确认不会造成损坏。"},
		{"Do you want to reconnect to the lab instruments or load session data for offline analysis?\n", "要重新连接实验室仪器，还是加载会话数据进行离线分析？\n"},
		{"Deskew", "去偏斜"},
		{"Error", "错误"},
		{"Warning", "警告"},
		{"Notice", "通知"},
		{"Verbose", "详细"},
		{"Trace", "跟踪"},
		{"Connect", "连接"},
		{"Name", "名称"},
		{"Nickname", "自定义名称"},
		{"Vendor", "厂商"},
		{"All Instruments", "所有仪器"},
		{"Model", "型号"},
		{"Serial", "序列号"},
		{"Transport", "传输"},
		{"Path", "路径"},
		{"Connection", "连接"},
		{"Type", "类型"},
		{"Status", "状态"},
		{"Input", "输入"},
		{"Output", "输出"},
		{"Channel", "通道"},
		{"Color", "颜色"},
		{"Label", "标签"},
		{"Description", "描述"},
		{"Value", "值"},
		{"Default", "默认"},
		{"###default", "###默认"},
		{"Reset", "重置"},
		{"Browse", "浏览"},
		{"Clear", "清除"},
		{"Start", "开始"},
		{"Stop", "停止"},
		{"Force", "强制"},
		{"Download", "下载"},
		{"Upload", "上传"},
		{"Measure", "测量"},
		{"Properties", "属性"},
		{"General", "常规"},
		{"Appearance", "外观"},
		{"Files", "文件"},
		{"Miscellaneous", "杂项"},
		{"Drivers", "驱动"},
		{"Power", "电源"},
		{"Events", "事件"},
		{"Preferences", "首选项"},
		{"Reset section", "重置分区"},
		{"Reset category", "重置分类"},
		{"Reset all Preferences", "重置全部首选项"},
		{"Reset to default", "恢复默认值"},
		{"Reset all settings in this section to default?", "是否将此分区中的所有设置恢复为默认值？"},
		{"Reset all settings in this category to default?", "是否将此分类中的所有设置恢复为默认值？"},
		{"Reset all settings to default?", "是否将所有设置恢复为默认值？"},
		{"Instrument", "仪器"},
		{"Hardware Channel", "硬件通道"},
		{"Hardware Name", "硬件名称"},
		{"Make", "制造商"},
		{"Driver", "驱动"},
		{"Continue", "继续"},
		{"inputs", "输入"},
		{"outputs", "输出"},
		{"icon", "图标"},
		{"Timestamp", "时间戳"},
		{"Requested", "请求值"},
		{"Avoided", "已避免"},
		{"Blocking", "阻塞"},
		{"Non-blocking", "非阻塞"},
		{"Output Enable", "输出使能"},
		{"Done", "完成"},
		{"Pending", "等待中"},
		{"Voltage", "电压"},
		{"Current", "电流"},
		{"Filter", "滤波器"},
		{"Filter Type", "滤波器类型"},
		{"Actions", "操作"},
		{"Object", "对象"},
		{"Skew", "偏斜"},
		{"Budget", "预算"},
		{"Usage", "用量"},
		{"Custom Pattern", "自定义码型"},
		{"Clock Out Frequency", "时钟输出频率"},
		{"Clock In Frequency", "时钟输入频率"},
		{"Sample X", "采样 X"},
		{"Sample Y", "采样 Y"},
		{"Pre-cursor", "前游标"},
		{"Post-cursor", "后游标"},
		{"Probe Type", "探头类型"},
		{"Averaging", "平均"},
		{"Search", "搜索"},
		{"Analog channel", "模拟通道"},
		{"Digital channel", "数字通道"},
		{"Digital bus", "数字总线"},
		{"Eye pattern", "眼图"},
		{"Spectrogram", "频谱图"},
		{"Waterfall", "瀑布图"},
		{"Protocol data", "协议数据"},
		{"External trigger", "外部触发"},
		{"Analog value:", "模拟值:"},
		{"Unknown channel type", "未知通道类型"},
		{"No waveform data", "无波形数据"},
		{"2D density plot", "二维密度图"},
		{"Uniformly sampled analog data, %s at %s intervals", "均匀采样模拟数据，%s，间隔 %s"},
		{"Uniformly sampled digital data, %s at %s intervals", "均匀采样数字数据，%s，间隔 %s"},
		{"Sparsely sampled analog data, %s at %s resolution", "稀疏采样模拟数据，%s，分辨率 %s"},
		{"Sparsely sampled digital data, %s at %s resolution", "稀疏采样数字数据，%s，分辨率 %s"},
		{"Uniformly sampled data, %s at %s intervals", "均匀采样数据，%s，间隔 %s"},
		{"Sparsely sampled data, %s at %s resolution", "稀疏采样数据，%s，分辨率 %s"},
		{"Create", "创建"},
		{"New Group", "新建组"},
		{"New Filter", "新建滤波器"},
		{"Parameter %s is unimplemented type", "参数 %s 的类型尚未实现"},
		{"This dialog allows you to override hardware feature flag detection.", "此对话框允许覆盖硬件特性标志检测。"},
		{"Legacy GPU filter enable", "启用旧版 GPU 滤波器"},
		{"Memory budget", "内存预算"},
		{"Push descriptor", "推送描述符"},
		{"Shader float64", "着色器 float64"},
		{"Shader int64", "着色器 int64"},
		{"Shader atomic int64", "着色器原子 int64"},
		{"Shader int16", "着色器 int16"},
		{"Shader int8", "着色器 int8"},
		{"Shader atomic float", "着色器原子 float"},
		{"Debug utils", "调试工具"},
		{"History Depth", "历史深度"},
		{"Pin", "固定"},
		{"Load Enable", "负载使能"},
		{"Mode", "模式"},
		{"Class", "类别"},
		{"Function", "功能"},
		{"Severity", "严重级别"},
		{"Message", "消息"},
		{"Intensity", "强度"},
		{"Group", "组"},
		{"Hardware", "硬件"},
		{"Session file", "会话文件"},
		{"Info", "信息"},
		{"Features", "特性"},
		{"Size (elements)", "大小 (元素)"},
		{"Size (bytes)", "大小 (字节)"},
		{"Capacity (elements)", "容量 (元素)"},
		{"Capacity (bytes)", "容量 (字节)"},
		{"Overhead (elements)", "开销 (元素)"},
		{"Overhead (bytes)", "开销 (字节)"},
		{"Overhead (%)", "开销 (%)"},
		{"Framerate", "帧率"},
		{"Refresh rate", "刷新率"},
		{"Rasterize time", "栅格化时间"},
		{"Tone map time", "色调映射时间"},
		{"Vertices", "顶点"},
		{"Indices", "索引"},
		{"Total filters", "滤波器总数"},
		{"Exec time", "执行时间"},
		{"Waveform rate", "波形速率"},
		{"Pending waveforms", "等待中的波形"},
		{"Total", "总计"},
		{"Setup Notes", "设置记录"},
		{"###Setup Notes", "###设置记录"},
		{"General Notes", "常规记录"},
		{"Decay Coefficient", "衰减系数"},
		{"Overload shutdown", "过载关断"},
		{"Overcurrent Shutdown", "过流关断"},
		{"Soft Start", "软启动"},
		{"Data Format", "数据格式"},
		{"Data", "数据"},
		{"Image", "图像"},
		{"Level", "电平"},
		{"Frequency", "频率"},
		{"Modulation Enable", "调制使能"},
		{"FM Enable", "FM 使能"},
		{"Welcome", "欢迎"},
		{"Cross-Trigger Cabling", "交叉触发接线"},
		{"Cross-Trigger Setup", "交叉触发设置"},
		{"Calibration Signal Setup", "校准信号设置"},
		{"Reference Clock Setup", "参考时钟设置"},
		{"Use external reference on primary", "主设备使用外部参考"},
		{"Use external reference on secondary", "从设备使用外部参考"},
		{"Calibration Measurements", "校准测量"},
		{"Acquire", "采集"},
		{"Correlate", "相关"},
		{"Correlation", "相关性"},
		{"Acquiring", "正在采集"},
		{"Calculating", "正在计算"},
		{"Command", "命令"},
		{"Value 1", "值 1"},
		{"Value 2", "值 2"},
		{"Delta", "差值"},
		{"Band", "频段"},
		{"Please review your lab notes and confirm that the experimental setup matches your previous session.",
			"请检查实验记录，并确认实验设置与之前的会话一致。"},
		{"Some of the instrument settings in the session you are loading do not match \n",
			"正在加载的会话中有部分仪器设置不匹配。\n"},
		{"Some of the instrument settings in the session you are loading do not match ",
			"正在加载的会话中有部分仪器设置不匹配。"},
		{"Some of the instrument settings in the session you are loading do not match "
		 "the current hardware configuration, and if set incorrectly could potentially "
		 "damage the instrument and/or DUT.",
		 "正在加载的会话中有部分仪器设置与当前硬件配置不匹配，如果设置错误，可能会损坏仪器和/或被测设备。"},
		{"Drag to the root of a trigger group to add this instrument to the group.\n",
			"拖到触发组根节点即可将此仪器加入该组。\n"},
		{"Drag to the root of a trigger group to add this instrument to the group.\n"
		 "Drag to an ungrouped instrument to create a new group under it.\n"
		 "Drag an instrument to the root of its current group to make it the primary.\n",
			"拖到触发组根节点即可将此仪器加入该组。\n"
			"拖到未分组仪器上即可在其下创建新组。\n"
			"将仪器拖到其当前组的根节点即可设为主设备。\n"},
		{"Bus", "总线"},
		{"Clocking", "时钟"},
		{"Export", "导出"},
		{"Generation", "信号生成"},
		{"Math", "数学运算"},
		{"Measurement", "测量"},
		{"Memory", "存储/内存"},
		{"Miscellaneous", "其他/杂项"},
		{"Optical", "光学/光信号"},
		{"Power", "电源/功率"},
		{"RF", "射频信号处理"},
		{"Serial_p", "串行"},
		{"Signal integrity", "信号完整性"},
		{"Arm the trigger in normal mode", "常规触发模式"},
		{"Arm the trigger in one-shot mode", "单次触发模式"},
		{"Acquire a waveform immediately, ignoring the trigger condition", "强制触发"},
		{"Stop acquiring waveforms", "停止采集"},
		{"Show waveform history window", "历史波形"},
		{"Flush PC-side cached instrument state and reload configuration from the instrument.\n\n"
		 "This will cause a brief slowdown of the application, but can be used to re-sync when\n"
		 "changes are made on the instrument front panel that ngscopeclient does not detect.",
		 "清除 PC 端缓存的仪器状态，并从仪器重新加载配置\n"
		 "此操作会导致应用程序短暂卡顿，但当在仪器前面板上进行了\n"
		 "ngscopeclient 未检测到的更改时，可通过该操作重新同步。"},
		{"Clear waveform persistence, eye patterns, and accumulated filter block state", "清除波形余辉、眼图及滤波器模块累积状态"},
		{"Enter fullscreen mode", "全屏显示"},
		{"Leave fullscreen mode", "退出全屏显示"},
		{"Interleaving", "交错采样"},
		{"Combine ADCs from multiple channels to get higher sampling rate on a subset of channels.\n"
		 "\n"
		 "Some instruments do not have an explicit interleaving switch, but available sample rates "
		 "may vary depending on which channels are active.", 
		 "合并多通道模数转换器，使部分通道获得更高采样率。\n"
		 "部分仪器无独立的交错采样开关，可用采样率会随启用通道的不同而变化。"
		},
		{"Sample Rate", "采样率"},
		{"Adjust the ADC sampling rate.\n\n"
		 "Note that with some instruments, the set of available sampling rates varies depending on which channels are active.", 
		 "调整模数转换器采样率。\n"
		 "注：部分仪器的可选采样率会随启用通道变化。"},
		{"Memory Depth", "存储深度"},
		{"Adjust the number of samples captured each trigger event.\n\n"
		 "Note that with some instruments, the maximum memory depth varies depending on which channels are active.", 
		 "设置每次触发所采集的采样点数。\n"
		 "注：部分仪器的最大存储深度会随启用通道数量变化。"},
	};
	return translations;
}

static string TranslateString(const string& text)
{
	if(text.empty())
		return text;

	// Preserve ImGui hidden IDs ("Label##id" or "Label###id").
	auto idpos = text.find("##");
	string visible = (idpos == string::npos) ? text : text.substr(0, idpos);
	string id = (idpos == string::npos) ? "" : text.substr(idpos);
	if(visible.empty())
		return text;

	auto& translations = GetTranslations();
	auto it = translations.find(visible);
	if(it == translations.end())
		return text;

	return it->second + id;
}

const char* Tr(const char* text)
{
	if(text == nullptr)
		return nullptr;

	thread_local string translated;
	translated = TranslateString(text);
	return translated.c_str();
}

string Tr(string text)
{
	return TranslateString(text);
}

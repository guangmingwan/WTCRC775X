#include "ui.h"
#include "stdint.h"
#include <stdlib.h>

#include "global.h"
#include "tuner.h"
#include <string.h>
#include "soft_i2c.h"
#include "nv_memory.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifdef LCD25696
#include "lcd25696.h"
#else
#include "oled.h"
#endif

uint8_t nDelayedSmooth = 0;

volatile int8_t nLRot;
volatile int8_t nRRot;
volatile int8_t nIntSeqs[] = {0, 0};
volatile int8_t nFlags[] = {0, 0};

const char *pszTuneTypes[] = {"FS", "SK", "CH", "SC", "AN", "SS"};
const char *pszBands[] = {"LW", "MW", "SW", "FL", "FM"}; // Band name to display
char* pszFilter = NULL;
char szStep[4] = "";
extern uint8_t bDisp_USN;
extern uint32_t IRCode;
extern uint8_t IR_Flag;
uint8_t irKey = 0;
uint8_t bExitMenu;
uint8_t nSNRAnt = 0;
void processRemoteInput(uint32_t irCode);
// SineGen volume
const uint8_t SINE_GEN_VOL[] =
	{
		5, 0xF4, 0x46, 0x1A, 0x0F, 0x33, // SineGen volume, left ch, -20dB
		5, 0xF4, 0x45, 0x1B, 0x0F, 0x33, // SineGen volume, right ch, -20dB
		0};

// Special menu strings
const char MT_RET[] = "RET";
const char MT_EXIT[] = "EXIT";

const char MT_RADI[] = "RADI";
const char MT_AUDI[] = "AUDI";
const char MT_APPL[] = "APPL";

struct M_ITEM M_Option[] =
	{
		{MID_RADI, MT_RADI, "收音机设置"},
		{MID_AUDI, MT_AUDI, "音频设置"},
		{MID_APPL, MT_APPL, "应用设置"},
		{MID_EXIT, MT_EXIT, "退出菜单"}};

// Menu Option(Left encoder)
const char MT_SQU1[] = "Mute Threshold";
const char MT_SQU2[] = "Search Threshold";
const char MT_LSIG[] = "Low Signal Thre";  // Normal/reduced signal quality for seek/scan/any
const char MT_FIRM[] = "Firmware Sel";	   // Firmware
const char MT_FMST[] = "FM Stereo";		   // FM stereo
const char MT_FMAT[] = "FM Antenna Sel";   // FM antenna selection
const char MT_FMSI[] = "FM StereoImprov";  // FM stereo improvement
const char MT_FMCE[] = "FM ChannelEqu";	   // FM channel equalizer
const char MT_FMMP[] = "FM MultpathImprv"; // FM enhanced multipath suppression
const char MT_FMNS[] = "FM ClickSuppresn"; // FM click noise suppression
const char MT_INCA[] = "Imprved NoiseCal"; // FM AM improvec noise canceller
const char MT_FMBW[] = "FM Bandwidth";	   // FM dynamic bandwidth
const char MT_DEEM[] = "Deemphasize";	   // FM de-emphasis
const char MT_AGC[] = "AGC Threshold";	   // RFAGC wideband threshold
const char MT_NB[] = "Noise Blanker";	   // Noise blanker sensitivity
const char MT_TONE[] = "Tone Ctrl";		   // Bass, middle & treble
const char MT_BAL[] = "Balance & Fader";   // Balance & fader
const char MT_BKLT[] = "BackLight";
const char MT_TSCN[] = "Time Scan";
const char MT_TANY[] = "Time Any";
const char MT_TIME[] = "Time Set";

struct M_ITEM M_Radio[] =
	{
		{MID_SQUELCH1, MT_SQU1, "静音阈值"},
		{MID_SQUELCH2, MT_SQU2, "搜索阈值"},
		{MID_LSIG, MT_LSIG,"信号低阈值"},
		{MID_FIRM, MT_FIRM,"固件选择"},
		{MID_FMST, MT_FMST,"FM立体声设定"},
		{MID_FMAT, MT_FMAT,"FM天线选择"},
		{MID_FMSI, MT_FMSI,"FM立体声增强"},
		{MID_FMCE, MT_FMCE,"FM频道均衡"},
		{MID_FMMP, MT_FMMP,"FM多路径增强"},
		{MID_FMNS, MT_FMNS,"FM滴答声消除"},
		{MID_INCA, MT_INCA,"FM/AM静噪"},
		{MID_FMBW, MT_FMBW,"FM动态带宽"},
		{MID_DEEM, MT_DEEM,"FM去加重"},
		{MID_AGC, MT_AGC,"AGC阈值"},
		{MID_NB, MT_NB,"噪音消除"},
		{MID_RET, MT_RET,"返回"}};

struct M_ITEM M_Audio[] =
	{
		{MID_TONE, MT_TONE, "音效设置"},
		{MID_BAL, MT_BAL,"高中低平衡"},
		{MID_RET, MT_RET,"返回"}};

struct M_ITEM M_Appli[] =
	{
		{MID_BKLT, MT_BKLT, "背光"},
		{MID_TSCN, MT_TSCN, "扫描模式等待时间"},
		{MID_TANY, MT_TANY, "任意模式等待时间"},
		{MID_TIME, MT_TIME, "设置时间"},
		{MID_RET, MT_RET,"返回"}};

// Menu Option->LSIG
const char MT_LSIGNORM[] = "NORM";
const char MT_LSIGLOW[] = "LOW";

struct M_ITEM M_LSIG[] =
	{
		{MID_LSIGNORM, MT_LSIGNORM, "正常"},
		{MID_LSIGLOW, MT_LSIGLOW,"低"}};

// Menu Option->FIRM
const char MT_FIRM1[] = "EMBE";
const char MT_FIRM2[] = "R7.1";
const char MT_FIRM3[] = "R8.0";

struct M_ITEM M_FIRM[] =
	{
		{MID_FIRM1, MT_FIRM1, "内置固件"},
		{MID_FIRM2, MT_FIRM2, "7.1固件"},
		{MID_FIRM3, MT_FIRM3, "8.0固件"}};

// Menu Option->FMAT
const char MT_FMAT1[] = "ANT1";
const char MT_FMAT2[] = "ANT2";
const char MT_FMAT3[] = "DUAL";

struct M_ITEM M_FMAT[] =
	{
		{MID_FMAT1, MT_FMAT1,"天线1"},
		{MID_FMAT2, MT_FMAT2,"天线2"},
		{MID_FMAT3, MT_FMAT3,"双天线"}};

// Menu Option->FMSI
const char MT_FMSIOFF[] = "OFF";
const char MT_FMSION[] = "ON";

struct M_ITEM M_FMSI[] =
	{
		{MID_FMSIOFF, MT_FMSIOFF, "停用"},
		{MID_FMSION, MT_FMSION,"启用"}
	};

// Menu Option->FMCE
const char MT_FMCEOFF[] = "OFF";
const char MT_FMCEON[] = "ON";

struct M_ITEM M_FMCE[] =
	{
		{MID_FMCEOFF, MT_FMCEOFF, "停用"},
		{MID_FMCEON, MT_FMCEON,"启用"}};

// Menu Option->FMMP
const char MT_FMMPOFF[] = "OFF ";
const char MT_FMMPON[] = "ON";

struct M_ITEM M_FMMP[] =
	{
		{MID_FMMPOFF, MT_FMMPOFF, "停用"},
		{MID_FMMPON, MT_FMMPON,"启用"}};

// Menu Option->FMNS
const char MT_FMNSOFF[] = "OFF";
const char MT_FMNSON[] = "ON";

struct M_ITEM M_FMNS[] =
	{
		{MID_FMNSOFF, MT_FMNSOFF, "停用"},
		{MID_FMNSON, MT_FMNSON,"启用"}};

// Menu Option->INCA
const char MT_INCAOFF[] = "OFF";
const char MT_INCAON[] = "ON";

struct M_ITEM M_INCA[] =
	{
		{MID_INCAOFF, MT_INCAOFF, "停用"},
		{MID_INCAON, MT_INCAON,"启用"}};

// Menu Option->DEEM
const char MT_DEEM0[] = "OFF";
const char MT_DEEM50[] = "50US";
const char MT_DEEM75[] = "75US";

struct M_ITEM M_Deem[] =
	{
		{MID_DEEM0, MT_DEEM0, "停用"},
		{MID_DEEM50, MT_DEEM50, "五十us"},
		{MID_DEEM75, MT_DEEM75, "七十五us"}};

// Menu Option->BKLT
const char MT_BKKEEP[] = "KEEP";
const char MT_BKADJ[] = "ADJ";

struct M_ITEM M_BkLt[] =
	{
		{MID_BKKEEP, MT_BKKEEP,"背光持续时间"},
		{MID_BKADJ, MT_BKADJ,"背光亮度"},
		{MID_RET, MT_RET, "返回"}};

// Menu Frequency(Right encoder)
const char MT_MODE[] = "Work Mode";
const char MT_STAT[] = "Signal Quality";
const char MT_TUNE[] = "Tune Type";
const char MT_BAND[] = "Band Select";
const char MT_FILT[] = "IF Filter Select";
const char MT_SINE[] = "Sinewave Gen";
const char MT_HELP[] = "Usage & Help";

struct M_ITEM M_Frequency[] =
	{
		{MID_MODE, MT_MODE,"工作模式"},
		{MID_STAT, MT_STAT,"信号质量"},
		{MID_TUNE, MT_TUNE,"调谐方式"},
		{MID_BAND, MT_BAND,"波段选择"},
		{MID_FILT, MT_FILT,"射频带宽选择"},
		{MID_SINE, MT_SINE,"正弦波发生器"},
		{MID_HELP, MT_HELP,"使用帮助"},
		{MID_EXIT, MT_EXIT,"退出"}};

// Menu Frequency->MODE
const char MT_MODERF[] = "RF";
const char MT_MODEAUX[] = "AUX";

struct M_ITEM M_Mode[] =
	{
		{MID_MODERF, MT_MODERF,"射频"},
		{MID_MODEAUX, MT_MODEAUX,"混合音频输入"},
		{MID_RET, MT_RET,"返回"}};

// Menu Frequency->TUNE
const char MT_FREQ[] = "FREQ";
const char MT_SEEK[] = "SEEK";
const char MT_CH[] = "CH";
const char MT_SCAN[] = "SCAN";
const char MT_ANY[] = "ANY";
const char MT_SCSV[] = "S&S";

struct M_ITEM M_Tune[] =
	{
		{MID_FREQ, MT_FREQ,"自由调谐"},
		{MID_SEEK, MT_SEEK,"上下搜台"},
		{MID_CH, MT_CH,"频道模式"},
		{MID_SCAN, MT_SCAN,"连续搜台"},
		{MID_ANY, MT_ANY,"任意模式"},
		{MID_SCSV, MT_SCSV,"扫描并存台"},
		{MID_RET, MT_RET,"返回"}};

// Menu Frequency->BAND
const char MT_LW[] = "LW";
const char MT_MW[] = "MW";
const char MT_SW[] = "SW";
const char MT_FL[] = "FM-L";
const char MT_FM[] = "FM";

struct M_ITEM M_Band[] =
	{
		{MID_LW, MT_LW,"长波"},
		{MID_MW, MT_MW,"中波"},
		{MID_SW, MT_SW,"短波"},
		{MID_FL, MT_FL,"低频段FM"},
		{MID_FM, MT_FM,"调频"},
		{MID_RET, MT_RET}};

// Menu Frequency->FILT
const char MT_FTFM00[] = "AUTO";
const char MT_FTFM01[] = "72K ";
const char MT_FTFM02[] = "89K ";
const char MT_FTFM03[] = "107K";
const char MT_FTFM04[] = "124K";
const char MT_FTFM05[] = "141K";
const char MT_FTFM06[] = "159K";
const char MT_FTFM07[] = "176K";
const char MT_FTFM08[] = "193K";
const char MT_FTFM09[] = "211K";
const char MT_FTFM10[] = "228K";
const char MT_FTFM11[] = "245K";
const char MT_FTFM12[] = "262K";
const char MT_FTFM13[] = "280K";
const char MT_FTFM14[] = "297K";
const char MT_FTFM15[] = "314K";

const char MT_FTAM00[] = "AUTO";
const char MT_FTAM01[] = "1K8 ";
const char MT_FTAM02[] = "2K2 ";
const char MT_FTAM03[] = "2K6 ";
const char MT_FTAM04[] = "3K0 ";
const char MT_FTAM05[] = "3K4 ";
const char MT_FTAM06[] = "4K0 ";
const char MT_FTAM07[] = "4K6 ";
const char MT_FTAM08[] = "5K2 ";
const char MT_FTAM09[] = "6K0 ";
const char MT_FTAM10[] = "7K0 ";
const char MT_FTAM11[] = "8K0 ";
const char MT_FTAM12[] = "9K2 ";
const char MT_FTAM13[] = "10K4";
const char MT_FTAM14[] = "12K0";
const char MT_FTAM15[] = "13K6";

struct M_ITEM M_FMFilter[] =
	{
		{MID_FTFM00, MT_FTFM00,"自动"},
		{MID_FTFM01, MT_FTFM01,MT_FTFM01},
		{MID_FTFM02, MT_FTFM02,MT_FTFM02},
		{MID_FTFM03, MT_FTFM03,MT_FTFM03},
		{MID_FTFM04, MT_FTFM04,MT_FTFM04},
		{MID_FTFM05, MT_FTFM05,MT_FTFM05},
		{MID_FTFM06, MT_FTFM06,MT_FTFM06},
		{MID_FTFM07, MT_FTFM07,MT_FTFM07},
		{MID_FTFM08, MT_FTFM08, MT_FTFM08},
		{MID_FTFM09, MT_FTFM09, MT_FTFM09},
		{MID_FTFM10, MT_FTFM10, MT_FTFM10},
		{MID_FTFM11, MT_FTFM11, MT_FTFM11},
		{MID_FTFM12, MT_FTFM12, MT_FTFM12},
		{MID_FTFM13, MT_FTFM13, MT_FTFM13},
		{MID_FTFM14, MT_FTFM14, MT_FTFM14},
		{MID_FTFM15, MT_FTFM15, MT_FTFM15}};

struct M_ITEM M_AMFilter[] =
	{
		{MID_FTAM00, MT_FTAM00,"自动"},
		{MID_FTAM01, MT_FTAM01,MT_FTAM01},
		{MID_FTAM02, MT_FTAM02,MT_FTAM02},
		{MID_FTAM03, MT_FTAM03,MT_FTAM03},
		{MID_FTAM04, MT_FTAM04,MT_FTAM04},
		{MID_FTAM05, MT_FTAM05,MT_FTAM05},
		{MID_FTAM06, MT_FTAM06,MT_FTAM06},
		{MID_FTAM07, MT_FTAM07,MT_FTAM07},
		{MID_FTAM08, MT_FTAM08,MT_FTAM08},
		{MID_FTAM09, MT_FTAM09,MT_FTAM09},
		{MID_FTAM10, MT_FTAM10,MT_FTAM10},
		{MID_FTAM11, MT_FTAM11,MT_FTAM11},
		{MID_FTAM12, MT_FTAM12,MT_FTAM12},
		{MID_FTAM13, MT_FTAM13,MT_FTAM13},
		{MID_FTAM14, MT_FTAM14,MT_FTAM14},
		{MID_FTAM15, MT_FTAM15,MT_FTAM15}};

struct M_SUBMENU SM_List[] =
	{
		{MID_OPTION, M_Option, sizeof(M_Option) / sizeof(struct M_ITEM)},		   // Menu Option
		{MID_FREQUENCY, M_Frequency, sizeof(M_Frequency) / sizeof(struct M_ITEM)}, // Menu Frequency
		{MID_BKLT, M_BkLt, sizeof(M_BkLt) / sizeof(struct M_ITEM)},				   // Menu Option->BKLT
		{MID_RADI, M_Radio, sizeof(M_Radio) / sizeof(struct M_ITEM)},			   // Menu Option->RADI
		{MID_AUDI, M_Audio, sizeof(M_Audio) / sizeof(struct M_ITEM)},			   // Menu Option->AUDI
		{MID_APPL, M_Appli, sizeof(M_Appli) / sizeof(struct M_ITEM)},			   // Menu Option->APPL
		{MID_LSIG, M_LSIG, sizeof(M_LSIG) / sizeof(struct M_ITEM)},				   // Menu Option->LSIG
		{MID_FIRM, M_FIRM, sizeof(M_FIRM) / sizeof(struct M_ITEM)},				   // Menu Option->FIRM
		{MID_FMAT, M_FMAT, sizeof(M_FMAT) / sizeof(struct M_ITEM)},				   // Menu Option->FMAT
		{MID_FMSI, M_FMSI, sizeof(M_FMSI) / sizeof(struct M_ITEM)},				   // Menu Option->FMSI
		{MID_FMCE, M_FMCE, sizeof(M_FMCE) / sizeof(struct M_ITEM)},				   // Menu Option->FMCE
		{MID_FMMP, M_FMMP, sizeof(M_FMMP) / sizeof(struct M_ITEM)},				   // Menu Option->FMMP
		{MID_FMNS, M_FMNS, sizeof(M_FMNS) / sizeof(struct M_ITEM)},				   // Menu Option->FMNS
		{MID_INCA, M_INCA, sizeof(M_INCA) / sizeof(struct M_ITEM)},				   // Menu Option->INCA
		{MID_DEEM, M_Deem, sizeof(M_Deem) / sizeof(struct M_ITEM)},				   // Menu Option->DEEM
		{MID_MODE, M_Mode, sizeof(M_Mode) / sizeof(struct M_ITEM)},				   // Menu Frequency->MODE
		{MID_TUNE, M_Tune, sizeof(M_Tune) / sizeof(struct M_ITEM)},				   // Menu Frequency->TUNE
		{MID_BAND, M_Band, sizeof(M_Band) / sizeof(struct M_ITEM)},				   // Menu Frequency->BAND
		{MID_FILT, M_FMFilter, sizeof(M_FMFilter) / sizeof(struct M_ITEM)}		   // Menu Frequency->FILT, toggle AM/FM by nRFMode
};

////////////////////////////////////////////////////////////
// LCD utility

void Delay(uint16_t time)
{
	while (time--)
	{
		for (int i = 0; i < 10; i++)
			;
	}
}

void IR_Check()
{
	if (IR_Flag == 1)
	{
		// printf("%08x\r\n",IRCode);
		OLED_Display_On();
		bLCDOff = false;
		nBacklightTimer = HAL_GetTick();
		switch (IRCode)
		{
		case 0x00FFe01f: // tune -
			--nRRot;
			break;
		case 0x00FF906f: // tune +
			++nRRot;
			break;
		case 0x00FF9867: // volume -
			nVolume--;
			AddSyncBits(NEEDSYNC_VOL);
			CheckUpdateAlt(ALT_VOL); // Show volume for a period
			printf("音量%d\n", nVolume);
	fflush(stdout);
			break;
		case 0x00FF02FD: // volume +
			nVolume++;
			AddSyncBits(NEEDSYNC_VOL);
			CheckUpdateAlt(ALT_VOL); // Show volume for a period
			printf("音量%d\n", nVolume);
	fflush(stdout);
			break;
		case 0x00FFa25d: // TUNE
			irKey = KEY_TUNE;
			break;
		case 0x00FFe21d: // STEP
			irKey = KEY_STEP;
			break;
		case 0x00FF22dd: // band
			irKey = KEY_BAND;
			break;
		case 0x00FFc23d: // filter
			irKey = KEY_FILTER;
			break;
		default:
			processRemoteInput(IRCode);
			break;
		}
		IR_Flag = 0;
	}
}
char* getTurnName(uint8_t turn_type) {
	switch(turn_type) {
		case TYPE_FREQ:
			return "调谐";
		case TYPE_SEEK:
			return "定位";
		case TYPE_CH:
			return "频道";
		case TYPE_SCAN:
			return "搜索";
		case TYPE_ANY:
			return "任意";
		case TYPE_SCSV:
			return "搜存";
		default:
			return "未知模式";
	}
	
}
char* getBandName(uint8_t band_type) {
	switch(band_type) {
		case BAND_LW:
			return "长波";
		case BAND_MW:
			return "中波";
		case BAND_SW:
			return "短波";
		case BAND_FM:
			return "调频";
		case BAND_FL:
			return "FL";
		default:
			return "未知频率";
	}
	
}
// 函数：切换FM频道
void switchFMChannel(float channel)
{
	// 在这里添加切换FM频道的代码

	int fmChannel = channel * 1000;
	printf("%s %.1f\n", getBandName(BAND_FM), channel);
	//printf("切换到FM频道 %.1f\n", channel);
	fflush(stdout);
	nBand = BAND_FM;
	nBandFreq[nBand] = fmChannel;

	TuneFreqDisp();

	ProcBand(BAND_FM);
	LCDUpdate();
	// currentFMChannel = channel;
}

// 函数：切换AM频道
void switchAMChannel(int channel, uint8_t band_type)
{
	// 在这里添加切换AM频道的代码
	printf("%s %dK\n", getBandName(band_type), channel);
	fflush(stdout);
	nBand = band_type;
	nBandFreq[nBand] = channel;

	TuneFreqDisp();

	ProcBand(band_type);
	LCDUpdate();
	// currentAMChannel = channel;
}

// 函数：处理遥控器输入
void processRemoteInput(uint32_t irCode)
{
	// 定义红外码和频道映射关系

	// printf("\xCA\xD5\xB5\xBD\xBA\xEC\xCD\xE2\xC2\xEB：%08X\n", irCode);
	//  fflush(stdout);
	uint32_t irCodeMap[] = {
		0x00FF6897, // 映射为 "0"
		0x00FF30cf, // 映射为 "1"
		0x00FF18e7, // 映射为 "2"
		0x00FF7a85, // 映射为 "3"
		0x00FF10ef, // 映射为 "4"
		0x00FF38c7, // 映射为 "5"
		0x00FF5aa5, // 映射为 "6"
		0x00FF42bd, // 映射为 "7"
		0x00FF4ab5, // 映射为 "8"
		0x00FF52ad, // 映射为 "9"
		0x00FFb04f, // 映射为 "."
		0x00FFa857,	// 映射为 "#"
		0x00FF629d, //reset
	};
	char channelMap[][10] = {
		"0", // 对应频道 "0"
		"1", // 对应频道 "1"
		"2", // 对应频道 "2"
		"3", // 对应频道 "3"
		"4", // 对应频道 "4"
		"5", // 对应频道 "5"
		"6", // 对应频道 "6"
		"7", // 对应频道 "7"
		"8", // 对应频道 "8"
		"9", // 对应频道 "9"
		".", // 对应频道 "."
		"#",	 // 对应频道 "#"
		"*"
	};
	int mapSize = sizeof(irCodeMap) / sizeof(irCodeMap[0]);
	static char input[10] = "";
	static int inputIndex = 0;
	// 查找红外码对应的频道映射值
	char *channel = NULL;
	int lasti = 0;
	for (int i = 0; i < mapSize; i++)
	{
		if (irCode == irCodeMap[i])
		{
			lasti = i;
			channel = channelMap[i];
			break;
		}
	}
	if (channel != NULL)
	{
		// 将映射值存储到字符串数组
		strcat(input, channel);
		if (strcmp(channel, "*") == 0) {
			printf("<Z>0");
		}
		else if (strcmp(channel, ".") == 0) {
			printf("点");
		}
		else {
			printf("%s\n", channel);
			//fflush(stdout);
			//printf("<Z>%d", lasti % 7);
		}
  		fflush(stdout);
		if (strcmp(channel, "#") == 0)
		{
			// 检测到 # 键，执行跳转命令
			input[inputIndex] = '\0'; // 去除最后的#
			if (inputIndex > 0)
			{
				// 根据前面的数字执行跳转
				if(strcmp(input, "**********") == 0) {
					//reset all params
					NVMInitStation();
					NVMInitSetting();
					NVIC_SystemReset();
				}
				else {
					int channel = atoi(input);
					if (channel >= 88 && channel <= 108)
					{
						float fchanel = atof(input);
						switchFMChannel(fchanel);
					}
					else if (channel >= 153 && channel <= 279)
					{
						switchAMChannel(channel, BAND_LW);
					}
					else if (channel >= 520 && channel <= 1710)
					{
						switchAMChannel(channel, BAND_MW);
					}
					else if (channel >= 2300 && channel <= 26100)
					{
						switchAMChannel(channel, BAND_SW);
					}
					else
					{
						printf("输入无效：%s\n", input);
					}
				}
			}
			else
			{
				printf("输入无效：%s\n", input);
			}
			// 清空输入
			memset(input, 0, sizeof(input));
			inputIndex = 0;
		}
		else
		{
			inputIndex++;
		}
	}
	else
	{
		//printf("未知红外码：0x%08X\n", irCode);
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == SH1A_Pin)
	{
		if (HAL_GPIO_ReadPin(SH1A_GPIO_Port, SH1A_Pin) == HAL_GPIO_ReadPin(SH1B_GPIO_Port, SH1B_Pin))
			--nLRot;
		else
			++nLRot;
	}
	else if (GPIO_Pin == SH2A_Pin)
	{
		if (HAL_GPIO_ReadPin(SH2A_GPIO_Port, SH2A_Pin) == HAL_GPIO_ReadPin(SH2B_GPIO_Port, SH2B_Pin))
			--nRRot;
		else
			++nRRot;
	}
}
////////////////////////////////////////////////////////////
// Rotary encoder & key utility

void HAL_GPIO_EXTI_Callback_new(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == SH1A_Pin)
	{
		if (nIntSeqs[0] == 0 && HAL_GPIO_ReadPin(SH1A_GPIO_Port, SH1A_Pin) == GPIO_PIN_RESET)
		{ // ???
			nFlags[0] = 0;
			if (HAL_GPIO_ReadPin(SH1B_GPIO_Port, SH1B_Pin) == GPIO_PIN_SET)
			{
				nFlags[0] = 1;
			}
			nIntSeqs[0] = 1;
		}
		if (nIntSeqs[0] && HAL_GPIO_ReadPin(SH1A_GPIO_Port, SH1A_Pin) == GPIO_PIN_SET)
		{ // ??????ж??????A??????????
			if (HAL_GPIO_ReadPin(SH1B_GPIO_Port, SH1B_Pin) == GPIO_PIN_RESET && nFlags[0] == 1)
			{
				++nLRot;
			}
			if (HAL_GPIO_ReadPin(SH1B_GPIO_Port, SH1B_Pin) == GPIO_PIN_SET && nFlags[0] == 0)
			{
				--nLRot;
			}
			nIntSeqs[0] = 0;
		}
	}
	else if (GPIO_Pin == SH2A_Pin)
	{
		if (nIntSeqs[1] == 0 && HAL_GPIO_ReadPin(SH2A_GPIO_Port, SH2A_Pin) == GPIO_PIN_RESET)
		{ // ???
			nFlags[1] = 0;
			if (HAL_GPIO_ReadPin(SH2B_GPIO_Port, SH2B_Pin) == GPIO_PIN_SET)
			{
				nFlags[1] = 1; // ???
			}
			nIntSeqs[1] = 1;
		}
		if (nIntSeqs[1] && HAL_GPIO_ReadPin(SH2A_GPIO_Port, SH2A_Pin) == GPIO_PIN_SET)
		{ // ??????ж??????A??????????
			if (HAL_GPIO_ReadPin(SH2B_GPIO_Port, SH2B_Pin) == GPIO_PIN_RESET && nFlags[1] == 1)
			{
				++nRRot;
			}
			if (HAL_GPIO_ReadPin(SH2B_GPIO_Port, SH2B_Pin) == GPIO_PIN_SET && nFlags[1] == 0)
			{
				--nRRot;
			}
			nIntSeqs[1] = 0;
		}
	}
}

int8_t GetLRot(void)
{
	int8_t n;

	if (nLRot)
	{
		HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
		n = nLRot;
		nLRot = 0;
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
		OLED_Display_On();
		bLCDOff = false;
		nBacklightTimer = HAL_GetTick();
		return n;
	}
	return 0;
}

int8_t GetRRot(void)
{
	int8_t n;

	if (nRRot)
	{
		HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
		n = nRRot;
		nRRot = 0;
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
		OLED_Display_On();
		bLCDOff = false;
		nBacklightTimer = HAL_GetTick();
		return n;
	}
	return 0;
}

uint8_t PeekKey(void)
{
	int8_t i;
	uint8_t nKey0 = 0xff, nKey;

	for (i = 0; i < 30;)
	{

		if (HAL_GPIO_ReadPin(KS0_GPIO_Port, KS0_Pin) == GPIO_PIN_RESET)
			nKey = KEY_LROT;
		else if (HAL_GPIO_ReadPin(KS1_GPIO_Port, KS1_Pin) == GPIO_PIN_RESET)
			nKey = KEY_RROT;
		else if (HAL_GPIO_ReadPin(KS2_GPIO_Port, KS2_Pin) == GPIO_PIN_RESET)
			nKey = KEY_TUNE;
		else if (HAL_GPIO_ReadPin(KS3_GPIO_Port, KS3_Pin) == GPIO_PIN_RESET)
			nKey = KEY_STEP;
		else if (HAL_GPIO_ReadPin(KS4_GPIO_Port, KS4_Pin) == GPIO_PIN_RESET)
			nKey = KEY_BAND;
		else if (HAL_GPIO_ReadPin(KS5_GPIO_Port, KS5_Pin) == GPIO_PIN_RESET)
			nKey = KEY_FILTER;
		else
			nKey = 0;
		if (nKey == nKey0)
			++i;
		else
		{
			i = 0;
			nKey0 = nKey;
		}
	}
	return nKey;
}

uint8_t GetKey(void)
{
	uint8_t nKey0, nKey;
	uint32_t timer1; // Long press timer
	static uint8_t nKeyWaitUp = 0;
	static uint32_t tKeyWaitUp;
	IR_Check();
	// ir key
	if (irKey > 0)
	{
		uint8_t tmpKey = irKey;
		irKey = 0;
		return tmpKey;
	}
	if (nKeyWaitUp && (HAL_GetTick() - tKeyWaitUp) < TIMER_LAST_LP)
		while ((nKey0 = PeekKey()) == nKeyWaitUp)
			;
	else
		nKey0 = PeekKey();

	nKeyWaitUp = 0;
	if (!nKey0)
		return 0;

	timer1 = HAL_GetTick();
	while ((HAL_GetTick() - timer1) < TIMER_LONGPRESS)
	{
		if ((nKey = PeekKey()) == 0)
		{
			OLED_Display_On();
			bLCDOff = false;
			nBacklightTimer = HAL_GetTick();
			return nKey0; // Key up
		}

		if (nKey != nKey0)
		{ // Another key pressed
			nKey0 = nKey;
			timer1 = HAL_GetTick(); // Reset long press timer for new key
			continue;
		}
	}

	tKeyWaitUp = HAL_GetTick();
	nKeyWaitUp = nKey0;
	OLED_Display_On();
	bLCDOff = false;
	nBacklightTimer = HAL_GetTick();
	return nKey0 | KEY_LONGPRESS;
} // uint8_t GetKey(void)

void TestRotKey(void) // Show Rotary encoder and key data on LCD
{
	uint8_t uLRot = 0;
	int8_t iRRot = 0, i, iTmp;

	OLED_Clear1();
	OLED_Clear2();
	OLED_XYStr(6, 0, "TEST");
	nLRot = 0;
	nRRot = 0;

	for (i = 0;; i++)
	{
		iTmp = GetLRot();
		if (iTmp && PeekKey() == KEY_RROT)
		{
			// Right rotary encoder pressed while rotate left rotary encoder
			if (iTmp < 0)
			{ // Left encoder: CCW
				if (uLRot == 67 && iRRot == 77)
					NVMInitStation();
				if (uLRot == 67 && iRRot == 97)
					NVMInitSetting();
				else if (uLRot == 66)
				{
					if (iRRot == 35 || iRRot == 38)
					{
						bDisp_USN = 1; // Display FM USN reg instead of SNR
						OLED_XYStr(0, 2, "DISPLAY USN REG ");
						HAL_Delay(1000);
						GetKey();
					}
				}
			}
			return;
		}
		uLRot += iTmp;
		OLED_XYIntLen(0, 1, uLRot, 4);

		iRRot += GetRRot();
		OLED_XYIntLen(12, 1, iRRot, 4);
	}
}

////////////////////////////////////////////////////////////
// User interface
////////////////////////////////////////////////////////////
void LCDUpdate(void)
{
	int8_t i8;
	const char *p;

	// Update band: LW/MW/SW/FL/FM, FL=FM Low
	OLED_XYStr(BAND_X, BAND_Y, (char *)pszBands[nBand]);

	// Update tune type: FS/SK/CH/SC/AN/SS
	OLED_XYStr(TYPE_X, TYPE_Y, (char *)pszTuneTypes[nTuneType]);

	if ((nTuneType == TYPE_CH) || (nTuneType == TYPE_SCSV)) // CH/SS type
		OLED_XYUIntLenZP(STEP_X, STEP_Y, nBandCh[nBand], 3);
	else
	{ // None ch/ss type
	  // Update step: 1K/5K/9K/10K/25K/45K/50K/90K/100/500
		if (nBandStep[nBand][nStepIdx] < 100)
			i8 = 1;
		else
			i8 = 0;
		OLED_XYIntLen(STEP_X, STEP_Y, nBandStep[nBand][nStepIdx], 3 - i8);
		if (i8)
			OLED_XYChar(STEP_X + 2, STEP_Y, 'K');
		sprintf(szStep,"%d%s",nBandStep[nBand][nStepIdx], (i8? "K": ""));
	}

	// Update IF filter bandwidth
	if (nRFMode == RFMODE_FM)
		p = M_FMFilter[nFMFilter].pszMTxt;
	else
		p = M_AMFilter[nAMFilter].pszMTxt;
	OLED_XYStrLen(FILTER_X, FILTER_Y, p, 4, false);
	OLED_Refresh();
	pszFilter = p;
}

void CheckUpdateSig(void)
{
	char c;
	float fv;
	if (nRFMode == RFMODE_FM && nFMAT == 2)
	{
		dsp_start_subaddr(0x00);
		I2C_Restart(DSP_I2C | I2C_READ);
		bSTIN = (I2C_ReadByte(true) >> 3) & 1;
		I2C_Stop();

		uint8_t addr;
		if (nSNRAnt == 0)
		{
			nSNRAnt = 1;
			addr = 0x74;
		}
		else
		{
			nSNRAnt = 0;
			addr = 0x75;
		}

		dsp_start_subaddr(addr);
		I2C_Restart(DSP_I2C | I2C_READ);
		nRSSI = (int8_t)(I2C_ReadByte(false) >> 1) - 8;
		I2C_ReadByte(false);
		REG_USN = I2C_ReadByte(true);
		I2C_Stop();

		if (nRFMode == RFMODE_FM)														 // In FM mode, SNR is NOT calibrated accurately. For reference only
			nSNR = (int)(0.46222375 * (float)nRSSI - 0.082495118 * (float)REG_USN) + 10; // Emprical formula
		else																			 // AM
			nSNR = -((int8_t)REG_USN);
	}
	else
		GetRFStatReg();

	// FM stereo indicator. 'S' for FM stereo, ' ' for FM mono or AM mode,  'M' for FM forced mono
	c = ' '; // AM mode or FM mono
	if (nRFMode == RFMODE_FM)
	{
		if (!nStereo)
			c = 'M'; // FM forced mono
		else if (bSTIN)
			c = 'S'; // Stereo
	}

	if (nDelayedSmooth--)
	{
		nRSSI_Disp = nRSSI;
		nSNR_Disp = nSNR;
	}
	else
	{
		nDelayedSmooth = 0;
		fv = (float)nRSSI_Last * 0.7 + (float)nRSSI * 0.3;
		if (fv > 0.0)
			nRSSI_Disp = (int)(fv + 0.5);
		else
			nRSSI_Disp = (int)(fv - 0.5);

		fv = (float)nSNR_Last * 0.7 + (float)nSNR * 0.3;
		if (fv > 0.0)
			nSNR_Disp = (int)(fv + 0.5);
		else
			nSNR_Disp = (int)(fv - 0.5);
	}

	OLED_XYIntLen(RSSI_X, RSSI_Y, constrain(nRSSI_Disp, -9, 99), 2); // Update RF signal level in dBuv
	OLED_XYChar(STEREO_X, STEREO_Y, c);								 // Update FM stereo indicator
	if (bDisp_USN && nRFMode == RFMODE_FM)
		OLED_XYIntLen(SN_X, SN_Y, REG_USN, 3); // Update FM USN reg
	else
		OLED_XYIntLen(SN_X, SN_Y, constrain(nSNR_Disp, -99, 127), 3); // Update S/N ratio in dB

	nRSSI_Last = nRSSI_Disp;
	nSNR_Last = nSNR_Disp;
}

void ShowMisc(void)
{
	char s[] = "3e- A";
	const char cmode[] = "FAXX";

	s[0] = '0' + nAGC; // AGC threshold, 0 for lowest, 3 for highest

	if (nRFMode == RFMODE_FM)
	{
		if (nFMCEQ)
			s[1] = 'E';			   // FM channel equalizer on
		s[2] = '0' + nFMAT;		   // FM phase diversity on
		s[3] = '0' + nFMDynamicBW; // FM dynamic bandwidth, 0 to 3 = narrow bandwidth to wide bandwidth
	}

	// Update mode: F/A/X, F=FM, A=AM, X=AUX
	s[4] = cmode[(nMode << 1) + nRFMode];

	OLED_XYStr(ALT_X, ALT_Y, s);
}

void ShowTime(void)
{
	uint32_t nMinutes;

	nMinutes = (HAL_GetTick() / 1000 + nSecondsOffset) / 60;
	OLED_XYUIntLenZP(ALT_X, ALT_Y, (nMinutes / 60) % 24, 2); // Hour
	OLED_XYChar(ALT_X + 2, ALT_Y, ':');
	OLED_XYUIntLenZP(ALT_X + 3, ALT_Y, nMinutes % 60, 2); // Minute
}

void ShowVol(void)
{
	OLED_XYStr(ALT_X, ALT_Y, "VOL");
	OLED_XYIntLen(ALT_X + 3, ALT_Y, nVolume, 2); // Volume
}

void CheckUpdateAlt(int8_t nShow) // Check and update ALT area
{
	static int8_t nShowLast = ALT_MISC;
	static uint32_t nTimerAlt = 0;

	if (nShow != ALT_AUTO)
	{
		nTimerAlt = HAL_GetTick();
		nShowLast = nShow;
	}

	if ((HAL_GetTick() - nTimerAlt) < TIMER_ALTDISP)
	{ // Not timeout
		switch (nShowLast)
		{
		case ALT_MISC:
			ShowMisc();
			break;

		case ALT_TIME:
			ShowTime();
			break;

		case ALT_VOL:
			ShowVol();
			break;
		}
	}
	else
	{ // Timeout, swap beteen ALT_MISC/ALT_TIME
		if (nShowLast != ALT_MISC)
		{
			nShowLast = ALT_MISC;
			ShowMisc();
		}
		else
		{
			nShowLast = ALT_TIME;
			ShowTime();
		}
		nTimerAlt = HAL_GetTick();
	}
} // void CheckUpdateAlt(int8_t nShow)  // Check and update ALT area
void tipsForLRorAnykey() {
	printf("旋转，按压编码器修改，任意键返回\n");
}
void Menu_Squelch(uint8_t nIdx)
{
	int16_t i16 = nSquelch[nIdx];
	uint8_t nKey, lp;

	// 0123456789012345
	OLED_XYStr(0, 2, "SQUELCH1:       ");
	if (nIdx)
		OLED_XYChar(7, 2, '2');
	tipsForLRorAnykey();
	for (lp = 0;; lp++)
	{
		int16_t lrValue = GetLRot() + GetRRot();
		
		i16 += lrValue;
		i16 = constrain(i16, -99, 99);

		if(lrValue != false) {
			printf("%d", i16);
		}
		OLED_XYIntLen(10, 2, i16, 3);

		if ((nKey = GetKey()) != false)
		{
			nSquelch[nIdx] = (int8_t)i16;
			OLED_Clear2();
			if (!(nKey & (KEY_LROT | KEY_RROT)))
				bExitMenu = true;
			NV_write_byte(NVMADDR_SQU1 + nIdx, nSquelch[nIdx]);
			return;
		}

		if (!(lp % 16))
			CheckUpdateSig();
		HAL_Delay(64);
		OLED_Refresh();
	}
} // void Menu_Squelch(uint8_t nIdx)

void Menu_FMDynamicBW(void)
{
	int8_t i8;
	uint8_t nKey, lp;

	// 0123456789012345
	OLED_XYStr(0, 2, ("FM DYNAMIC BW:  "));
	OLED_XYIntLen(15, 2, nFMDynamicBW, 1);
	printf("动态带宽%d", nFMDynamicBW);
	for (lp = 0;; lp++)
	{
		if ((i8 = GetLRot() + GetRRot()) != false)
		{
			;
			i8 += nFMDynamicBW;
			nFMDynamicBW = constrain(i8, 0, 3);
			printf("%d", nFMDynamicBW);
			OLED_XYIntLen(15, 2, nFMDynamicBW, 1);
			SetRFCtrlReg();
		}

		if ((nKey = GetKey()) != false)
		{
			OLED_Clear2();
			if (!(nKey & (KEY_LROT | KEY_RROT)))
				bExitMenu = true;
			AddSyncBits(NEEDSYNC_MISC1);
			return;
		}

		if (!(lp % 16))
			CheckUpdateSig();
		HAL_Delay(64);
		OLED_Refresh();
	}
} // void Menu_FMDynamicBW(void)

void Menu_AGC(void)
{
	int8_t i8;
	uint8_t nKey, lp;

	// 0123456789012345
	OLED_XYStr(0, 2, ("AGC THRESHOLD:  "));
	OLED_XYIntLen(15, 2, nAGC, 1);
	printf("自动增益阈值%d", nAGC);
	for (lp = 0;; lp++)
	{
		if ((i8 = GetLRot() + GetRRot()) != false)
		{
			i8 += nAGC;
			nAGC = constrain(i8, 0, 3);
			printf("%d", nAGC);
			OLED_XYIntLen(15, 2, nAGC, 1);
			SetRFCtrlReg();
		}

		if ((nKey = GetKey()) != false)
		{
			OLED_Clear2();
			if (!(nKey & (KEY_LROT | KEY_RROT)))
				bExitMenu = true;
			AddSyncBits(NEEDSYNC_MISC1);
			return;
		}

		if (!(lp % 16))
			CheckUpdateSig();
		HAL_Delay(64);
		OLED_Refresh();
	}
} // void Menu_AGC(void)

void Menu_Stereo(void)
{
	int8_t i8;
	uint8_t nKey, lp;

	// 0123456789012345
	OLED_XYStr(0, 2, ("FM STEREO:     "));
	OLED_XYIntLen(15, 2, nStereo, 1);
	printf("立体声%d", nStereo);
	for (lp = 0;; lp++)
	{
		int lrValue = GetLRot();
		int rrValue = GetRRot();
		if ((i8 = lrValue + rrValue) != false)
		{
			i8 += nStereo;
			nStereo = constrain(i8, 0, 9);
			printf("%d", nStereo);
			OLED_XYIntLen(15, 2, nStereo, 1);
			SetRFCtrlReg();
		}

		if ((nKey = GetKey()) != false)
		{
			OLED_Clear2();
			if (!(nKey & (KEY_LROT | KEY_RROT)))
				bExitMenu = true;
			AddSyncBits(NEEDSYNC_MISC3);
			return;
		}

		if (!(lp % 16))
			CheckUpdateSig();
		HAL_Delay(64);
		OLED_Refresh();
	}
} // void Menu_Stereo(void)

void Menu_NoiseBlanker(void)
{
	int8_t i8;
	uint8_t nKey, lp;

	// 0123456789012345
	OLED_XYStr(0, 2, ("NOISE BLANKER:  "));
	OLED_XYIntLen(15, 2, nNBSens, 1);
	printf("降噪%d", nNBSens);
	for (lp = 0;; lp++)
	{
		if ((i8 = GetLRot() + GetRRot()) != false)
		{
			i8 += nNBSens;
			nNBSens = constrain(i8, 0, 3);
			printf("%d", nNBSens);
			OLED_XYIntLen(15, 2, nNBSens, 1);
			SetRFCtrlReg();
		}

		if ((nKey = GetKey()) != false)
		{
			OLED_Clear2();
			if (!(nKey & (KEY_LROT | KEY_RROT)))
				bExitMenu = true;
			AddSyncBits(NEEDSYNC_MISC2);
			return;
		}

		if (!(lp % 16))
			CheckUpdateSig();
		HAL_Delay(64);
		OLED_Refresh();
	}
} // void Menu_NoiseBlanker(void)

void Menu_BacklightAdj(void)
{
	int16_t i16 = nBacklightAdj;
	uint8_t nKey, lp;

	// 0123456789012345
	OLED_XYStr(0, 2, ("BACKLT ADJ:     "));

	for (lp = 0;; lp++)
	{
		i16 = (i16 + GetLRot() + GetRRot() + 256) % 256;
		OLED_XYIntLen(12, 2, i16, 3);
		OLED_SetBackLight(i16);
		if ((nKey = GetKey()) != false)
		{
			nBacklightAdj = (uint8_t)i16;
			OLED_Clear2();
			if (!(nKey & (KEY_LROT | KEY_RROT)))
				bExitMenu = true;
			NV_write_byte(NVMADDR_BKADJ, nBacklightAdj);
			return;
		}

		if (!(lp % 16))
			CheckUpdateSig();
		HAL_Delay(64);
		OLED_Refresh();
	}
} // void Menu_BacklightAdj(void)

void Menu_BacklightKeep(void)
{
	int16_t i16 = nBacklightKeep;
	uint8_t nKey, lp;

	// 0123456789012345
	OLED_XYStr(0, 2, ("BACKLT KEEP:    "));
	printf("背光保持时间%d\n",i16 );
	for (lp = 0;; lp++)
	{
		int lrValue = GetLRot();
		int rrValue = GetRRot();
		i16 = (i16 + lrValue + rrValue + 256) % 256;
		if(lrValue !=0 || rrValue !=0) {
			printf("%d",i16 );
		}
		OLED_XYIntLen(13, 2, i16, 3);
		
		if ((nKey = GetKey()) != false)
		{
			nBacklightKeep = (uint8_t)i16;
			OLED_Clear2();
			if (!(nKey & (KEY_LROT | KEY_RROT)))
				bExitMenu = true;
			NV_write_byte(NVMADDR_BKKEEP, nBacklightKeep);
			return;
		}

		if (!(lp % 16))
			CheckUpdateSig();
		HAL_Delay(64);
		OLED_Refresh();
	}
} // void Menu_BacklightKeep(void)

void Menu_ScanStayTime(void)
{
	int16_t i16 = nScanStayTime;
	uint8_t nKey, lp;

	// 0123456789012345
	OLED_XYStr(0, 2, ("TIME SCAN:     S"));
	printf("扫描到节目后停留 %d", i16 );
	for (lp = 0;; lp++)
	{
		int lrValue = GetLRot();
		int rrValue = GetRRot();
		i16 += lrValue + rrValue;
		i16 = constrain(i16, 0, 255);
		if(lrValue !=0 || rrValue !=0) {
			printf("%d",i16 );
		}
		OLED_XYIntLen(12, 2, i16, 3);
		//printf("时间%d\n",i16 );
		if ((nKey = GetKey()) != false)
		{
			nScanStayTime = (uint8_t)i16;
			OLED_Clear2();
			if (!(nKey & (KEY_LROT | KEY_RROT)))
				bExitMenu = true;
			NV_write_byte(NVMADDR_SCSTAY, nScanStayTime);
			return;
		}

		if (!(lp % 16))
			CheckUpdateSig();
		HAL_Delay(64);
		OLED_Refresh();
	}
} // void Menu_ScanStayTime(void)

void Menu_AnyHoldTime(void)
{
	int16_t i16 = nAnyHoldTime;
	uint8_t nKey, lp;

	// 0123456789012345
	OLED_XYStr(0, 2, ("TIME ANY:     S"));
	printf("任意模式停留时间%d", i16 );
	for (lp = 0;; lp++)
	{
		int lrValue = GetLRot();
		int rrValue = GetRRot();
		i16 += lrValue + rrValue;
		i16 = constrain(i16, 0, 255);
		if(lrValue !=0 || rrValue !=0) {
			printf("%d",i16 );
		}
		OLED_XYIntLen(11, 2, i16, 3);
		// printf("时间%d\n",i16 );
		if ((nKey = GetKey()) != false)
		{
			nAnyHoldTime = (uint8_t)i16;
			OLED_Clear2();
			if (!(nKey & (KEY_LROT | KEY_RROT)))
				bExitMenu = true;
			NV_write_byte(NVMADDR_ANYHOLD, nAnyHoldTime);
			return;
		}

		if (!(lp % 16))
			CheckUpdateSig();
		HAL_Delay(64);
		OLED_Refresh();
	}
} // void Menu_AnyHoldTime(void)

void Menu_Time(void)
{
	int32_t nSeconds;
	uint8_t u8_data, lp;

	// 0123456789012345
	OLED_XYStr(0, 2, ("TIME    :  :    "));

	nSeconds = HAL_GetTick() / 1000 + nSecondsOffset;
	if (nSeconds < 0)
	{
		nSeconds = 0;
	}
	printf("时间%02d时%02d分%02d秒",(nSeconds / 3600) % 24,(nSeconds / 60) % 60 ,nSeconds % 60);

	for (lp = 0;; lp++)
	{
		int lrValue = GetLRot();
		int rrValue = GetRRot();
		nSecondsOffset += (int16_t)lrValue * 3600 + (int16_t)rrValue * 60;
		nSeconds = HAL_GetTick() / 1000 + nSecondsOffset;
		if (nSeconds < 0)
		{
			nSeconds = 0;
			nSecondsOffset = 3600L * 24 * 15 - HAL_GetTick() / 1000;
		}

		OLED_XYUIntLenZP(6, 2, (nSeconds / 3600) % 24, 2); // Hours
		OLED_XYUIntLenZP(9, 2, (nSeconds / 60) % 60, 2);   // Minutes
		OLED_XYUIntLenZP(12, 2, nSeconds % 60, 2);		   // Seconds
		if(lrValue !=0 || rrValue !=0) {
			printf("%02d时%02d分%02d秒",(nSeconds / 3600) % 24,(nSeconds / 60) % 60 ,nSeconds % 60);
		}
		
		if ((u8_data = GetKey()) != false)
		{
			nSeconds = HAL_GetTick() / 1000 + nSecondsOffset;
			if (u8_data == (KEY_FILTER | KEY_LONGPRESS))
			{ // Adjust to on the minute, i.e. hh:mm:00
				u8_data = nSeconds % 60;
				if (u8_data < 30)
					nSeconds -= u8_data;
				else
					nSeconds += 60 - u8_data;
			}
			nSecondsOffset = nSeconds - HAL_GetTick() / 1000;
			OLED_Clear2();
			if (!(u8_data & (KEY_LROT | KEY_RROT)))
				bExitMenu = true;
			return;
		}

		if (!(lp % 16))
			CheckUpdateSig();
		HAL_Delay(64);
		OLED_Refresh();
	}
} // void Menu_Time(void)

void sprhex2(char *str, uint8_t v)
{
	uint8_t n;

	n = v >> 4;
	*str = ((n < 10) ? '0' : ('A' - 10)) + n;
	n = v & 0x0f;
	*(str + 1) = ((n < 10) ? '0' : ('A' - 10)) + n;
}

void Menu_Stat(void)
{
	uint8_t nKey, u8_data, lp = 0;
	int8_t i8, nItem = 0, nItemOld = -1, nItems;
	const char *p;
	char s[10];
	// 0123456789012345
	const char *str[] =
		{
			("FM US NOISE:"), // 1st item
			("FM MULTIPATH:"),
			("RF OFFSET:     K"),
			("IF FILTER:"),
			("MODULATION:    %"),
			("FIRM:"),
			("IF:"),		 // Last item
			("AM ADJACENT:") // Another 1st item for AM mode
		};
	const char *chs_str[] =
		{
			("调频噪声:"), // 1st item
			("FM多路径:"),
			("射频偏移:K"),
			("射频带宽:"),
			("调制:%"),
			("芯片固件:"),
			("中频:"),		 // Last item
			("调幅相邻:") // Another 1st item for AM mode
		};

	nItems = sizeof(str) / sizeof(const char *) - 1;
	for (;;)
	{
		if ((nKey = GetKey()) != false)
		{
			if (!(nKey & (KEY_LROT | KEY_RROT)))
				bExitMenu = true;
			OLED_Clear2();
			return;
		}

		if ((i8 = GetLRot() + GetRRot()) != false)
			nItem = (nItems + nItem + i8) % nItems;

		if (nItem != nItemOld)
		{
			if (nItem == 0 && nRFMode == RFMODE_AM)
				u8_data = nItems;
			else
				u8_data = nItem;
			OLED_Clear2();
			OLED_XYStr(0, 2, str[u8_data]);
			printf(chs_str[u8_data]);
			nItemOld = nItem;
			lp = 0;
		}
		else if (++lp % 16)
		{
			HAL_Delay(64);
			OLED_Refresh();
			continue;
		}

		CheckUpdateSig();
		int16_t iIFFreq1, iIFFreq2;
		int v;
		switch (nItem)
		{
		case 0:
			if (nRFMode == RFMODE_FM) {
				int v = dsp_query1(0x02);
				OLED_XYIntLen(13, 2, v, 3); // FM ultrasonic noise detector
				if((HAL_GetTick()/1000) % 2 == 0) {
					printf("%d", v);
				}
			}
			else
				OLED_XYIntLen(12, 2, -((int8_t)dsp_query1(0x02)), 4); // AM adjacent channel detector
			break;

		case 1:
			if (nRFMode == RFMODE_FM) {
				int v = dsp_query1(0x03);
				OLED_XYIntLen(13, 2, v, 3); // FM multipath
				if((HAL_GetTick()/1000) % 2 == 0) {
					printf("%d", v);
				}
			}
			else
				OLED_XYStr(13, 2, ("---")); // FM multipath not applicable to AM mode
			break;

		case 2:
			u8_data = dsp_query1(0x04); // Frequency offset
			if (u8_data & 0x80)
				i8 = u8_data & 0x7f;
			else
				i8 = -u8_data;
			if (nRFMode == RFMODE_FM) {
				OLED_XYIntLen(11, 2, i8, 4);
				if((HAL_GetTick()/1000) % 2 == 0) {
					printf("%d", i8);
				}
			}
			else
			{
				OLED_XYIntLen(10, 2, i8 / 10, 3);
				OLED_XYChar(13, 2, '.');
				OLED_XYIntLen(14, 2, (int32_t)fabs((float)i8) % 10, 1);
			}
			break;

		case 3:
			u8_data = dsp_query1(0x05) >> 4; // IF filter bandwidth
			if (nRFMode == RFMODE_FM)
			{
				if (!u8_data)
					p = ("55K ");
				else {
					p = M_FMFilter[u8_data].pszMTxt;
					if((HAL_GetTick()/1000) % 2 == 0) {
						printf("%s", p);
					}
				}
			}
			else
				p = M_AMFilter[u8_data].pszMTxt;

			OLED_XYStrLen(12, 2, p, 4, false);
			break;

		case 4:
			v = dsp_query1(0x06);
			OLED_XYIntLen(12, 2, v, 3); // Modulation index
			if((HAL_GetTick()/1000) % 2 == 0) {
				printf("%d", v);
			}
			break;

		case 5:
			memset(s, 0x20, sizeof(s));
			*(s + 1) = '.';
			*(s + 3) = ' ';
			dsp_start_subaddr(0xE2);
			I2C_Restart(DSP_I2C | I2C_READ);
			*s = (I2C_ReadByte(false) & 0x0F) + '0';
			*(s + 2) = (I2C_ReadByte(true) & 0x0F) + '0';
			I2C_Stop();
			u8_data = dsp_query1(0x14);
			if (u8_data & 0x01)
				strcpy(s + 9, "Eg");
			else
				strcpy(s + 9, "Pr");
			u8_data = (u8_data & 0xF0) >> 4;
			if (u8_data == 0x08)
				strcpy(s + 4, "7758");
			else if (u8_data == 0x09)
				strcpy(s + 4, "7753");
			else if (u8_data == 0x0A)
				strcpy(s + 4, "7755");
			else if (u8_data == 0x0B)
				strcpy(s + 4, "7751");
			else if (u8_data == 0x0E)
				strcpy(s + 4, "7754");
			*(s + 8) = ' ';
			OLED_XYStr(5, 2, s);
			if((HAL_GetTick()/1000) % 4 == 0) {
				printf("%s", s);
				fflush(stdout);
			}
			break;

		case 6:
			dsp_start_subaddr(0xC2);
			I2C_Restart(DSP_I2C | I2C_READ);
			iIFFreq1 = I2C_ReadByte(false);
			iIFFreq1 = (iIFFreq1 << 8) | I2C_ReadByte(false);
			iIFFreq2 = I2C_ReadByte(false);
			iIFFreq2 = (iIFFreq2 << 8) | I2C_ReadByte(true);
			I2C_Stop();
			OLED_XYIntLen(4, 2, iIFFreq1, 5);
			OLED_XYIntLen(11, 2, iIFFreq2, 5);
			if((HAL_GetTick()/1000) % 2 == 0) {
				printf("%d %d", iIFFreq1, iIFFreq2);
			}
			break;
		}
		HAL_Delay(64);
		OLED_Refresh();
	}
} // void Menu_Stat(void)

void Menu_Tone(void)
{
	uint8_t nKey;
	int8_t i8, nItem, nVal;

	// 01234567890123450123456789012345
	OLED_FullStr("BASS:    MID    TREBLE");
	OLED_XYIntLen(5, 1, nBass, 2);
	OLED_XYIntLen(13, 1, nMiddle, 2);
	OLED_XYIntLen(7, 2, nTreble, 2);
	
	tipsForLRorAnykey();
	nItem = 0;
	nVal = nBass;
	for (;;)
	{
		if ((nKey = GetKey()) != false)
		{
			if (!(nKey & (KEY_LROT | KEY_RROT)))
			{
				bExitMenu = true;
				OLED_Clear1();
				OLED_Clear2();
				TuneFreqDisp();
				LCDUpdate();
				return;
			}

			if (nItem == 0)
			{
				nItem = 1;
				nVal = nMiddle;
				OLED_XYChar(4, 1, ' ');
				OLED_XYChar(12, 1, ':');
			}

			else if (nItem == 1)
			{
				nItem = 2;
				nVal = nTreble;
				OLED_XYChar(12, 1, ' ');
				OLED_XYChar(6, 2, ':');
			}

			else
			{
				nItem = 0;
				nVal = nBass;
				OLED_XYChar(6, 2, ' ');
				OLED_XYChar(4, 1, ':');
			}
		}

		if ((i8 = GetLRot() + GetRRot()) != false)
		{
			nVal = nVal + i8;
			nVal = constrain(nVal, -9, 9);

			if (nItem == 0)
			{ // Bass
				nBass = nVal;
				OLED_XYIntLen(5, 1, nBass, 2);
				printf("低音%d\n",nVal );
			}

			else if (nItem == 1)
			{ // Middle
				nMiddle = nVal;
				OLED_XYIntLen(13, 1, nMiddle, 2);
				printf("中音%d\n",nVal );
			}

			else
			{ // Treble
				nTreble = nVal;
				OLED_XYIntLen(7, 2, nTreble, 2);
				printf("高音%d\n",nVal );
			}

			SetTone();
			AddSyncBits(NEEDSYNC_TONE);
		}

		HAL_Delay(50);
		OLED_Refresh();
	} // for(;;)
} // void Menu_Tone(void)

void Menu_BalFader(void)
{
	uint8_t nKey;
	int8_t i8, nItem, nVal;

	// 01234567890123450123456789012345
	OLED_FullStr("BALANCE:        FADER");
	OLED_XYIntLen(9, 1, nBalance, 3);
	OLED_XYIntLen(9, 2, nFader, 3);
	printf("旋转，按压编码器修改，任意键返回\n");
	nItem = 0;
	nVal = nBalance;
	for (;;)
	{
		if ((nKey = GetKey()) != false)
		{
			if (!(nKey & (KEY_LROT | KEY_RROT)))
			{
				bExitMenu = true;
				OLED_Clear1();
				OLED_Clear2();
				TuneFreqDisp();
				LCDUpdate();
				return;
			}

			if (nItem == 0)
			{
				nItem = 1;
				nVal = nFader;
				OLED_XYChar(7, 1, ' ');
				OLED_XYChar(5, 2, ':');
			}

			else
			{
				nItem = 0;
				nVal = nBalance;
				OLED_XYChar(5, 2, ' ');
				OLED_XYChar(7, 1, ':');
			}
		}

		if ((i8 = GetLRot() + GetRRot()) != false)
		{
			nVal = nVal + i8;
			nVal = constrain(nVal, -15, 15);

			if (nItem == 0)
			{ // Balance
				nBalance = nVal;
				OLED_XYIntLen(9, 1, nBalance, 3);
				printf("平衡%d\n",nVal );
			}

			else
			{ // Fader
				nFader = nVal;
				OLED_XYIntLen(9, 2, nFader, 3);
				printf("声场%d\n",nVal );
			}

			SetBalFader();
			AddSyncBits(NEEDSYNC_BALFADER);
		}

		HAL_Delay(50);
		OLED_Refresh();
	} // for(;;)
} // void Menu_BalFader(void)

bool YesNo(bool bChkSig)
{
	uint8_t u8_data, lp;

	OLED_XYStr(13, 2, ("Y/N"));
	GetKey(); // Clear key

	for (lp = 0;; lp++)
	{
		if ((u8_data = GetKey()) != false)
		{
			if (u8_data == KEY_LROT)
				return true;
			else if (u8_data == KEY_RROT)
				return false;
		}

		if (bChkSig && !(lp % 16))
			CheckUpdateSig();
		HAL_Delay(64);
		OLED_Refresh();
	}
}

void Menu_SCSV(void)
{
	uint16_t i;
	uint8_t j;
	bool bReturn = false;

	// 0123456789012345
	OLED_XYStr(0, 2, ("Overwrite?   ")); // Confirm overwrite currrent band ch data
	OLED_Refresh();
	if (!YesNo(true))
		return;

	nTuneType = TYPE_SCSV;
	if (nMode != MODE_AUX)
		SetVolume(0); // Mute
	if (nRFMode == RFMODE_FM)
	{
		if (!nFMFilter || nFMFilter > SEEK_FM_FILTER)
			dsp_set_filter(SEEK_FM_FILTER);
	}
	else
	{ // AM
		if (!nAMFilter || nAMFilter > SEEK_AM_FILTER)
			dsp_set_filter(SEEK_AM_FILTER);
	}

	nBandCh[nBand] = 0;
	OLED_Clear1();
	LCDUpdate();
	for (i = 0; i <= ((nBandFMax[nBand] - nBandFMin[nBand]) / nBandStep[nBand][0]); i++)
	{
		nBandFreq[nBand] = nBandFMin[nBand] + (int32_t)nBandStep[nBand][0] * i;
		AdjFreq(false);
		TuneFreqDisp();
		HAL_Delay(5);
		OLED_Refresh();
		for (j = 0; j < 10; j++)
		{
			HAL_Delay(5);
			GetRFStatReg();
			if (IsSigOK())
			{ // Find one signal
				WriteChFreq(true);
				LCDUpdate();
				if (++nBandCh[nBand] >= nBandChs[nBand]) // NV memory full for current band
					bReturn = true;
				break;
			}
		}

		CheckUpdateSig();
		CheckUpdateAlt(ALT_AUTO);
		HAL_Delay(5);
		OLED_Refresh();

		if (bReturn || GetKey())
			break;
	}

	for (; nBandCh[nBand] < nBandChs[nBand]; ++nBandCh[nBand])
		WriteChFreq(false); // Delete extra band chs

	SetFilter(false);
	nTuneType = TYPE_CH;
	nBandCh[nBand] = 0;
	SeekCh(0);
	AddSyncBits(NEEDSYNC_TUNE);
	if (nMode != MODE_AUX)
		SetVolume(nVolume); // Unmute
} // void Menu_SCSV(void)

void AddDelCh(void)
{
	uint8_t u8_data, lp;
	int8_t i8;
	bool bReDISP = true;
	bool bReturn;
	uint32_t u32 = 0;
	char s[4] = "A/D";

	// 0123456789012345
	OLED_XYStr(0, 2, ("CH           "));
	for (bReturn = false, lp = 0; !bReturn; lp++)
	{
		if ((i8 = GetLRot()) != false)
		{
			nBandCh[nBand] = (nBandCh[nBand] + i8 + nBandChs[nBand]) % nBandChs[nBand];
			bReDISP = true;
		}

		if ((i8 = GetRRot()) != false)
		{
			nBandFreq[nBand] += i8 * nBandStep[nBand][nStepIdx];
			AdjFreq(true);
			TuneFreqDisp();
		}

		if (bReDISP)
		{
			u32 = ReadChFreq();
			OLED_XYUIntLenZP(2, 2, nBandCh[nBand], 3);
			OLED_XYIntLen(6, 2, u32, 6);
			if (u32)		// Has frequency
				s[2] = 'D'; // Not empty ch, can be deleted
			else
				s[2] = 'd'; // Empty ch
			OLED_XYStr(13, 2, s);
			bReDISP = false;
		}

		if ((u8_data = GetKey()) != false)
		{
			switch (u8_data)
			{
			case KEY_LROT: // Add ch
				if (YesNo(true))
				{					   // Confirmed
					WriteChFreq(true); // Add current frequency to this ch
					u32 = nBandFreq[nBand];
				}
				bReDISP = true;
				break;

			case KEY_RROT: // Del ch
				if (u32)
				{ // Not empty ch, can be deleted
					if (YesNo(true))
					{						// Confirmed
						WriteChFreq(false); // Delete this ch
						u32 = 0;
					}
					bReDISP = true;
				}
				break;

			case KEY_STEP:
				nStepIdx = (nStepIdx + 1) % NUM_STEPS;
				break;

			case KEY_STEP | KEY_LONGPRESS: // Default step
				nStepIdx = 0;
				break;

			default:
				bReturn = true;
				break;
			}
		}

		if (!(lp % 16))
			CheckUpdateSig();
		HAL_Delay(64);
		OLED_Refresh();
	}

	OLED_Clear2();
	LCDUpdate();
	CheckUpdateAlt(ALT_AUTO);
} // void AddDelCh(void)

void Menu_Help(void)
{
	int8_t nItem, nItems, nLine, nLines;
	int8_t i8;
	uint8_t nKey;
	// Line           12              23
	// 012345678901234501234567890123450123456789012345
	const char *s[] =
		{
			("SQU1:SQUELCH FORLISTEN TO RADIO"),
			("SQU2:SQUELCH FORSEEK/SCAN/ANY"),
			("LSIG:LOWER SIG- NAL QUALITY FOR SEEK/SCAN/ANY"),
			("FMST:FM STEREO  0=FORCE MONO    5=DEFAULT       9=STRONGEST"),
			("FMAT:FM ANTENNA SELECTION"),
			("FMSI:FM STEREO  IMPROVEMENT"),
			("FMCE:FM CHANNEL EQUALIZER"),
			("FMMP:FM ENHANCEDMULTIPATH SUPP- RESSION"),
			("FMNS:FM CLICK   NOISE SUPPRESSOR"),
			("INCA:FM AM IMPROVEC NOISE CAN-  CELLER"),
			("FMBW:FM DYNAMIC BANDWIDTH CON-  TROL PREFERENCE"),
			("DEEM:FM DEEMPHA-SIS CONSTANT"),
			("AGC:WIDEBAND AGCTHRESHOLD"),
			("NB:NOISE BLANKERSENSITIVITY"),
			("TONE:ADJUST BASSMIDDDLE & TREBLE"),
			("BAL:ADJUST      BALANCE & FADER"),
			("BKLT:ADJUST LCD BACKLIGHT"),
			("KEEP:SECONDS OF LCD KEEP ON"),
			("ADJ:ADJUST LCD  BRIGHTNESS"),
			("TSCN:TIME FOR   SCAN STATION"),
			("TANY:TIME FOR   ANY STATION"),
			("TIME:ADJUST TIMEOF CLOCK"),
			("MODE:SET MODE TORF/AUX"),
			("STAT:SHOW OTHER RF STATUS"),
			("TUNE:SET TUNING METHODS"),
			("FILT:SET FILTER BANDWIDTH"),
			("FREQ:TUNING BY  FREQUENCY STEP"),
			("SEEK:TUNING TO  NEXT STATION"),
			("CH:TUNING BY    CHANNEL No."),
			("SCAN:TUNING TO  EVERY STATION & KEEP A WHILE"),
			("ANY:TUNING TO   ANY STATION.KEEPTILL LOST SIGNAL"),
			("S&S:SCAN & SAVE STATIONS IN BAND"),
			("FM-L:BAND BELOW FM BAND"),
			("SINE:SINE WAVE  GENERATOR"),
		};
	const char *chs_s[] =
		{
			("SQU1:收听的时候静噪"),
			("SQU2:定位，搜索，任意模式的时候静噪"),
			("LSIG:寻道/扫描/任意信号质量较低的参考值"),
			("FMST:调频立体声  0=强制单声道    5=默认       9=最强"),
			("FMAT:调频天线选择"),
			("FMSI:调频立体声增强"),
			("FMCE:调频信道均衡器"),
			("FMMP:FM增强多路支持"),
			("FMNS:调频嘀嗒噪声抑制器"),
			("INCA:调频调幅改进型噪声量筒"),
			("FMBW:调频动态带宽控制偏好"),
			("DEEM:调频去加重常量"),
			("AGC:宽带agc阈值"),
			("NB:噪声屏蔽灵敏度"),
			("TONE:调整低音和高音"),
			("BAL:调整平衡和场深控制器"),
			("BKLT:调节液晶背光"),
			("KEEP:液晶显示器持续亮起的秒数"),
			("ADJ:调整液晶显示器亮度"),
			("TSCN:扫描到电台停留时间"),
			("TANY:任何电台的时间"),
			("TIME:调整时钟的时间"),
			("MODE:将模式设置为RF/AUX"),
			("STAT:显示其他RF状态"),
			("TUNE:设置调整方法"),
			("FILT:设置滤波器带宽"),
			("FREQ:调谐频率的步进"),
			("SEEK:调谐到下一个电台"),
			("CH:通过频道编号进行调谐"),
			("SCAN:调到每一个电台，且保持一段时间"),
			("ANY:调谐到任何电台。持续播放直到丢失前一个信号"),
			("S&S:扫描并保存波段中的电台"),
			("FM-L:调频波段以下的波段"),
			("SINE:正弦波发生器"),
		};


	nItems = sizeof(s) / sizeof(const char *);
	// 01234567890123450123456789012345
	OLED_FullStr(("TURN ENCODERS TOSHOW HELP INFO"));
	tipsForLRorAnykey();
	for (nItem = -1;;)
	{
		if ((nKey = GetKey()) != false)
		{
			if (!(nKey & (KEY_LROT | KEY_RROT)))
				bExitMenu = 1;
			break;
		}

		if ((i8 = GetRRot()) != false)
		{
			if (nItem == -1 && i8 < 0)
				nItem = 0;
			nItem = (nItem + i8 + nItems) % nItems;
			OLED_FullStr(s[nItem]);
			printf("%s\n",chs_s[nItem]);
			nLines = (strlen(s[nItem]) - 1) / 16 + 1;
			nLine = 1;
		}

		if ((i8 = GetLRot()) != false)
		{
			if (nItem == -1)
			{
				nItem = 0;
				nLines = (strlen(s[0]) - 1) / 16 + 1;
				nLine = 1;
			}

			nLine += i8;
			if (nLine < 1 || nLines <= 2)
				nLine = 1;
			else if (nLine > (nLines - 1))
				nLine = nLines - 1;

			OLED_FullStr(s[nItem] + ((nLine - 1) << 4));
			printf("%s\n",chs_s[nItem] + ((nLine - 1) << 4));
		}
		OLED_Refresh();
	}

	OLED_Clear1();
	OLED_Clear2();
	TuneFreqDisp();
	OLED_XYStr(0, 3, "*Powered by ADM*"); // fix message
	LCDUpdate();
} // void Menu_Help(void)

void SetSineFreq(int16_t Freq)
{
	double t;
	int32_t i32;
	uint8_t u8_data[6] =
		{0, 0, 0, 0, 0x00, 0x01};
	int8_t i;

	t = cos((double)Freq * 3.1416 / 22050.0);
	if (t == 1.0)
		i32 = 0x7FFFFF;
	else
	{
		t *= 8388608.0;
		if (t > 0.0)
			t += 0.5;
		else
			t -= 0.5;
		i32 = (int32_t)t;
	}

	u8_data[0] = (i32 >> 20) & 0x0F;
	u8_data[1] = (i32 >> 12) & 0xFF;
	u8_data[2] = (i32 >> 9) & 0x07;
	u8_data[3] = (i32 >> 1) & 0xFF;

	dsp_start_subaddr3(0xf44614);
	for (i = 0; i < 6; i++)
		I2C_WriteByte(u8_data[i]);
	I2C_Stop();
}

void Menu_Sine(void)
{
	int8_t i8;
	uint8_t nKey;
	uint8_t bExit;
	uint8_t bPressed;
	uint8_t bUpdateDisp;

	int8_t nSine = 0; // 0 for left ch, 1 for right ch
	int8_t nVol[2], nVolumeOld;
	static int16_t nFreq[2] =
		{500, 1000};
	uint16_t nStep = 100;
	uint8_t bContinuous = 0;

	SetVolume(0);
	nVol[0] = nVol[1] = nVolumeOld = nVolume;
	SetSineFreq(nFreq[0]);
	dsp_write_data(SINE_GEN_VOL); // Set SineGen volume
	dsp_write1(0x20, 0x1F);		  // Primary input: SineGen
								  // 01234567890123450123456789012345
	OLED_FullStr(("SINE-           STEP       VOL"));

	for (bExit = 0, bUpdateDisp = 1, bPressed = 0; bExit != 1;)
	{

		if (bUpdateDisp)
		{
			OLED_XYChar(5, 1, '1' + nSine);
			OLED_XYIntLen(7, 1, nFreq[nSine], 5);
			OLED_XYStr(13, 1, (bContinuous) ? ("---") : (".-."));
			OLED_XYIntLen(4, 2, nStep, 4);
			OLED_XYIntLen(14, 2, nVol[nSine], 2);
			bUpdateDisp = 0;
		}

		if ((i8 = GetLRot()) != false)
		{
			nVol[nSine] += i8;
			nVol[nSine] = constrain(nVol[nSine], MIN_VOL, MAX_VOL);
			if (bContinuous || bPressed)
				SetVolume(nVol[nSine]);
			bUpdateDisp = 1;
		}

		if ((i8 = GetRRot()) != false)
		{
			nFreq[nSine] += i8 * nStep;
			nFreq[nSine] = constrain(nFreq[nSine], 10, 22000);
			SetSineFreq(nFreq[nSine]);
			bUpdateDisp = 1;
		}

		if ((nKey = PeekKey()) != false)
		{ // Some key pressed

			if (nKey & (KEY_LROT | KEY_RROT))
			{
				if ((nKey == KEY_LROT && nSine == 1) || (nKey == KEY_RROT && nSine == 0))
				{ // Switch left/right
					nSine = 1 - nSine;
					SetSineFreq(nFreq[nSine]);
					SetVolume(nVol[nSine]);
					bUpdateDisp = 1;
				}
				else if (!bPressed)
					SetVolume(nVol[nSine]);

				if (!bContinuous)
					bPressed = 1;
			}

			else
			{
				switch (GetKey())
				{
				case KEY_TUNE:
					nStep /= 10;
					if (!nStep)
						nStep = 1000;
					break;

				case KEY_TUNE | KEY_LONGPRESS:
					nStep = 100;
					break;

				case KEY_STEP:
					nStep *= 10;
					if (nStep > 1000)
						nStep = 1;
					break;

				case KEY_STEP | KEY_LONGPRESS:
					nStep = 300;
					break;

				case KEY_BAND:
				case KEY_BAND | KEY_LONGPRESS:
					bContinuous = !bContinuous;
					if (bContinuous)
						SetVolume(nVol[nSine]);
					else
						SetVolume(0);
					break;

				case KEY_FILTER:
				case KEY_FILTER | KEY_LONGPRESS:
					bExit = 1;
					break;
				} // switch()

				bUpdateDisp = 1;
			}
		}

		else
		{ // No key pressed
			if (bPressed && !bContinuous)
			{
				bPressed = 0;
				SetVolume(0);
			}
		}
		OLED_Refresh();

	} // for()

	SetVolume(0);
	OLED_Clear1();
	OLED_Clear2();
	TuneFreqDisp();
	LCDUpdate();
	if (nMode == MODE_AUX)
		dsp_write1(0x20, 0x09); // Primary input: analog input AIN1
	else
		dsp_write1(0x20, 0x00); // Primary input: radio
	nVolume = nVolumeOld;
	SetVolume(nVolume);
} // void Menu_Sine(void)

////////////////////////////////////////////////////////////
// Menu
////////////////////////////////////////////////////////////

bool IsMenuVisible(uint8_t nMenuID)
{
	if (nMenuID == MID_INCA)
	{
		uint8_t u8_data = dsp_query1(0x14);
		uint8_t uHWTYP = u8_data >> 4;
		if (uHWTYP == 0x08 || uHWTYP == 0x0A || uHWTYP == 0x0E)
			return true;
		else
			return false;
	}
	else if (nMenuID == MID_FMAT)
	{
		if (nRFMode == RFMODE_AM)
			return false;
		uint8_t u8_data = dsp_query1(0x14);
		uint8_t uHWTYP = u8_data >> 4;
		if (uHWTYP == 0x0E)
			return false;
		else
			return true;
	}
	else if (nMenuID == MID_FMSI)
	{
		if (nRFMode == RFMODE_AM)
			return false;
		uint8_t u8_data = dsp_query1(0xE2);
		u8_data = u8_data & 0x0F;
		if (u8_data < 6)
			return false;
		else
			return true;
	}
	else if (nMenuID == MID_FMCE || nMenuID == MID_FMMP || nMenuID == MID_FMNS || nMenuID == MID_DEEM || nMenuID == MID_FMST || nMenuID == MID_FMBW)
	{
		if (nRFMode == RFMODE_AM)
			return false;
	}
	return true;
}

void ProcSubMenu(struct M_SUBMENU *pSubMenu)
{
	int8_t i8;
	uint8_t u8_data, lp, nHit;
	int8_t nFirst = 0;
	uint8_t textlen = 0;

	while (!IsMenuVisible((pSubMenu->pMItem + (nFirst % pSubMenu->nItemCount))->nMID))
		nFirst++;

	for (;;)
	{
		OLED_Clear2();
		if (pSubMenu->nMID < MID_MIN_AUTORET && pSubMenu->nMID > MID_OPTION)
		{
			textlen = strlen((pSubMenu->pMItem + (nFirst % pSubMenu->nItemCount))->pszMTxt);
			OLED_Clear2();
			OLED_XYStrLen(8 - textlen / 2, 2, (pSubMenu->pMItem + (nFirst % pSubMenu->nItemCount))->pszMTxt, textlen, 1);
			printf((pSubMenu->pMItem + (nFirst % pSubMenu->nItemCount))->chs_pszMTxt);
			printf("\n");
		}
		else
		{
			for (i8 = 0; i8 < ((pSubMenu->nItemCount < 3) ? pSubMenu->nItemCount : 3); i8++) {
				if(i8 == 1) {
					printf((pSubMenu->pMItem + ((nFirst + i8) % pSubMenu->nItemCount))->chs_pszMTxt);
					printf("\n");
				}
				OLED_XYStrLen(1 + i8 * 5, 2, (pSubMenu->pMItem + ((nFirst + i8) % pSubMenu->nItemCount))->pszMTxt, 4, 1);
			}
		}

		// Display special indicator char for selected item
		switch (pSubMenu->nMID)
		{
		case MID_LSIG:
			nHit = nLowerSig;
			break;

		case MID_FMAT:
			nHit = nFMAT;
			break;

		case MID_FMSI:
			nHit = nFMSI;
			break;

		case MID_FMCE:
			nHit = nFMCEQ;
			break;

		case MID_FIRM:
			nHit = nFirm;
			break;

		case MID_FMMP:
			nHit = nFMEMS;
			break;

		case MID_FMNS:
			nHit = nFMCNS;
			break;

		case MID_INCA:
			nHit = nINCA;
			break;

		case MID_DEEM:
			nHit = nDeemphasis;
			break;

		case MID_MODE:
			nHit = nMode;
			break;

		case MID_TUNE:
			nHit = nTuneType;
			break;

		case MID_BAND:
			nHit = nBand;
			break;

		case MID_FILT:
			if (nRFMode == RFMODE_FM)
				nHit = nFMFilter;
			else
				nHit = nAMFilter;
			break;

		default:
			nHit = MID_NONE; // Magic number, no item selected
			break;
		}

		if (nHit != MID_NONE)
		{
			nHit = (nHit - nFirst + pSubMenu->nItemCount) % pSubMenu->nItemCount;
			if (nHit <= 2)
				OLED_XYChar(nHit * 5, 2, CHAR_SEL);
		}

		for (lp = 0;; lp++)
		{
			// Process key
			if ((i8 = GetKey()) != false)
			{
				if (i8 & (KEY_LROT | KEY_RROT))
				{ // Item selected
					if (pSubMenu->nMID < MID_MIN_AUTORET && pSubMenu->nMID > MID_OPTION)
						u8_data = (pSubMenu->pMItem + (nFirst % pSubMenu->nItemCount))->nMID;
					else
						u8_data = (pSubMenu->pMItem + ((nFirst + 1) % pSubMenu->nItemCount))->nMID;
					if (u8_data == MID_RET)
						return;

					ProcMenuItem(u8_data);

					if ((pSubMenu->nMID >= MID_MIN_AUTORET) || bExitMenu)
						return; // Auto return sub menu, or none left/right key pressed in sub menu
					else
						break; // Redraw sub menu items
				}
				else
				{
					bExitMenu = 1; // None left/right key pressed, exit
					return;
				}
			}

			// Process rotary encoder, adjust nFirst
			if ((i8 = (GetLRot() + GetRRot())) != false)
			{
				nFirst = (nFirst + i8 + pSubMenu->nItemCount) % pSubMenu->nItemCount;
				if (i8 > 0)
					while (!IsMenuVisible((pSubMenu->pMItem + (nFirst % pSubMenu->nItemCount))->nMID))
						nFirst++;
				else
					while (!IsMenuVisible((pSubMenu->pMItem + (nFirst % pSubMenu->nItemCount))->nMID))
						nFirst--;
				nFirst = (nFirst + pSubMenu->nItemCount) % pSubMenu->nItemCount;
				break;
			}

			// Update sig
			if (!(lp % 16))
				CheckUpdateSig();
			HAL_Delay(64);
			OLED_Refresh();
		}
		OLED_Refresh();
	}
} // void ProcSubMenu(struct M_SUBMENU *pSubMenu)

void ProcMenuItem(uint8_t nMenuID)
{
	if (bExitMenu) {
		printf("退出\n");		
		return;
	}

	// Sub menu items
	if (nMenuID <= MID_MAX_SUB)
	{
		ProcSubMenu(&SM_List[nMenuID]);
		return;
	}

	// Leaf menu items(w/o sub menu)
	if ((nMenuID >= MID_FTFM00) && (nMenuID <= MID_FTFM15))
	{
		nFMFilter = nMenuID - MID_FTFM00;
		SetFilter(1); // Set FIR filter for FM
		return;
	}

	if ((nMenuID >= MID_FTAM00) && (nMenuID <= MID_FTAM15))
	{
		nAMFilter = nMenuID - MID_FTAM00;
		SetFilter(1); // Set FIR filter for AM
		return;
	}

	if ((nMenuID >= MID_LW) && (nMenuID <= MID_FM))
	{
		ProcBand(BAND_LW + (nMenuID - MID_LW)); // Set band
		bExitMenu = 1;
		return;
	}

	if ((nMenuID >= MID_FREQ) && (nMenuID <= MID_ANY))
	{
		nTuneType = TYPE_FREQ + (nMenuID - MID_FREQ); // Set tune type
		AddSyncBits(NEEDSYNC_TUNE);
		return;
	}

	if ((nMenuID >= MID_LSIGNORM) && (nMenuID <= MID_LSIGLOW))
	{
		nLowerSig = nMenuID - MID_LSIGNORM; // Normal/reduced signal quality for seek/scan/any, 0=normal, 1=lower
		AddSyncBits(NEEDSYNC_MISC1);
		return;
	}

	if ((nMenuID >= MID_FIRM1) && (nMenuID <= MID_FIRM3))
	{
		nFirm = nMenuID - MID_FIRM1; // Firmware
		BootDirana3();
		if (nMode == MODE_AUX)
			SetMode_AUX();
		else
			SetMode_RF();
		SetRFCtrlReg();
		TuneReg();
		SetTone();			// Set bass, middle & treble
		SetBalFader();		// Set balance & fader
		SetVolume(nVolume); // Unmute
		AddSyncBits(NEEDSYNC_MISC3);
		return;
	}

	if ((nMenuID >= MID_FMAT1) && (nMenuID <= MID_FMAT3))
	{
		nFMAT = nMenuID - MID_FMAT1; // FM antenna selection, 0=ANT1, 1=ANT2, 2=phase diversity
		SetRFCtrlReg();
		AddSyncBits(NEEDSYNC_MISC2);
		return;
	}

	if ((nMenuID >= MID_FMSIOFF) && (nMenuID <= MID_FMSION))
	{
		nFMSI = nMenuID - MID_FMSIOFF; // FM stereo improvement, 0=off, 1=on
		SetRFCtrlReg();
		AddSyncBits(NEEDSYNC_MISC2);
		return;
	}

	if ((nMenuID >= MID_FMCEOFF) && (nMenuID <= MID_FMCEON))
	{
		nFMCEQ = nMenuID - MID_FMCEOFF; // FM channel equalizer, 0=off, 1=on
		SetRFCtrlReg();
		AddSyncBits(NEEDSYNC_MISC2);
		return;
	}

	if ((nMenuID >= MID_FMMPOFF) && (nMenuID <= MID_FMMPON))
	{
		nFMEMS = nMenuID - MID_FMMPOFF; // FM enhanced multipath suppression, 0=off, 1=on
		SetRFCtrlReg();
		AddSyncBits(NEEDSYNC_MISC2);
		return;
	}

	if ((nMenuID >= MID_FMNSOFF) && (nMenuID <= MID_FMNSON))
	{
		nFMCNS = nMenuID - MID_FMNSOFF; // FM click noise suppression, 0=off, 1=on
		SetRFCtrlReg();
		AddSyncBits(NEEDSYNC_MISC2);
		return;
	}

	if ((nMenuID >= MID_INCAOFF) && (nMenuID <= MID_INCAON))
	{
		nINCA = nMenuID - MID_INCAOFF; // FM AM improvec noise canceller, 0=off, 1=on
		SetRFCtrlReg();
		AddSyncBits(NEEDSYNC_MISC3);
		return;
	}

	if ((nMenuID >= MID_DEEM0) && (nMenuID <= MID_DEEM75))
	{
		nDeemphasis = DEEMPHOFF + (nMenuID - MID_DEEM0); // FM de-emphasis, 0=off, 1=50us, 2=75us
		SetRFCtrlReg();
		AddSyncBits(NEEDSYNC_MISC1);
		return;
	}

	// Misc leaf menu items(w/o sub menu)
	switch (nMenuID)
	{
	case MID_EXIT:
		bExitMenu = 1;

	case MID_RET:
		return;

	case MID_SQUELCH1:
		Menu_Squelch(0);
		break;

	case MID_SQUELCH2:
		Menu_Squelch(1);
		break;

	case MID_FMBW:
		Menu_FMDynamicBW();
		break;

	case MID_AGC:
		Menu_AGC();
		break;

	case MID_FMST:
		Menu_Stereo();
		break;

	case MID_NB:
		Menu_NoiseBlanker();
		break;

	case MID_TONE:
		Menu_Tone();
		break;

	case MID_BAL:
		Menu_BalFader();
		break;

	case MID_BKKEEP:
		Menu_BacklightKeep();
		break;

	case MID_BKADJ:
		Menu_BacklightAdj();
		break;

	case MID_TSCN:
		Menu_ScanStayTime();
		break;

	case MID_TANY:
		Menu_AnyHoldTime();
		break;

	case MID_TIME:
		Menu_Time(); // Adjust time
		break;

	case MID_MODERF:
		SetMode_RF(); // RF
		bExitMenu = 1;
		break;

	case MID_MODEAUX:
		SetMode_AUX(); // AUX
		bExitMenu = 1;
		break;

	case MID_STAT:
		Menu_Stat(); // Show status
		break;

	case MID_SCSV:
		Menu_SCSV();
		bExitMenu = 1;
		break;

	case MID_SINE: // Sine wave generator
		Menu_Sine();
		bExitMenu = 1;
		break;

	case MID_HELP: // Show help
		Menu_Help();
		break;
	}
} // void ProcMenuItem(uint8_t nMenuID)

void Menu(uint8_t nMenuID)
{
	bExitMenu = 0;
	ProcMenuItem(nMenuID);
	OLED_Clear2();
	LCDUpdate();
}

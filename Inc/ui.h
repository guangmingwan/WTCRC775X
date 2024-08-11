#ifdef STM32F405xx
#include "stm32f4xx_hal.h"
#else
#include "stm32f1xx_hal.h"
#endif
#include "global.h"
#include <stdbool.h>

#ifndef __UI_H__
#define __UI_H__

#define KEY_LROT        0x01
#define KEY_RROT        0x02
#define KEY_TUNE        0x04
#define KEY_STEP        0x08
#define KEY_BAND        0x10
#define KEY_FILTER      0x20
#define KEY_LONGPRESS   0x80

#define TIMER_LONGPRESS    1200  // Key long press time, ms
#define TIMER_LAST_LP       400  // Is last long press key? ms


// Char display on LCD for selected item
#define CHAR_SEL    '*'//0x7e  // Right arrow


// LCD locations
#define BAND_X      0
#define BAND_Y      1

#define FREQ_X      2
#define FREQ_Y      1

#define RSSI_X     10
#define RSSI_Y      1

#define STEREO_X   12
#define STEREO_Y    1

#define SN_X       13
#define SN_Y        1

#define TYPE_X      0
#define TYPE_Y      2

#define STEP_X      2
#define STEP_Y      2

#define FILTER_X    6
#define FILTER_Y    2

#define ALT_X      11
#define ALT_Y       2


// Display contents in ALT area
#define ALT_AUTO    0
#define ALT_MISC    1
#define ALT_TIME    2
#define ALT_VOL     3


// Menu IDs with sub menus, not auto return
#define MID_OPTION      0x00
#define MID_FREQUENCY   0x01
#define MID_BKLT        0x02

#define MID_RADI        0x03
#define MID_AUDI        0x04
#define MID_APPL        0x05

// Menu IDs with sub menus, auto return
#define MID_MIN_AUTORET 0x06  // Min ID number with auto return property

#define MID_LSIG        0x06  // Normal/reduced signal quality for seek/scan/any
#define MID_FIRM        0x07  // Firmware version
#define MID_FMAT        0x08  // FM antenna selection
#define MID_FMSI        0x09  // FM stereo improvement
#define MID_FMCE        0x0A  // FM channel equalizer
#define MID_FMMP        0x0B  // FM improved multipath suppression
#define MID_FMNS        0x0C  // FM click noise suppression
#define MID_INCA        0x0D  // FM/AM INCA
#define MID_DEEM        0x0E  // FM de-emphasis
#define MID_MODE        0x0F
#define MID_TUNE        0x10
#define MID_BAND        0x11
#define MID_FILT        0x12
#define MID_MAX_SUB     0x12  // Max ID number with sub menus


// Menu IDs of leaves
#define MID_SCS			0x19
#define MID_SQUELCH1    0x20
#define MID_SQUELCH2    0x21
#define MID_LSIGNORM    0x22  // Normal signal quality for seek/scan/any
#define MID_LSIGLOW     0x23  // Reduced signal quality for seek/scan/any
#define MID_FMST        0x24  // FM stereo
#define MID_FIRM1       0x25
#define MID_FIRM2       0x26
#define MID_FIRM3       0x27
#define MID_FMAT1       0x30
#define MID_FMAT2       0x31
#define MID_FMAT3       0x32
#define MID_FMSIOFF     0x33
#define MID_FMSION      0x34
#define MID_FMCEOFF     0x35
#define MID_FMCEON      0x36
#define MID_FMMPOFF     0x37
#define MID_FMMPON      0x38
#define MID_FMNSOFF     0x39
#define MID_FMNSON      0x3a
#define MID_INCAOFF     0x40
#define MID_INCAON      0x41
#define MID_FMBW        0x42  // FM dynamic bandwidth
#define MID_DEEM0       0x43
#define MID_DEEM50      0x44
#define MID_DEEM75      0x45
#define MID_AGC         0x46  // RFAGC wideband threshold
#define MID_NB          0x47  // Noise blanker sensitivity
#define MID_TONE        0x48  // Bass, middle & treble
#define MID_BAL         0x49  // Balance & fader
#define MID_BKKEEP      0x4a
#define MID_BKADJ       0x4b
#define MID_TSCN        0x4c
#define MID_TANY        0x4d
#define MID_TIME        0x4e

#define MID_MODERF      0x50
#define MID_MODEAUX     0x51

#define MID_STAT        0x52
#define MID_SINE        0x54

#define MID_FREQ        0x55
#define MID_SEEK        0x56
#define MID_CH          0x57
#define MID_SCAN        0x58
#define MID_ANY         0x59
#define MID_SCSV        0x5a

#define MID_LW          0x60
#define MID_MW          0x61
#define MID_SW          0x62
#define MID_FL          0x63
#define MID_FM          0x64

#define MID_FTFM00      0x6F
#define MID_FTFM01      0x70
#define MID_FTFM02      0x71
#define MID_FTFM03      0x72
#define MID_FTFM04      0x73
#define MID_FTFM05      0x74
#define MID_FTFM06      0x75
#define MID_FTFM07      0x76
#define MID_FTFM08      0x77
#define MID_FTFM09      0x78
#define MID_FTFM10      0x79
#define MID_FTFM11      0x7a
#define MID_FTFM12      0x7b
#define MID_FTFM13      0x7c
#define MID_FTFM14      0x7d
#define MID_FTFM15      0x7e
#define MID_FTFM16      0x7f


#define MID_FTAM00      0x80
#define MID_FTAM01      0x81
#define MID_FTAM02      0x82
#define MID_FTAM03      0x83
#define MID_FTAM04      0x84
#define MID_FTAM05      0x85
#define MID_FTAM06      0x86
#define MID_FTAM07      0x87
#define MID_FTAM08      0x88
#define MID_FTAM09      0x89
#define MID_FTAM10      0x8a
#define MID_FTAM11      0x8b
#define MID_FTAM12      0x8c
#define MID_FTAM13      0x8d
#define MID_FTAM14      0x8e
#define MID_FTAM15      0x8f

#define MID_HELP        0xf1

// Special menu IDs
#define MID_RET         0xfe
#define MID_EXIT        0xff
#define MID_NONE        0xf0  // Magic number, no item selected

// Menu item structure
struct M_ITEM
{
	uint8_t nMID;
	const char *pszMTxt;
};

// Sub menu structure
struct M_SUBMENU
{
	uint8_t nMID;
	struct M_ITEM *pMItem;
	uint8_t nItemCount;
};

typedef enum {  
    STAGE_MAIN = 0,  // Main Loop  
    STAGE_SUBMENU = 1,  // Sub Menu Loop  
} UI_STAGE; 

void Delay(uint16_t time);
void LCDSetBackLight(uint8_t Data);
void LCDEN(void);
void LCDCmd(uint8_t Cmd);
void LCDData(uint8_t Data);
void LCDClr(void);
void LCDClr1(void);
void LCDOn(void);
void LCDInit(void);
void LCDXY(uint8_t x, uint8_t y);
void LCDXYChar(uint8_t x, uint8_t y, const char c);
void OLED_XYStr(uint8_t x, uint8_t y, const char *str);
void LCDFullStr(const char *str);
void LCDXYStrLen(uint8_t x, uint8_t y, const char *str, uint8_t nLen, uint8_t bLeftAlign);
void LCDXYIntLen(uint8_t x, uint8_t y, int32_t n, uint8_t nLen);
void LCDXYUIntLenZP(uint8_t x, uint8_t y, uint32_t n, uint8_t nLen);

int8_t GetLRot(void);
int8_t GetRRot(void);
uint8_t PeekKey(void);
uint8_t GetKey(void);
void TestRotKey(void);

void LCDUpdate(void);
void CheckUpdateSig(UI_STAGE stage);
void ShowMisc(void);
void ShowTime(void);
void ShowVol(void);
void CheckUpdateAlt(int8_t nShow);

void Menu_Squelch(uint8_t nIdx);
void Menu_FMDynamicBW(void);
void Menu_AGC(void);
void Menu_Stereo(void);
void Menu_NoiseBlanker(void);
void Menu_BacklightAdj(void);
void Menu_BacklightKeep(void);
void Menu_ScanStayTime(void);
void Menu_AnyHoldTime(void);
void Menu_Time(void);
void sprhex2(char *str, uint8_t v);
void Menu_Stat(void);
void Menu_Tone(void);
void Menu_BalFader(void);
bool YesNo(bool bChkSig);
void Menu_SCSV(void);
void AddDelCh(void);
void Menu_Help(void);
void SetSineFreq(int16_t Freq);
void Menu_Sine(void);

bool IsMenuVisible(uint8_t nMenuID);
void ProcSubMenu(struct M_SUBMENU *pSubMenu);
void ProcMenuItem(uint8_t nMenuID);
void Menu(uint8_t nMenuID);



extern uint8_t nStereo;        // FM stereo, 0=off, 5=default, 9=strongest
extern uint8_t nFMAT;          // FM antenna selection, 0=ANT1, 1=ANT2, 2=phase diversity
extern uint8_t nFMSI;          // FM stereo improvement, 0=off, 1=on
extern uint8_t nFMCEQ;         // FM channel equalizer, 0=off, 1=on
extern uint8_t nFirm;          // Firmware Version,0,1,2
extern uint8_t nFMEMS;         // FM enhanced multipath suppression, 0=off, 1=on
extern uint8_t nFMCNS;         // FM click noise suppression, 0=off, 1=on
extern uint8_t nINCA;          // FM AM improvec noise canceller, 0=off, 1=on
extern uint8_t nFMDynamicBW;   // FM dynamic bandwidth, 0 to 3 = narrow bandwidth to wide bandwidth
extern uint8_t nDeemphasis;    // FM de-emphasis, 0=off, 1=50us, 2=75us
extern uint8_t nAGC;           // RFAGC wideband threshold, 0 to 3 = lowest to highest start level
extern uint8_t nNBSens;        // Noise blanker sensitivity,  0 to 3 = lowest to highest sensitivity
extern uint8_t nLowerSig;      // Normal/reduced signal quality for seek/scan/any, 0=normal, 1=lower

extern int8_t nMainRSSI, nRSSI, nRSSI_Last, nRSSI_Disp;  // Received signal strength, in dBuv
extern int8_t nSNR, nSNR_Last, nSNR_Disp;     // Signal noise ratio, in dB
extern bool bSTIN;                            // FM stereo indicator
extern uint8_t REG_USN;

extern uint8_t nVolume;      // Audio volume control, 0-29
extern int8_t nBass;         // Bass, -9 to +9
extern int8_t nMiddle;       // Middle, -9 to +9
extern int8_t nTreble;       // Treble, -9 to +9
extern int8_t nBalance;      // Balance, -15 to +15
extern int8_t nFader;        // Fader, -15 to +15

extern uint8_t nMode;        // RF/AUX
extern uint8_t nRFMode;      // FM/AM

extern uint8_t nTuneType;    // Freq_Step/Seek/Ch/Scan/Any/Scan_Save

extern uint8_t nBand;        // Band: LW/MW/SW/FL/FM
extern const int32_t nBandFMin[NUM_BANDS];
extern const int32_t nBandFMax[NUM_BANDS];
extern const int16_t nBandStep[NUM_BANDS][NUM_STEPS];
extern const uint8_t nBandChs[NUM_BANDS];  // Band total channels
extern int32_t nBandFreq[NUM_BANDS];       // Band default frequency
extern int16_t nBandCh[NUM_BANDS];

extern uint8_t nStepIdx;    // Step index for current band

extern uint8_t nFMFilter;   // Current FIR filter index for FM
extern uint8_t nAMFilter;   // Current FIR filter index for AM
extern int8_t nSquelch[2];  // Signal squelch value in dBuv, -99~99
							// 1st element for auto mute, 2nd element for seek/scan/any

extern uint8_t nBacklightAdj;     // LCD backlight value, 0-255
extern uint8_t nBacklightKeep;    // LCD backlight auto keep seconds, 0-255, 0 for always on
extern bool bLCDOff;              // true for LCD is off
extern uint32_t nBacklightTimer;  // LCD backlight auto keep timer in ms
extern int32_t nSecondsOffset;    // Seconds of real time offset, preset to 15 days

extern uint8_t nScanStayTime;     // Seconds to stay at current frequency
extern uint8_t nAnyHoldTime;      // Seconds to hold current frequency after lost signal

#endif

#ifdef STM32F405xx
#include "stm32f4xx_hal.h"
#else
#include "stm32f1xx_hal.h"
#endif
#include <stdint.h>
#include "global.h"
#include <stdbool.h>

#ifndef __TUNER_H
#define __TUNER_H
// Define enumeration for FM antenna selection  
#define FM_PHASE_DIVERSITY 2  // phase diversity
typedef enum {  
    FM_ANT1 = 0,  // ANT1  
    FM_ANT2 = 1,  // ANT2  
} FM_ANT_SEL;
// opt for nFMAT
#define SET_ANTENNA(nFMAT, antenna) ((nFMAT) = (antenna))
#define ENABLE_PHASE_DIVERSITY(nFMAT) ((nFMAT) |= FM_PHASE_DIVERSITY)
#define DISABLE_PHASE_DIVERSITY(nFMAT) ((nFMAT) &= ~FM_PHASE_DIVERSITY)
#define IS_PHASE_DIVERSITY_ENABLED(nFMAT) (((nFMAT) & FM_PHASE_DIVERSITY) != 0)
#define GET_ANTENNA(nFMAT) ((nFMAT) & 1)

void dsp_start_subaddr(uint8_t);
void dsp_start_subaddr3(uint32_t subaddr);
uint8_t dsp_query1(uint8_t);
uint16_t dsp_query2(uint8_t);
void dsp_write1(uint8_t, uint8_t);
void dsp_write6(uint32_t subaddr, uint32_t data);
void WaitEasyReady(void);
void dsp_write_data(const uint8_t*);
void BootDirana3(void);

void SetVolume(uint8_t);
void CheckVolume(void);
void displayStrings(void);
void SetTone(void);
void SetBalFader(void);
void dsp_set_filter(int8_t);
void NextFilter(void);
void SetFilter(bool bNeedSync);
void SetRFCtrlReg(void);
void TuneReg(void);
bool TuneFreq(int32_t);
void AdjFreq(bool bCurrStep);
void TuneFreqDisp(void);
uint32_t ReadChFreq(void);
void WriteChFreq(bool bAdd);
void SeekCh(int8_t nDir);
void ProcBand(uint8_t nBd);
void ProcStepFilter(uint8_t nKey);
void GetRFStatReg(void);
bool IsSigOK(void);
int8_t Seek(int8_t nDir);
uint8_t ScanAny(void);
void SetRFMode(void);
void SetMode_RF(void);
void SetMode_AUX(void);
void AddSyncBits(uint32_t SyncBit);

void TunerInit(void);
void TunerLoop(void);

#endif

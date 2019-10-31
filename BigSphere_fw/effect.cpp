/*
 * effect.cpp
 *
 *  Created on: 28 окт. 2019 г.
 *      Author: Kreyl
 */

#include "effects.h"
#include "kl_lib.h"
#include "LEDs.h"
#include "shell.h"

#if 1 // ======================= Defines ===========================
#define CMD_SETPIC          0x18
#define CMD_WAIT            0x36
#define CMD_REPEAT_START    0x22
#define CMD_REPEAT_END      0x23
#define CMD_FINISH          0xF1
#define VALUE_REPEAT_FOREVER    0xFF

#define EFF_TIM             TIM4
#define EFF_TIM_IRQ_VECTOR  VectorB8

#define PIC_SZ              63

static uint8_t *ptr = nullptr;
static const Timer_t ITmr{EFF_TIM};
static uint32_t WaitCnt = 0;
// Repeat
static uint32_t RepeatCnt = 0;
static uint8_t *RepeatStart = nullptr;
#endif

void EffectInit() {
    ITmr.Init();
    ITmr.SetTopValue(100);
    ITmr.SetupPrescaler(10000);
    ITmr.EnableIrqOnUpdate();
    ITmr.EnableIrq(TIM4_IRQn, IRQ_PRIO_LOW);
    ITmr.Enable();
}

void EffectStart(uint8_t *PPic) {
    ptr = PPic;
}


#if 1 // ==== Timer IRQ ====
extern "C"
void EFF_TIM_IRQ_VECTOR() { // New timeslot started
    EFF_TIM->SR = 0; // Clear interrupt
    // Wait if needed
    if(WaitCnt) {
        WaitCnt--;
        return;
    }

    if(ptr) switch(*ptr) {
        case CMD_SETPIC:
            LedsShowPic((ptr + 1), PIC_SZ);
//            PrintfI("%X\r", *(ptr + 1));
            ptr += (PIC_SZ + 1);
            break;

        case CMD_WAIT:
            PrintfI("Wait %u\r", *(ptr + 1));
            WaitCnt = *(ptr + 1);
            ptr += 2;
            break;

        case CMD_REPEAT_START:
            RepeatCnt = *(ptr + 1);
            PrintfI("Rpt %u\r", RepeatCnt);
            ptr += 2;
            RepeatStart = ptr;
            break;

        case CMD_REPEAT_END:
            if(RepeatCnt != VALUE_REPEAT_FOREVER and RepeatCnt != 0) RepeatCnt--; // 0xFF means forever
            PrintfI("RptEnd %u\r", RepeatCnt);
            if(RepeatCnt == 0) ptr++; // Go ahead
            else ptr = RepeatStart;
            ITmr.GenerateUpdateEvt(); // Process nrxt cmd
            break;

        case CMD_FINISH:
            PrintfI("Finish\r");
            ptr = nullptr;
            break;

        default: break;
    }
}
#endif

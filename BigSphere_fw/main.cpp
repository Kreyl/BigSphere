#include "hal.h"
#include "MsgQ.h"
#include "kl_lib.h"
#include "Sequences.h"
#include "uart.h"
#include "shell.h"
#include "led.h"
#include "buttons.h"
#include "PinSnsSettings.h"
#include "LEDs.h"
#include "TestPic.h"
#include "radio_lvl1.h"

#include "effects.h"

#if 1 // =============== Defines ================
// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
static const UartParams_t CmdUartParams(115200, CMD_UART_PARAMS);
CmdUart_t Uart{&CmdUartParams};
static void ITask();
static void OnCmd(Shell_t *PShell);

LedBlinker_t LedBlink{BLINK_LED};
#endif

#if 1 // ============= Power ============
const PinOutput_t Pwr[PWR_CNT] = {
        {PWR_CTRL1},
        {PWR_CTRL2},
        {PWR_CTRL3},
        {PWR_CTRL4},
        {PWR_CTRL5},
        {PWR_CTRL6},
        {PWR_CTRL7},
        {PWR_CTRL8},
        {PWR_CTRL9},
};
#endif

int main() {
    // ==== Setup clock ====
    Clk.SetCoreClk(cclk120MHz);
    Clk.UpdateFreqValues();

    // ==== Init OS ====
    halInit();
    chSysInit();

    // ==== Init Hard & Soft ====
    EvtQMain.Init();
    Uart.Init();
    Printf("\r%S %S\r\n", APP_NAME, XSTRINGIFY(BUILD_TIME));
    Clk.PrintFreqs();

    // Debug pin
    PinSetupOut(DBG_PIN, omPushPull);

    LedBlink.Init();
    LedBlink.On();

//    SimpleSensors::Init(); // Buttons

    // Power
    for(const PinOutput_t &Pin : Pwr) {
        Pin.Init();
        Pin.SetHi();
    }
//    Pwr[2].Init();
//    Pwr[2].SetHi();

    Radio.Init();

    LedsInit();
//    LedsShowPic((uint8_t*)TestPic, sizeof(TestPic));

    EffectInit();
    EffectStart();

    // ==== Main cycle ====
    ITask();
}

__noreturn
void ITask() {
    while(true) {
        EvtMsg_t Msg = EvtQMain.Fetch(TIME_INFINITE);
        switch(Msg.ID) {
            case evtIdShellCmd:
                LedBlink.StartOrContinue(lsqCmd);
                OnCmd((Shell_t*)Msg.Ptr);
                ((Shell_t*)Msg.Ptr)->SignalCmdProcessed();
                break;

            default: break;
        } // switch
    } // while true
}


#if 1 // ======================= Command processing ============================
void OnCmd(Shell_t *PShell) {
    Cmd_t *PCmd = &PShell->Cmd;
//    Printf("%S  ", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) PShell->Ack(retvOk);
    else if(PCmd->NameIs("Version")) PShell->Print("%S %S\r\n", APP_NAME, XSTRINGIFY(BUILD_TIME));

    else if(PCmd->NameIs("Set")) {
        uint8_t Indx = 0, Value = 0;
        PCmd->GetNext<uint8_t>(&Indx);
        PCmd->GetNext<uint8_t>(&Value);
        TestPic[Indx] = Value;
        LedsShowPic((uint8_t*)TestPic, sizeof(TestPic));
        PShell->Ack(retvOk);
    }


    else {
        Printf("%S\r\n", PCmd->Name);
        PShell->Ack(retvCmdUnknown);
    }
}
#endif

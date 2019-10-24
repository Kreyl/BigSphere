#include "hal.h"
#include "MsgQ.h"
#include "kl_lib.h"
#include "Sequences.h"
#include "uart.h"
#include "shell.h"
#include "led.h"
#include "buttons.h"
#include "PinSnsSettings.h"

#if 1 // =============== Defines ================
// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
static const UartParams_t CmdUartParams(115200, CMD_UART_PARAMS);
CmdUart_t Uart{&CmdUartParams};
static void ITask();
static void OnCmd(Shell_t *PShell);

static TmrKL_t TmrOneSecond {TIME_MS2I(999), evtIdEverySecond, tktPeriodic};

LedBlinker_t LedBlink{BLINK_LED};
#endif

#if 1 // ============= Power ============
const PinOutput_t Pwr[THE_CNT] = {
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
//    Clk.SetCoreClk(cclk48MHz);
    Clk.UpdateFreqValues();

    // ==== Init OS ====
    halInit();
    chSysInit();

    // ==== Init Hard & Soft ====
    EvtQMain.Init();
    Uart.Init();
    Printf("\r%S %S\r\n", APP_NAME, XSTRINGIFY(BUILD_TIME));
    Clk.PrintFreqs();

    LedBlink.Init();
    LedBlink.On();

//    SimpleSensors::Init(); // Buttons

    // Power
    for(const PinOutput_t &Pin : Pwr) {
        Pin.Init();
//        Pin.SetHi();
//        chThdSleepMilliseconds(126);
    }

    TmrOneSecond.StartOrRestart();

    // ==== Main cycle ====
    ITask();
}

__noreturn
void ITask() {
    while(true) {
        EvtMsg_t Msg = EvtQMain.Fetch(TIME_INFINITE);
//        Printf("Msg.ID %u\r", Msg.ID);
        switch(Msg.ID) {
            case evtIdButtons:
//                Printf("Btn %u; %u\r", Msg.BtnEvtInfo.Type, Msg.BtnEvtInfo.BtnID);
                break;

            case evtIdShellCmd:
                LedBlink.StartOrContinue(lsqCmd);
                OnCmd((Shell_t*)Msg.Ptr);
                ((Shell_t*)Msg.Ptr)->SignalCmdProcessed();
                break;

            case evtIdEverySecond:
//                Printf("Second\r");
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


    else {
        Printf("%S\r\n", PCmd->Name);
        PShell->Ack(retvCmdUnknown);
    }
}
#endif

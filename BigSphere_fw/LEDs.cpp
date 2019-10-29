#include "LEDs.h"
#include "board.h"
#include "shell.h"

#if 1 // ============================= Variables ===============================
static const Timer_t ITmr{LEDS_TIM};

static const Spi_t ISpi{LEDS_SPI};
static const stm32_dma_stream_t *IDma;
static volatile uint32_t Timeslot = 0;

// Leds raw buf and new pic
#define LEDS_RAWBUF_SZ   ((TIMESLOT_TOP+1UL) * LEDS_BYTE_CNT)
static volatile uint8_t Buf1[LEDS_RAWBUF_SZ], Buf2[LEDS_RAWBUF_SZ], *CurrBuf = Buf1, *CurrPtr = Buf1;
static volatile bool NewPicIsReady = false;
#endif

static inline void EnableOutput()  { PinSetLo(LED_OE); }
static inline void DisableOutput() { PinSetHi(LED_OE); }

void LedsInit() {
#if 1 // ==== Display ====
//    DisableOutput();
    PinSetupOut(LED_OE, omPushPull);
    PinSetupOut(LED_LATCH, omPushPull, psHigh);
    PinSetHi(LED_LATCH);

    // SPI
    PinSetupAlterFunc(LED_CLK, psHigh);
    PinSetupAlterFunc(LED_MOSI, psHigh);
    ISpi.Setup(boMSB, cpolIdleLow, cphaFirstEdge, 50000000);
    IDma = dmaStreamAlloc(LEDS_SPI_DMA_TX, IRQ_PRIO_MEDIUM, nullptr, nullptr);
    dmaStreamSetPeripheral(IDma, &LEDS_SPI->DR);
    dmaStreamSetMode(IDma, LEDS_DMA_TX_MODE);
    ISpi.EnableTxDma();
    ISpi.Enable();
    EnableOutput();
#endif
    // Dimmer
//    Dimmer.Init();
//    Dimmer.Set(100);

#if 1 // ==== Timer ====
    ITmr.Init();
    ITmr.SetTopValue(540); // Framerate
    ITmr.EnableIrqOnUpdate();
    ITmr.EnableIrq(TIM3_IRQn, IRQ_PRIO_HIGH);
    ITmr.Enable();
#endif
}

void LedsShowPic(uint8_t *Pic, uint32_t Sz) {
    NewPicIsReady = false;
    // Where to write?
    volatile uint8_t *pDst = ((CurrBuf == Buf1)? Buf2 : Buf1) + (LEDS_BYTE_CNT-1); // set pointer to last element
    uint8_t *p = Pic, *PicEnd = Pic + LEDS_CNT;
    register uint32_t w = 0;
    // Iterate brightness
    for(uint32_t brt=0; brt<=TIMESLOT_TOP; brt++) {
        while(p < PicEnd) { // Iterate LEDs. Pic size is equal to LED cnt.
            // Construct byte
            if(*p++ > brt) w |= (1UL<<0);
            if(*p++ > brt) w |= (1UL<<1);
            if(*p++ > brt) w |= (1UL<<2);
            if(*p++ > brt) w |= (1UL<<3);
            if(*p++ > brt) w |= (1UL<<4);
            if(*p++ > brt) w |= (1UL<<5);
            if(*p++ > brt) w |= (1UL<<6);
            if(*p++ > brt) w |= (1UL<<7);
            // Put it to Raw Buf
            *pDst-- = (uint8_t)w;
            w = 0;
        }
        // Pic bytes iterated, prepare for next brt
        pDst += (1 + LEDS_BYTE_CNT + (LEDS_BYTE_CNT-1)); // Eliminate last --, move to next part, move to last element of next part
        p = Pic;
    } // for brt
    NewPicIsReady = true;
}

#if 1 // ==== Timer IRQ ====
extern "C"
void LEDS_TIM_IRQ_VECTOR() { // New timeslot started
    DBG_HI(); // Timeslot rate
    LEDS_TIM->SR = 0; // Clear interrupt
    // Disable DMA and set transaction sz
    dmaStreamDisable(IDma);
    dmaStreamSetTransactionSize(IDma, LEDS_BYTE_CNT);

    // Output loaded data
    PinSetHi(LED_LATCH);
    PinSetLo(LED_LATCH);

    // ==== Get next piece of data ====
    CurrPtr += LEDS_BYTE_CNT;
    // Calculate new timeslot and RowN and check if frame is done
    Timeslot++;
    if(Timeslot > TIMESLOT_TOP) {
        Timeslot = 0;
        if(NewPicIsReady) {
            CurrBuf = (CurrBuf == Buf1)? Buf2 : Buf1;
            NewPicIsReady = false;
        }
        CurrPtr = CurrBuf;
    } // if(Timeslot > TIMESLOT_TOP)

    // Send new data
    dmaStreamSetMemory0(IDma, CurrPtr);
    dmaStreamEnable(IDma);
    DBG_LO();
}
#endif

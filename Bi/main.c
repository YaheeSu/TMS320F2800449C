

/**
 * main.c
 */

//
// Included Files
//

#include "F28x_Project.h"
#include "Init.h"
#include "pid.h"

// define

#define __FLASH_RAM   1//FLASH:1//RAM:0
#define __PWM_FREQ    1000      // PWM freq 100kHz


// Function Prototypes
__interrupt void adcA1ISR(void);

unsigned int ADCBUFF_Value_V = 0;
unsigned int ADCBUFF_Value_I = 0;
unsigned int PWM_duty=394;

PID_STRUCT*     psPID;

void main(void)
{
    // Copy time critical code and Flash setup code to RAM This includes the following functions:  InitFlash();
    // The  RamfuncsLoadStart, RamfuncsLoadSize, and RamfuncsRunStart symbols are created by the linker. Refer to the device .cmd file.
#if __FLASH_RAM
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
#endif

    // Initial PID Parameter
    psPID = Init_PID();

    // Initialize device clock and peripherals
    InitSysCtrl();
    Init_GPIO();
    Init_PIE();
//  Init_CpuTimer();
    Init_ADC();
    Init_ADCSOC();
    Init_ePWM(__PWM_FREQ,PWM_duty);
    Set_DeadBand(20);

    // Map ISR functions
    EALLOW;
    PieVectTable.ADCA1_INT = &adcA1ISR;     // Function for ADCA interrupt 1
    EDIS;

    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    IER |= M_INT1;  // Enable group 1 interrupts
    EINT;
    ERTM;

    // Enable PIE interrupt
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;

    // Sync ePWM
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;

    // Start ePWM
    EPwm5Regs.ETSEL.bit.SOCAEN = 1;    // Enable SOCA
    EPwm5Regs.TBCTL.bit.CTRMODE = 0;   // Unfreeze, and enter up count mode

    // Loop Forever
    while(1)
    {

    }

}


// adcA1ISR - ADC A Interrupt 1 ISR
__interrupt void adcA1ISR(void)
{
    // Add the latest result to the buffer ADCRESULT0 is the result register of SOC0
    ADCBUFF_Value_V = AdcaResultRegs.ADCRESULT0;  //voltage sense
    ADCBUFF_Value_I = AdcaResultRegs.ADCRESULT1;  //current sense


    psPID->Feedback_AdcPoint_V = ADCBUFF_Value_V;
    psPID->Feedback_AdcPoint_I = ADCBUFF_Value_I;

    PID_Process(psPID);
    EPwm5Regs.CMPA.bit.CMPA = psPID->Output;

    // Clear the interrupt flag and issue ACK
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

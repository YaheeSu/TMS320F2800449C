/*
 * Init.c
 *
 */

#include "F28x_Project.h"


void Init_GPIO(void)
{

    // Initialize GPIO and configure the GPIO pin as a push-pull output
    InitGpio();

    EALLOW;

    //
    // Disable internal pull-up for the selected output pins
    // for reduced power consumption
    // Pull-ups can be enabled or disabled by the user.
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 1;    // Disable pull-up on GPIO10 (EPWM6A)
    GpioCtrlRegs.GPAPUD.bit.GPIO9 = 1;    // Disable pull-up on GPIO11 (EPWM6B)

    //
    // Configure EPWM-6 pins using GPIO regs
    // This specifies which of the possible GPIO pins will be EPWM6 functional
    // pins.
    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;   // Configure GPIO10 as EPWM6A
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;   // Configure GPIO11 as EPWM6B
    EDIS;

}


void Init_PIE(void)
{

    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    DINT;

    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags
    IER = 0x0000;
    IFR = 0x0000;

    // Initialize the PIE vector table with pointers to the shell Interrupt Service Routines (ISR).
    InitPieVectTable();
}


void Init_CpuTimer(void)
{

    // Initialize the Device Peripheral. For this example, only initialize the Cpu Timers.
    InitCpuTimers();

    // Configure CPU-Timer 0, 1, and 2 to interrupt every second: 100MHz CPU Freq, 1 second Period (in uSeconds)
    ConfigCpuTimer(&CpuTimer0, 100, 500000);


    // To ensure precise timing, use write-only instructions to write to the entire register. Therefore, if any of the configuration bits are changed
    // in ConfigCpuTimer and InitCpuTimers, the below settings must also be be updated.
    CpuTimer0Regs.TCR.all = 0x4000;

    // Enable CPU int1 which is connected to CPU-Timer 0, CPU int13 which is connected to CPU-Timer 1, and CPU int 14, which is connected to CPU-Timer 2
    IER |= M_INT1;

    // Enable TINT0 in the PIE: Group 1 interrupt 7
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
}

void Init_ePWM(unsigned int period, unsigned int PWM_duty)
{
    EALLOW;

    EPwm5Regs.TBCTL.bit.PRDLD = TB_SHADOW;  // set Immediate load
    EPwm5Regs.TBPRD = period ;             // PWM frequency = 1 / period
    EPwm5Regs.CMPA.bit.CMPA = PWM_duty;   // set duty 50% initially //period >> 1
    EPwm5Regs.CMPB.bit.CMPB = period - PWM_duty;   // set duty 50% initially
    EPwm5Regs.CMPB.all |= (1 << 8);         // initialize HRPWM extension
    EPwm5Regs.TBPHS.all = 0;
    EPwm5Regs.TBCTR = 0;

    EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;

    EPwm5Regs.TBCTL.bit.PHSEN = TB_DISABLE;
    EPwm5Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_DISABLE;
    EPwm5Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;
    EPwm5Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm5Regs.TBCTL.bit.FREE_SOFT = 11;

    EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
    EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;

    EPwm5Regs.AQCTLA.bit.ZRO = AQ_SET;      // PWM toggle high/low
    EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR;
    EPwm5Regs.AQCTLB.bit.ZRO = AQ_SET;
    EPwm5Regs.AQCTLB.bit.CBU = AQ_CLEAR;

    //------------------------------Dead-Band Control------------------------------------------------
    EPwm5Regs.DBCTL.bit.IN_MODE     = DBA_ALL;              // EpwmA for rising-edge delayed signal and falling-edge delayed signal
    EPwm5Regs.DBCTL.bit.DEDB_MODE   = 0;                    // OutA is selected from S4  ,   OutB is selected from S5
    EPwm5Regs.DBCTL.bit.HALFCYCLE   = 0;                    // Full cycle clocking enabled (if Half cycle clocking enabled　→　TBCLK*2 = 2 times resolution)
    EPwm5Regs.DBCTL.bit.POLSEL      = DB_ACTV_HIC;          // Active high mode. EPWMxB is inverted (default).
    EPwm5Regs.DBCTL.bit.OUT_MODE    = DB_FULL_ENABLE;       // DBM is fully enabled
        EPwm5Regs.DBCTL.bit.OUTSWAP     = 00;                   // OutA and OutB are all defined by OUT-MODE bits
    //------------------------------Dead-Band Control------------------------------------------------
        EPwm5Regs.ETSEL.bit.SOCAEN = 0;     // Disable SOC on A group
        EPwm5Regs.ETSEL.bit.SOCASEL = 4;    // Select SOC on up-count
        EPwm5Regs.ETPS.bit.SOCAPRD = 1;     // Generate pulse on 1st event

//    EPwm6Regs.CMPA.bit.CMPA = 0x0800;   // Set compare A value to 2048 counts
//    EPwm6Regs.TBPRD = 0x1000;           // Set period to 4096 counts


        EPwm5Regs.TBCTL.bit.CTRMODE = 3;    // Freeze counter
    EDIS;

}


// initADC - Function to configure and power up ADCA.
void Init_ADC(void)
{
    // Setup VREF as internal
    SetVREF(ADC_ADCA, ADC_INTERNAL, ADC_VREF3P3);

    EALLOW;

    // Set ADCCLK divider to /4
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;
    //AdcaRegs.ADCCTL2.bit.PRESCALE = 2;
    // Set ADCCLK divider to /1
    //AdcaRegs.ADCCTL2.bit.PRESCALE = 0;//(沒差)


    // Set pulse positions to late
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;

    // Power up the ADC and then delay for 1 ms
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    EDIS;

    DELAY_US(1000);

}

void Init_ADCSOC(void)
{
    // Select the channels to convert and the end of conversion flag
    EALLOW;
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;     // SOC0 will convert pin A0(70);CHSEL=1 pin A1(30)
                                           // 0:A0  1:A1  2:A2  3:A3
                                           // 4:A4   5:A5   6:A6   7:A7
                                           // 8:A8   9:A9   A:A10  B:A11
                                           // C:A12  D:A13  E:A14  F:A15
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 9;     // Sample window is 10 SYSCLK cycles
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 13;   // Trigger on ePWM6 SOCA
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0; // End of SOC0 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   // Enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; // Make sure INT1 flag is cleared

    //Pin-sing modify--------------------------------------------------------------------------------------------------
        AdcaRegs.ADCSOC1CTL.bit.CHSEL = 1;     // SOC0 will convert pin A1
                                               // 0:A0  1:A1  2:A2  3:A3
                                               // 4:A4   5:A5   6:A6   7:A7
                                               // 8:A8   9:A9   A:A10  B:A11
                                               // C:A12  D:A13  E:A14  F:A15
        AdcaRegs.ADCSOC1CTL.bit.ACQPS = 9;     // Sample window is 10 SYSCLK cycles
        AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 13;   // Trigger on ePWM6之SOCA/B=15,SOCC/D=16
                                                // Trigger on ePWM1之SOCA/B=5,SOCC/D=6
                                                // Trigger on ePWM2之SOCA/B=7,SOCC/D=8
                                                // 以此類推

        AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0; // End of SOC0 will set INT1 flag
        AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   // Enable INT1 flag
        AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; // Make sure INT1 flag is cleared

        //Pin-sing modify--------------------------------------------------------------------------------------------------
        EDIS;
}


//Pin-sing modify--------------------------------------------------------------------------------------------------
void Set_DeadBand(unsigned int deadtime)
{

    EPwm5Regs.DBRED.bit.DBRED       = deadtime;         // Set Dead-Band rising-edge = DBRED * T_TBCLK(=1/100M=10ns) = 1us
    EPwm5Regs.DBFED.bit.DBFED       = deadtime;

}



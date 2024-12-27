/*
 * pid.c
 *
 */

#include    "pid.h"


static PID_STRUCT sPID;

PID_STRUCT* Init_PID(void)
{
    sPID.stay = 0;
    sPID.count = 0;
    sPID.mode = 0;     //當下電流模式階段(1~5)
                       //ADC讀值//實際對應電流值(理想電流值)
    sPID.RefSetPoint_I1 = 3269;//3269; //3.25A(3.250A)
    sPID.RefSetPoint_I2 = 2194;//2194; //2.16A(2.173A)
    sPID.RefSetPoint_I3 = 1487;//1487; //1.4525A(1.453A)
    sPID.RefSetPoint_I4 = 1009;//1009; //0.9737A(0.972A)
    sPID.RefSetPoint_I5 = 690;//690;  //0.6504A(0.650A)

    // Set PID Coefficients
    sPID.Kp = 0.002;
    sPID.Ki = 0.0001;
    sPID.Kd = 0;
    sPID.Error = 0;
    sPID.LastError_1 = 0;
    sPID.LastError_2 = 0;

    // Set PID Setpoint
    sPID.RefSetPoint_V = 2200;  //理想電壓值25.2V(對應參數約2200)

    sPID.OutputMax = 450;//最大責任週期
    sPID.OutputMin = 50;//最小責任週期

    sPID.lastPID = EPwm5Regs.CMPA.bit.CMPA;

    return &sPID;

}


void PID_Process(PID_STRUCT* psPID)
{
    if(psPID->mode<6){//在1~5階段時可判定是否進入下一階段

        if(psPID->stay!=0)//當此參數(停留時間)未達到0時，每經過一次迴圈就減少1(直到此參數為0時才可轉態)
            psPID->stay--;

        if((psPID->Feedback_AdcPoint_V > psPID->RefSetPoint_V)){//當達到轉態電壓時判定是否符合轉態條件
            psPID->count++; //每達到轉態電壓，此參數即加1
            if(psPID->count>=2&&psPID->stay==0){//轉態條件:至少連續2次超過轉態電壓 且 至少在原階段超過停留時間 即可轉態
                //完全符合轉態條件後
                psPID->count=0; //達到轉態電壓之計數參數歸零
                psPID->mode++;  //進入下一階段(五階段定電流結合定電壓)
                //if(psPID->mode!=5)    //若只監測五階段定電流，需避免進入CV模式
                //psPID->mode=psPID->mode+1;  //進入下一階段(定電流轉定電壓)
                psPID->stay=1500000;//設定轉換到下一階段停留之時間(100000:約1sec)
            }
        }
        else//若未達到轉態電壓，計數參數歸零(為了使用在未連續達到電壓時)
            psPID->count=0;
    }

    //在當前階段，使用對應不同電流參數
    if(psPID->mode == 0)
        psPID->RefSetPoint_I=psPID->RefSetPoint_I1;
    if(psPID->mode == 1)
        psPID->RefSetPoint_I=psPID->RefSetPoint_I1;
    else if(psPID->mode == 2)
        psPID->RefSetPoint_I=psPID->RefSetPoint_I2;
    else if(psPID->mode == 3)
        psPID->RefSetPoint_I=psPID->RefSetPoint_I3;
    else if(psPID->mode == 4)
        psPID->RefSetPoint_I=psPID->RefSetPoint_I4;
    else if(psPID->mode == 5)
        psPID->RefSetPoint_I=psPID->RefSetPoint_I5;/**/


    // Compute the error
    if(psPID->mode == 6)//除了在第六階段使用定電壓外，其餘用定電流
        psPID->Error = psPID->RefSetPoint_V - psPID->Feedback_AdcPoint_V;
    else
        psPID->Error = psPID->RefSetPoint_I - psPID->Feedback_AdcPoint_I;

    // Compute PID
    psPID->incPID = psPID->Kp * (psPID->Error - psPID->LastError_1);

    psPID->incPID += (psPID->Ki * psPID->Error);

    psPID->incPID += (psPID->Kd * (psPID->Error - 2 * psPID->LastError_1 + psPID->LastError_2));

    psPID->Output = psPID->lastPID + psPID->incPID;


    // Limit the output
    if(psPID->Output > psPID->OutputMax)
       psPID->Output = psPID->OutputMax;
    else if(psPID->Output < psPID->OutputMin)
       psPID->Output = psPID->OutputMin;

    psPID->LastError_2 = psPID->LastError_1;
    psPID->LastError_1 = psPID->Error;
    psPID->lastPID = psPID->Output;

}

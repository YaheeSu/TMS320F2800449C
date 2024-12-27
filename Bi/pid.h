/*
 * pid.h
 *
 */

#ifndef PID_H_
#define PID_H_

#include "F28x_Project.h"



typedef struct PID_STRUCT{

    float stay;     //轉到下一階段後強制保留在新階段的時間
    float count;    //判斷連續幾次達到轉態電壓後可進入下一階段

    float  mode;    //當下電流模式階段
    float  RefSetPoint_I;
    //五階段之各階段設定電流參考值
    float  RefSetPoint_I1;
    float  RefSetPoint_I2;
    float  RefSetPoint_I3;
    float  RefSetPoint_I4;
    float  RefSetPoint_I5;
    float  RefSetPoint_V;               // Input: Reference input
    float  Feedback_AdcPoint_I;         // Input: Feedback input
    float  Feedback_AdcPoint_V;         // Input: Feedback input
    float  Error;                     // Variable: Error

    float  Error_Kp;
    float  Error_Kd;

    float  Kp;          // Parameter: Proportional gain
    float  Ki;          // Parameter: Integral gain
    float  Kd;          // Parameter: Derivative gain

    float  incPID;
    float  lastPID;

    float  LastError_1;
    float  LastError_2;

    float  OutputMax;      // Parameter: Maximum output
    float  OutputMin;      // Parameter: Minimum output
    float  Output;         // Output: PID output

}PID_STRUCT;


extern PID_STRUCT* Init_PID();
extern void PID_Process(PID_STRUCT* psPID);


#endif /* PID_H_ */

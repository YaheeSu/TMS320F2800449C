/*
 * pid.h
 *
 */

#ifndef PID_H_
#define PID_H_

#include "F28x_Project.h"



typedef struct PID_STRUCT{

    float stay;     //���U�@���q��j��O�d�b�s���q���ɶ�
    float count;    //�P�_�s��X���F����A�q����i�i�J�U�@���q

    float  mode;    //��U�q�y�Ҧ����q
    float  RefSetPoint_I;
    //�����q���U���q�]�w�q�y�Ѧҭ�
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

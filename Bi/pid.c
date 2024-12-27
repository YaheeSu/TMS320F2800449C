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
    sPID.mode = 0;     //��U�q�y�Ҧ����q(1~5)
                       //ADCŪ��//��ڹ����q�y��(�z�Q�q�y��)
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
    sPID.RefSetPoint_V = 2200;  //�z�Q�q����25.2V(�����ѼƬ�2200)

    sPID.OutputMax = 450;//�̤j�d���g��
    sPID.OutputMin = 50;//�̤p�d���g��

    sPID.lastPID = EPwm5Regs.CMPA.bit.CMPA;

    return &sPID;

}


void PID_Process(PID_STRUCT* psPID)
{
    if(psPID->mode<6){//�b1~5���q�ɥi�P�w�O�_�i�J�U�@���q

        if(psPID->stay!=0)//���Ѽ�(���d�ɶ�)���F��0�ɡA�C�g�L�@���j��N���1(���즹�ѼƬ�0�ɤ~�i��A)
            psPID->stay--;

        if((psPID->Feedback_AdcPoint_V > psPID->RefSetPoint_V)){//��F����A�q���ɧP�w�O�_�ŦX��A����
            psPID->count++; //�C�F����A�q���A���ѼƧY�[1
            if(psPID->count>=2&&psPID->stay==0){//��A����:�ܤֳs��2���W�L��A�q�� �B �ܤ֦b�춥�q�W�L���d�ɶ� �Y�i��A
                //�����ŦX��A�����
                psPID->count=0; //�F����A�q�����p�ưѼ��k�s
                psPID->mode++;  //�i�J�U�@���q(�����q�w�q�y���X�w�q��)
                //if(psPID->mode!=5)    //�Y�u�ʴ������q�w�q�y�A���קK�i�JCV�Ҧ�
                //psPID->mode=psPID->mode+1;  //�i�J�U�@���q(�w�q�y��w�q��)
                psPID->stay=1500000;//�]�w�ഫ��U�@���q���d���ɶ�(100000:��1sec)
            }
        }
        else//�Y���F����A�q���A�p�ưѼ��k�s(���F�ϥΦb���s��F��q����)
            psPID->count=0;
    }

    //�b��e���q�A�ϥι������P�q�y�Ѽ�
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
    if(psPID->mode == 6)//���F�b�Ĥ����q�ϥΩw�q���~�A��l�Ωw�q�y
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

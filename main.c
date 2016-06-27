
#include <stdint.h>
#include <stdio.h>
#include "STM8S.h"
#include "misc.h"


unsigned char time10ms,RXTimer,ADCTimer;
unsigned char RXIndex;
unsigned char RXBuf[20];
struct BitFlag SysFlag;
extern unsigned int OldFilterValue;

void UART2Recive(void) __interrupt 21
{
    RXBuf[RXIndex++] = UART2->DR;
    RXTimer=0;
    __asm__("bres 0x5240,#5\n"); //clear UART2->SR->RXNE
}
void Tim4Int(void) __interrupt 23
{
    TIM4->SR1=0;
    time10ms++;
    if(RXIndex>0){RXTimer++;}
    if(SysFlag.ADCGo){ADCTimer++;}
}

void main(void)
{

    unsigned char ADCCounter;
    SystemInit();
    UartInit();
    ADCInit();



    SysFlag.RXDone = 0;SysFlag.ADCGo=0;
    RXIndex=0;
    time10ms=0;RXTimer=0;
    ADCTimer=0;ADCCounter=0;
    OldFilterValue=0;
    GPIOE->ODR&=~(0x20);

    enableInterrupts();

	while(1)
    {
        if(time10ms>50)
        {
            __asm__("bcpl 0x5014,#5\n");//GPIOE->ODR = GPIOE->ODR ^ 0x20;
            time10ms=time10ms-50;
        }
        if(RXTimer>10)
        {
            RXIndex=0;
            RXTimer=0;
            RXProcess(&RXBuf[0]);
        }
        if((SysFlag.RXDone)&&(SysFlag.ADCGo))
        {
            unsigned int ADCvalue,FiltedADCvalue;
            if(ADCTimer>5)
            {
                ADC1->CR1 |=0x01;
                while(((ADC1->CSR)&0x80)!=0x80);
                ADC1->CSR &= 0x7F;

                ADCvalue=(((unsigned int)ADC1->DRH)<<8) | ((unsigned int)ADC1->DRL);
                FiltedADCvalue = LowPassFilter(ADCvalue);
                OldFilterValue=FiltedADCvalue;
                printf("%d,%d,\n",ADCvalue,FiltedADCvalue);
                ADCCounter++;
                ADCTimer=ADCTimer-5;
            }

            if(ADCCounter==100)
            {
                //__asm__("bres _SysFlag,#1\n");
                ADC1->DRH=0;ADC1->DRL=0;OldFilterValue=0;
                SysFlag.RXDone=0;
                SysFlag.ADCGo=0;
                RXIndex=0;
                ADCCounter=0;
            }

        }
    }
}

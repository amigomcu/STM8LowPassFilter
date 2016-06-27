#include "stm8s.h"
#include "misc.h"
extern struct BitFlag SysFlag;
extern unsigned char RXBuf[20];
unsigned int OldFilterValue;

void RXProcess(unsigned char *p)
{

    if((*p)=='$')
    {
        SysFlag.RXDone=1;
        if((*(p+1)=='A')&&(*(p+2)=='D')&&(*(p+3)=='C'))
        {
            SysFlag.ADCGo=1;
        }
    }
    else
    {
        SysFlag.RXDone=0;
        SysFlag.ADCGo=0;
    }

}

unsigned int LowPassFilter(unsigned int NewADCValue)
{
    unsigned int FiltedValue;

    /*
     half Sampling rate=10, BW = 4.41Hz
     Filter factor
     = exp(-pi*BW / Fs)
     = exp (-3.14*4.4/10) = 0.251
     = 256/1024 = (2^8) / (2^10)
     right shift two times
     */

    if(NewADCValue < OldFilterValue)
    {
        FiltedValue=OldFilterValue-NewADCValue;
        FiltedValue=FiltedValue>>2;
        FiltedValue=OldFilterValue-FiltedValue;
    }
    else if(NewADCValue > OldFilterValue)
    {
        FiltedValue=NewADCValue-OldFilterValue;
        FiltedValue=FiltedValue>>2;
        FiltedValue=OldFilterValue+FiltedValue;
    }



    return FiltedValue;
}
void ADCInit(void)
{
    ADC1->CR1=0x03;       // fADC=fMaster/2, CONT=1, ADON=1
    ADC1->CR2=0x08;       // Right alignment
    ADC1->CR3=0x00;
    ADC1->CSR=0x00;
    ADC1->TDRL = 0x01;    // PB0 Schmitt trigger disable
    GPIOB->DDR &= 0xFE;   // PB0 output mode
    GPIOB->CR1 &= 0xFE;   // PB0 floating input
    GPIOB->CR2 &= 0xFE;   // External interrupt disable
}
void SystemInit(void)
{
    CLK->CKDIVR = CLK_CKDIVR_RESET_VALUE; // Set the frequency to 2 MHz

	GPIOE->DDR = 0x20;
	GPIOE->CR1 = 0x20;

    /*
       Configure timer
       fmaster = 2 MHz,
       Tim4 clk time = 1/(2M/(2^7)) =1/15.625KHz=64uS
       one interrupt time is 64us * 157 = 10.048mS
    */
    TIM4->CR1 = TIM4_CR1_CEN;
    TIM4->PSCR = 0x07;
    TIM4->EGR = 0x01;
    TIM4->ARR = 157;
    TIM4->IER = 0x01;

}

void UartInit(void)
{
    /*
       UART2 Baud rate =9600,
       BR register = 2M / 9600 = 208.3 = 0xD1

    */
    UART2->SR &=0xBF;
    UART2->BRR1 =0x0D;UART2->BRR2=0x01;
    UART2->CR1 = 0x00;
    UART2->CR2 = UART2_CR2_RIEN | UART2_CR2_TEN|UART2_CR2_REN;
    UART2->CR3 = 0x00;
}

void putchar(char c)
{
    while(((UART2->SR)&0x80) != 0x80);
    UART2->DR=c;
}

void Delay1ms(unsigned int c)
{
    unsigned int i,j;
    for(i=c;i>0;i--)
        for(j=330;j>0;j--)
    {
        __asm__("NOP\n");
    }
}

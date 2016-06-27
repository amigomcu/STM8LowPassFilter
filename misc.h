#ifndef _MISC_H
#define _MISC_H

void RXProcess(unsigned char *p);
unsigned int LowPassFilter(unsigned int NewADCValue);
void ADCInit(void);
void SystemInit(void);
void UartInit(void);
void putchar(char);
void Delay1ms(unsigned int c);

struct BitFlag{
        unsigned x1     :1;  // UART received OK
        unsigned RXDone :1;
        unsigned ADCGo  :1;  // Go to boot loader
        unsigned x3     :1;  //
        unsigned x4     :1;
        unsigned x5     :1;
        unsigned x6     :1;
        unsigned x7     :1;
        };

#endif // misc_H

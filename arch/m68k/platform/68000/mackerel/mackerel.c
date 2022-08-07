#include <asm/mackerel.h>

void set_exception_handler(unsigned char exception_number, void (*exception_handler)(void *))
{
    *((int *)(exception_number * 4)) = (int)exception_handler;
}

void mfp_init(void)
{
    MEM(MFP_DDR) = 0xFF; // Set GPIO direction to output

    // Set up timer A as the UART clock (pin TA0 is connected to TC and RC)
    MEM(MFP_TACR) = 0b00010001; // timer A enabled, delay mode, /4 prescalar
    MEM(MFP_TADR) = 3;          // 153.6 kHz square wave on timer A (with 3.6864 MHz oscillator)

    // Set up UART
    MEM(MFP_UCR) = 0b10001000; // UART uses 1/16 clock rate, 8 data, 1 stop bit, no parity
    MEM(MFP_TSR) = 1;          // Enable transmitter
    MEM(MFP_RSR) = 1;          // Enable receiver
}

void mfp_putc(char s)
{
    while ((MEM(MFP_TSR) & 0b10000000) == 0)
    {
    }

    MEM(MFP_UDR) = s;

    if (s == '\n')
    {
        mfp_putc('\r');
    }
}

void mfp_puts(const char *s)
{
    unsigned i = 0;

    while (s[i] != 0)
    {
        mfp_putc(s[i]);
        i++;
    }
}

char mfp_getc(void)
{
    while ((MEM(MFP_RSR) & 0b10000000) == 0)
    {
    }

    return MEM(MFP_UDR);
}

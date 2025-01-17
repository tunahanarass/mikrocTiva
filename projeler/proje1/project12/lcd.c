#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_sysctl.h"
#include "lcd.h"
#include "driverlib/interrupt.h"

void baslangic(void)   //LCD ba�lang�� ayarlar�n� yapar.
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, 0xFF);
    SysCtlDelay(50000);
    GPIOPinWrite(GPIO_PORTB_BASE, RS,  0x00 );

    GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7,  0x30 );
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);
    SysCtlDelay(10);
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);
    SysCtlDelay(50000);

    GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7,  0x30 );
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);
    SysCtlDelay(10);
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);
    SysCtlDelay(50000);

    GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7,  0x30 );
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);
    SysCtlDelay(10);
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);
    SysCtlDelay(50000);

    GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7,  0x20 );
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);
    SysCtlDelay(10);
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);
    SysCtlDelay(50000);

    LCD_sil();
        komut_yaz(0x28);//fonksiyon secimi
        komut_yaz(0x06);//kurs�r� sa�a kayd�r
        komut_yaz(0x80);//kurs�r konumu
        komut_yaz(0x0f);//kurs�r� a�
        LCD_sil();
}
//===========================================================================
void komut_yaz(unsigned char c)
{
    GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7, (c & 0xf0) );
    GPIOPinWrite(GPIO_PORTB_BASE, RS, 0x00);
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);
    SysCtlDelay(10);
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);
    SysCtlDelay(50000);


    GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7, (c & 0x0f) << 4 );
    GPIOPinWrite(GPIO_PORTB_BASE, RS, 0x00);
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);
    SysCtlDelay(10);
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);
    SysCtlDelay(50000);
}
//===========================================================================
void satir_sutun(unsigned char satir, unsigned char sutun)
{
    if(satir==1)
        komut_yaz(0x80+((sutun-1)%16));
    else
        komut_yaz(0xC0+((sutun-1)%16));
}
//===========================================================================
void LCD_sil(void)   //LCD ekran�n� siler.
{
    komut_yaz(0x01);
    SysCtlDelay(10);
}

void veri(char deger)
{
    GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7, (deger & 0xf0) );
    GPIOPinWrite(GPIO_PORTB_BASE, RS, 0x01);
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);
    SysCtlDelay(10);
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);
    SysCtlDelay(50000);


    GPIOPinWrite(GPIO_PORTB_BASE, D4 | D5 | D6 | D7, (deger & 0x0f) << 4 );
    GPIOPinWrite(GPIO_PORTB_BASE, RS, 0x01);
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x02);
    SysCtlDelay(10);
    GPIOPinWrite(GPIO_PORTB_BASE, E, 0x00);
    SysCtlDelay(50000);
}
//===========================================================================
void veri_yaz(char* s)
{
    while(*s)
        veri(*s++);
}
void portfkesmesifonksiyonu()
{
    int a;
    a=GPIOIntStatus(GPIO_PORTF_BASE,true);  // hangi pinden kesme geldi�ini g�steriyor
    // false = maskeleme yapma demek. true maskeleme yap demek.
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
    if (a==16)
    {
        baslangic();
        satir_sutun(2,8);
        veri_yaz("a");
    }
    if (a==1)
    {
        LCD_sil();
    }
}





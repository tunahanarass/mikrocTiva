#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "driverlib/adc.h"
#include "odev.h"
#include"inc/tm4c123gh6pm.h"
#include "driverlib/systick.h"

void SetInitSettings();
void saatkontrol();
void pcyesaatgonder();
void adcseridengonder(uint32_t deger);
void lcdyeSaatYaz();

bool saatmi= false;
int gelensaatno=0;
int gelenveri;



int saatdizi[8]={1,3,10,4,5,10,3,5};
int timerkesmesi;
uint32_t adcdata[4];
int gelenveri;
char hane3;
char hane2;
char hane1;
char hane0;


int main(void)
{

   SetInitSettings();

     while(1){

       timerkesmesi=TimerIntStatus(TIMER0_BASE, true);

     if (timerkesmesi!=0){

          TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

          saatkontrol();
          pcyesaatgonder();
          lcdyeSaatYaz();

          ADCProcessorTrigger(ADC0_BASE,1);


     }

       if(ADCIntStatus(ADC0_BASE, 1, false)){

           ADCIntClear(ADC0_BASE, 1);
           ADCSequenceDataGet(ADC0_BASE, 1,adcdata);

           uint32_t ortalamadeger=(adcdata[0]+adcdata[1]+adcdata[2]+adcdata[3])/4;
           adcseridengonder(ortalamadeger);


       }

       if(UARTCharsAvail(UART0_BASE)){

           gelenveri=UARTCharGet(UART0_BASE);
           if(gelenveri=='%'){

               saatmi=true;
               gelensaatno=0;
               TimerDisable(TIMER0_BASE, TIMER_A);
                }

           else if(saatmi==true){

               saatdizi[gelensaatno]=gelenveri-48;
               gelensaatno++;

               if(gelensaatno==8){

                   saatmi=false;
                   gelensaatno=0;

                   TimerEnable(TIMER0_BASE, TIMER_A);

               }
           }
       }


    }

 }

void lcdyeSaatYaz()
{
    baslangic();

    TimerIntClear(TIMER0_BASE,TIMER_TIMA_MATCH);

    satir_sutun(2, 8); // LCD'nin sað alt köþesi
        veri(saatdizi[0] + 48); // Saat onlar
        veri(saatdizi[1] + 48); // Saat birler
        veri(':');
        veri(saatdizi[3] + 48); // Dakika onlar
        veri(saatdizi[4] + 48); // Dakika birler
        veri(':');
        veri(saatdizi[6] + 48); // Saniye onlar
        veri(saatdizi[7] + 48); // Saniye birler



           satir_sutun(2, 1);
           veri(hane3+48);
           veri(hane2+48);
           veri(hane1+48);
           veri(hane0+48);


   }




void adcseridengonder(uint32_t deger){



    if(deger<10){
        hane3=0; hane2=0; hane1=0; hane0=deger;

    }
    //34
    else if(deger<100){
        hane3=0; hane2=0; hane1=(deger-hane0)/10; hane0=(deger % 10);

    }
    //345
    else if(deger<1000){
        hane3=0;    hane2=(deger-hane1*10-hane0*1)/100;      hane1=((deger-hane0)/10) % 10;      hane0=(deger % 10);

    }
    //3456
    else{
      hane3=(deger-hane2*100-hane1*10-hane0*1)/1000;       hane2=((deger-hane1*10-hane0*1)/100)%10;   hane1=((deger-hane0)/10) % 10;   hane0=(deger % 10);

    }

    UARTCharPut(UART0_BASE, '(');
    UARTCharPut(UART0_BASE, hane3+48);
    UARTCharPut(UART0_BASE, hane2+48);
    UARTCharPut(UART0_BASE, hane1+48);
    UARTCharPut(UART0_BASE, hane0+48);
    UARTCharPut(UART0_BASE, ')');
    UARTCharPut(UART0_BASE,'\n');


}
    void pcyesaatgonder(){

    int i;

    UARTCharPut(UART0_BASE, '{');

    for(i=0;i<8;i++){

     UARTCharPut(UART0_BASE,saatdizi[i]+48);

    }

    UARTCharPut(UART0_BASE,'\n');



}



void saatkontrol(){

  int sa= saatdizi[0]*10+saatdizi[1];
  int dk= saatdizi[3]*10+saatdizi[4];
  int sn= saatdizi[6]*10+saatdizi[7];

  sn++;

    if(sn==60){

        sn=0;
        dk++;
        if(dk==60){

            dk=0;
            sa++;
            if(sa==24){

               sa=0;
            }
        }
    }

    saatdizi[1]=sa % 10;
    saatdizi[0]=(sa-saatdizi[1])/10;


    saatdizi[4]=dk % 10;
    saatdizi[3]=(dk-saatdizi[4])/10;


    saatdizi[7]=sn % 10;
    saatdizi[6]=(sn-saatdizi[7])/10;






}

void SetInitSettings()
{

       SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN); // clock ayarý yaptýk
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // portf aktif
       GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

       HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
       HWREG(GPIO_PORTF_BASE + GPIO_O_CR) = 0x01;

       GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_0|GPIO_PIN_4);
       GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);


       SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

       TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); // timer geriye sayýcý ayarlandý .
       TimerLoadSet(TIMER0_BASE, TIMER_A, SysCtlClockGet()-1); // timerA yüklendi
       TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // timer kesmesi TimerA time out interrupt olarak ayarlandý .
       TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT); // timer kesmesi temizlendi .


       SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0); // uart0 çevre birimi aktif
       SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA); // porta aktif

       GPIOPinConfigure(GPIO_PA0_U0RX); // porta'nýn 0. pinini receiver olarak kullanacaðým.
       GPIOPinConfigure(GPIO_PA1_U0TX); // porta'nýn 1. pinini transmitter olarak kullanacaðým.
       GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); // uart pinlerini belirledim .

       UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, UART_CONFIG_PAR_NONE|UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE);

       // uart ayarlarý yapýldý .

       // baudrate bit sayýsý parity ve stop biti ayarý yapýldý .

       UARTEnable(UART0_BASE); // uart enable edildi .

       // uart ayarlarýný yaparken önce uart çevre birimini aktif et sonrasýnda receiver ve transmitter için porta'yý kullanacaðýn için
       //porta çevre birimini aktif et , uart için kullanacaðýn pinleri configure et , uart pinlerini belirle , uart ayarlarýný yap baudrate vs.
       // uartý enable et

       SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0); // ADC HAYAT=1

       ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0); /// adc0 kullan ve sequence 1 seç , adcyi yazýlýmla tetikledim.
       //(adc0_ base , sequence number , trigger , priority)

       // sekansör configüre edildi .

       ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS); // sýcaklýk sensörü seçildi
       ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
       ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
       ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);


       ADCSequenceEnable(ADC0_BASE, 1); // adc ayarlanmasý bitti

       // sekansör enable edildi .

        // adc çevre birimi enerjilendirildi . adc sekansörü configüre edildi . sekansör numarasý ve önceliði seçildi ayrýca
       // adc'nin nasýl tetikleneceði seçildi . sekansörün adýmlarý configüre edildi . adc'nin analog veriyi hangi yolla alacaðý seçildi.
       // adc enable edildi.

       TimerEnable(TIMER0_BASE, TIMER_A);

}

//
// Smpl_Timer_LCD
// 
// Timer0 set at one-shot   @ 1 sec or 1 Hz
// Timer1 set at periodic   @ 1 sec or 1 Hz
// Timer2 set at contiuous  @ 1 sec or 1 Hz

#include <stdio.h>	
#include <string.h>
#include "NUC1xx.h"
#include "LCD.h"
#include "GPIO.h"

#define  ONESHOT  0   // counting and interrupt when reach TCMPR number, then stop
#define  PERIODIC 1   // counting and interrupt when reach TCMPR number, then counting from 0 again
#define  TOGGLE   2   // keep counting and interrupt when reach TCMPR number, tout toggled (between 0 and 1)
#define  CONTINUOUS 3 // keep counting and interrupt when reach TCMPR number

static uint16_t Timer0Counter=0;
static uint16_t Timer1Counter=0;
static uint16_t Timer2Counter=0;
volatile uint32_t ledState = 0;

//---------------------------------------------------------------------------------TIMER
void InitTIMER0(void)
{
	/* Step 1. Enable and Select Timer clock source */          
	SYSCLK->CLKSEL1.TMR0_S = 0;	//Select 12Mhz for Timer0 clock source 
  SYSCLK->APBCLK.TMR0_EN = 1;	//Enable Timer0 clock source

	/* Step 2. Select Operation mode */	
	TIMER0->TCSR.MODE=PERIODIC;		//Select once mode for operation mode

	/* Step 3. Select Time out period = (Period of timer clock input) * (8-bit Prescale + 1) * (24-bit TCMP)*/
	TIMER0->TCSR.PRESCALE=255;	// Set Prescale [0~255]
	TIMER0->TCMPR = 46875;		// Set TCMPR [0~16777215]
								// (1/12000000)*(255+1)* 46875 = 1 sec / 1 Hz

	/* Step 4. Enable interrupt */
	TIMER0->TCSR.IE = 1;
	TIMER0->TISR.TIF = 1;		//Write 1 to clear for safty		
	NVIC_EnableIRQ(TMR0_IRQn);	//Enable Timer0 Interrupt

	/* Step 5. Enable Timer module */
	TIMER0->TCSR.CRST = 1;		//Reset up counter
	TIMER0->TCSR.CEN = 1;		//Enable Timer0

//  	TIMER0->TCSR.TDR_EN=1;		// Enable TDR function
}

void TMR0_IRQHandler(void) // Timer0 interrupt subroutine 
{
	char TEXT1[20]="Time Cnt0 :         ";
	Timer0Counter++;
	sprintf(TEXT1+11,"%d",Timer0Counter);
	print_Line(1, TEXT1);
	ledState = ~ ledState;  // changing ON/OFF state
	if(ledState) DrvGPIO_ClrBit(E_GPC,12);
	else	DrvGPIO_SetBit(E_GPC,12);
 	TIMER0->TISR.TIF =1; 	   
}

//---------------------------------------------------------------------------------TIMER
void InitTIMER1(void)
{
	/* Step 1. Enable and Select Timer clock source */          
	SYSCLK->CLKSEL1.TMR1_S = 0;	//Select 12Mhz for Timer1 clock source 
    SYSCLK->APBCLK.TMR1_EN =1;	//Enable Timer1 clock source

	/* Step 2. Select Operation mode */	
	TIMER1->TCSR.MODE=PERIODIC;		//Select periodic mode for operation mode

	/* Step 3. Select Time out period = (Period of timer clock input) * (8-bit Prescale + 1) * (24-bit TCMP)*/
	TIMER1->TCSR.PRESCALE=255;	// Set Prescale [0~255]
	TIMER1->TCMPR = 46875*2;		// Set TCMPR [0~16777215]								
								// (1/12000000)*(255+1)*46875 = 1 sec / 1 Hz

	/* Step 4. Enable interrupt */
	TIMER1->TCSR.IE = 1;
	TIMER1->TISR.TIF = 1;		//Write 1 to clear for safty		
	NVIC_EnableIRQ(TMR1_IRQn);	//Enable Timer1 Interrupt

	/* Step 5. Enable Timer module */
	TIMER1->TCSR.CRST = 1;		//Reset up counter
	TIMER1->TCSR.CEN = 1;		//Enable Timer1

//  	TIMER1->TCSR.TDR_EN=1;		// Enable TDR function
}

void TMR1_IRQHandler(void) // Timer1 interrupt subroutine 
{
	char TEXT2[20]="Time Cnt1 :         ";
	Timer1Counter+=1;
	sprintf(TEXT2+11,"%d",Timer1Counter);
	print_Line(2, TEXT2);
	ledState = ~ ledState;  // changing ON/OFF state
	if(ledState) DrvGPIO_ClrBit(E_GPC,13);
	else   DrvGPIO_SetBit(E_GPC,13);
 	TIMER1->TISR.TIF =1; 	   
}

void InitTIMER2(void)
{
	/* Step 1. Enable and Select Timer clock source */          
	SYSCLK->CLKSEL1.TMR2_S = 0;	//Select 12Mhz for Timer1 clock source 
    SYSCLK->APBCLK.TMR2_EN =1;	//Enable Timer1 clock source

	/* Step 2. Select Operation mode */	
	TIMER2->TCSR.MODE=PERIODIC;		//Select continuous mode for operation mode

	/* Step 3. Select Time out period = (Period of timer clock input) * (8-bit Prescale + 1) * (24-bit TCMP)*/
	TIMER2->TCSR.PRESCALE=255;	// Set Prescale [0~255]
	TIMER2->TCMPR = 46875*4;			// Set TCMPR [0~16777215]
								// (1/12000000)*(255+1)* (46875/625) = 1/625 sec or 625 Hz

	/* Step 4. Enable interrupt */
	TIMER2->TCSR.IE = 1;
	TIMER2->TISR.TIF = 1;		//Write 1 to clear for safty		
	NVIC_EnableIRQ(TMR2_IRQn);	//Enable Timer1 Interrupt

	/* Step 5. Enable Timer module */
	TIMER2->TCSR.CRST = 1;		//Reset up counter
	TIMER2->TCSR.CEN = 1;		//Enable Timer2

//  	TIMER2->TCSR.TDR_EN=1;		// Enable TDR function
}

void TMR2_IRQHandler(void) // Timer2 interrupt subroutine 
{
	char TEXT3[20]="Time Cnt2 :         ";
	Timer2Counter+=1;
	sprintf(TEXT3+11,"%d",Timer2Counter);
	print_Line(3, TEXT3);
	ledState = ~ ledState;  // changing ON/OFF state
	if(ledState)	DrvGPIO_ClrBit(E_GPC,14);
	else  DrvGPIO_SetBit(E_GPC,14);
 	TIMER2->TISR.TIF =1; 	   
}

int32_t main (void)
{
	UNLOCKREG();
	SYSCLK->PWRCON.XTL12M_EN = 1;//Enable 12MHz Crystal
	SYSCLK->CLKSEL0.HCLK_S = 0;
	LOCKREG();

	init_LCD(); 
	clear_LCD();
	
	print_Line(0,"Task3C_Timer");
	
	DrvGPIO_Open(E_GPC,12, E_IO_OUTPUT); // set GPC12 output for LED	
	InitTIMER0();
	
	DrvGPIO_Open(E_GPC,13, E_IO_OUTPUT); // set GPC13 output for LED
	InitTIMER1();
	
	DrvGPIO_Open(E_GPC,14, E_IO_OUTPUT); // set GPC14 output for LED
	InitTIMER2();		 	

	while(1)
	{
		__NOP(); // No Operation
	}
}
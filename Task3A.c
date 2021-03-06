#include <stdio.h>
#include "NUC1xx.h"
#include "GPIO.h"
#include "SYS.h"
#include "ADC.h"
#include "LCD.h"


volatile uint8_t gu8AdcIntFlag;

void AdcIntCallback(uint32_t u32UserData)
{
    gu8AdcIntFlag = 1;	
}

unsigned char bmp32x32[128] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x80,0xC0,0x40,0x20,0x30,0x10,0x18,0x18,0x08,0x08,0xFF,0x08,0x08,0x18,0x18,0x30,0x30,0x60,0xC0,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,
	0x80,0x80,0x80,0x80,0xF8,0x8E,0x81,0x81,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0xFF,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x81,0x87,0x9E,0xF8,0x80,0x80,0x80,
	0x01,0x01,0x01,0x01,0x0F,0x39,0xE1,0x81,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x81,0x61,0x3D,0x0F,0x00,0x01,0x01,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x02,0x06,0x04,0x0C,0x08,0x08,0x18,0xFF,0x08,0x08,0x08,0x0C,0x04,0x04,0x03,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00
};

int32_t main (void)
{
	int i, j;
	float y;
	char TEXT[4][16];
	uint16_t adc_value[8];


	UNLOCKREG();
	SYSCLK->PWRCON.XTL12M_EN = 1; // enable external clock (12MHz)
	SYSCLK->CLKSEL0.HCLK_S = 0;	  // select external clock (12MHz)
	LOCKREG();
		
	init_LCD();  // initialize LCD pannel
	clear_LCD();  // clear LCD panel 
	//print_Line(0,"Smpl_ADC_IRQ");
	
  // The maximum ADC clock rate is 16MHz
	// open 8 ADC channels for continously sampling
  DrvADC_Open(ADC_SINGLE_END, ADC_CONTINUOUS_OP, 0xFF, INTERNAL_HCLK, 1);
  DrvADC_EnableADCInt(AdcIntCallback, 0);

	i = 7;                 // ADC channel = 7 
	j = 6;                 // ADC channel = 6 
	gu8AdcIntFlag =0;      // reset AdcIntFlag
 	DrvADC_StartConvert(); // start ADC sampling
	
	while(1) {	
		while(gu8AdcIntFlag==0); // wait till AdcIntFlag is set by AdcIntCallback
		gu8AdcIntFlag=0;		     // reset AdcIntFlag
    adc_value[i] = DrvADC_GetConversionData(i);   // read ADC7 value from ADC registers
		adc_value[j] = DrvADC_GetConversionData(j);   // read ADC6 value from ADC registers
	
		//Replacing adc_value[i] as x-axis and adc_value[j] as y-axis
		adc_value[i] = adc_value[i] / 43 ;
		adc_value[j] = adc_value[j] / 135 ;
		draw_Bmp32x32(adc_value[i],adc_value[j],FG_COLOR,BG_COLOR,bmp32x32);
		DrvSYS_Delay(300000);
		clear_LCD();
		
	}
	//DrvADC_Close();	
}
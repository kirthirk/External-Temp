//*****************************************************************************
//
//Kirthi R Kulkarni
//Fan control using Temperature sensors.
//
//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
//*****************************************************************************
//
//In this example we control switching on/off of fan using the on board temp sensor and and external temperature sensor (using ADC).
//The on board temp sensor measures the ambient temperature; the external temp sensor is connected to pin E3.
//If external temp value is greater than or equal the internal temp sensor, a fan/LED is switched on;
//Also, if the ext temp sensor value is less than internal temp value; the fan/led is switched off.
//The temp and ADC values are displayed on the console using UART.
//*****************************************************************************
//
// This function sets up UART0 to be used for a console to display information
// as the example is running.
//
//*****************************************************************************
void
InitConsole(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

//*****************************************************************************
//
// Configure ADC0 for a single-ended input and a single sample.  Once the
// sample is ready, an interrupt flag will be set.  Using a polling method,
// the data will be read then displayed on the console via UART0.
//
//*****************************************************************************
int
main(void)
{
	int i;
    //
    // This array is used for storing the data read from the ADC FIFO. It
    // must be as large as the FIFO for the sequencer in use.  This example
    // uses sequence 3 which has a FIFO depth of 1.  If another sequence
    // was used with a deeper FIFO, then the array size must be changed.
    //
	volatile uint32_t ui32TempAvg1, ui32TempAvg2;
    uint32_t ui32ADC0Value2[8]; //External
   uint32_t ui32ADC0Value1[8]; //Internal
    //  volatile uint32_t ui32TempValueF1;
    volatile uint32_t ui32TempValueC1;//INTERNAL
    // volatile uint32_t ui32TempValueF2;
    volatile uint32_t ui32TempValueC2;//EXTERNAL
    //
    // Set the clocking to run at 20 MHz (200 MHz / 10) using the PLL.  When
    // using the ADC, you must either use the PLL or supply a 16 MHz clock
    // source.
    // TODO: The SYSCTL_XTAL_ value must be changed to match the value of the
    // crystal on your board.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);
    //
    // Set up the serial console to use for displaying messages.  This is
    // just for this example program and is not needed for ADC operation.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5);
	//Call the UART initialization fucntion
    InitConsole();
    //
    // Display the setup on the console.
    //
    UARTprintf("Temperature values using ADC ->\n");
    // The ADC0 peripheral must be enabled for use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    //
    // For this example ADC0 is used with AIN0 on port E3.
    // The actual port and pins used may be different on your part, consult
    // the data sheet for more information.  GPIO port E needs to be enabled
    // so these pins can be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    //
    // Select the analog ADC function for these pins.
    // Consult the data sheet to see which functions are allocated per pin.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);
    //
    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.  Each ADC module has 4 programmable sequences, sequence 0
    // to sequence 3.  This example is arbitrarily using sequence 3.
    //
    ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);
    //
    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH1) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // information on the ADC sequences and steps, reference the datasheet.
    //
    ADCSequenceStepConfigure(ADC1_BASE, 0, 0, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 1, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 2, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 3, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 4, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 5, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 6, ADC_CTL_CH0);
    ADCSequenceStepConfigure(ADC1_BASE, 0, 7, ADC_CTL_CH0| ADC_CTL_IE |ADC_CTL_END);//PE3
    //ADCSequenceEnable(ADC0_BASE, 0);//For External temp sensor
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 1, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 2, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 3, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 4, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 5, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 6, ADC_CTL_TS);
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0,  ADC_CTL_TS | ADC_CTL_IE |ADC_CTL_END);//For Internal temp sensor
    //
    // Since sample sequence 3 is now configured, it must be enabled.
    //
    ADCSequenceEnable(ADC1_BASE, 0);//External temp sensor
    ADCSequenceEnable(ADC0_BASE, 0);//Internal temp sensor
    //
    // Clear the interrupt status flag.  This is done to make sure the
    // interrupt flag is cleared before we sample.
    //
    ADCIntClear(ADC1_BASE, 0);
    ADCIntClear(ADC0_BASE, 0);
    //
    // Sample AIN0 forever.  Display the value on the console.
    //
while(1)
{
    ADCIntClear(ADC1_BASE, 0);
    ADCIntClear(ADC0_BASE, 0);
        //
        // Trigger the ADC conversion.
        //
        ADCProcessorTrigger(ADC1_BASE, 0);
        ADCProcessorTrigger(ADC0_BASE, 0);
        //
        // Wait for conversion to be completed.
        //
        while(!ADCIntStatus(ADC1_BASE, 0, false) && !ADCIntStatus(ADC0_BASE, 0, false))
        {
        }
        //
        // Clear the ADC interrupt flag.
        //
        //ADCIntClear(ADC0_BASE, 3);
        //
        // Read ADC Value.
        //
        ADCSequenceDataGet(ADC1_BASE, 0, ui32ADC0Value2);//external
        ADCSequenceDataGet(ADC0_BASE, 0, ui32ADC0Value1); //internal

    	//ui32TempValueF2 = ((ui32TempValueC * 9) + 160) / 5;
        ui32TempAvg2=(ui32ADC0Value2[0]+ui32ADC0Value2[1]+ui32ADC0Value2[2]+ui32ADC0Value2[3]+ui32ADC0Value2[4]+ui32ADC0Value2[5]+ui32ADC0Value2[6]+ui32ADC0Value2[7])/8;
        ui32TempAvg1=(ui32ADC0Value1[0]+ui32ADC0Value1[1]+ui32ADC0Value1[2]+ui32ADC0Value1[3]+ui32ADC0Value1[4]+ui32ADC0Value1[5]+ui32ADC0Value1[6]+ui32ADC0Value1[7])/8;
    	UARTprintf("\n ADC value 1: %d",ui32TempAvg1);
    	UARTprintf("\n ADC value 2: %d", ui32TempAvg2);
        //
        // Display the AIN0 (PE3) digital value on the console.
        //
    	ui32TempValueC1 = (1475 - (2475 * ui32TempAvg1) / 4096)/10;
        //ui32TempValueF1 = ((ui32TempValueC * 9) + 160) / 5;
    	ui32TempValueC2 = 100- (373-(330*ui32TempAvg2/4096));
        UARTprintf("\n1.Temperature in Celsuis of INTERNAL: %d \r", ui32TempValueC1);
        UARTprintf("\n2.Temperature in Celsius of EXTERNAL: %d \r\n", ui32TempValueC2);

        if(ui32TempAvg1>=ui32TempAvg2)
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_4|GPIO_PIN_5);
        else
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5,0);
        //Delay
        for(i=0;i<6;i++)
        {
        SysCtlDelay(2000000);
        }
        SysCtlDelay(200000);
}//end of while

}//end of main

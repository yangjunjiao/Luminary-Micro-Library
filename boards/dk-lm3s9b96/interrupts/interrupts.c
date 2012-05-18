//*****************************************************************************
//
// interrupts.c - Interrupt preemption and tail-chaining example.
//
// Copyright (c) 2008-2012 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 8555 of the DK-LM3S9B96 Firmware Package.
//
//*****************************************************************************

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include "drivers/kitronix320x240x16_ssd2119_8bit.h"
#include "drivers/set_pinout.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Interrupts (interrupts)</h1>
//!
//! This example application demonstrates the interrupt preemption and
//! tail-chaining capabilities of Cortex-M3 microprocessor and NVIC.  Nested
//! interrupts are synthesized when the interrupts have the same priority,
//! increasing priorities, and decreasing priorities.  With increasing
//! priorities, preemption will occur; in the other two cases tail-chaining
//! will occur.  The currently pending interrupts and the currently executing
//! interrupt will be displayed on the display; GPIO pins B3, F2 and F3 (the
//! GPIO on jumper JP40 and the two Ethernet LEDs) will be asserted upon
//! interrupt handler entry and de-asserted before interrupt handler exit so
//! that the off-to-on time can be observed with a scope or logic analyzer to
//! see the speed of tail-chaining (for the two cases where tail-chaining is
//! occurring).
//
//*****************************************************************************

//*****************************************************************************
//
// The count of interrupts received.  This is incremented as each interrupt
// handler runs, and its value saved into interrupt handler specific values to
// determine the order in which the interrupt handlers were executed.
//
//*****************************************************************************
volatile unsigned long g_ulIndex;

//*****************************************************************************
//
// The value of g_ulIndex when the INT_GPIOA interrupt was processed.
//
//*****************************************************************************
volatile unsigned long g_ulGPIOa;

//*****************************************************************************
//
// The value of g_ulIndex when the INT_GPIOB interrupt was processed.
//
//*****************************************************************************
volatile unsigned long g_ulGPIOb;

//*****************************************************************************
//
// The value of g_ulIndex when the INT_GPIOC interrupt was processed.
//
//*****************************************************************************
volatile unsigned long g_ulGPIOc;

//*****************************************************************************
//
// Graphics context used to show text on the CSTN display.
//
//*****************************************************************************
tContext g_sContext;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
}
#endif

//*****************************************************************************
//
// Delay for the specified number of seconds.  Depending upon the current
// SysTick value, the delay will be between N-1 and N seconds (i.e. N-1 full
// seconds are guaranteed, along with the remainder of the current second).
//
//*****************************************************************************
void
Delay(unsigned long ulSeconds)
{
    //
    // Loop while there are more seconds to wait.
    //
    while(ulSeconds--)
    {
        //
        // Wait until the SysTick value is less than 1000.
        //
        while(ROM_SysTickValueGet() > 1000)
        {
        }

        //
        // Wait until the SysTick value is greater than 1000.
        //
        while(ROM_SysTickValueGet() < 1000)
        {
        }
    }
}

//*****************************************************************************
//
// Display the interrupt state on the CSTN.  The currently active and pending
// interrupts are displayed.
//
//*****************************************************************************
void
DisplayIntStatus(void)
{
    unsigned long ulTemp;
    char pcBuffer[6];

    //
    // Display the currently active interrupts.
    //
    ulTemp = HWREG(NVIC_ACTIVE0);
    pcBuffer[0] = ' ';
    pcBuffer[1] = (ulTemp & 1) ? '1' : ' ';
    pcBuffer[2] = (ulTemp & 2) ? '2' : ' ';
    pcBuffer[3] = (ulTemp & 4) ? '3' : ' ';
    pcBuffer[4] = ' ';
    pcBuffer[5] = '\0';
    GrStringDrawCentered(&g_sContext, pcBuffer, -1, 130, 150, 1);

    //
    // Display the currently pending interrupts.
    //
    ulTemp = HWREG(NVIC_PEND0);
    pcBuffer[1] = (ulTemp & 1) ? '1' : ' ';
    pcBuffer[2] = (ulTemp & 2) ? '2' : ' ';
    pcBuffer[3] = (ulTemp & 4) ? '3' : ' ';
    GrStringDrawCentered(&g_sContext, pcBuffer, -1, 270, 150, 1);

    //
    // Flush the display.
    //
    GrFlush(&g_sContext);
}

//*****************************************************************************
//
// This is the handler for INT_GPIOA.  It simply saves the interrupt sequence
// number.
//
//*****************************************************************************
void
IntGPIOa(void)
{
    //
    // Set PB3 high to indicate entry to this interrupt handler.
    //
    ROM_GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_PIN_3);

    //
    // Put the current interrupt state on the CSTN.
    //
    DisplayIntStatus();

    //
    // Wait two seconds.
    //
    Delay(2);

    //
    // Save and increment the interrupt sequence number.
    //
    g_ulGPIOa = g_ulIndex++;

    //
    // Set PB3 low to indicate exit from this interrupt handler.
    //
    ROM_GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0);
}

//*****************************************************************************
//
// This is the handler for INT_GPIOB.  It triggers INT_GPIOA and saves the
// interrupt sequence number.
//
//*****************************************************************************
void
IntGPIOb(void)
{
    //
    // Set PF2 high to indicate entry to this interrupt handler.
    //
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);

    //
    // Put the current interrupt state on the CSTN.
    //
    DisplayIntStatus();

    //
    // Trigger the INT_GPIOA interrupt.
    //
    HWREG(NVIC_SW_TRIG) = INT_GPIOA - 16;

    //
    // Put the current interrupt state on the CSTN.
    //
    DisplayIntStatus();

    //
    // Wait two seconds.
    //
    Delay(2);

    //
    // Save and increment the interrupt sequence number.
    //
    g_ulGPIOb = g_ulIndex++;

    //
    // Set PDF2 low to indicate exit from this interrupt handler.
    //
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0);
}

//*****************************************************************************
//
// This is the handler for INT_GPIOC.  It triggers INT_GPIOB and saves the
// interrupt sequence number.
//
//*****************************************************************************
void
IntGPIOc(void)
{
    //
    // Set PF3 high to indicate entry to this interrupt handler.
    //
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);

    //
    // Put the current interrupt state on the CSTN.
    //
    DisplayIntStatus();

    //
    // Trigger the INT_GPIOB interrupt.
    //
    HWREG(NVIC_SW_TRIG) = INT_GPIOB - 16;

    //
    // Put the current interrupt state on the CSTN.
    //
    DisplayIntStatus();

    //
    // Wait two seconds.
    //
    Delay(2);

    //
    // Save and increment the interrupt sequence number.
    //
    g_ulGPIOc = g_ulIndex++;

    //
    // Set PF3 low to indicate exit from this interrupt handler.
    //
    ROM_GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
}

//*****************************************************************************
//
// This is the main example program.  It checks to see that the interrupts are
// processed in the correct order when they have identical priorities,
// increasing priorities, and decreasing priorities.  This exercises interrupt
// preemption and tail chaining.
//
//*****************************************************************************
int
main(void)
{
    tRectangle sRect;
    unsigned long ulError;

    //
    // Set the clocking to run directly from the crystal.
    //
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    //
    // Set the device pinout appropriately for this board.
    //
    PinoutSet();

    //
    // Enable the peripherals used by this example.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    //
    // Initialize the display driver.
    //
    Kitronix320x240x16_SSD2119Init();

    //
    // Initialize the graphics context and find the middle X coordinate.
    //
    GrContextInit(&g_sContext, &g_sKitronix320x240x16_SSD2119);

    //
    // Fill the top 15 rows of the screen with blue to create the banner.
    //
    sRect.sXMin = 0;
    sRect.sYMin = 0;
    sRect.sXMax = GrContextDpyWidthGet(&g_sContext) - 1;
    sRect.sYMax = 23;
    GrContextForegroundSet(&g_sContext, ClrDarkBlue);
    GrRectFill(&g_sContext, &sRect);

    //
    // Put a white box around the banner.
    //
    GrContextForegroundSet(&g_sContext, ClrWhite);
    GrRectDraw(&g_sContext, &sRect);

    //
    // Put the application name in the middle of the banner.
    //
    GrContextFontSet(&g_sContext, g_pFontCm20);
    GrStringDrawCentered(&g_sContext, "interrupts", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 11, 0);
    GrContextFontSet(&g_sContext, g_pFontCmss22b);

    //
    // Put the status header text on the display.
    //
    GrStringDrawCentered(&g_sContext, "Active:      Pending:     ", -1,
                 GrContextDpyWidthGet(&g_sContext) / 2, 150, 0);

    //
    // Configure the B3, F2 and F3 to be outputs to indicate entry/exit of one
    // of the interrupt handlers.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0);

    //
    // Set up and enable the SysTick timer.  It will be used as a reference
    // for delay loops in the interrupt handlers.  The SysTick timer period
    // will be set up for one second.
    //
    ROM_SysTickPeriodSet(ROM_SysCtlClockGet());
    ROM_SysTickEnable();

    //
    // Reset the error indicator.
    //
    ulError = 0;

    //
    // Enable interrupts to the processor.
    //
    ROM_IntMasterEnable();

    //
    // Enable the interrupts.
    //
    ROM_IntEnable(INT_GPIOA);
    ROM_IntEnable(INT_GPIOB);
    ROM_IntEnable(INT_GPIOC);

    //
    // Indicate that the equal interrupt priority test is beginning.
    //
    GrStringDrawCentered(&g_sContext, "Equal Priority", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 60, 1);

    //
    // Set the interrupt priorities so they are all equal.
    //
    ROM_IntPrioritySet(INT_GPIOA, 0x00);
    ROM_IntPrioritySet(INT_GPIOB, 0x00);
    ROM_IntPrioritySet(INT_GPIOC, 0x00);

    //
    // Reset the interrupt flags.
    //
    g_ulGPIOa = 0;
    g_ulGPIOb = 0;
    g_ulGPIOc = 0;
    g_ulIndex = 1;

    //
    // Trigger the interrupt for GPIO C.
    //
    HWREG(NVIC_SW_TRIG) = INT_GPIOC - 16;

    //
    // Put the current interrupt state on the LCD.
    //
    DisplayIntStatus();

    //
    // Verify that the interrupts were processed in the correct order.
    //
    if((g_ulGPIOa != 3) || (g_ulGPIOb != 2) || (g_ulGPIOc != 1))
    {
        ulError |= 1;
    }

    //
    // Wait two seconds.
    //
    Delay(2);

    //
    // Indicate that the decreasing interrupt priority test is beginning.
    //
    GrStringDrawCentered(&g_sContext, " Decreasing Priority ", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 60, 1);

    //
    // Set the interrupt priorities so that they are decreasing (i.e. C > B >
    // A).
    //
    ROM_IntPrioritySet(INT_GPIOA, 0x80);
    ROM_IntPrioritySet(INT_GPIOB, 0x40);
    ROM_IntPrioritySet(INT_GPIOC, 0x00);

    //
    // Reset the interrupt flags.
    //
    g_ulGPIOa = 0;
    g_ulGPIOb = 0;
    g_ulGPIOc = 0;
    g_ulIndex = 1;

    //
    // Trigger the interrupt for GPIO C.
    //
    HWREG(NVIC_SW_TRIG) = INT_GPIOC - 16;

    //
    // Put the current interrupt state on the CSTN.
    //
    DisplayIntStatus();

    //
    // Verify that the interrupts were processed in the correct order.
    //
    if((g_ulGPIOa != 3) || (g_ulGPIOb != 2) || (g_ulGPIOc != 1))
    {
        ulError |= 2;
    }

    //
    // Wait two seconds.
    //
    Delay(2);

    //
    // Indicate that the increasing interrupt priority test is beginning.
    //
    GrStringDrawCentered(&g_sContext, " Increasing Priority ", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 60, 1);

    //
    // Set the interrupt priorities so that they are increasing (i.e. C < B <
    // A).
    //
    ROM_IntPrioritySet(INT_GPIOA, 0x00);
    ROM_IntPrioritySet(INT_GPIOB, 0x40);
    ROM_IntPrioritySet(INT_GPIOC, 0x80);

    //
    // Reset the interrupt flags.
    //
    g_ulGPIOa = 0;
    g_ulGPIOb = 0;
    g_ulGPIOc = 0;
    g_ulIndex = 1;

    //
    // Trigger the interrupt for GPIO C.
    //
    HWREG(NVIC_SW_TRIG) = INT_GPIOC - 16;

    //
    // Put the current interrupt state on the CSTN.
    //
    DisplayIntStatus();

    //
    // Verify that the interrupts were processed in the correct order.
    //
    if((g_ulGPIOa != 1) || (g_ulGPIOb != 2) || (g_ulGPIOc != 3))
    {
        ulError |= 4;
    }

    //
    // Wait two seconds.
    //
    Delay(2);

    //
    // Disable the interrupts.
    //
    ROM_IntDisable(INT_GPIOA);
    ROM_IntDisable(INT_GPIOB);
    ROM_IntDisable(INT_GPIOC);

    //
    // Disable interrupts to the processor.
    //
    ROM_IntMasterDisable();

    //
    // Print out the test results.
    //
    GrStringDrawCentered(&g_sContext, " Interrupt Priority ", -1,
                         GrContextDpyWidthGet(&g_sContext) / 2, 60, 1);
    if(ulError)
    {
        GrStringDrawCentered(&g_sContext, "     Equal: P  Inc: P  Dec: P     ",
                             -1, GrContextDpyWidthGet(&g_sContext) / 2, 150, 1);
        if(ulError & 1)
        {
            GrStringDrawCentered(&g_sContext, " F ", -1, 113, 150, 1);
        }
        if(ulError & 2)
        {
            GrStringDrawCentered(&g_sContext, " F ", -1, 187, 150, 1);
        }
        if(ulError & 4)
        {
            GrStringDrawCentered(&g_sContext, " F ", -1, 272, 150, 1);
        }
    }
    else
    {
        GrStringDrawCentered(&g_sContext, "           Success!           ", -1,
                             GrContextDpyWidthGet(&g_sContext) / 2, 150, 1);
    }

    //
    // Flush the display.
    //
    GrFlush(&g_sContext);

    //
    // Loop forever.
    //
    while(1)
    {
    }
}

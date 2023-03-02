#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/hw_memmap.h" //Memory mapping of the used device 
#include "driverlib/fpu.h" //Routines for manipulating the floating-point unit in the Cortex
#include "driverlib/sysctl.h" //Driver for the system controller
#include "driverlib/rom.h"                //Macros to facilitate calling functions in the RO
#include "driverlib/pin_map.h" //Mapping of peripherals to pins for all parts
#include "driverlib/uart.h"         // Driver for the UART
#include "grlib/grlib.h"        //Prototypes for the low level primitives provided by the gr
#include "drivers/ili9341_240x320x262K.h" //Display driver for the MULTI-INNO TECHNOLOGY
#include "utils/uartstdio.h"                //Prototypes for the UART console functions.
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"
#include "drivers/touch.h"



#define GPIO_PINS_ALL GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7



//*****************************************************************************
//
// Global system tick counter holds elapsed time since the application started
// expressed in 100ths of a second.
//
//*****************************************************************************
volatile uint32_t g_ui32SysTickCount;

int_fast32_t i32CenterX, i32CenterY;
//int i32CenterX_temp = 100;
//int i32CenterY_temp = 100;

tContext sContext;
tRectangle sRect;
unsigned long color = ClrBlack;
bool type = false;
long size = 4;

//*****************************************************************************
//
// This is the interrupt handler for the SysTick interrupt.  It is used to
// update our local tick count which, in turn, is used to check for transmit
// timeouts.
//
//*****************************************************************************
void
SysTickIntHandler(void)
{
    g_ui32SysTickCount++;
}


//*****************************************************************************
//
//! Callback for calibration process
//!
//! \param X - x coordinate of point to check
//! \param Y - y coordinate of point to check
//! \param cX - x coordinate of circle center
//! \param cY - y coordinate of circle center
//! \param radius - circle radius
//!
//! \return boolean true if coordinates are within circle
//
//*****************************************************************************
inline bool IsInCircle(int32_t X,int32_t Y,int32_t cX,int32_t cY,int32_t radius)
{
        return ( (X-cX)*(X-cX) + (Y-cY)*(Y-cY) < (radius*radius) );
}



//*****************************************************************************
//
//! Callback for calibration process
//!
//! \param ulMessage is type of event
//! \param lX is a x location of touch
//! \param lY is a y location of cross center
//!
//! \return None.
//
//*****************************************************************************


unsigned long Kolor_narzedzia(unsigned long color, int32_t X, int32_t Y) 
{
        if((X > 0 && X < 40) && (Y > 200 && Y < 240)) {
        return ClrBlack;
    }

    if((X > 40 && X < 80) && (Y > 200 && Y < 240)) {
        return ClrBlue;
    }

    if((X > 80 && X < 120) && (Y > 200 && Y < 240)) {
        return ClrGreen;
    }

    if((X > 120 && X < 160) && (Y > 200 && Y < 240)) {
        return ClrRed;
    }

    if((X > 280 && X < 320) && (Y > 200 && Y < 240)) {
        return ClrWhite;
    }


    return color;
}

bool narzedzie(bool type, int32_t X, int32_t Y) {
    
    if((X > 240 && X < 280) && (Y > 200 && Y < 240)) {
        return true;
    }
    
    if((X > 200 && X < 240) && (Y > 200 && Y < 240)) {
        return false;
    }
    
    return type;
}
void rozmiar(int32_t X, int32_t Y) {
    if ((X > 180 && X < 200) && (Y > 200 && Y < 240)) {
        if(size > 1)
            size -= 1;
    }
    
    if ((X > 160 && X < 180) && (Y > 200 && Y < 240)) {
        if (size < 20)
            size += 1;
    }
}

int32_t TouchCallback(uint32_t ulMessage, int32_t lX, int32_t lY)
{

    color = Kolor_narzedzia(color, lX, lY);
    type = narzedzie(type, lX, lY);
    rozmiar(lX, lY);

    if(lY > 0 && lY < 200) {
        if(type) {
            sRect.i16XMin = lX - size;
            sRect.i16YMin = lY - size;
            sRect.i16XMax = lX + size;
            sRect.i16YMax = lY + size;
            GrContextForegroundSet(&sContext, color);
            GrRectFill(&sContext, &sRect);
        }
        else if(!type) {
            GrContextForegroundSet(&sContext, color);
            GrCircleFill(&sContext, lX, lY, size);
        }
        
    }

    

        return 0;
}


int
main(void)
{

    
    ROM_FPULazyStackingEnable();
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_8MHZ);

    //
    // Initialize the display driver.
    //
    ILI9341_240x320x262K_Init();

    //
    // Initialize the graphics context.
    //
    GrContextInit(&sContext, &g_sILI9341_240x320x262K);

    //
    // Initialize touchscreen driver
    //
    TouchScreenInit();
    TouchScreenCalibrate(&sContext);
    TouchScreenCallbackSet(TouchCallback);

    GrContextFontSet(&sContext, g_psFontCm12);

    
    //Miejsce do rysowania
    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = 320;
    sRect.i16YMax = 200;
    GrContextForegroundSet(&sContext, ClrWhite);
    GrRectFill(&sContext, &sRect);
    //Czarny kolor
    sRect.i16XMin = 0;
    sRect.i16YMin = 200;
    sRect.i16XMax = 40;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
    //Niebieski kolor
    sRect.i16XMin = 40;
    sRect.i16YMin = 200;
    sRect.i16XMax = 80;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrBlue);
    GrRectFill(&sContext, &sRect);
    //Zielony
    sRect.i16XMin = 80;
    sRect.i16YMin = 200;
    sRect.i16XMax = 120;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrGreen);
    GrRectFill(&sContext, &sRect);
    //Czerwony
    sRect.i16XMin = 120;
    sRect.i16YMin = 200;
    sRect.i16XMax = 160;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrRed);
    GrRectFill(&sContext, &sRect);
    //Zmiana + -
    sRect.i16XMin = 160;
    sRect.i16YMin = 200;
    sRect.i16XMax = 200;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrWhite);
    GrRectFill(&sContext, &sRect);
    GrStringDrawCentered(&sContext, "+", 160, 170, 220, 240);
    GrStringDrawCentered(&sContext, "-", 160, 190, 220, 240);
    //ramka
    sRect.i16XMin = 160;
    sRect.i16YMin = 200;
    sRect.i16XMax = 161;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
    sRect.i16XMin = 199;
    sRect.i16YMin = 200;
    sRect.i16XMax = 200;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
    sRect.i16XMin = 180;
    sRect.i16YMin = 200;
    sRect.i16XMax = 181;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
    sRect.i16XMin = 160;
    sRect.i16YMin = 200;
    sRect.i16XMax = 200;
    sRect.i16YMax = 201;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
    //ksztalt kolo
    sRect.i16XMin = 200;
    sRect.i16YMin = 200;
    sRect.i16XMax = 240;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrWhite);
    GrRectFill(&sContext, &sRect);
    sRect.i16XMin = 200;
    sRect.i16YMin = 200;
    sRect.i16XMax = 240;
    sRect.i16YMax = 201;
    
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);

    GrContextForegroundSet(&sContext, ClrBlack);
    GrCircleFill(&sContext, 220,220 , 10);
    //ksztalt kwadrat
    sRect.i16XMin = 240;
    sRect.i16YMin = 200;
    sRect.i16XMax = 280;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrWhite);
    GrRectFill(&sContext, &sRect);
    sRect.i16XMin = 250;
    sRect.i16YMin = 210;
    sRect.i16XMax = 270;
    sRect.i16YMax = 230;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
    sRect.i16XMin = 240;
    sRect.i16YMin = 200;
    sRect.i16XMax = 280;
    sRect.i16YMax = 201;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
    sRect.i16XMin = 240;
    sRect.i16YMin = 200;
    sRect.i16XMax = 241;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
    //gumka
     sRect.i16XMin = 280;
    sRect.i16YMin = 200;
    sRect.i16XMax = 320;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrWhite);
    GrRectFill(&sContext, &sRect);
    sRect.i16XMin = 280;
    sRect.i16YMin = 200;
    sRect.i16XMax = 320;
    sRect.i16YMax = 201;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
    sRect.i16XMin = 280;
    sRect.i16YMin = 200;
    sRect.i16XMax = 281;
    sRect.i16YMax = 240;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
    

    
    

    while(1)
    {

    }

}

//*****************************************************************************
//
// grlib_demo.c - Demonstration of the Stellaris Graphics Library.
//
// Copyright (c) 2008-2009 Luminary Micro, Inc.  All rights reserved.
// Software License Agreement
// 
// Luminary Micro, Inc. (LMI) is supplying this software for use solely and
// exclusively on LMI's microcontroller products.
// 
// The software is owned by LMI and/or its suppliers, and is protected under
// applicable copyright laws.  All rights are reserved.  You may not combine
// this software with "viral" open-source software in order to form a larger
// program.  Any use in violation of the foregoing restrictions may subject
// the user to criminal sanctions under applicable laws, as well as to civil
// liability for the breach of the terms and conditions of this license.
// 
// THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
// OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
// LMI SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
// CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 4905 of the DK-LM3S9B96 Firmware Package.
//
//*****************************************************************************
#include <stdio.h>
#include <string.h>
#include "hw_memmap.h"
#include "hw_nvic.h"
#include "hw_sysctl.h"
#include "hw_types.h"
#include "gpio.h"
#include "flash.h"
#include "sysctl.h"
#include "systick.h"
#include "uart.h"
#include "udma.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "grlib/canvas.h"
#include "grlib/checkbox.h"
#include "grlib/container.h"
#include "grlib/pushbutton.h"
#include "grlib/radiobutton.h"
#include "grlib/slider.h"
#include "kitronix320x240x16_ssd2119_8bit.h"
#include "sound.h"
#include "touch.h"
#include "set_pinout.h"
#include "images.h"

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Graphics Library Demonstration (grlib_demo)</h1>
//!
//! This application provides a demonstration of the capabilities of the
//! Stellaris Graphics Library.  A series of panels show different features of
//! the library.  For each panel, the bottom provides a forward and back button
//! (when appropriate), along with a brief description of the contents of the
//! panel.
//!
//! The first panel provides some introductory text and basic instructions for
//! operation of the application.
//!
//! The second panel shows the available drawing primitives: lines, circles,
//! rectangles, strings, and images.
//!
//! The third panel shows the canvas widget, which provides a general drawing
//! surface within the widget hierarchy.  A text, image, and application-drawn
//! canvas are displayed.
//!
//! The fourth panel shows the check box widget, which provides a means of
//! toggling the state of an item.  Four check boxes are provided, with each
//! having a red ``LED'' to the right.  The state of the LED tracks the state
//! of the check box via an application callback.
//!
//! The fifth panel shows the container widget, which provides a grouping
//! construct typically used for radio buttons.  Containers with a title, a
//! centered title, and no title are displayed.
//!
//! The sixth panel shows the push button widget.  Two columns of push buttons
//! are provided; the appearance of each column is the same but the left column
//! does not utilize auto-repeat while the right column does.  Each push button
//! has a red ``LED'' to its left, which is toggled via an application callback
//! each time the push button is pressed.
//!
//! The seventh panel shows the radio button widget.  Two groups of radio
//! buttons are displayed, the first using text and the second using images for
//! the selection value.  Each radio button has a red ``LED'' to its right,
//! which tracks the selection state of the radio buttons via an application
//! callback.  Only one radio button from each group can be selected at a time,
//! though the radio buttons in each group operate independently.
//!
//! The eighth and final panel shows the slider widget.  Six sliders
//! constructed using the various supported style options are shown.  The
//! slider value callback is used to update two widgets to reflect the values
//! reported by sliders.  A canvas widget near the top right of the display
//! tracks the value of the red and green image-based slider to its left and
//! the text of the grey slider on the left side of the panel is update to show
//! its own value.  The slider on the right is configured as an indicator
//! which tracks the state of the upper slider and ignores user input.
//
//*****************************************************************************

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
// The DMA control structure table.
//
//*****************************************************************************
#ifdef ewarm
#pragma data_alignment=1024
tDMAControlTable sDMAControlTable[64];
#pragma data_alignment=4
#else
tDMAControlTable sDMAControlTable[64] __attribute__ ((aligned(1024)));
#endif

//*****************************************************************************
//
// The sound effect that is played when a key is pressed.
//
//*****************************************************************************
static const unsigned short g_pusKeyClick[] =
{
    0, G5,
    25, SILENCE
};

//*****************************************************************************
//
// Forward declarations for the globals required to define the widgets at
// compile-time.
//
//*****************************************************************************
void OnPrevious(tWidget *pWidget);
void OnNext(tWidget *pWidget);
void OnIntroPaint(tWidget *pWidget, tContext *pContext);
void OnPrimitivePaint(tWidget *pWidget, tContext *pContext);
void OnCanvasPaint(tWidget *pWidget, tContext *pContext);
void OnCheckChange(tWidget *pWidget, unsigned long bSelected);
void OnButtonPress(tWidget *pWidget);
void OnRadioChange(tWidget *pWidget, unsigned long bSelected);
void OnSliderChange(tWidget *pWidget, long lValue);
extern tCanvasWidget g_psPanels[];

//*****************************************************************************
//
// The first panel, which contains introductory text explaining the
// application.
//
//*****************************************************************************
Canvas(g_sIntroduction, g_psPanels, 0, 0, &g_sKitronix320x240x16_SSD2119, 0, 24,
       320, 166, CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0, OnIntroPaint);

//*****************************************************************************
//
// The second panel, which demonstrates the graphics primitives.
//
//*****************************************************************************
Canvas(g_sPrimitives, g_psPanels + 1, 0, 0, &g_sKitronix320x240x16_SSD2119, 0,
       24, 320, 166, CANVAS_STYLE_APP_DRAWN, 0, 0, 0, 0, 0, 0,
       OnPrimitivePaint);

//*****************************************************************************
//
// The third panel, which demonstrates the canvas widget.
//
//*****************************************************************************
Canvas(g_sCanvas3, g_psPanels + 2, 0, 0, &g_sKitronix320x240x16_SSD2119, 205,
       27, 110, 158, CANVAS_STYLE_OUTLINE | CANVAS_STYLE_APP_DRAWN, 0, ClrGray,
       0, 0, 0, 0, OnCanvasPaint);
Canvas(g_sCanvas2, g_psPanels + 2, &g_sCanvas3, 0,
       &g_sKitronix320x240x16_SSD2119, 5, 109, 195, 76,
       CANVAS_STYLE_OUTLINE | CANVAS_STYLE_IMG, 0, ClrGray, 0, 0, 0, g_pucLogo,
       0);
Canvas(g_sCanvas1, g_psPanels + 2, &g_sCanvas2, 0,
       &g_sKitronix320x240x16_SSD2119, 5, 27, 195, 76,
       CANVAS_STYLE_FILL | CANVAS_STYLE_OUTLINE | CANVAS_STYLE_TEXT,
       ClrMidnightBlue, ClrGray, ClrSilver, &g_sFontCm22, "Text", 0, 0);

//*****************************************************************************
//
// The fourth panel, which demonstrates the checkbox widget.
//
//*****************************************************************************
tCanvasWidget g_psCheckBoxIndicators[] =
{
    CanvasStruct(g_psPanels + 3, g_psCheckBoxIndicators + 1, 0,
                 &g_sKitronix320x240x16_SSD2119, 230, 30, 50, 42,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psPanels + 3, g_psCheckBoxIndicators + 2, 0,
                 &g_sKitronix320x240x16_SSD2119, 230, 82, 50, 48,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psPanels + 3, 0, 0,
                 &g_sKitronix320x240x16_SSD2119, 230, 134, 50, 42,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0)
};
tCheckBoxWidget g_psCheckBoxes[] =
{
    CheckBoxStruct(g_psPanels + 3, g_psCheckBoxes + 1, 0,
                   &g_sKitronix320x240x16_SSD2119, 40, 30, 185, 42,
                   CB_STYLE_OUTLINE | CB_STYLE_FILL | CB_STYLE_TEXT, 16,
                   ClrMidnightBlue, ClrGray, ClrSilver, &g_sFontCm22, "Select",
                   0, OnCheckChange),
    CheckBoxStruct(g_psPanels + 3, g_psCheckBoxes + 2, 0,
                   &g_sKitronix320x240x16_SSD2119, 40, 82, 185, 48,
                   CB_STYLE_IMG, 16, 0, ClrGray, 0, 0, 0, g_pucLogo,
                   OnCheckChange),
    CheckBoxStruct(g_psPanels + 3, g_psCheckBoxIndicators, 0,
                   &g_sKitronix320x240x16_SSD2119, 40, 134, 189, 42,
                   CB_STYLE_OUTLINE | CB_STYLE_TEXT, 16,
                   0, ClrGray, ClrGreen, &g_sFontCm20, "Select",
                   0, OnCheckChange),
};
#define NUM_CHECK_BOXES         (sizeof(g_psCheckBoxes) /   \
                                 sizeof(g_psCheckBoxes[0]))

//*****************************************************************************
//
// The fifth panel, which demonstrates the container widget.
//
//*****************************************************************************
Container(g_sContainer3, g_psPanels + 4, 0, 0, &g_sKitronix320x240x16_SSD2119,
          210, 47, 105, 118, CTR_STYLE_OUTLINE | CTR_STYLE_FILL,
          ClrMidnightBlue, ClrGray, 0, 0, 0);
Container(g_sContainer2, g_psPanels + 4, &g_sContainer3, 0,
          &g_sKitronix320x240x16_SSD2119, 5, 109, 200, 76,
          (CTR_STYLE_OUTLINE | CTR_STYLE_FILL | CTR_STYLE_TEXT |
           CTR_STYLE_TEXT_CENTER), ClrMidnightBlue, ClrGray, ClrSilver,
          &g_sFontCm22, "Group2");
Container(g_sContainer1, g_psPanels + 4, &g_sContainer2, 0,
          &g_sKitronix320x240x16_SSD2119, 5, 27, 200, 76,
          CTR_STYLE_OUTLINE | CTR_STYLE_FILL | CTR_STYLE_TEXT, ClrMidnightBlue,
          ClrGray, ClrSilver, &g_sFontCm22, "Group1");

//*****************************************************************************
//
// The sixth panel, which contains a selection of push buttons.
//
//*****************************************************************************
tCanvasWidget g_psPushButtonIndicators[] =
{
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 1, 0,
                 &g_sKitronix320x240x16_SSD2119, 40, 85, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 2, 0,
                 &g_sKitronix320x240x16_SSD2119, 90, 85, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 3, 0,
                 &g_sKitronix320x240x16_SSD2119, 145, 85, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 4, 0,
                 &g_sKitronix320x240x16_SSD2119, 40, 165, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 5, 0,
                 &g_sKitronix320x240x16_SSD2119, 90, 165, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 6, 0,
                 &g_sKitronix320x240x16_SSD2119, 145, 165, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 7, 0,
                 &g_sKitronix320x240x16_SSD2119, 190, 35, 110, 24,
                 CANVAS_STYLE_TEXT, 0, 0, ClrSilver, &g_sFontCm20, "Non-auto",
                 0, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 8, 0,
                 &g_sKitronix320x240x16_SSD2119, 190, 55, 110, 24,
                 CANVAS_STYLE_TEXT, 0, 0, ClrSilver, &g_sFontCm20, "repeat",
                 0, 0),
    CanvasStruct(g_psPanels + 5, g_psPushButtonIndicators + 9, 0,
                 &g_sKitronix320x240x16_SSD2119, 190, 115, 110, 24,
                 CANVAS_STYLE_TEXT, 0, 0, ClrSilver, &g_sFontCm20, "Auto",
                 0, 0),
    CanvasStruct(g_psPanels + 5, 0, 0,
                 &g_sKitronix320x240x16_SSD2119, 190, 135, 110, 24,
                 CANVAS_STYLE_TEXT, 0, 0, ClrSilver, &g_sFontCm20, "repeat",
                 0, 0),
};
tPushButtonWidget g_psPushButtons[] =
{
    RectangularButtonStruct(g_psPanels + 5, g_psPushButtons + 1, 0,
                            &g_sKitronix320x240x16_SSD2119, 30, 35, 40, 40,
                            PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT,
                            ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                            &g_sFontCm22, "1", 0, 0, 0, 0, OnButtonPress),
    CircularButtonStruct(g_psPanels + 5, g_psPushButtons + 2, 0,
                         &g_sKitronix320x240x16_SSD2119, 100, 55, 20,
                         PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT,
                         ClrMidnightBlue, ClrBlack, ClrGray, ClrSilver,
                         &g_sFontCm22, "3", 0, 0, 0, 0, OnButtonPress),
    RectangularButtonStruct(g_psPanels + 5, g_psPushButtons + 3, 0,
                            &g_sKitronix320x240x16_SSD2119, 130, 30, 50, 50,
                            PB_STYLE_IMG | PB_STYLE_TEXT, 0, 0, 0, ClrSilver,
                            &g_sFontCm22, "5", g_pucBlue50x50,
                            g_pucBlue50x50Press, 0, 0, OnButtonPress),
    RectangularButtonStruct(g_psPanels + 5, g_psPushButtons + 4, 0,
                            &g_sKitronix320x240x16_SSD2119, 30, 115, 40, 40,
                            (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT |
                             PB_STYLE_AUTO_REPEAT), ClrMidnightBlue, ClrBlack,
                            ClrGray, ClrSilver, &g_sFontCm22, "2", 0, 0, 125,
                            25, OnButtonPress),
    CircularButtonStruct(g_psPanels + 5, g_psPushButtons + 5, 0,
                         &g_sKitronix320x240x16_SSD2119, 100, 135, 20,
                         (PB_STYLE_FILL | PB_STYLE_OUTLINE | PB_STYLE_TEXT |
                          PB_STYLE_AUTO_REPEAT), ClrMidnightBlue, ClrBlack,
                         ClrGray, ClrSilver, &g_sFontCm22, "4", 0, 0, 125, 25,
                         OnButtonPress),
    RectangularButtonStruct(g_psPanels + 5, g_psPushButtonIndicators, 0,
                            &g_sKitronix320x240x16_SSD2119, 130, 110, 50, 50,
                            (PB_STYLE_IMG | PB_STYLE_TEXT |
                             PB_STYLE_AUTO_REPEAT), 0, 0, 0, ClrSilver,
                            &g_sFontCm22, "6", g_pucBlue50x50,
                            g_pucBlue50x50Press, 125, 25, OnButtonPress),
};
#define NUM_PUSH_BUTTONS        (sizeof(g_psPushButtons) /   \
                                 sizeof(g_psPushButtons[0]))
unsigned long g_ulButtonState;

//*****************************************************************************
//
// The seventh panel, which contains a selection of radio buttons.
//
//*****************************************************************************
tContainerWidget g_psRadioContainers[];
tCanvasWidget g_psRadioButtonIndicators[] =
{
    CanvasStruct(g_psRadioContainers, g_psRadioButtonIndicators + 1, 0,
                 &g_sKitronix320x240x16_SSD2119, 95, 62, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psRadioContainers, g_psRadioButtonIndicators + 2, 0,
                 &g_sKitronix320x240x16_SSD2119, 95, 107, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psRadioContainers, 0, 0,
                 &g_sKitronix320x240x16_SSD2119, 95, 152, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psRadioContainers + 1, g_psRadioButtonIndicators + 4, 0,
                 &g_sKitronix320x240x16_SSD2119, 260, 62, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psRadioContainers + 1, g_psRadioButtonIndicators + 5, 0,
                 &g_sKitronix320x240x16_SSD2119, 260, 107, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
    CanvasStruct(g_psRadioContainers + 1, 0, 0,
                 &g_sKitronix320x240x16_SSD2119, 260, 152, 20, 20,
                 CANVAS_STYLE_IMG, 0, 0, 0, 0, 0, g_pucLightOff, 0),
};
tRadioButtonWidget g_psRadioButtons1[] =
{
    RadioButtonStruct(g_psRadioContainers, g_psRadioButtons1 + 1, 0,
                      &g_sKitronix320x240x16_SSD2119, 10, 50, 80, 45,
                      RB_STYLE_TEXT, 16, 0, ClrSilver, ClrSilver, &g_sFontCm20,
                      "One", 0, OnRadioChange),
    RadioButtonStruct(g_psRadioContainers, g_psRadioButtons1 + 2, 0,
                      &g_sKitronix320x240x16_SSD2119, 10, 95, 80, 45,
                      RB_STYLE_TEXT, 16, 0, ClrSilver, ClrSilver, &g_sFontCm20,
                      "Two", 0, OnRadioChange),
    RadioButtonStruct(g_psRadioContainers, g_psRadioButtonIndicators, 0,
                      &g_sKitronix320x240x16_SSD2119, 10, 140, 80, 45,
                      RB_STYLE_TEXT, 24, 0, ClrSilver, ClrSilver, &g_sFontCm20,
                      "Three", 0, OnRadioChange)
};
#define NUM_RADIO1_BUTTONS      (sizeof(g_psRadioButtons1) /   \
                                 sizeof(g_psRadioButtons1[0]))
tRadioButtonWidget g_psRadioButtons2[] =
{
    RadioButtonStruct(g_psRadioContainers + 1, g_psRadioButtons2 + 1, 0,
                      &g_sKitronix320x240x16_SSD2119, 175, 50, 80, 45,
                      RB_STYLE_IMG, 16, 0, ClrSilver, 0, 0, 0, g_pucLogo,
                      OnRadioChange),
    RadioButtonStruct(g_psRadioContainers + 1, g_psRadioButtons2 + 2, 0,
                      &g_sKitronix320x240x16_SSD2119, 175, 95, 80, 45,
                      RB_STYLE_IMG, 24, 0, ClrSilver, 0, 0, 0, g_pucLogo,
                      OnRadioChange),
    RadioButtonStruct(g_psRadioContainers + 1, g_psRadioButtonIndicators + 3,
                      0, &g_sKitronix320x240x16_SSD2119, 175, 140, 80, 45,
                      RB_STYLE_IMG, 24, 0, ClrSilver, 0, 0, 0, g_pucLogo,
                      OnRadioChange)
};
#define NUM_RADIO2_BUTTONS      (sizeof(g_psRadioButtons2) /   \
                                 sizeof(g_psRadioButtons2[0]))
tContainerWidget g_psRadioContainers[] =
{
    ContainerStruct(g_psPanels + 6, g_psRadioContainers + 1, g_psRadioButtons1,
                    &g_sKitronix320x240x16_SSD2119, 5, 27, 148, 160,
                    CTR_STYLE_OUTLINE | CTR_STYLE_TEXT, 0, ClrGray, ClrSilver,
                    &g_sFontCm20, "Group One"),
    ContainerStruct(g_psPanels + 6, 0, g_psRadioButtons2,
                    &g_sKitronix320x240x16_SSD2119, 167, 27, 148, 160,
                    CTR_STYLE_OUTLINE | CTR_STYLE_TEXT, 0, ClrGray, ClrSilver,
                    &g_sFontCm20, "Group Two")
};

//*****************************************************************************
//
// The eighth panel, which demonstrates the slider widget.
//
//*****************************************************************************
Canvas(g_sSliderValueCanvas, g_psPanels + 7, 0, 0,
       &g_sKitronix320x240x16_SSD2119, 210, 30, 60, 40,
       CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_OPAQUE, ClrBlack, 0, ClrSilver,
       &g_sFontCm24, "50%",
       0, 0);

tSliderWidget g_psSliders[] =
{
    SliderStruct(g_psPanels + 7, g_psSliders + 1, 0,
                 &g_sKitronix320x240x16_SSD2119, 5, 115, 220, 30, 0, 100, 25,
                 (SL_STYLE_FILL | SL_STYLE_BACKG_FILL | SL_STYLE_OUTLINE |
                  SL_STYLE_TEXT | SL_STYLE_BACKG_TEXT),
                 ClrGray, ClrBlack, ClrSilver, ClrWhite, ClrWhite,
                 &g_sFontCm20, "25%", 0, 0, OnSliderChange),
    SliderStruct(g_psPanels + 7, g_psSliders + 2, 0,
                 &g_sKitronix320x240x16_SSD2119, 5, 155, 220, 25, 0, 100, 25,
                 (SL_STYLE_FILL | SL_STYLE_BACKG_FILL | SL_STYLE_OUTLINE |
                  SL_STYLE_TEXT),
                 ClrWhite, ClrBlueViolet, ClrSilver, ClrBlack, 0,
                 &g_sFontCm18, "Foreground Text Only", 0, 0, OnSliderChange),
    SliderStruct(g_psPanels + 7, g_psSliders + 3, 0,
                 &g_sKitronix320x240x16_SSD2119, 240, 70, 26, 110, 0, 100, 50,
                 (SL_STYLE_FILL | SL_STYLE_BACKG_FILL | SL_STYLE_VERTICAL |
                  SL_STYLE_OUTLINE | SL_STYLE_LOCKED), ClrDarkGreen,
                  ClrDarkRed, ClrSilver, 0, 0, 0, 0, 0, 0, 0),
    SliderStruct(g_psPanels + 7, g_psSliders + 4, 0,
                 &g_sKitronix320x240x16_SSD2119, 280, 30, 30, 150, 0, 100, 75,
                 (SL_STYLE_IMG | SL_STYLE_BACKG_IMG | SL_STYLE_VERTICAL |
                 SL_STYLE_OUTLINE), 0, ClrBlack, ClrSilver, 0, 0, 0,
                 0, g_pucGettingHotter28x148, g_pucGettingHotter28x148Mono,
                 OnSliderChange),
    SliderStruct(g_psPanels + 7, g_psSliders + 5, 0,
                 &g_sKitronix320x240x16_SSD2119, 5, 30, 195, 37, 0, 100, 50,
                 SL_STYLE_IMG | SL_STYLE_BACKG_IMG, 0, 0, 0, 0, 0, 0,
                 0, g_pucGreenSlider195x37, g_pucRedSlider195x37,
                 OnSliderChange),
    SliderStruct(g_psPanels + 7, &g_sSliderValueCanvas, 0,
                 &g_sKitronix320x240x16_SSD2119, 5, 80, 220, 25, 0, 100, 50,
                 (SL_STYLE_FILL | SL_STYLE_BACKG_FILL | SL_STYLE_TEXT |
                  SL_STYLE_BACKG_TEXT | SL_STYLE_TEXT_OPAQUE |
                  SL_STYLE_BACKG_TEXT_OPAQUE),
                 ClrBlue, ClrYellow, ClrSilver, ClrYellow, ClrBlue,
                 &g_sFontCm18, "Text in both areas", 0, 0,
                 OnSliderChange),
};

#define SLIDER_TEXT_VAL_INDEX   0
#define SLIDER_LOCKED_INDEX     2
#define SLIDER_CANVAS_VAL_INDEX 4

#define NUM_SLIDERS (sizeof(g_psSliders) / sizeof(g_psSliders[0]))

//*****************************************************************************
//
// An array of canvas widgets, one per panel.  Each canvas is filled with
// black, overwriting the contents of the previous panel.
//
//*****************************************************************************
tCanvasWidget g_psPanels[] =
{
    CanvasStruct(0, 0, &g_sIntroduction, &g_sKitronix320x240x16_SSD2119, 0, 24,
                 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &g_sPrimitives, &g_sKitronix320x240x16_SSD2119, 0, 24,
                 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &g_sCanvas1, &g_sKitronix320x240x16_SSD2119, 0, 24, 320,
                 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, g_psCheckBoxes, &g_sKitronix320x240x16_SSD2119, 0, 24,
                 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, &g_sContainer1, &g_sKitronix320x240x16_SSD2119, 0, 24,
                 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, g_psPushButtons, &g_sKitronix320x240x16_SSD2119, 0, 24,
                 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, g_psRadioContainers, &g_sKitronix320x240x16_SSD2119, 0,
                 24, 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
    CanvasStruct(0, 0, g_psSliders, &g_sKitronix320x240x16_SSD2119, 0,
                 24, 320, 166, CANVAS_STYLE_FILL, ClrBlack, 0, 0, 0, 0, 0, 0),
};

//*****************************************************************************
//
// The number of panels.
//
//*****************************************************************************
#define NUM_PANELS              (sizeof(g_psPanels) / sizeof(g_psPanels[0]))

//*****************************************************************************
//
// The names for each of the panels, which is displayed at the bottom of the
// screen.
//
//*****************************************************************************
char *g_pcPanelNames[] =
{
    "     Introduction     ",
    "     Primitives     ",
    "     Canvas     ",
    "     Checkbox     ",
    "     Container     ",
    "     Push Buttons     ",
    "     Radio Buttons     ",
    "     Sliders     ",
    "     S/W Update    "
};

//*****************************************************************************
//
// The buttons and text across the bottom of the screen.
//
//*****************************************************************************
RectangularButton(g_sPrevious, 0, 0, 0, &g_sKitronix320x240x16_SSD2119, 0, 190,
                  50, 50, PB_STYLE_FILL, ClrBlack, ClrBlack, 0, ClrSilver,
                  &g_sFontCm20, "-", g_pucBlue50x50, g_pucBlue50x50Press, 0, 0,
                  OnPrevious);
Canvas(g_sTitle, 0, 0, 0, &g_sKitronix320x240x16_SSD2119, 50, 190, 220, 50,
       CANVAS_STYLE_TEXT | CANVAS_STYLE_TEXT_OPAQUE, 0, 0, ClrSilver,
       &g_sFontCm20, 0, 0, 0);
RectangularButton(g_sNext, 0, 0, 0, &g_sKitronix320x240x16_SSD2119, 270, 190,
                  50, 50, PB_STYLE_IMG | PB_STYLE_TEXT, ClrBlack, ClrBlack, 0,
                  ClrSilver, &g_sFontCm20, "+", g_pucBlue50x50,
                  g_pucBlue50x50Press, 0, 0, OnNext);

//*****************************************************************************
//
// The panel that is currently being displayed.
//
//*****************************************************************************
unsigned long g_ulPanel;

//*****************************************************************************
//
// Handles presses of the previous panel button.
//
//*****************************************************************************
void
OnPrevious(tWidget *pWidget)
{
    //
    // There is nothing to be done if the first panel is already being
    // displayed.
    //
    if(g_ulPanel == 0)
    {
        return;
    }

    //
    // Remove the current panel.
    //
    WidgetRemove((tWidget *)(g_psPanels + g_ulPanel));

    //
    // Decrement the panel index.
    //
    g_ulPanel--;

    //
    // Add and draw the new panel.
    //
    WidgetAdd(WIDGET_ROOT, (tWidget *)(g_psPanels + g_ulPanel));
    WidgetPaint((tWidget *)(g_psPanels + g_ulPanel));

    //
    // Set the title of this panel.
    //
    CanvasTextSet(&g_sTitle, g_pcPanelNames[g_ulPanel]);
    WidgetPaint((tWidget *)&g_sTitle);

    //
    // See if this is the first panel.
    //
    if(g_ulPanel == 0)
    {
        //
        // Clear the previous button from the display since the first panel is
        // being displayed.
        //
        PushButtonImageOff(&g_sPrevious);
        PushButtonTextOff(&g_sPrevious);
        PushButtonFillOn(&g_sPrevious);
        WidgetPaint((tWidget *)&g_sPrevious);
    }

    //
    // See if the previous panel was the last panel.
    //
    if(g_ulPanel == (NUM_PANELS - 2))
    {
        //
        // Display the next button.
        //
        PushButtonImageOn(&g_sNext);
        PushButtonTextOn(&g_sNext);
        PushButtonFillOff(&g_sNext);
        WidgetPaint((tWidget *)&g_sNext);
    }

    //
    // Play the key click sound.
    //
    SoundPlay(g_pusKeyClick, sizeof(g_pusKeyClick) / 2);
}

//*****************************************************************************
//
// Handles presses of the next panel button.
//
//*****************************************************************************
void
OnNext(tWidget *pWidget)
{
    //
    // There is nothing to be done if the last panel is already being
    // displayed.
    //
    if(g_ulPanel == (NUM_PANELS - 1))
    {
        return;
    }

    //
    // Remove the current panel.
    //
    WidgetRemove((tWidget *)(g_psPanels + g_ulPanel));

    //
    // Increment the panel index.
    //
    g_ulPanel++;

    //
    // Add and draw the new panel.
    //
    WidgetAdd(WIDGET_ROOT, (tWidget *)(g_psPanels + g_ulPanel));
    WidgetPaint((tWidget *)(g_psPanels + g_ulPanel));

    //
    // Set the title of this panel.
    //
    CanvasTextSet(&g_sTitle, g_pcPanelNames[g_ulPanel]);
    WidgetPaint((tWidget *)&g_sTitle);

    //
    // See if the previous panel was the first panel.
    //
    if(g_ulPanel == 1)
    {
        //
        // Display the previous button.
        //
        PushButtonImageOn(&g_sPrevious);
        PushButtonTextOn(&g_sPrevious);
        PushButtonFillOff(&g_sPrevious);
        WidgetPaint((tWidget *)&g_sPrevious);
    }

    //
    // See if this is the last panel.
    //
    if(g_ulPanel == (NUM_PANELS - 1))
    {
        //
        // Clear the next button from the display since the last panel is being
        // displayed.
        //
        PushButtonImageOff(&g_sNext);
        PushButtonTextOff(&g_sNext);
        PushButtonFillOn(&g_sNext);
        WidgetPaint((tWidget *)&g_sNext);
    }

    //
    // Play the key click sound.
    //
    SoundPlay(g_pusKeyClick, sizeof(g_pusKeyClick) / 2);
}

//*****************************************************************************
//
// Handles paint requests for the introduction canvas widget.
//
//*****************************************************************************
void
OnIntroPaint(tWidget *pWidget, tContext *pContext)
{
    //
    // Display the introduction text in the canvas.
    //
    GrContextFontSet(pContext, &g_sFontCm18);
    GrContextForegroundSet(pContext, ClrSilver);
    GrStringDraw(pContext, "Dies ist eine Einfache Graphische Anwendung", -1,
                 0, 32, 0);
    GrStringDraw(pContext, "Graphics Library.", -1, 0, 50, 0);
    GrStringDraw(pContext, "Bearbeitet von Martin Anzinger", -1, 0,
                 74, 0);
}

//*****************************************************************************
//
// Handles paint requests for the primitives canvas widget.
//
//*****************************************************************************
void
OnPrimitivePaint(tWidget *pWidget, tContext *pContext)
{
    unsigned long ulIdx;
    tRectangle sRect;

    //
    // Draw a vertical sweep of lines from red to green.
    //
    for(ulIdx = 0; ulIdx <= 8; ulIdx++)
    {
        GrContextForegroundSet(pContext,
                               (((((10 - ulIdx) * 255) / 10) << ClrRedShift) |
                                (((ulIdx * 255) / 10) << ClrGreenShift)));
        GrLineDraw(pContext, 115, 120, 5, 120 - (11 * ulIdx));
    }

    //
    // Draw a horizontal sweep of lines from green to blue.
    //
    for(ulIdx = 1; ulIdx <= 10; ulIdx++)
    {
        GrContextForegroundSet(pContext,
                               (((((10 - ulIdx) * 255) / 10) <<
                                 ClrGreenShift) |
                                (((ulIdx * 255) / 10) << ClrBlueShift)));
        GrLineDraw(pContext, 115, 120, 5 + (ulIdx * 11), 29);
    }

    //
    // Draw a filled circle with an overlapping circle.
    //
    GrContextForegroundSet(pContext, ClrBrown);
    GrCircleFill(pContext, 185, 69, 40);
    GrContextForegroundSet(pContext, ClrSkyBlue);
    GrCircleDraw(pContext, 205, 99, 30);

    //
    // Draw a filled rectangle with an overlapping rectangle.
    //
    GrContextForegroundSet(pContext, ClrSlateGray);
    sRect.sXMin = 20;
    sRect.sYMin = 100;
    sRect.sXMax = 75;
    sRect.sYMax = 160;
    GrRectFill(pContext, &sRect);
    GrContextForegroundSet(pContext, ClrSlateBlue);
    sRect.sXMin += 40;
    sRect.sYMin += 40;
    sRect.sXMax += 30;
    sRect.sYMax += 28;
    GrRectDraw(pContext, &sRect);

    //
    // Draw a piece of text in fonts of increasing size.
    //
    GrContextForegroundSet(pContext, ClrSilver);
    GrContextFontSet(pContext, &g_sFontCm14);
    GrStringDraw(pContext, "Strings", -1, 125, 110, 0);
    GrContextFontSet(pContext, &g_sFontCm18);
    GrStringDraw(pContext, "Strings", -1, 145, 124, 0);
    GrContextFontSet(pContext, &g_sFontCm22);
    GrStringDraw(pContext, "Strings", -1, 165, 142, 0);
    GrContextFontSet(pContext, &g_sFontCm24);
    GrStringDraw(pContext, "Strings", -1, 185, 162, 0);

    //
    // Draw an image.
    //
    GrImageDraw(pContext, g_pucLogo, 270, 80);
}

//*****************************************************************************
//
// Handles paint requests for the canvas demonstration widget.
//
//*****************************************************************************
void
OnCanvasPaint(tWidget *pWidget, tContext *pContext)
{
    unsigned long ulIdx;

    //
    // Draw a set of radiating lines.
    //
    GrContextForegroundSet(pContext, ClrGoldenrod);
    for(ulIdx = 50; ulIdx <= 180; ulIdx += 10)
    {
        GrLineDraw(pContext, 210, ulIdx, 310, 230 - ulIdx);
    }

    //
    // Indicate that the contents of this canvas were drawn by the application.
    //
    GrContextFontSet(pContext, &g_sFontCm12);
    GrStringDrawCentered(pContext, "App Drawn", -1, 260, 50, 1);
}

//*****************************************************************************
//
// Handles change notifications for the check box widgets.
//
//*****************************************************************************
void
OnCheckChange(tWidget *pWidget, unsigned long bSelected)
{
    unsigned long ulIdx;

    //
    // Find the index of this check box.
    //
    for(ulIdx = 0; ulIdx < NUM_CHECK_BOXES; ulIdx++)
    {
        if(pWidget == (tWidget *)(g_psCheckBoxes + ulIdx))
        {
            break;
        }
    }

    //
    // Return if the check box could not be found.
    //
    if(ulIdx == NUM_CHECK_BOXES)
    {
        return;
    }

    //
    // Set the matching indicator based on the selected state of the check box.
    //
    CanvasImageSet(g_psCheckBoxIndicators + ulIdx,
                   bSelected ? g_pucLightOn : g_pucLightOff);
    WidgetPaint((tWidget *)(g_psCheckBoxIndicators + ulIdx));

    //
    // Play the key click sound.
    //
    SoundPlay(g_pusKeyClick, sizeof(g_pusKeyClick) / 2);
}

//*****************************************************************************
//
// Handles press notifications for the push button widgets.
//
//*****************************************************************************
void
OnButtonPress(tWidget *pWidget)
{
    unsigned long ulIdx;

    //
    // Find the index of this push button.
    //
    for(ulIdx = 0; ulIdx < NUM_PUSH_BUTTONS; ulIdx++)
    {
        if(pWidget == (tWidget *)(g_psPushButtons + ulIdx))
        {
            break;
        }
    }

    //
    // Return if the push button could not be found.
    //
    if(ulIdx == NUM_PUSH_BUTTONS)
    {
        return;
    }

    //
    // Toggle the state of this push button indicator.
    //
    g_ulButtonState ^= 1 << ulIdx;

    //
    // Set the matching indicator based on the selected state of the check box.
    //
    CanvasImageSet(g_psPushButtonIndicators + ulIdx,
                   (g_ulButtonState & (1 << ulIdx)) ? g_pucLightOn :
                   g_pucLightOff);
    WidgetPaint((tWidget *)(g_psPushButtonIndicators + ulIdx));

    //
    // Play the key click sound.
    //
    SoundPlay(g_pusKeyClick, sizeof(g_pusKeyClick) / 2);
}

//*****************************************************************************
//
// Handles notifications from the slider controls.
//
//*****************************************************************************
void
OnSliderChange(tWidget *pWidget, long lValue)
{
    static char pcCanvasText[5];
    static char pcSliderText[5];

    //
    // Is this the widget whose value we mirror in the canvas widget and the
    // locked slider?
    //
    if(pWidget == (tWidget *)&g_psSliders[SLIDER_CANVAS_VAL_INDEX])
    {
        //
        // Yes - update the canvas to show the slider value.
        //
        sprintf(pcCanvasText, "%3d%%", lValue);
        CanvasTextSet(&g_sSliderValueCanvas, pcCanvasText);
        WidgetPaint((tWidget *)&g_sSliderValueCanvas);

        //
        // Also update the value of the locked slider to reflect this one.
        //
        SliderValueSet(&g_psSliders[SLIDER_LOCKED_INDEX], lValue);
        WidgetPaint((tWidget *)&g_psSliders[SLIDER_LOCKED_INDEX]);
    }

    if(pWidget == (tWidget *)&g_psSliders[SLIDER_TEXT_VAL_INDEX])
    {
        //
        // Yes - update the canvas to show the slider value.
        //
        sprintf(pcSliderText, "%3d%%", lValue);
        SliderTextSet(&g_psSliders[SLIDER_TEXT_VAL_INDEX], pcSliderText);
        WidgetPaint((tWidget *)&g_psSliders[SLIDER_TEXT_VAL_INDEX]);
    }
}

//*****************************************************************************
//
// Handles change notifications for the radio button widgets.
//
//*****************************************************************************
void
OnRadioChange(tWidget *pWidget, unsigned long bSelected)
{
    unsigned long ulIdx;

    //
    // Find the index of this radio button in the first group.
    //
    for(ulIdx = 0; ulIdx < NUM_RADIO1_BUTTONS; ulIdx++)
    {
        if(pWidget == (tWidget *)(g_psRadioButtons1 + ulIdx))
        {
            break;
        }
    }

    //
    // See if the radio button was found.
    //
    if(ulIdx == NUM_RADIO1_BUTTONS)
    {
        //
        // Find the index of this radio button in the second group.
        //
        for(ulIdx = 0; ulIdx < NUM_RADIO2_BUTTONS; ulIdx++)
        {
            if(pWidget == (tWidget *)(g_psRadioButtons2 + ulIdx))
            {
                break;
            }
        }

        //
        // Return if the radio button could not be found.
        //
        if(ulIdx == NUM_RADIO2_BUTTONS)
        {
            return;
        }

        //
        // Sind the radio button is in the second group, offset the index to
        // the indicators associated with the second group.
        //
        ulIdx += NUM_RADIO1_BUTTONS;
    }

    //
    // Set the matching indicator based on the selected state of the radio
    // button.
    //
    CanvasImageSet(g_psRadioButtonIndicators + ulIdx,
                   bSelected ? g_pucLightOn : g_pucLightOff);
    WidgetPaint((tWidget *)(g_psRadioButtonIndicators + ulIdx));

    //
    // Play the key click sound.
    //
    SoundPlay(g_pusKeyClick, sizeof(g_pusKeyClick) / 2);
}

//*****************************************************************************
//
// A simple demonstration of the features of the Stellaris Graphics Library.
//
//*****************************************************************************
int
main(void)
{
    tContext sContext;
    tRectangle sRect;

    //
    // Set the clocking to run from the PLL.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    //
    // Set the device pinout appropriately for this board.
    //
    PinoutSet();

    //
    // Initialize the display driver.
    //
    Kitronix320x240x16_SSD2119Init();

    //
    // Initialize the graphics context.
    //
    GrContextInit(&sContext, &g_sKitronix320x240x16_SSD2119);

    //
    // Fill the top 24 rows of the screen with blue to create the banner.
    //
    sRect.sXMin = 0;
    sRect.sYMin = 0;
    sRect.sXMax = GrContextDpyWidthGet(&sContext) - 1;
    sRect.sYMax = 23;
    GrContextForegroundSet(&sContext, ClrDarkBlue);
    GrRectFill(&sContext, &sRect);

    //
    // Put a white box around the banner.
    //
    GrContextForegroundSet(&sContext, ClrWhite);
    GrRectDraw(&sContext, &sRect);

    //
    // Put the application name in the middle of the banner.
    //
    GrContextFontSet(&sContext, &g_sFontCm20);
    GrStringDrawCentered(&sContext, "grlib demo", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 8, 0);

    //
    // Configure and enable uDMA
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
    SysCtlDelay(10);
    uDMAControlBaseSet(&sDMAControlTable[0]);
    uDMAEnable();

    //
    // Initialize the sound driver.
    //
    SoundInit(0);

    //
    // Initialize the touch screen driver and have it route its messages to the
    // widget tree.
    //
    TouchScreenInit();
    TouchScreenCallbackSet(WidgetPointerMessage);

    //
    // Add the title block and the previous and next buttons to the widget
    // tree.
    //
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sPrevious);
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sTitle);
    WidgetAdd(WIDGET_ROOT, (tWidget *)&g_sNext);

    //
    // Add the first panel to the widget tree.
    //
    g_ulPanel = 0;
    WidgetAdd(WIDGET_ROOT, (tWidget *)g_psPanels);
    CanvasTextSet(&g_sTitle, g_pcPanelNames[0]);

    //
    // Issue the initial paint request to the widgets.
    //
    WidgetPaint(WIDGET_ROOT);

    //
    // Loop forever handling widget messages.
    //
    while(1)
    {
        //
        // Process any messages in the widget message queue.
        //
        WidgetMessageQueueProcess();
    }
}

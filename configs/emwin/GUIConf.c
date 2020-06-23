/*********************************************************************
*                SEGGER Microcontroller GmbH                         *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2018  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.48 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software  has been licensed to  Cypress Semiconductor Corporation,
whose registered  office is situated  at 198 Champion Ct. San Jose, CA
95134 USA  solely for the  purposes of creating  libraries for Cypress
PSoC3 and  PSoC5 processor-based devices,  sublicensed and distributed
under  the  terms  and  conditions  of  the  Cypress  End User License
Agreement.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Microcontroller Systems LLC
Licensed to:              Cypress Semiconductor Corp, 198 Champion Ct., San Jose, CA 95134, USA
Licensed SEGGER software: emWin
License number:           GUI-00319
License model:            Services and License Agreement, signed June 10th, 2009
Licensed platform:        Any Cypress platform (Initial targets are: PSoC3, PSoC5)
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2009-06-12 - 2022-07-27
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : GUIConf.c
Purpose     : Display controller initialization
---------------------------END-OF-HEADER------------------------------
*/

#include "emwin.h"

#ifdef EMWIN_ENABLED

#include "GUI.h"
#include "LCDConf.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

//
// Define the available number of bytes available for the GUI
//
#if !defined(GUI_NUMBYTES)
    #if ((MTB_DISPLAY_SIZE_X * MTB_DISPLAY_SIZE_Y * MTB_DISPLAY_COLOR_BITS / 4) > 0x7800)
        #define GUI_NUMBYTES  (1024*32)
    #else
        #define GUI_NUMBYTES  ((MTB_DISPLAY_SIZE_X * MTB_DISPLAY_SIZE_Y * \
                                MTB_DISPLAY_COLOR_BITS / 4) + 2048)
    #endif
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       GUI_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   available memory for the GUI.
*/
void GUI_X_Config(void)
{
    //
    // 32 bit aligned memory area
    //
    static U32 aMemory[GUI_NUMBYTES / 4];
    //
    // Assign memory to emWin
    //
    GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
    //
    // Set default font
    //
    GUI_SetDefaultFont(GUI_FONT_6X8);
}

#if defined(__cplusplus)
}
#endif

#endif /* EMWIN_ENABLED */


/*************************** End of file ****************************/

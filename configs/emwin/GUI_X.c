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
File        : GUI_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "emwin.h"

#ifdef EMWIN_ENABLED

#include "cyhal.h"
#include "GUI.h"

#if (GUI_OS == 1)
    #include "cyabs_rtos.h"
#endif


#if defined(__cplusplus)
extern "C"
{
#endif


#if (GUI_OS == 1)
    cy_mutex_t emwin_mutex;
#else

    /* emWin timer clock value in Hz  */
    #define EMWIN_TIMER_CLOCK_HZ          (100000u)

    /* emWin timer period value */
    #define EMWIN_TIMER_PERIOD            (99u)

    /* emWin timer interrupt priority */
    #define EMWIN_TIMER_IRQ_PRIORITY      (7u)

    cyhal_timer_t emwin_timer;
    volatile GUI_TIMER_TIME emwin_time_ms;

    cy_rslt_t emwin_timer_init(void);
    void isr_emwin_timer(void *callback_arg, cyhal_timer_event_t event);
    bool emwin_timer_syspm_callback(cyhal_syspm_callback_state_t state, cyhal_syspm_callback_mode_t mode, void* callback_arg);

    static cyhal_syspm_callback_data_t emwin_timer_syspm_callback_data =
    {
        .callback = &emwin_timer_syspm_callback,
        .states = (cyhal_syspm_callback_state_t)(CYHAL_SYSPM_CB_CPU_DEEPSLEEP | CYHAL_SYSPM_CB_SYSTEM_HIBERNATE),
        .next = NULL,
        .args = NULL,
        .ignore_modes = 0,
    };

#endif

/*********************************************************************
*
*      Timing:
*                 GUI_X_GetTime()
*                 GUI_X_Delay(int)
*
*   Some timing dependent routines require a GetTime
*   and delay function. Default time unit (tick), normally is
*   1 ms.
*/
GUI_TIMER_TIME GUI_X_GetTime(void)
{
    GUI_TIMER_TIME time;

    #if (GUI_OS == 0)
        time = emwin_time_ms;
    #else
        cy_time_t rtos_time;
        cy_rslt_t status = cy_rtos_get_time(&rtos_time);
        CY_UNUSED_PARAMETER(status); /* CY_ASSERT only processes in DEBUG, ignores for others */
        CY_ASSERT(CY_RSLT_SUCCESS == status);

        time = (GUI_TIMER_TIME)rtos_time;
    #endif

    return time;
}

void GUI_X_Delay(int ms)
{
    #if (GUI_OS == 0)
        int tEnd = emwin_time_ms + ms;
        while ((tEnd - emwin_time_ms) > 0)
        {
            /* Wait */
        }
    #else
        cy_rslt_t status = cy_rtos_delay_milliseconds((cy_time_t)ms);
        CY_UNUSED_PARAMETER(status); /* CY_ASSERT only processes in DEBUG, ignores for others */
        CY_ASSERT(CY_RSLT_SUCCESS == status);
    #endif
}

/*********************************************************************
*
*      GUI_X_Init()
*
* Note:
*   This routine is called from GUI_Init() in any case whether there
*   is an RTOS or not. You can use it for additional initializations
*   needed.
*/
void GUI_X_Init(void)
{
    #if (GUI_OS == 0)
        cy_rslt_t status = emwin_timer_init();
        cyhal_syspm_register_callback(&emwin_timer_syspm_callback_data);
        CY_UNUSED_PARAMETER(status); /* CY_ASSERT only processes in DEBUG, ignores for others */
        CY_ASSERT(CY_RSLT_SUCCESS == status);
    #endif
}

/*********************************************************************
*
*       GUI_X_ExecIdle
*
* Note:
*  Called if WM is in idle state
*/
void GUI_X_ExecIdle(void)
{
    #if (GUI_OS == 1)
        cy_rslt_t status = cy_rtos_delay_milliseconds((cy_time_t)1u);
        CY_UNUSED_PARAMETER(status); /* CY_ASSERT only processes in DEBUG, ignores for others */
        CY_ASSERT(CY_RSLT_SUCCESS == status);
    #endif
}

/*********************************************************************
*
*      Logging: OS dependent
*
* Note:
*   Logging is used in higher debug levels only. The typical target
*   build does not use logging and does therefor not require any of
*   the logging routines below. For a release build without logging
*   the routines below may be eliminated to save some space.
*   (If the linker is not function aware and eliminates unreferenced
*   functions automatically)
*/
void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }


#if (GUI_OS == 1)

    /*********************************************************************
    *
    *      Multitasking:
    *
    *                 GUI_X_InitOS()
    *                 GUI_X_GetTaskId()
    *                 GUI_X_Lock()
    *                 GUI_X_Unlock()
    *
    * Note:
    *   The following routines are required only if emWin is used in a
    *   true multitask environment, which means you have more than one
    *   thread using the emWin API.
    */
    void GUI_X_InitOS(void)
    {
        cy_rslt_t status = cy_rtos_init_mutex(&emwin_mutex);
        CY_UNUSED_PARAMETER(status); /* CY_ASSERT only processes in DEBUG, ignores for others */
        CY_ASSERT(CY_RSLT_SUCCESS == status);
    }

    void GUI_X_Unlock(void)
    {
        cy_rslt_t status = cy_rtos_set_mutex(&emwin_mutex);
        CY_UNUSED_PARAMETER(status); /* CY_ASSERT only processes in DEBUG, ignores for others */
        CY_ASSERT(CY_RSLT_SUCCESS == status);
    }

    void GUI_X_Lock(void)
    {
        cy_rslt_t status = cy_rtos_get_mutex(&emwin_mutex, CY_RTOS_NEVER_TIMEOUT);
        CY_UNUSED_PARAMETER(status); /* CY_ASSERT only processes in DEBUG, ignores for others */
        CY_ASSERT(CY_RSLT_SUCCESS == status);
    }

    U32 GUI_X_GetTaskId(void)
    {
        cy_thread_t current_thread;
        cy_rslt_t status =  cy_rtos_get_thread_handle(&current_thread);
        CY_UNUSED_PARAMETER(status); /* CY_ASSERT only processes in DEBUG, ignores for others */
        CY_ASSERT(CY_RSLT_SUCCESS == status);

        return (U32)current_thread;
    }

#else

    /*********************************************************************
    *
    *         Setup emWin timer
    *
    */
    cy_rslt_t emwin_timer_init(void)
    {
        const cyhal_timer_cfg_t emwin_timer_cfg = \
        {
            .compare_value = EMWIN_TIMER_PERIOD,    /* Timer compare value, not used */
            .period = EMWIN_TIMER_PERIOD,           /* Defines the timer period */
            .direction = CYHAL_TIMER_DIR_UP,        /* Timer counts up */
            .is_compare = true,                     /* Don't use compare mode */
            .is_continuous = true,                  /* Run timer indefinitely */
            .value = 0u                             /* Initial value of counter */
        };

        /* Initialize the timer object. Does not use pin output ('pin' is NC) and
        * does not use a pre-configured clock source ('clk' is NULL). */
        cy_rslt_t status = cyhal_timer_init(&emwin_timer, (cyhal_gpio_t) NC, NULL);

        if (CY_RSLT_SUCCESS == status)
        {
            /* Configure timer period and operation mode such as count direction,
            duration */
            status = cyhal_timer_configure(&emwin_timer, &emwin_timer_cfg);
        }

        if (CY_RSLT_SUCCESS == status)
        {
            /* Set the frequency of timer's clock source */
            status = cyhal_timer_set_frequency(&emwin_timer, EMWIN_TIMER_CLOCK_HZ);
        }

        if (CY_RSLT_SUCCESS == status)
        {
            /* Assign the ISR to execute on timer interrupt */
            cyhal_timer_register_callback(&emwin_timer, isr_emwin_timer, NULL);

            /* Set the event on which timer interrupt occurs and enable it */
            cyhal_timer_enable_event(&emwin_timer, CYHAL_TIMER_IRQ_CAPTURE_COMPARE,
                                    EMWIN_TIMER_IRQ_PRIORITY, true);

            /* Start the timer with the configured settings */
            status = cyhal_timer_start(&emwin_timer);
        }

        return status;
    }

    bool emwin_timer_syspm_callback(cyhal_syspm_callback_state_t state, cyhal_syspm_callback_mode_t mode, void* callback_arg)
    {
        switch(mode)
        {
            case CYHAL_SYSPM_CHECK_READY:
                cyhal_timer_free(&emwin_timer);
                return true;
                break;
            case CYHAL_SYSPM_CHECK_FAIL:
            case CYHAL_SYSPM_AFTER_TRANSITION:
                emwin_timer_init();
                break;
            case CYHAL_SYSPM_BEFORE_TRANSITION:
            default:
                break;
        }

        return true;
    }


    /*******************************************************************************
    * Function Name: isr_timer
    ********************************************************************************
    * Summary:
    * This is the interrupt handler function for the timer interrupt.
    *
    * Parameters:
    *  void
    *
    * Return:
    *  void
    *
    *******************************************************************************/
    void isr_emwin_timer(void *callback_arg, cyhal_timer_event_t event)
    {
        (void) callback_arg;
        (void) event;

        emwin_time_ms++;
    }

#endif

#if defined(__cplusplus)
}
#endif

#endif /* EMWIN_ENABLED */


/*************************** End of file ****************************/

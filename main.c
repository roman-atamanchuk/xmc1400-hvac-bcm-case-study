/*
 * main.c
 *
 *  Created on: 2026 Jul 16 02:07:31
 *  Author: roman
 */




#include "DAVE.h"                 //Declarations from DAVE Code Generation (includes SFR declaration)
#include "BlowerSpeedCtrl.h"
#include "BlowerTest.h"

/**

 * @brief main() - Application entry point
 *
 * <b>Details of function</b><br>
 * This routine is the application entry point. It is invoked by the device startup code. It is responsible for
 * invoking the APP initialization dispatcher routine - DAVE_Init() and hosting the BCM polling superloop.
 */

/* Overall superloop-iteration WCET high-water mark, in microseconds - static so uC/Probe can bind to it directly */
static uint32_t wcetMainLoop_us;

int main(void)
{
  DAVE_STATUS_t status;
  uint32_t t_loopStart, t_loopEnd, t_loopElapsed_us;

  status = DAVE_Init();           /* Initialization of DAVE APPs  */

  if (status != DAVE_STATUS_SUCCESS)
  {
    /* Placeholder for error handler code. The while loop below can be replaced with an user error handler. */
    XMC_DEBUG("DAVE APPs initialization failed\n");

    while(1U)
    {

    }
  }

  /* BCM main polling loop (no interrupts/RTOS, so timing can be reasoned about by hand) */
  while(1U)
  {
    t_loopStart = TIMER_GetTime(&TIMER_0);

    runBlowerSpeedCtrl();

#ifdef TESTMODE
    /* BCM_2.2: test generator only ever built into TESTMODE images */
    runBlowerTest();
#endif

    /* WCET measurement: overall time for one pass through the superloop, used for the Nyquist/CPU-load calcs */
    t_loopEnd = TIMER_GetTime(&TIMER_0);
    /* TIMER_0's counter wraps every period (1ms); if a wrap landed between t_loopStart and t_loopEnd, a plain
     * subtraction would underflow to a huge bogus value - add back one period instead. */
    t_loopElapsed_us = ((t_loopEnd >= t_loopStart) ? (t_loopEnd - t_loopStart)
                                                    : (t_loopEnd + TIMER_0.time_interval_value_us - t_loopStart)) / 100U;
    if (t_loopElapsed_us > wcetMainLoop_us)
    {
      wcetMainLoop_us = t_loopElapsed_us;
    }
  }
}

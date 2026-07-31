/*
 * BlowerTest.c
 *
 *  BCM_2 / BCM_2.1: generates simulated HVAC test signals and transmits
 *  them on CAN message 0x23A via TestNode, so that BcmNode's receive MO
 *  picks them up over the internal loopback bus (SiL testing).
 *
 *  simBlowerSpeedCmd/simLeftTempCmd/simRightTempCmd are set externally via
 *  the uC/Probe dashboard sliders while the debugger is attached.
 *
 *  Message 0x23A layout (see HVAC CAN Message diagram):
 *    byte 4 = Left Temp, byte 5 = Right Temp, byte 6 lower nibble = Blower Speed
 */

#include "DAVE.h"
#include "BlowerTest.h"

/* BlowerTest properties - file-scope statics so uC/Probe can bind to them directly */
static uint8_t simBlowerSpeedCmd;
static uint8_t simLeftTempCmd;
static uint8_t simRightTempCmd;

/* WCET high-water mark for this function, in microseconds - static so uC/Probe can bind to it directly */
static uint32_t wcetBlowerTest_us;

void runBlowerTest(void)
{
  uint8_t txData[8] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U };
#if 1 /* per-function WCET tracking (disable only for a clean Pass-3 main-loop measurement) */
  uint32_t t_start, t_end, t_elapsed_us;

  /* WCET measurement: TIMER_0 is a free-running 1ms-period stopwatch; TIMER_GetTime() is scaled x100,
   * so divide back down to get microseconds. */
  t_start = TIMER_GetTime(&TIMER_0);
#endif

  /* 2.1: read simulated signals (set by the uC/Probe dashboard) */
  txData[4] = simLeftTempCmd;
  txData[5] = simRightTempCmd;
  txData[6] = simBlowerSpeedCmd & 0x0FU;  /* blower speed 0-7, bits 4-7 stay zero */

  /* 2.2: load the data into the TestNode transmit MO */
  CAN_NODE_MO_UpdateData(&TestNode_LMO_01_Config, txData);

  /* 2.3: transmit on message id 0x23A over the internal loopback bus */
  CAN_NODE_MO_Transmit(&TestNode_LMO_01_Config);

#if 1 /* per-function WCET tracking (disable only for a clean Pass-3 main-loop measurement) */
  /* BCM_2.3: this measurement is what proves the test generator isn't delaying the main polling loop */
  t_end = TIMER_GetTime(&TIMER_0);
  /* TIMER_0's counter wraps every period (1ms); if a wrap landed between t_start and t_end, a plain
   * subtraction would underflow to a huge bogus value - add back one period instead. */
  t_elapsed_us = ((t_end >= t_start) ? (t_end - t_start) : (t_end + TIMER_0.time_interval_value_us - t_start)) / 100U;
  if (t_elapsed_us > wcetBlowerTest_us)
  {
    wcetBlowerTest_us = t_elapsed_us;
  }
#endif
}

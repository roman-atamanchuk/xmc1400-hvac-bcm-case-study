/*
 * BlowerSpeedCtrl.c
 *
 *  BCM_1 / BCM_3: reads the HVAC blower-speed setting (and temperature
 *  commands) from CAN message 0x23A via BcmNode's receive MO, and drives
 *  the blower-speed LED display (P4.0-P4.2, binary 0-7).
 *
 *  Message 0x23A layout (see HVAC CAN Message diagram):
 *    byte 4 = Left Temp, byte 5 = Right Temp, byte 6 lower nibble = Blower Speed
 */

#include "DAVE.h"
#include "BlowerSpeedCtrl.h"

/* BlowerSpeedCtrl properties - file-scope statics so uC/Probe can bind to them directly */
static uint8_t inBlowerSpeedCmd;
static uint8_t inLeftTempCmd;
static uint8_t inRightTempCmd;

/* WCET high-water mark for this function, in microseconds - static so uC/Probe can bind to it directly */
static uint32_t wcetBlowerSpeedCtrl_us;

void runBlowerSpeedCtrl(void)
{
  uint32_t MO_Status;
#if 1 /* per-function WCET tracking (disable only for a clean Pass-3 main-loop measurement) */
  uint32_t t_start, t_end, t_elapsed_us;

  /* WCET measurement: TIMER_0 is a free-running 1ms-period stopwatch; TIMER_GetTime() is scaled x100,
   * so divide back down to get microseconds. */
  t_start = TIMER_GetTime(&TIMER_0);
#endif

  /* 1.1: poll for a new HVAC signal on the BcmNode receive MO */
  MO_Status = CAN_NODE_MO_GetStatus(&BcmNode_LMO_01_Config);

  if (MO_Status & XMC_CAN_MO_STATUS_RX_PENDING)
  {
    /* 1.2: clear the pending flag */
    CAN_NODE_MO_ClearStatus(&BcmNode_LMO_01_Config, XMC_CAN_MO_RESET_STATUS_RX_PENDING);

    /* 1.3: read the received message object */
    CAN_NODE_MO_Receive((CAN_NODE_LMO_t *)&BcmNode_LMO_01_Config);

    /* 1.4: extract signals from the message data bytes */
    inLeftTempCmd    = BcmNode_LMO_01_Config.mo_ptr->can_data_byte[4];
    inRightTempCmd   = BcmNode_LMO_01_Config.mo_ptr->can_data_byte[5];
    inBlowerSpeedCmd = BcmNode_LMO_01_Config.mo_ptr->can_data_byte[6];

    /* Drive the blower-speed LED display: P4.0 (LSB) .. P4.2 (MSB).
     * Boot Kit LEDs are active-low, so bit=1 means drive the pin LOW (LED on). */
    (inBlowerSpeedCmd & 0x01U) ? DIGITAL_IO_SetOutputLow(&led0) : DIGITAL_IO_SetOutputHigh(&led0);
    (inBlowerSpeedCmd & 0x02U) ? DIGITAL_IO_SetOutputLow(&led1) : DIGITAL_IO_SetOutputHigh(&led1);
    (inBlowerSpeedCmd & 0x04U) ? DIGITAL_IO_SetOutputLow(&led2) : DIGITAL_IO_SetOutputHigh(&led2);
  }

#if 1 /* per-function WCET tracking (disable only for a clean Pass-3 main-loop measurement) */
  /* WCET measurement: update the high-water mark if this call ran longer than any call seen before.
   * Worst case (and hence the value that matters) happens on the branch above, when a CAN message is pending. */
  t_end = TIMER_GetTime(&TIMER_0);
  /* TIMER_0's counter wraps every period (1ms); if a wrap landed between t_start and t_end, a plain
   * subtraction would underflow to a huge bogus value - add back one period instead. */
  t_elapsed_us = ((t_end >= t_start) ? (t_end - t_start) : (t_end + TIMER_0.time_interval_value_us - t_start)) / 100U;
  if (t_elapsed_us > wcetBlowerSpeedCtrl_us)
  {
    wcetBlowerSpeedCtrl_us = t_elapsed_us;
  }
#endif
}

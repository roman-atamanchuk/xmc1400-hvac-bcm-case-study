/*
 * BlowerSpeedCtrl.h
 *
 *  BCM_1: blower speed control - reads HVAC blower-speed setting from CAN
 *  (BcmNode Rx MO, id 0x23A) and drives the LED display.
 */

#ifndef BLOWERSPEEDCTRL_H_
#define BLOWERSPEEDCTRL_H_

void runBlowerSpeedCtrl(void);

#endif /* BLOWERSPEEDCTRL_H_ */

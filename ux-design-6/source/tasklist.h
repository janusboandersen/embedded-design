/*
 * tasklist.h
 *
 *  Created on: 16 Feb 2020
 *      Author: janusboandersen
 */

#ifndef TASKLIST_H_
#define TASKLIST_H_

#include <stdlib.h>

#define db_MS_WAIT (2)

#define FlashOKDelayMS (3)
#define FlashOKLongWait (1500)

#define FlashBusyDelayMS (1)
#define FlashBusyRandMax (700)

#define FlashWarningDelayMS (100)

#define FlashErrorNumBeeps (3)
#define FlashErrorBeepDelayMS (75)
#define FlashErrorInterbeepDelayMS (150)
#define FlashErrorDelayMS (750)

extern TaskHandle_t hdlHandleSwitch;
extern TaskHandle_t hdlFlashOK;
extern TaskHandle_t hdlFlashBusy;
extern TaskHandle_t hdlFlashWarning;
extern TaskHandle_t hdlFlashError;
extern TaskHandle_t hdlBuzzSeq;
extern TaskHandle_t hdlVibSeq;
extern TaskHandle_t hdlFlexSens;

typedef enum {zero, wait0, one, wait1} db_state_t;
typedef enum {entrypoint, ok, busy, warning, error} sys_state_t;

/* Handle switch and state machines */
void TaskHandleSwitch(void * p);

/* OK signal */
void TaskFlashOK(void * p);

/* Busy signal */
void TaskFlashBusy(void * p);

/* Warning signal */
void TaskFlashWarning(void * p);

/* Error signal */
void TaskFlashError(void * p);

/* Buzz some sequence */
void TaskBuzzSeq(void * p);

/* Vibrate some sequence */
void TaskVibSeq(void * p);

/* Read flex sensor value and set servo */
void TaskFlexSens(void * p);

#endif /* TASKLIST_H_ */

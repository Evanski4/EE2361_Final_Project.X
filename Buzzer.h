/* 
 * File:   Buzzer.h
 * Author: madpe
 *
 * Created on May 1, 2025, 2:37 PM
 */

#ifndef BUZZER_H
#define	BUZZER_H
void PWM_OC1_Init(void) ;

extern int wompActive;
extern int wompTimer;
extern int wompState;  // 0 = off, 1 = on
extern int wompBeepCount;
//
//void set_tone(int)
void buzzerOFF(void) ;

void buzzerON(void) ;

void updateWompwomp(void);
#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* BUZZER_H */


#define FCY 16000000UL
#define DESIRED_FREQ 150  // 2kHz tone
#include "xc.h"
void buzzerOFF(void) {
    OC1CONbits.OCM = 0b000;  
}

void buzzerON(void) {
    OC1CONbits.OCM = 0b0110;  
}
void PWM_OC1_Init(void) {
    // Set RB2 (RP2) as output for OC1
    TRISBbits.TRISB2 = 0;
    RPOR1bits.RP2R = 18; // RP2 = OC1

    // Configure Timer2 for desired frequency
    T2CON = 0;           // Stop timer and reset control bits
    PR2 = (FCY / (DESIRED_FREQ * 1)) - 1;  // PR2 = 1999 for 2kHz
    TMR2 = 0;
    T2CONbits.TCKPS = 0; // Prescaler 1:1
    T2CONbits.TON = 1;   // Turn on Timer2

    // Set up OC1 in PWM mode
    OC1CON = 0;                // Reset control bits
    OC1CONbits.OCTSEL = 0;     // Use Timer2
    OC1CONbits.OCM = 0b110;    // Edge-aligned PWM
    OC1R = PR2 / 2;             // Initial duty cycle = 50%
    OC1RS = PR2 / 2;
   
    buzzerOFF();
   
}
//
//void set_tone(int)

void updateWompwomp(void) {
    if (wompActive) {
        if (wompTimer == 0) {
            if (wompState == 0) {
                buzzerON();
                wompState = 1;
            } else {
                buzzerOFF();
                wompState = 0;
                wompBeepCount++;
            }

            if (wompBeepCount >= 10) {
                wompActive = 0;
                buzzerOFF();
            } else {
                wompTimer = 500; // ms
            }
        }
    }
}
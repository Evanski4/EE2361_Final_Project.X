#define FCY 16000000UL
#define DESIRED_FREQ 150  // 2kHz tone
#include "xc.h"
int wompActive = 0;
int wompTimer = 0;
int wompState = 0;
int wompBeepCount = 0;

/**
 * @brief Turns the buzzer off by disabling the PWM output.
 *
 * Sets the OC1 output mode to 0 (off), which stops the buzzer signal.
 * Used to silence the buzzer after it's been on.
 */
void buzzerOFF(void) {
    OC1CONbits.OCM = 0b000;  
}
/**
 * @brief Turns the buzzer off by disabling the PWM output.
 *
 * Sets the OC1 output mode to 0110 (on), which starts the buzzer signal.
 * Used to start the buzzer after it's been off.
 */
void buzzerON(void) {
    OC1CONbits.OCM = 0b0110;  
}
/**
 * @brief Sets up PWM on OC1 using Timer2 for buzzer control.
 *
 * - Configures RB2 (RP2) as the output pin for OC1.
 * - Sets up Timer2 to generate a PWM frequency (default: 2kHz).
 * - Initializes OC1 in edge-aligned PWM mode with 50% duty cycle.
 * - Starts Timer2 and disables the buzzer output initially.
 */
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
/**
 * @brief Handles the "womp womp" beep pattern for the buzzer.
 *
 * This function should be called repeatedly (ideally in a loop).
 * It turns the buzzer on and off every 500 ms to make 10 beeps total.
 * When the timer hits zero, it toggles the buzzer state and updates the count.
 * After 10 beeps, it stops everything and turns the buzzer off.
 */

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
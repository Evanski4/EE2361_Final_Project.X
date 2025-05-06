#pragma config ICS = PGx1
#pragma config FWDTEN = OFF
#pragma config GWRP = OFF
#pragma config GCP = OFF
#pragma config JTAGEN = OFF
#pragma config I2C1SEL = PRI
#pragma config IOL1WAY = OFF
#pragma config OSCIOFNC = ON
#pragma config FCKSM = CSECME
#pragma config FNOSC = FRCPLL
#include <stdio.h>
#include "xc.h"
#include <stdlib.h> // for rand
#include "iLEDasm.h"
#include "iLEDwriteColor.h"
#include "LCD.h"
#include "Buzzer.h"
// #include "bluetooth.h"   // <-- Bluetooth include removed

#define FREQ    16000000UL




#define BUTTON PORTBbits.RB7
#define DEBOUNCE_DELAY 50
/**
 * @brief Simple delay function, pauses the program for a given number of milliseconds.
 *
 * Each millisecond delay is based on running some no-operation commands.
 * The delay is tuned for a 16 MHz clock; it's approximate.
 *
 * @param ms Number of milliseconds to pause.
 */
void delay_ms(unsigned int ms) {
    while (ms-- > 0) {
        asm("repeat #15998");
        asm("nop");
    }
}
//clock overflow counter
volatile unsigned long timerCounter = 0;
/**
 * @brief Sets up my PIC24: clock, ports, I2C, and Timer1 interrupts.
 *
 * - System clock to full speed.
 * - All pins digital (no analog).
 * - RA0 output, RB7 input (button with pull-up), rest outputs.
 * - I2C1 at about 100 kHz.
 * - Quick reset sequence on RB6 for peripherals.
 * - Timer1 set to trigger interrupts every 1 ms.
 */
void setup(void) {
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0xFFFF;

    TRISA = 0b1111111111111110; // RA0 = output
    TRISB = 0b10000000;         // RB7 = input (button)
    LATA = 0x0000;
    LATB = 0x0000;

    I2C1CONbits.I2CEN = 0;
    I2C1BRG = 0x9D;
    I2C1CONbits.I2CEN = 1;
    CNPU2bits.CN23PUE = 1;

    delay_ms(5);
    PORTBbits.RB6 = 0;
    delay_ms(10);
    PORTBbits.RB6 = 1;
    delay_ms(1);
    
        //timer 1 setup 
    T1CONbits.TON = 0;       // Disable timer
    T1CONbits.TCS = 0;       // Internal clock (FOSC/2 = 8 MHz)
    T1CONbits.TCKPS = 0b10;  // 1:64 prescaler (8 MHz / 64 = 125 kHz ? 8 µs/tick)
    PR1 = 125;               // 125 ticks × 8 µs = 1.000 ms
    TMR1 = 0;                // Reset timer
    IEC0bits.T1IE = 1;       // Enable interrupt
    IFS0bits.T1IF = 0;       // Clear flag
}
/**
 * @brief Timer1 interrupt, runs every 1 ms.
 *
 * Just increments my global millisecond counter (`timerCounter`) and resets the interrupt flag.
 */
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void) {
    timerCounter++;         // Increment ms counter
    IFS0bits.T1IF = 0;      // Clear interrupt flag
}
//Array to store all the times it took to press the button on green rounds
unsigned long int times [5];
//Counter variable to show where to write in the times array
int i =0;
//variable is 1 if it is a green round 0 0 otherwise
int greenRound;
/**
 * @brief Waits until the button is pressed, handles debounce, and records press duration if it's the "green" round.
 * - Resets the timer counter at the start.
 * - Starts Timer1 only if `greenRound` is active.
 * - Waits (blocking) until button press is detected.
 * - Turns on an indicator (LED) after the button press.
 * - Records how long the button was pressed (using `timerCounter`) if `greenRound` is true.
 * - Waits again until the button is released.
 * - Turns off the indicator LED and handles debounce delays.
 */
void waitForButtonPress(void) {
    timerCounter = 0;  // reset at start
    TMR1=0;
   if(greenRound)
   { T1CONbits.TON = 1;
   } // start Timer1

    while (BUTTON == 1);
    LATA = 0x0001;
    delay_ms(DEBOUNCE_DELAY);
    unsigned long pressTime = timerCounter;
    T1CONbits.TON = 0;
    if(greenRound){
        times[i++] = pressTime;
    }
    while (BUTTON == 0);
    delay_ms(DEBOUNCE_DELAY);
    LATA = 0x0000;
}


/**
 * @brief Returns the average of the first 5 values in the `times` array.
 *
 * Adds up the first 5 times and divides by 5 to get the average.
 * Assumes all 5 values in the array are already filled in.
 *
 * @return The average time as an unsigned long.
 */
unsigned long getAvgTime(void){
    unsigned long int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += times[i];
    }
    return sum / 5;
}

/**
 * @brief Main game loop: reaction time test using RGB LED, LCD, and button.
 *
 * Here's what it does:
 * - Waits for the player to press a button to start.
 * - Randomly shows either a red or green light.
 *   - If it's red and you press the button, you lose.
 *   - If it's green, you need to press the button as fast as you can.
 * - Repeats this until you've correctly responded to 5 green lights.
 * - At the end, it shows your average reaction time on the LCD.
 */
int main(void) {
    while (1) {  
        setup();
        initLCD();

        scrollText("Press to start", 250, 3000);
        waitForButtonPress();

        scrollText("Press on GREEN", 200, 5000);
        
        lcd_printStr("Get ready...");
        delay_ms(1500);

        int greenRoundsCompleted = 0;  // Track completed green rounds

        while (greenRoundsCompleted < 5) {  // Only proceed until 5 green rounds are completed

            clear();
            lcd_setCursor(0, 0);

            // Random wait time between 2000 and 8000 ms before showing the light
            int waitTime = 2000 + rand() % 6001;  

            int randomColor = 1 + rand() % 2;  // Randomly choose red (1) or green (2)

            if (randomColor == 1) {
                // RED light
                delay_ms(waitTime);
                writeColor(255, 0, 0);  // Turn on red light
                int duration = 2000;  // 2 seconds watching time
                while (duration > 0) {
                    if (BUTTON == 0) {  // Pressed during red
                            writeColor(0, 0, 0);
                            clear();
                            lcd_setCursor(0, 0);
                            lcd_printStr("You failed!");
                            lcd_setCursor(1, 0);
                            lcd_printStr("Game over.");

                            // Start the non-blocking beep sequence
                            wompActive = 1;
                            wompTimer = 0;
                            wompState = 0;
                            wompBeepCount = 0;

                            // Wait while beeping (non-blocking)
                            while (wompActive) {
                                updateWompwomp();
                                delay_ms(1);
                                if (wompTimer > 0) wompTimer--;
                            }

                            delay_ms(1000);  // Optional extra pause
//                            buzzerOFF();
                            goto restartGame;
}  
                    delay_ms(10);
                    duration -= 10;
                }
                writeColor(0, 0, 0);  // Turn off red light
            } else if (randomColor == 2) {
                // GREEN light
                greenRound = 1;
                clear();
                lcd_setCursor(0, 0);

                delay_ms(waitTime);  // Wait before showing green light

                lcd_printStr("GO!");
                writeColor(0, 255, 0);  // Turn on green light

                waitForButtonPress();  // Wait for button press to finish round

                writeColor(0, 0, 0);  // Turn off LED
                clear();
                lcd_setCursor(1, 0);

                greenRoundsCompleted++;  // Increment green round counter
                greenRound=0;
            }
        }

        clear();
        lcd_setCursor(0, 0);
        lcd_printStr("All done!");
        delay_ms(3000);
     lcd_setCursor(1,0);
  char buffer[16];
    unsigned long avg = getAvgTime();
    sprintf(buffer, "%lu.%03lu", avg / 1000, avg % 1000); // seconds.milliseconds
    lcd_printStr(buffer);
    restartGame:
        ;
    }

    return 0;
}

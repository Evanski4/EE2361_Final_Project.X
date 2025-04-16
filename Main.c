#include "iLEDasm.h"
#include "stdlib.h" // for psuedo random num generation

//#include "iLEDwriteColor.h"
#include "stdlib.h"
#include "xc.h"

#include "lcdLib.h"

#define DEBOUNCE_DELAY 50
#pragma config ICS = PGx1          // Comm Channel Select (Emulator EMUC1/EMUD1 pins are shared with PGC1/PGD1)
#pragma config FWDTEN = OFF        // Watchdog Timer Enable (Watchdog Timer is disabled)
#pragma config GWRP = OFF          // General Code Segment Write Protect (Writes to program memory are allowed)
#pragma config GCP = OFF           // General Code Segment Code Protect (Code protection is disabled)
#pragma config JTAGEN = OFF        // JTAG Port Enable (JTAG port is disabled)


// CW2: FLASH CONFIGURATION WORD 2 (see PIC24 Family Reference Manual 24.1)
#pragma config I2C1SEL = PRI       // I2C1 Pin Location Select (Use default SCL1/SDA1 pins)
#pragma config IOL1WAY = OFF       // IOLOCK Protection (IOLOCK may be changed via unlocking seq)
#pragma config OSCIOFNC = ON       // Primary Oscillator I/O Function (CLKO/RC15 functions as I/O pin)
#pragma config FCKSM = CSECME      // Clock Switching and Monitor (Clock switching is enabled,
#pragma config FNOSC = FRCPLL // Oscillator Select (Fast RC Oscillator with PLL module (FRCPLL))

//void waitForButtonPress(void);

int main (void){
//    setup();
    /* Trying to make randim color generation;
     * After random amount of time but at least 2 seconds and less than 8 
     * output from RA0
     */
    int randomTimeNum;
    int randomColor;
    while(1){
      
        randomTimeNum = 2 + rand() % (8-2 + 1);
        delay(randomTimeNum*100);
        randomColor = 1 + rand() % (3-1+1);
        switch(randomColor){
            case 1:
                writeColor(255,0,0);
                break;
            case 2:
                writeColor(0,255,0);
                break;
            case 3:
                writeColor(0,0,255);
                break;
            }
    }
}

//    initLCD();
//    // working on the starting screen
//    while (1) {
//        // Initial start screen
//        clear();
//        lcd_setCursor(0, 0);
//        lcd_printString("Press button to");
//        lcd_setCursor(1, 0);
//        lcd_printString("start");
//        waitForButtonPress();
//
//        // Game instructions
//        clear();
//        lcd_setCursor(0, 0);
//        lcd_printString("Press button when light turns green");
//        lcd_setCursor(1, 0);
//        lcd_printString("You have 5 rounds");
//        delay_ms(2500);
//
//        clear();
//        lcd_setCursor(0, 0);
//        lcd_printString("Get ready...");
//        delay_ms(1500);
//        // Game loop: 5 rounds
//        for (int round = 1; round <= 5; round++) {
//            clear();
//            lcd_setCursor(0, 0);
//            lcd_printString("Round ");
//            lcd_printChar('0' + round);
//            delay_ms(1000);
//
//            int waitTime = 2000 + rand() % 6001; // 2 to 8 sec
//            delay_ms(waitTime);
//
//        
//
//        // Play again 
//        clear();
//        lcd_setCursor(0, 0);
//        lcd_printString("Game Over!");
//        lcd_setCursor(1, 0);
//        lcd_printString("Play again?");
//
//        delay_ms(1000);
//        waitForButtonPress();
//    }
//    return 0;
//}
//void waitForButtonPress(void) {
//    // Wait for the button press (debounced)
//    while (BUTTON == 1); // Wait for button to be released
//    delay_ms(DEBOUNCE_DELAY); // Debounce delay
//    while (BUTTON == 0); // Wait for button to be pressed
//    delay_ms(DEBOUNCE_DELAY); // Debounce delay
//    while (BUTTON == 1); // Wait for button to be released
//}


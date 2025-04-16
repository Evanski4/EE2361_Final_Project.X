#include "lcdLib.h"
#include "xc.h"
#include <string.h>

// I2C/LCD Setup Constants
#define LCDaddy 0b0111100
#define addressWrite (LCDaddy << 1) & 0b11111110

// Send a command byte to the LCD
void lcd_cmd(char package) {
    I2C1CONbits.SEN = 1;
    while (I2C1CONbits.SEN);

    _MI2C1IF = 0;
    I2C1TRN = addressWrite;
    while (I2C1STATbits.TRSTAT && !_MI2C1IF);

    _MI2C1IF = 0;
    I2C1TRN = 0x00; // Command control byte
    while (I2C1STATbits.TRSTAT && !_MI2C1IF);

    _MI2C1IF = 0;
    I2C1TRN = package;
    while (I2C1STATbits.TRSTAT && !_MI2C1IF);

    I2C1CONbits.PEN = 1;
    while (I2C1CONbits.PEN);
}

// Initialize the LCD
void initLCD(void) {
    I2C1CONbits.I2CEN = 0;
    I2C1BRG = 0x9D;
    I2C1CONbits.I2CEN = 1;

    delay_ms(5);
    PORTBbits.RB6 = 0;
    delay_ms(10);
    PORTBbits.RB6 = 1;
    delay_ms(1);

    lcd_cmd(0x3A);
    lcd_cmd(0x09);
    lcd_cmd(0x06);
    lcd_cmd(0x1E);
    lcd_cmd(0x39);
    lcd_cmd(0x1B);
    lcd_cmd(0x6E);
    lcd_cmd(0x56);
    lcd_cmd(0x7A);
    lcd_cmd(0x38);
    lcd_cmd(0x0F);
    lcd_cmd(0x3a);
    lcd_cmd(0x09);
    lcd_cmd(0x1a);
    lcd_cmd(0x3c);
    lcd_cmd(0x01);  // Clear
}

// Print a character to the LCD
void lcd_printChar(char myChar) {
    I2C1CONbits.SEN = 1;
    while (I2C1CONbits.SEN);

    _MI2C1IF = 0;
    I2C1TRN = addressWrite;
    while (I2C1STATbits.TRSTAT && !_MI2C1IF);

    _MI2C1IF = 0;
    I2C1TRN = 0x40; // Data control byte
    while (I2C1STATbits.TRSTAT && !_MI2C1IF);

    _MI2C1IF = 0;
    I2C1TRN = myChar;
    while (I2C1STATbits.TRSTAT && !_MI2C1IF);

    I2C1CONbits.PEN = 1;
    while (I2C1CONbits.PEN);
}

// Set the cursor on the LCD
void lcd_setCursor(char x, char y) {
    lcd_cmd(((0x20) * x + y) | 0x80);
}

// Clear the screen
void clear(void) {
    lcd_cmd(0x01);
}

// Delay function (approx. 1 ms delay)
void delay_ms(unsigned int ms) {
    while (ms-- > 0) {
        asm("repeat #15998");
        asm("nop");
    }
}

// Scroll a message across the screen
void lcd_scrollMessage(const char* msg, int width) {
    int len = strlen(msg);
    int col = width;

    while (1) {
        lcd_setCursor(0, 0);
        for (int i = 0; i < width; i++) {
            lcd_printChar(' ');
        }

        lcd_setCursor(0, col >= 0 ? col : 0);

        for (int i = 0; i < len; i++) {
            int pos = col + i;
            if (pos >= 0 && pos < width) {
                lcd_printChar(msg[i]);
            }
        }

        col--;

        if (col < -len) {
            col = width;
        }

        delay_ms(500);
    }
}

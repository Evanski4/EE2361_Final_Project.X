#include "xc.h"
#define LCDaddy 0b0111100
#define addressWrite (LCDaddy << 1) & 0b11111110
#define commandControlByte 0x00
#define dataControlByte 0x40
#define dataStringControlByte 0xC0
#define LCD_WIDTH 10
/**
 * @brief Sends a single command byte to the LCD using I2C.
 * - Starts I2C communication with the LCD.
 * - Sends the LCD's address (write mode).
 * - Tells the LCD it's receiving a command.
 * - Sends the actual command byte (`package`) provided.
 * - Ends the I2C communication.
 *
 * @param package The command byte you want the LCD to execute.
 */
void lcd_cmd(char package) {
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN);

    _MI2C1IF = 0;
    I2C1TRN = addressWrite;
    while(I2C1STATbits.TRSTAT && !_MI2C1IF);

    _MI2C1IF = 0;
    I2C1TRN = commandControlByte;
    while(I2C1STATbits.TRSTAT && !_MI2C1IF);

    _MI2C1IF = 0;
    I2C1TRN = package;
    while(I2C1STATbits.TRSTAT && !_MI2C1IF);

    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN);
}
/**
 * @brief Initializes the LCD by sending a predefined sequence of commands.
 *
 * Basically, this sets up the LCD with the settings it needs to start correctly.
 * Just calls the `lcd_cmd()` function a bunch of times with specific commands from datasheet.
 */
void initLCD(void) {
    lcd_cmd(0x3A); lcd_cmd(0x09); lcd_cmd(0x06);
    lcd_cmd(0x1E); lcd_cmd(0x39); lcd_cmd(0x1B);
    lcd_cmd(0x6E); lcd_cmd(0x56); lcd_cmd(0x7A);
    lcd_cmd(0x38); lcd_cmd(0x0F); lcd_cmd(0x3A);
    lcd_cmd(0x09); lcd_cmd(0x1A); lcd_cmd(0x3C);
    lcd_cmd(0x01);
}
/**
 * @brief Sets the LCD cursor position based on given coordinates (x, y).
 *
 * Takes the (x, y) coordinates you give it, calculates the correct LCD memory address,
 * then sends it as a command. The equation matches exactly what's in the LCD datasheet.
 *
 * @param x The row position (usually 0 or 1 for a 2-row display).
 * @param y The column position (0 to LCD width - 1).
 */
void lcd_setCursor(char x, char y) {
    lcd_cmd(((0x20)*x+y) | 0x80);
}
/**
 * @brief Prints a single character to the LCD over I2C.
 *
 * Starts I2C, sends the LCD's write address, tells it we're sending data (not a command),
 * then sends the character you pass in. Ends the I2C transmission after that.
 *
 * @param myChar The character you want to show on the LCD.
 */
void lcd_printChar(char myChar) {
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN);

    _MI2C1IF = 0;
    I2C1TRN = addressWrite;
    while(I2C1STATbits.TRSTAT && !_MI2C1IF);

    _MI2C1IF = 0;
    I2C1TRN = dataControlByte;
    while(I2C1STATbits.TRSTAT && !_MI2C1IF);

    _MI2C1IF = 0;
    I2C1TRN = myChar;
    while(I2C1STATbits.TRSTAT && !_MI2C1IF);

    I2C1CONbits.PEN = 1;
    while(I2C1CONbits.PEN);
}
 /* @brief Clears the LCD screen.
 *
 * Sends the clear screen command (0x01) to the LCD.
 */
void clear(void) {
    lcd_cmd(0x01);
}
/**
 * @brief Prints a full string to the LCD, one character at a time.
 *
 * Loops through the string you pass in and sends each character using `lcd_printChar`.
 * Stops when it hits the null terminator (`\0`).
 *
 * @param str The string you want to print on the LCD.
 */
void lcd_printStr(const char *str) {
    while(*str != '\0') {
        lcd_printChar(*str++);
    }
}

/**
 * @brief Scrolls a string across the LCD from right to left.
 *
 * Takes a string and moves it across the screen one step at a time.
 * Clears the screen each step, reprints the string in a new position,
 * and delays between each move to create a scrolling effect.
 *
 * @param str The message to scroll.
 * @param delay_per_step Delay (in ms) between scroll steps.
 * @param repeatTime_ms Total time (in ms) to keep scrolling before stopping.
 */
void scrollText(const char *str, int delay_per_step, int repeatTime_ms) {
    int len = 0;
    while (str[len] != '\0') len++;

    int totalSteps = repeatTime_ms / delay_per_step;
    int col = LCD_WIDTH;

    for (int s = 0; s < totalSteps; s++) {
        lcd_setCursor(0, 0);
        for (int i = 0; i < LCD_WIDTH; i++) lcd_printChar(' ');

        lcd_setCursor(0, col >= 0 ? col : 0);
        for (int i = 0; i < len; i++) {
            int pos = col + i;
            if (pos >= 0 && pos < LCD_WIDTH) {
                lcd_printChar(str[i]);
            }
        }

        col--;
        if (col < -len) col = LCD_WIDTH;

        delay_ms(delay_per_step);
    }
}

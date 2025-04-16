#ifndef LCDLIB_H
#define LCDLIB_H

// Initializes the LCD (must be called once at startup)
void initLCD(void);

// Scrolls a message across the LCD
void lcd_scrollMessage(const char* msg, int width);

// Delay function in milliseconds
void delay_ms(unsigned int ms);

// LCD helpers
void lcd_cmd(char package);
void lcd_printChar(char myChar);
void lcd_setCursor(char x, char y);
void clear(void);

#endif

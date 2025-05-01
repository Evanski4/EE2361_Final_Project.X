/* 
 * File:   LCDlibrary.h
 * Author: madpe
 *
 * Created on May 1, 2025, 1:40 PM
 */

#ifndef LCDLIBRARY_H
#define	LCDLIBRARY_H

#ifdef	__cplusplus
extern "C" {
#endif

void lcd_cmd(char package);
void initLCD(void) ;
void lcd_setCursor(char x, char y);
void lcd_printChar(char myChar);
void clear(void);
void lcd_printStr(const char *str);
void scrollText(const char *str, int delay_per_step, int repeatTime_ms);



#ifdef	__cplusplus
}
#endif

#endif	/* LCDLIBRARY_H */


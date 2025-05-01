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
// #include "bluetooth.h"   // <-- Bluetooth include removed

#define FREQ    16000000UL
#define BAUD    38400

#define LCDaddy 0b0111100
#define addressWrite (LCDaddy << 1) & 0b11111110
#define commandControlByte 0x00
#define dataControlByte 0x40
#define dataStringControlByte 0xC0
#define LCD_WIDTH 10

#define BUTTON PORTBbits.RB7
#define DEBOUNCE_DELAY 50

/*
void uart_init(void) {
    TRISBbits.TRISB7 = 1;   // U1RX as input
    TRISBbits.TRISB6 = 0;   // U1TX as output
    U1BRG = (unsigned int)((FREQ/(16UL*BAUD)) - 1);
    U1MODEbits.UARTEN = 1;  // enable UART
    U1STAbits.UTXEN  = 1;    // enable TX
}

void sendData(const char *s) {
    while (*s) {
        while (U1STAbits.UTXBF); 
        U1TXREG = *s++;
    }
    // terminate line
    while (U1STAbits.UTXBF); U1TXREG = '\r';
    while (U1STAbits.UTXBF); U1TXREG = '\n';
}

void readString(char *buffer, int maxLength) {
    int i = 0;
    char c;
    int timeout = 0;

    do {
        timeout = 0;
        while (!U1STAbits.URXDA) {
            delay(1);     // Wait 1 millisecond
            timeout++;
            if (timeout > 1000) { // Timeout after 1000ms (1 second)
                buffer[i] = '\0'; // Terminate string early
                return;
            }
        }
        c = U1RXREG;
        buffer[i++] = c;
    } while (c != '\n' && i < (maxLength - 1));

    buffer[i] = '\0'; // Null terminate
}
*/

void delay_ms(unsigned int ms) {
    while (ms-- > 0) {
        asm("repeat #15998");
        asm("nop");
    }
}

void write_color(int r, int g, int b) {
    int i;
    
    for (i = 7; i >= 0; i--) {
        if ((r & (1 << i)) > 0) write_1();    
        else write_0();        
    }
   
    for (i = 7; i >= 0; i--) {
        if ((g & (1 << i)) > 0) write_1();          
        else write_0();      
    }  

    for (i = 7; i >= 0; i--) {
        if ((b & (1 << i)) > 0) write_1();    
        else write_0();    
    }  
   
    ETO_wait_100us();
}
volatile unsigned long timerCounter = 0;

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
    PR1 = 124;               // 125 ticks × 8 µs = 1.000 ms
    TMR1 = 0;                // Reset timer
    IEC0bits.T1IE = 1;       // Enable interrupt
    IFS0bits.T1IF = 0;       // Clear flag
}
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void) {
    timerCounter++;         // Increment ms counter
    IFS0bits.T1IF = 0;      // Clear interrupt flag
}
unsigned long int times [5];

int i =0;
int greenRound;
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

void initLCD(void) {
    lcd_cmd(0x3A); lcd_cmd(0x09); lcd_cmd(0x06);
    lcd_cmd(0x1E); lcd_cmd(0x39); lcd_cmd(0x1B);
    lcd_cmd(0x6E); lcd_cmd(0x56); lcd_cmd(0x7A);
    lcd_cmd(0x38); lcd_cmd(0x0F); lcd_cmd(0x3A);
    lcd_cmd(0x09); lcd_cmd(0x1A); lcd_cmd(0x3C);
    lcd_cmd(0x01);
}

void lcd_setCursor(char x, char y) {
    lcd_cmd(((0x20)*x+y) | 0x80);
}

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
unsigned long getAvgTime(void){
    unsigned long int sum = 0;
    for (int i = 0; i < 5; i++) {
        sum += times[i];
    }



    return sum / 5;
}

void lcd_printStr(const char *str) {
    while(*str != '\0') {
        lcd_printChar(*str++);
    }
}

void clear(void) {
    lcd_cmd(0x01);
}

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
                        delay_ms(3000);
                        goto restartGame;  // Restart game from beginning
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

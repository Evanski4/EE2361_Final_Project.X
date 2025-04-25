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

#include "xc.h"
#include <stdlib.h> // for rand
#include "iLEDasm.h"
#include "iLEDwriteColor.h"

// UART parameters for TimeLcd board
#define FREQ    16000000UL
#define BAUD    9600

#define LCDaddy 0b0111100
#define addressWrite (LCDaddy << 1) & 0b11111110
#define commandControlByte 0x00
#define dataControlByte 0x40
#define dataStringControlByte 0xC0
#define LCD_WIDTH 10

#define BUTTON PORTBbits.RB7
#define DEBOUNCE_DELAY 50


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

    do {
        while (!U1STAbits.URXDA); // Wait for a new character
        c = U1RXREG;              // Read received character
        buffer[i++] = c;          // Store it in buffer
    } while (c != '\n' && i < (maxLength - 1)); // Stop on newline or buffer full

    buffer[i] = '\0'; // Null-terminate the string
}


void delay_ms(unsigned int ms) {
    while (ms-- > 0) {
        asm("repeat #15998");
        asm("nop");
    }
}

// === DELAY WRAPPER ===
//void delay(int delay_in_ms) {
//    int i;
//    for (i = 0; i < delay_in_ms; i++) {
//        ETO_wait_1ms();
//    }
//}

void write_color(int r, int g, int b) {
    int i;
    
   
    //red
    for (i = 7; i >= 0; i--) {
        if ((r & (1 << i)) > 0)
            write_1();    
        else
            write_0();        
    }
   
    //green
    for ( i = 7; i >= 0; i--) {
       if ((g & (1 << i)) > 0) write_1();          
       else write_0();      
    }  
    //blue
    for (i = 7; i >= 0; i--) {
        if ((b & (1 << i)) > 0) write_1();    
        else    write_0();    
    }  
   
    ETO_wait_100us();
     
}

// === LCD & GAME SETUP ===
void setup(void) {
    CLKDIVbits.RCDIV = 0;
    AD1PCFG = 0xFFFF;

    TRISA = 0b1111111111111110; // RA0 = output (bit 0 = 0), all others inputs
  // RA0 = output (LED)
    TRISB = 0b10000000;          // RB7 = input (button)
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
}

void waitForButtonPress(void) {
    // Wait for the button to be pressed
    while (BUTTON == 1);
    LATA = 0x0001;  // Turn on an LED (RA0) for feedback
    delay_ms(DEBOUNCE_DELAY);

    // Wait for the button to be released
    while (BUTTON == 0);
    delay_ms(DEBOUNCE_DELAY);
    LATA = 0x0000;  // Turn off the LED
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

void lcd_printStr(const char *str) {
    while(*str != '\0') {
        lcd_printChar(*str++);
    }
}
//TRISB = "AT+ROLE=1\r\n";
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
    setup();
    initLCD();
    char response[100];  // Buffer to hold incoming message
    initUart(); 
    // after we send a AT Command there should be a response we can check the response with readString function
    int randomColor;
    //setup for the HC-05 aka master will have to have a different file to set up the slave 
    sendData("AT+NAMEHC-05");
    readString(response,sizeof(response));
    //check response variable here when debugging should be okHC-05
    sendData("AT+PE"); //sets even parity bit 
    sendData("AT+ROLE=1");          // Set as master
    sendData("AT+CMODE=0");         // Connect to specific address
    sendData("AT+BIND=XXXX,YY,ZZZZZZ"); // Bind to HC-06?s MAC address this address will be found by using the sendData("AT+ADDR?");readString(response, sizeof(response)); on the HC-06 
    scrollText("Press to start", 250, 3000);
    waitForButtonPress();

    scrollText("Only press when light is GREEN", 200, 10000);

    for (int round = 1; round <= 5; round++) {
        clear();
        lcd_setCursor(0, 0);
        lcd_printStr("Round ");
        lcd_printChar('0' + round);
        delay_ms(5000);

        clear();
        lcd_setCursor(0, 0);
        lcd_printStr("Get ready...");
        delay_ms(1500);

        int waitTime = 2000 + rand() % 6001;
        delay_ms(waitTime);
        
        //while(!g)
        randomColor = 1 + rand() % (3-1+1);
        switch(randomColor){
            case 1:
                writeColor(255,0,0);
                break;
            case 2:
                //  g=1;
                writeColor(0,255,0);
                // add input capture stuff here as we want to only start the ic timer and all that when the light turns green 
                //so have to add all that like ic1setup(); and then a while statment to wait until they click and then save that data abd orubt ut )
                clear();
                lcd_setCursor(0, 0);
                lcd_printStr("GO!");
                write_color(0, 255, 0); // Green LED

                waitForButtonPress();
                write_color(0, 0, 0);   // Turn off LED

                clear();
                lcd_setCursor(1, 0);
                lcd_printStr("Nice!");
                delay_ms(1500);
                break;
            case 3:
                writeColor(0,0,255);
                break;
        
        }
        
    

    clear();
    lcd_setCursor(0, 0);
    lcd_printStr("All done!");
    delay_ms(3000);
    }

    
    return 0;
}

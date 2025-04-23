#include <xc.h>  // Include the default header for PIC24


#define BAUD 9600      // Desired baud rate
#define FREQ 16000000  // System clock (in Hz)

void sendData(const char *str){
    // Loop through each character in the string and send it
    while (*str != '\0') {
        while (U1STAbits.UTXBF);  // Wait until the transmit buffer is not full
        U1TXREG = *str;           // Send the current character
        str++;                    // Move to the next character in the string
    }

    // Send the carriage return and line feed after the command
    while (U1STAbits.UTXBF);  // Wait until the transmit buffer is not full
    U1TXREG = '\r';           // Send carriage return
    while (U1STAbits.UTXBF);  // Wait until the transmit buffer is not full
    U1TXREG = '\n';           // Send line feed
}

void initUart(void){
    // Configure U1TX as output, U1RX as input
    TRISBbits.TRISB7 = 0;   // Set U1TX (Pin RB7) as output
    TRISBbits.TRISB6 = 1;   // Set U1RX (Pin RB6) as input

    // Set the baud rate
    unsigned int baud_rate = (FREQ / (16 * BAUD)) - 1; // Formula for baud rate generator
    U1BRG = baud_rate;  // Set the baud rate

    // Enable UART1:
    U1MODEbits.UARTEN = 1;    // Enable UART1 (UxMODE<15>)
    U1STAbits.UTXEN = 1;      // Enable UART1 transmit (UxSTA<10>)
}
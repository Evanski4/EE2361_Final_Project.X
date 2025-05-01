    #include <xc.h>  // Include the default header for PIC24


#define BAUD   38400ul       
#define FOSC   16000000ul

void initUart(void) {
    TRISBbits.TRISB10 = 0;  // TX output
    TRISBbits.TRISB11 = 1;  // RX input

    U1MODE = 0;
    U1STA = 0;

    RPOR5bits.RP10R = 3;    // Map U1TX to RP10 (RB10)
    RPINR18bits.U1RXR = 11; // Map U1RX to RP11 (RB11)

    U1MODEbits.BRGH = 0;
    U1BRG = (FOSC / (16UL * BAUD)) - 1;

    U1MODEbits.UARTEN = 1;  // Enable UART
    U1STAbits.UTXEN = 1;    // Enable Transmit
}

// your sendData() can stay exactly the same
void sendData(const char *str) {
    while (*str) {
        while (U1STAbits.UTXBF);  // wait if TX buffer is full
        U1TXREG = *str++;
    }
    // append CR+LF
    while (U1STAbits.UTXBF);
    U1TXREG = '\r';
    while (U1STAbits.UTXBF);
    U1TXREG = '\n';
}
    /*
     * File:   Main.c
     * Author: madpe
     *
     * Created on February 11, 2025, 4:15 PM
     */

    #include "xc.h"
    #include "iLEDasm.h"
    // CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)


// CW1: FLASH CONFIGURATION WORD 1 (see PIC24 Family Reference Manual 24.1)

#define PERIOD 5 //Compiler constant

//     void delay(int delay_in_ms){
//                for(int i = 0; i<delay_in_ms;i++){
//                    ETO_wait_1ms();
//                }
//
//            }      
    /*Writes the specified RGB value to the iLED
     *r:value of red to output ranged 0-255
     *g:value of red to output ranged 0-255
     *b:value of red to output ranged 0-255
     */
    void writeColor(int r, int g, int b){
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

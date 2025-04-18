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
    void setup(void){
        // Execute once code goes here
    CLKDIVbits.RCDIV = 0;  //Set RCDIV=1:1 (default 2:1) 32MHz or FCY/2=16M
    AD1PCFG = 0x9fff;            //sets all pins to digital I/O
    TRISA = 0b1111111111111110;  //set port A to inputs,
    LATA = 0x0000;               //Set all of port A to HIGH
   
   
    }
     void delay(int delay_in_ms){
                for(int i = 0; i<delay_in_ms;i++){
                    ETO_wait_1ms();
                }

            }      
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
/*Cycles through from red to blue by bytNum 
 * bytNum: how quickly you want to gradient to the other color
 */
            void gradient(int byteNum){
            for(int byteFramNum = 255; byteFramNum >0; byteFramNum--){
                int byteR;
                int byteB;
                byteR = byteFramNum;
                byteB = 255 -byteFramNum;
                writeColor(byteR,0,byteB);
                delay(1);
            }

            for(int byteFramNum = 255; byteFramNum >0; byteFramNum--){
                int byteR,byteB;
                byteB=byteFramNum;
                byteR=255-byteFramNum;
                writeColor(byteR,0,byteB);
                delay(1);
            }

            }
            /*Creates a delay of specified mS
             *delay_in_ms: Time in ms to be delayed
             */
           
            /*Packs the red green adn blue numerical values into one unint32_t
             * Red: value of red in RGB
             * Grnalue of red in RGB
             * Blu: value of red in RGB
             * Return: The specified RGB value 
             */

        /*Gets the Red value from a unint32_t representing a RGB value
         * RGBval: Value of RGB that must pull red from
         * returns: Value of Red as a unsigned char 0-255
         */
       unsigned char getR(unsigned long int RGBval) {
            unsigned char red = 0;
            red = (unsigned char) (RGBval >> 16);
            return red;
   
       }
        /*Gets the Green value from a unint32_t representing a RGB value
         * RGBval: Value of RGB that must pull green from
         * returns: Value of green as a unsigned char 0-255
         */
       unsigned char getG(unsigned long int RGBval) {
        unsigned char green = 0;
        green = (unsigned char) (RGBval >> 8);
        return green;
   
        }
        /*Gets the Blue value from a unint32_t representing a RGB value
         * RGBval: Value of RGB that must pull Blue from
         * returns: Value of Blue as a unsigned char 0-255
         */
        unsigned char getB(unsigned long int RGBval) {

            unsigned char blue = 0;
            blue = (unsigned char) (RGBval);
            return blue;
        }
        /*using the previous functions decodes the packed color and writes it using the writeColor function
         *PackedColor: Color to be written to iLED
         */
        void writePacCol(uint32_t PackedColor){
            writeColor(getR(PackedColor), getG(PackedColor), getB(PackedColor));
            delay(10);
        }
        /*Cycles through the colors of RGB starting at a specified value
         *WheelPos:Value from 0 -255 representing a color
         * return: The color specified
         */
//        uint32_t wheel(unsigned char WheelPos){
//             WheelPos = 255 - WheelPos;
//        if(WheelPos < 85) {
//          return packColor(255 - WheelPos * 3, 0, WheelPos * 3);
//        }
//        if(WheelPos < 170) {
//          WheelPos -= 85;
//          return packColor(0, WheelPos * 3, 255 - WheelPos * 3);
//        }
//        WheelPos -= 170;
//        return packColor(WheelPos * 3, 255 - WheelPos * 3, 0);
//              }
//      void wheelLoop() {
//          int i;
//          int j;
//          //dark to white light
//          for (i = 0; i <= 255; i++) {
//              writePacCol(wheel(i));
//          }
//
//          //white to dark
//          for (j = 255; j >= 0; j--) {
//              writePacCol(wheel(j));
//          }
//}


/* 
 * File:   iLEDwriteColor.h
 * Author: madpe
 *
 * Created on April 16, 2025, 2:45 PM
 */


#ifndef ILEDWRITECOLOR_H
#define	ILEDWRITECOLOR_H
#define PERIOD 5 //Compiler constant
    void setup(void);
     void delay(int delay_in_ms);    
    /*Writes the specified RGB value to the iLED
     *r:value of red to output ranged 0-255
     *g:value of red to output ranged 0-255
     *b:value of red to output ranged 0-255
     */
    void writeColor(int r, int g, int b);
/*Cycles through from red to blue by bytNum 
 * bytNum: how quickly you want to gradient to the other color
 */
            void gradient(int byteNum);
            /*Creates a delay of specified mS
             *delay_in_ms: Time in ms to be delayed
             */
           
            /*Packs the red green adn blue numerical values into one unint32_t
             * Red: value of red in RGB
             * Grnalue of red in RGB
             * Blu: value of red in RGB
             * Return: The specified RGB value 
             */
            uint32_t packColor(unsigned char Red, unsigned char Grn, unsigned char Blu);
        /*Gets the Red value from a unint32_t representing a RGB value
         * RGBval: Value of RGB that must pull red from
         * returns: Value of Red as a unsigned char 0-255
         */
       unsigned char getR(unsigned long int RGBval);
        /*Gets the Green value from a unint32_t representing a RGB value
         * RGBval: Value of RGB that must pull green from
         * returns: Value of green as a unsigned char 0-255
         */
       unsigned char getG(unsigned long int RGBval);
        /*Gets the Blue value from a unint32_t representing a RGB value
         * RGBval: Value of RGB that must pull Blue from
         * returns: Value of Blue as a unsigned char 0-255
         */
        unsigned char getB(unsigned long int RGBval);
        /*using the previous functions decodes the packed color and writes it using the writeColor function
         *PackedColor: Color to be written to iLED
         */
        void writePacCol(uint32_t PackedColor);
        /*Cycles through the colors of RGB starting at a specified value
         *WheelPos:Value from 0 -255 representing a color
         * return: The color specified
         */
        uint32_t wheel(unsigned char WheelPos);
             
      void wheelLoop();


#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* ILEDWRITECOLOR_H */


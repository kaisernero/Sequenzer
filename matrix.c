/*
 * matrix.c
 *
 *  Created on: 22.06.2018
 *      Author: draab
 */

// NACK- not acknowledged

#include <msp430.h>
#include <stdbool.h>

int mts; //matrix timer step

char adress = 0x20; //Address of the pin expander //TODO: this also not working with define, why?
char output_register_address = 0x09; //address for the output register
char dir_register_adress = 0x00; //adress for the direction register

void I2C_init();
void I2C_send_byte(unsigned char adrress, unsigned char registerr, unsigned char data);
void matrix_interrupt();


void I2C_init() { //init the I2C interface

      //configures the microcontroler pins for I2C
      P3DIR |= BIT1 + BIT2;
      P3OUT &= ~(BIT1 + BIT2);
      P3SEL |= BIT1 + BIT2;                     //Enable I2C

      UCB0CTL1 |= UCSWRST;                      // Enable SW reset
      UCB0CTL0 = UCMST + UCMODE_3 + UCSYNC;     // I2C Master, synchronous mode
      UCB0CTL1 = UCSSEL_2 + UCSWRST;            // Use SMCLK, keep SW reset
      UCB0BR0  = 2;    //normal 3               // fSCL = SMCLK/12 = ~100kHz
                                                // fSCL = SMCLK/3  = ~400kHz
                                                // fSCL = SMCLK/2  = ~600kHz
      UCB0BR1 = 0;
      UCB0I2COA = 0x4e;                         // Own address

      UCB0CTL1 &= ~UCSWRST;                     // Clear SW reset, resume operation

}

void I2C_send_byte(unsigned char adrress, unsigned char registerr, unsigned char data){
//     while(UCB0STAT&&UCBBUSY);
//
//        UCB0I2CSA=adrress; //set address
//        UCB0CTL1 |= UCTR + UCTXSTT; //start
//
//        UCB0TXBUF=registerr; //send register address as data
//         while(!(UCB0TXIFG & IFG2)){
//
//                if(UCNACKIFG & UCB0STAT){
//                    //if NACK received
//                    nack_error=1; //set error bit
//                    UCB0CTL1 |= UCTXSTP; //generate stop condition
//                    }
//
//         }
//
//        if(!nack_error) UCB0TXBUF=data;  //send data (the output pattern or the direction pattern) as data
//
//        if(!nack_error) while(!(UCB0TXIFG & IFG2)){
//
//            if(UCNACKIFG & UCB0STAT){
//                //if NACK received
//                nack_error=1; //set error bit
//                UCB0CTL1 |= UCTXSTP; //generate stop condition
//            }
//
//        }
//
//        if(!nack_error) UCB0CTL1 |= UCTXSTP;            // I2C send Stop
//        IFG2 &= ~UCB0TXIFG;                             // delete IRQ Flag
}

void matrix_interrupt() {

}

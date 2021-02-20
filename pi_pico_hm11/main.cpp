#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "src/HM11.h"
#include "hardware/spi.h"
#include "src/NRF24L01.h"

#include "hardware/i2c.h"
#include "hardware/flash.h"
#include "src/BNO055.h"


int main(){

    gpio_init(25);
    gpio_set_dir(25, 1);

    HM11 phone(uart0, 9600, 16, 17);

    NRF24L01 nrf(spi1,9,8);
    nrf.config();

    uint64_t counter = 0;
    char msg[32];
    char buffer[32];
    uint8_t reg = 0;
    nrf.modeTX();

    BNO055 gyro(i2c1, 0x29, 14, 15);

  while(1){

        phone.getData();
        gyro.getData();


        phone.sendMessage((char*)"H:%d\rR:%d.%d\rP:%d\rCm:%d\rCa:%d\rCg:%d\rCs:%d\rCF:%d\rC:%d\r",gyro.heading, gyro.roll,gyro.rollDec,gyro.pitch,
        gyro.calMag, gyro.calAcc,gyro.calGyr,gyro.calSys, gyro.calibrate, gyro.calibrationData);

        sprintf(msg,"%d",gyro.roll);
        msg[30] = 'R';
        msg[31] = 'O';
        nrf.sendMessage(msg);
    
        if( nrf.newMessage() == 1)
        {
            nrf.receiveMessage(msg);

            sprintf(buffer,"%s\r",msg);
            phone.sendMessage(buffer);
        }

         if( phone.newMessage == true){
            phone.newMessage = false;
            phone.parseCommand();

            if( strcmp( phone.fieldName, "Calibrate") == 0){
                gyro.calibrate = phone.fieldValue;
            }
            if( strcmp( phone.fieldName, "Led") == 0){

                gpio_put(25, phone.fieldValue);
            }

        }


    }

    return 0;
}
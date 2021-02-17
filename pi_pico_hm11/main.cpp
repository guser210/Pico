#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "src/HM11.h"
#include "hardware/spi.h"
#include "src/NRF24L01.h"

int main(){

    uart_init(uart0, 9600);
    gpio_set_function(16, GPIO_FUNC_UART);
    gpio_set_function(17, GPIO_FUNC_UART);

    gpio_init(25);
    gpio_set_dir(25, 1);


    HM11 phone(uart0);

    NRF24L01 nrf(spi1, 9, 8);
    nrf.config();

    nrf.modeRX();
    //nrf.modeTX();

    char buffer[32];
    uint8_t reg = 0;

    char msg[32];
    while(1){
        phone.getData();


        if( nrf.newMessage() == 1){
            nrf.receiveMessage(msg);

            sprintf(buffer,"%s\r", msg);

            phone.sendMessage(buffer);
            
        }

        // sprintf(msg,"-45");
        // msg[30] = 'R';
        // msg[31] = 'O';
        // nrf.sendMessage(msg);
        // sleep_ms(1000);
        // sprintf(msg,"45");
        // msg[30] = 'R';
        // msg[31] = 'O';
        // nrf.sendMessage(msg);
        // sleep_ms(1000);

        // reg = nrf.readReg(0);

        // itoa(reg,buffer,2);

        // phone.sendMessage((char*)"reg=%d, %s\r", reg, buffer);



    }

    return 0;
}
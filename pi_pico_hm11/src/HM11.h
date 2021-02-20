#include <stdlib.h>
#include <stdio.h>

#include <string.h>
#include <stdarg.h>
#include "hardware/uart.h"


class HM11
{

    private: // vars.
        uart_inst_t *uartPort;
        char buffer[100] = {0};
        int bufferCounter = 0;
    public: // vars.
        char message[100] = {0};
        bool newMessage = false;

        char fieldName[100] = {0};
        char fieldValueString[100] = {0};
        int fieldValue = 0;



    public: // funcs.
        void getData();
        void parseCommand();

        void sendMessage(char *msg, ... );

private:
    /* data */
public:
    HM11(uart_inst_t *uartPort, uint boudRate, int8_t tx, int8_t rx);
    ~HM11();
};

HM11::HM11(uart_inst_t *uartPort, uint boudRate, int8_t tx, int8_t rx)
{
    this->uartPort  = uartPort;
    uart_init(uart0, boudRate);
    gpio_set_function(tx, GPIO_FUNC_UART);
    gpio_set_function(rx, GPIO_FUNC_UART);

}

HM11::~HM11()
{
}

void HM11::getData(){
    while( uart_is_readable(uartPort) > 0){
        char c = uart_getc(uartPort);

        if( c == '\r' || c == '\n' || bufferCounter >= 100){
            memcpy(message, buffer, sizeof(buffer));
            memset(buffer, 0, sizeof(buffer));

            newMessage = true;

            bufferCounter = 0;
        }else{
            buffer[ bufferCounter++ ] = c;
        }


    }

}

void HM11::parseCommand(){

    char* ptr = strtok(message, ":");

    sprintf(fieldName,"%s",ptr);
    ptr = strtok(NULL, ":");

    sprintf(fieldValueString,"%s",ptr);

    fieldValue = atoi(fieldValueString);


}

void HM11::sendMessage(char *msg, ... ){

    if( uart_is_writable(uartPort) == false) return;

    __builtin_va_list args;

    __builtin_va_start( args, msg);

    char buffer[100] ;
    
    vsprintf( buffer, msg, args);

    __builtin_va_end(args);

    uart_puts(uartPort, buffer);

}




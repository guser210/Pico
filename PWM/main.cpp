#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/uart.h"
#include "comm/HM11.h"


int main(){

    gpio_set_function(15,GPIO_FUNC_PWM);

    uint slice = pwm_gpio_to_slice_num(15);

    pwm_set_clkdiv(slice,123);
    pwm_set_wrap(slice,20000);

    pwm_set_chan_level(slice, PWM_CHAN_B,1500);
    pwm_set_enabled(slice,true);

    gpio_init(25);
    gpio_set_dir(25,1);
    gpio_put(25,1);

    HM11 phone(uart0,9600, 16, 17);
    int counter = 0;
    while(1){
        phone.getData();

        phone.sendMessage((char*)"counter:%d\r",counter++);

        if( phone.newMessage == true){
            phone.parseCommand();

            if( strcmp(phone.fieldName, "X") == 0){
                pwm_set_chan_level(slice, PWM_CHAN_B,phone.fieldValue);
                phone.sendMessage((char*)"Button:Not A\r");
            }
            if( strcmp(phone.fieldName, "B") == 0){
                pwm_set_chan_level(slice, PWM_CHAN_B,phone.fieldValue);
                phone.sendMessage((char*)"Button:Active\r");
            }

        }
    }

    return 0;
}
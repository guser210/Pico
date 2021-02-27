#include "pico/stdlib.h"


void lightUpLed( int bitsToLightUp){
    int bit = 0;
    int index = 0;

    int bitsCountDown = 24;

    while( bitsCountDown--){

        if( bitsToLightUp & (1<<bit++)){
            // one
            for( index =0; index < 11; index++)
                gpio_put(15,1);
            for( index = 0; index < 3; index++)
                gpio_put(15,0);

        } else{
            // zero
            for( index =0; index < 4; index++)
                gpio_put(15,1);
            for( index = 0; index < 11; index++)
                gpio_put(15,0);

        }
    }


}

int main(){
    gpio_init(15);
    gpio_set_dir(15,1);

    for( int index = 0; index < 1000; index++)
        lightUpLed(0);
    sleep_us(10);
    lightUpLed(0);
    sleep_us(10);

    lightUpLed(0x0100);
    lightUpLed(0x01);
    lightUpLed(0x0100);


    while(1){


    }


    return 0;
}
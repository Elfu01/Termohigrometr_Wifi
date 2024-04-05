#include "pico/stdlib.h"
// #include "Inc/eeprom.h"
#include <stdio.h>
#include "am2320.h"
#include "am2320.c"
#include "hardware/i2c.h"
int main()
{
    stdio_init_all();
    float temp, hum;
    init();
    while(1)
    {   
        sensor(&temp, &hum);

        printf("hum: %f\n", hum);
        printf("temp: %f\n", temp);
        printf("=====================\n");
        sleep_ms(2000);
    }

}
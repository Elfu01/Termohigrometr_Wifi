#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "sensor/am2320.h"
#include "sensor/am2320.c"

int main()
{
    stdio_init_all();
    float temp, hum, dewPoint;
    init();
    while(1)
    {   
        sensor(&temp, &hum);
        dewPoint = dew_piont(hum, temp);

        printf("hum: %f\n", hum);
        printf("temp: %f\n", temp);
        printf("dew point: %f\n", dewPoint);
        printf("=====================\n");
        sleep_ms(2000);
    }
}
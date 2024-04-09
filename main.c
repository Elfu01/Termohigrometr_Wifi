#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "sensor/am2320.h"
#include "sensor/am2320.c"

#include "web_server/webserver_connect.c"

int main()
{
    stdio_init_all();
    float temp, hum, dewPoint;
    struct data data1;
    data1.temp_sum = 0;
    data1.hum_sum = 0;
    data1.counter = 1;

    init();
    sensor(&temp, &hum, 1); //musi być wykonane przed webserver_connect() bo inaczej strona chyba jebnie
    //webserver_connect();
    while(1)
    {   
        sensor(&temp, &hum, 1);
        dewPoint = dew_piont(temp, hum);
        get_data(&data1, temp, hum);
        printf("hum: %f\n", hum);
        printf("temp: %f\n", units(temp, 1));
        printf("dew point: %f\n", units(dewPoint, 1));
        printf("temp avg: %f\n", units(data1.avg_temp, 1));
        printf("max temp: %f\n", units(data1.max_temp, 1));
        printf("=====================\n");
        sleep_ms(2000);
    }
}
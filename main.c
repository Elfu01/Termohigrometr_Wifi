#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#include "sensor/am2320.h"
#include "sensor/am2320.c"

#include "web_server/webserver_connect.c"

int main()
{
    stdio_init_all();
    
    float temp, hum, dewPoint;
    uint8_t unit = 0;
    bool mode = 0;
    struct data data1;
    data1.temp_sum = 0;
    data1.hum_sum = 0;
    data1.counter = 1;
    clock_t before = clock();
    clock_t now = clock();
    init();
    
    sensor(&temp, &hum, 1); //musi być wykonane przed webserver_connect() bo inaczej strona chyba jebnie
    //webserver_connect();
    while(1)
    {   
        if(now - before > 200)
        {
            sensor(&temp, &hum, 1);
            get_data(&data1, temp, hum);
            dewPoint = dew_point(temp, hum);
            before = now;
            units(&data1, &temp, &dewPoint, unit);
            printf("hum: %f\n", hum);
            printf("temp: %f\n", temp);
            printf("dew point: %f\n", dewPoint);
            printf("=====================\n");
        }
        
        switch(check_button())
        {
            case 1: //units
                if(unit >= 2)   unit = 0;             
                else unit = unit + 1;
            break;

            case 2: //mode
                mode = !mode;
                if(!mode) 
                {
                    units(&data1, &temp, &dewPoint, unit);
                    printf("hum: %f\n", hum);
                    printf("temp: %f\n", temp);
                    printf("dew point: %f\n", dewPoint);
                    printf("=====================\n");
                }
                else 
                {
                    units(&data1, &temp, &dewPoint, unit);
                    printf("temp avg: %f\n", data1.avg_temp);
                    printf("max temp: %f\n", data1.max_temp);
                    printf("=====================\n");
                }
            break;
        }

        now = clock();
    }
}
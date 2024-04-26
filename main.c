#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#include "sensor/am2320.h"
#include "sensor/am2320.c"

#include "eeprom/eeprom.h"
#include "eeprom/eeprom.c"

// #include "oled/raspberry26x32.h"
// #include "oled/ssd1306_font.h"
#include "oled/ssd1306_i2c.c"

#include "web_server/webserver_connect.c"

int main()
{
    stdio_init_all();
    float temp, hum, dewPoint;
    uint8_t unit = 0;
    uint8_t mode = 0;
    struct data data1;
    uint8_t bufw[8];
    uint8_t bufr[8];
    data1.temp_sum = 0;
    data1.hum_sum = 0;
    data1.counter = 1;
    clock_t before = clock();
    clock_t now = clock();
    float tr, hr;
    char temp_str, hum_str;
    init();

    //gotowiec raspberry--------------------------------------------------
    SSD1306_init();
    struct render_area frame_area = {
    start_col: 0,
    end_col : SSD1306_WIDTH - 1,
    start_page : 0,
    end_page : SSD1306_NUM_PAGES - 1
    };

    calc_render_area_buflen(&frame_area);

    // zero the entire display
    uint8_t buf[SSD1306_BUF_LEN];
    memset(buf, 0, SSD1306_BUF_LEN);
    render(buf, &frame_area);

    char *text[] = {
        "Termohigrometr",
        "Wifi",
        "projekt",
        "IPP"
    };

    int y = 0;
    for (int i = 0 ;i < count_of(text); i++) {
        WriteString(buf, 5, y, text[i]);
        y+=8;
    }
    render(buf, &frame_area);
    //gotowiec raspberry----------------------------------------------------


    sensor(&temp, &hum, 1); //musi być wykonane przed webserver_connect() bo inaczej strona chyba jebnie
    //webserver_connect();


    while(1)
    {   
        if(now - before > 300)
        {
            sensor(&temp, &hum, 1);
            get_data(&data1, temp, hum);
            dewPoint = dew_point(temp, hum);
            before = now;
            if(mode == 0)
            {
                printf("hum: %f\n", hum);
                printf("temp: %f\n", temp);
                printf("=====================\n");
                text[0] = "temperature";
                sprintf(&temp_str, "%f", temp);
                text[1] = &temp_str;
                //sprintf(&num_str,"0");
                text[2] = "humidity";
                sprintf(&hum_str, "%f", hum);
                text[3] = &hum_str;
                // zero the entire display
                memset(buf, 0, SSD1306_BUF_LEN);
                render(buf, &frame_area);
                y = 0;
                for (int i = 0 ;i < count_of(text); i++) {
                    WriteString(buf, 1, y, text[i]);
                    y+=8;
                }
                render(buf, &frame_area);
            }
        }

        switch(check_button())
        {
            case 1: //units
                if(unit >= 2)   unit = 0;             
                else unit = unit + 1;
            break;

            case 2: //mode
                mode = mode + 1;
                if(mode == 5) mode = 0;

                if(mode == 0) //normalne wyswietlanie
                {


                    printf("hum: %f\n", hum);
                    printf("temp: %f\n", temp);
                    printf("=====================\n");
                }

                else if(mode == 1)//punkt rosy
                {
                    printf("dew point: %f\n", dewPoint);
                    printf("=====================\n");
                }

                else if(mode == 2)//staty temp
                {
                    printf("temp avg: %f\n", data1.avg_temp);
                    printf("max temp: %f\n", data1.max_temp);
                    printf("min temp: %f\n", data1.min_temp);
                    printf("=====================\n");
                }

                else if(mode == 3)//staty hum
                {
                    printf("hum avg: %f\n", data1.avg_hum);
                    printf("max hum: %f\n", data1.max_hum);
                    printf("min hum: %f\n", data1.min_hum);
                    printf("=====================\n");
                }

                else if(mode == 4)//hold
                {
                    eeprom_read_byte(0, 0, bufr, 8);
                    tr = bytesToFloat(bufr);
                    hr = bytesToFloat(&bufr[4]);
                    printf("odczytane z eeprom %f | %f\n", tr, hr);
                    printf("=====================\n");
                }
                
            break;

            case 3: //eeprom hold
                float2Bytes(bufw, temp);
                float2Bytes(&bufw[4], hum);
                eeprom_write_byte(0, 0, bufw, 8);
            break;
        }

        now = clock();
    }
}
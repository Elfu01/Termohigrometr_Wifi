#include <stdio.h>
#include <time.h>

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
// .
#include "web_server/webserver_connect.c"

int main()
{
    srand(time(NULL)); 
    stdio_init_all();
    float temp, hum, dewPoint;
    unit = 0;
    uint8_t mode = 0;
    struct data data1;
    uint8_t bufw[8];
    uint8_t bufr[8];
    data1.temp_sum = 0;
    data1.hum_sum = 0;
    data1.counter = 1;
    clock_t before = clock();
    clock_t now = clock();
    //tr hr
    char *num_str[3] = {"0", "0", "0"}; //temp, hum, dew,  
    char disp_str[10];
    char disp_unit;
    init();
    eeprom_read_byte(0, 0, bufr, 8);
    tr = bytesToFloat(bufr);
    hr = bytesToFloat(&bufr[4]);
    // sleep_ms(1000);
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
    webserver_connect();
    

    //printf("przed while\n");
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
                // printf("hum: %f\n", hum);
                // printf("temp: %f\n", temp);
                printf("%0.1f\r%0.1f\n", hum, temp);
                memset(buf, 0, SSD1306_BUF_LEN);
                render(buf, &frame_area);
                WriteString(buf, 1, 0, "temperature");
                sprintf(disp_str, "%0.1f", units(temp, unit, &disp_unit));
                if(disp_unit != 'K')
                    sprintf(disp_str + strlen(disp_str), "%c", 248);
                sprintf(disp_str + strlen(disp_str), "%c", disp_unit);
                WriteString(buf, 1, 8, disp_str);
                WriteString(buf, 1, 16, "humidity");
                sprintf(disp_str, "%0.1f%%", hum);
                WriteString(buf, 1, 24, disp_str);
                render(buf, &frame_area);
            }

            else if(mode == 1)
            {
                memset(buf, 0, SSD1306_BUF_LEN);
                render(buf, &frame_area);
                WriteString(buf, 1, 0, "dew point");
                sprintf(disp_str, "%0.1f", units(dewPoint, unit, &disp_unit));
                if(disp_unit != 'K')
                    sprintf(disp_str + strlen(disp_str), "%c", 248);
                sprintf(disp_str + strlen(disp_str), "%c", disp_unit);
                WriteString(buf, 1, 8, disp_str);
                render(buf, &frame_area);
            }
        }

        switch(check_button())
        {
            case 1: //units
                if(unit >= 2)   unit = 0;             
                else unit = unit + 1;

                if(mode == 1)//punkt rosy
                {
                    memset(buf, 0, SSD1306_BUF_LEN);
                    render(buf, &frame_area);
                    WriteString(buf, 1, 0, "dew point");
                    sprintf(disp_str, "%0.1f", units(dewPoint, unit, &disp_unit));
                    if(disp_unit != 'K')
                        sprintf(disp_str + strlen(disp_str), "%c", 248);
                    sprintf(disp_str + strlen(disp_str), "%c", disp_unit);
                    WriteString(buf, 1, 8, disp_str);
                    render(buf, &frame_area);
                }

                else if(mode == 2)//staty temp
                {
                    memset(buf, 0, SSD1306_BUF_LEN);
                    render(buf, &frame_area);
                    WriteString(buf, 1, 0, "temperature");
                    sprintf(disp_str, "min %0.1f", units(data1.min_temp, unit, &disp_unit));
                    if(disp_unit != 'K')
                        sprintf(disp_str + strlen(disp_str), "%c", 248);
                    sprintf(disp_str + strlen(disp_str), "%c", disp_unit);
                    WriteString(buf, 1, 8, disp_str);
                    sprintf(disp_str, "max %0.1f", units(data1.max_temp, unit, &disp_unit));
                    if(disp_unit != 'K')
                        sprintf(disp_str + strlen(disp_str), "%c", 248);
                    sprintf(disp_str + strlen(disp_str), "%c", disp_unit);
                    WriteString(buf, 1, 16, disp_str);
                    sprintf(disp_str, "avg %0.1f", units(data1.avg_temp, unit, &disp_unit));
                    if(disp_unit != 'K')
                        sprintf(disp_str + strlen(disp_str), "%c", 248);
                    sprintf(disp_str + strlen(disp_str), "%c", disp_unit);
                    WriteString(buf, 1, 24, disp_str);
                    render(buf, &frame_area);
                }

                else if(mode == 4)//hold
                {
                    eeprom_read_byte(0, 0, bufr, 8);
                    tr = bytesToFloat(bufr);
                    hr = bytesToFloat(&bufr[4]);
                    memset(buf, 0, SSD1306_BUF_LEN);
                    render(buf, &frame_area);
                    WriteString(buf, 1, 0, "hold");
                    sprintf(disp_str, "temp %0.1f", units(tr, unit, &disp_unit));
                    if(disp_unit != 'K')
                        sprintf(disp_str + strlen(disp_str), "%c", 248);
                    sprintf(disp_str + strlen(disp_str), "%c", disp_unit);
                    WriteString(buf, 1, 8, disp_str);
                    sprintf(disp_str, "humi %0.1%%f", hr);
                    WriteString(buf, 1, 16, disp_str);
                    render(buf, &frame_area);
                }
            break;

            case 2: //mode
                mode = mode + 1;
                if(mode == 5) mode = 0;

                if(mode == 0) //normalne wyswietlanie
                {
                    memset(buf, 0, SSD1306_BUF_LEN);
                    render(buf, &frame_area);
                    WriteString(buf, 1, 0, "temperature");
                    sprintf(disp_str, "%0.1f", units(temp, unit, &disp_unit));
                    if(disp_unit != 'K')
                        sprintf(disp_str + strlen(disp_str), "%c", 248);
                    sprintf(disp_str + strlen(disp_str), "%c", disp_unit);
                    WriteString(buf, 1, 8, disp_str);
                    WriteString(buf, 1, 16, "humidity");
                    sprintf(disp_str, "%0.1f%%", hum);
                    WriteString(buf, 1, 24, disp_str);
                    render(buf, &frame_area);
                }

                else if(mode == 1)//punkt rosy
                {
                    memset(buf, 0, SSD1306_BUF_LEN);
                    render(buf, &frame_area);
                    WriteString(buf, 1, 0, "dew point");
                    sprintf(disp_str, "%0.1f", units(dewPoint, unit, &disp_unit));
                    if(disp_unit != 'K')
                        sprintf(disp_str + strlen(disp_str), "%c", 248);
                    sprintf(disp_str + strlen(disp_str), "%c", disp_unit);
                    WriteString(buf, 1, 8, disp_str);
                    render(buf, &frame_area);
                }

                else if(mode == 2)//staty temp
                {
                    memset(buf, 0, SSD1306_BUF_LEN);
                    render(buf, &frame_area);
                    WriteString(buf, 1, 0, "temperature");
                    sprintf(disp_str, "min %0.1f", units(data1.min_temp, unit, &disp_unit));
                    if(disp_unit != 'K')
                        sprintf(disp_str + strlen(disp_str), "%c", 248);
                    sprintf(disp_str + strlen(disp_str), "%c", disp_unit);
                    WriteString(buf, 1, 8, disp_str);
                    sprintf(disp_str, "max %0.1f", units(data1.max_temp, unit, &disp_unit));
                    if(disp_unit != 'K')
                        sprintf(disp_str + strlen(disp_str), "%c", 248);
                    sprintf(disp_str + strlen(disp_str), "%c", disp_unit);
                    WriteString(buf, 1, 16, disp_str);
                    sprintf(disp_str, "avg %0.1f", units(data1.avg_temp, unit, &disp_unit));
                    if(disp_unit != 'K')
                        sprintf(disp_str + strlen(disp_str), "%c", 248);
                    sprintf(disp_str + strlen(disp_str), "%c", disp_unit);
                    WriteString(buf, 1, 24, disp_str);
                    render(buf, &frame_area);
                }

                else if(mode == 3)//staty hum
                {
                    memset(buf, 0, SSD1306_BUF_LEN);
                    render(buf, &frame_area);
                    WriteString(buf, 1, 0, "humidity");
                    sprintf(disp_str, "min %0.1f%%", data1.min_hum);
                    WriteString(buf, 1, 8, disp_str);
                    sprintf(disp_str, "max %0.1f%%", data1.max_hum);
                    WriteString(buf, 1, 16, disp_str);
                    sprintf(disp_str, "avg %0.1f%%", data1.avg_hum);
                    WriteString(buf, 1, 24, disp_str);
                    render(buf, &frame_area);
                }

                else if(mode == 4)//hold
                {

                    // printf("odczytane z eeprom %f | %f\n", tr, hr);
                    // printf("=====================\n");

                    memset(buf, 0, SSD1306_BUF_LEN);
                    render(buf, &frame_area);
                    WriteString(buf, 1, 0, "hold");
                    sprintf(disp_str, "temp %0.1f", units(tr, unit, &disp_unit));
                    if(disp_unit != 'K')
                        sprintf(disp_str + strlen(disp_str), "%c", 248);
                    sprintf(disp_str + strlen(disp_str), "%c", disp_unit);
                    WriteString(buf, 1, 8, disp_str);
                    sprintf(disp_str, "humi %0.1f%%", hr);
                    WriteString(buf, 1, 16, disp_str);
                    render(buf, &frame_area);
                }
                
            break;

            case 3: //eeprom hold
                float2Bytes(bufw, temp);
                float2Bytes(&bufw[4], hum);
                eeprom_write_byte(0, 0, bufw, 8);
                tr = temp;
                hr = hum;
                // sleep_ms(100);
                // eeprom_read_byte(0, 0, bufr, 8);
                // tr = bytesToFloat(bufr);
                // hr = bytesToFloat(&bufr[4]);
            break;

            // default:

            // break;
        }

        now = clock();
    }
}
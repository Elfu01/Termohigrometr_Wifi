#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "../sensor/am2320.h"
#include "../eeprom/eeprom.h"

// CGI handler which is run when a request for /led.cgi is detected
const char * cgi_led_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    // Check if an request for LED has been made (/led.cgi?led=x)
    if (strcmp(pcParam[0] , "led") == 0){
        // Look at the argument to check if LED is to be turned on (x=1) or off (x=0)
        if(strcmp(pcValue[0], "0") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        else if(strcmp(pcValue[0], "1") == 0)
            cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    }
    
    // Send the index page back to the user
    return "/index.shtml";
}

const char * cgi_unit_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    if (strcmp(pcParam[0] , "unit") == 0)
    {
        if(strcmp(pcValue[0], "0") == 0)
            unit = 0;
        else if(strcmp(pcValue[0], "1") == 0)
            unit = 1;
        else if(strcmp(pcValue[0], "2") == 0)
            unit = 2;
    }

    return "/index.shtml";
}

const char * cgi_eeprom_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    float temp, hum;
    uint8_t bufw[8];
    if (strcmp(pcParam[0] , "eeprom") == 0)
    {
        if(strcmp(pcValue[0], "0") == 0) 
        {
            sensor(&temp, &hum, 0);
            float2Bytes(bufw, temp);
            float2Bytes(&bufw[4], hum);
            eeprom_write_byte(0, 0, bufw, 8);
        }

    }

    return "/index.shtml";
}

const char * cgi_frequency_handler(int iIndex, int iNumParams, char *pcParam[], char *pcValue[])
{
    float temp, hum;
    uint8_t bufw[8];
    if (strcmp(pcParam[0] , "frequency") == 0)
    {
        if(strcmp(pcValue[0], "0") == 0) 
            refresh_time = 500;

        else if(strcmp(pcValue[0], "1") == 0) 
            refresh_time = 3000;

        else if(strcmp(pcValue[0], "2") == 0) 
            refresh_time = 6000;

        else if(strcmp(pcValue[0], "3") == 0) 
            refresh_time = 10 * 6000;

    }

    return "/index.shtml";
}

// tCGI Struct
// Fill this with all of the CGI requests and their respective handlers
static const tCGI cgi_handlers[] = {
    {
        // Html request for "/led.cgi" triggers cgi_handler
        "/led.cgi", cgi_led_handler
    },
    {
        "/unit.cgi", cgi_unit_handler
    },
    {
        "/eeprom.cgi", cgi_eeprom_handler
    },
    {
        "/frequency.cgi", cgi_frequency_handler
    },
};

void cgi_init(void)
{
    http_set_cgi_handlers(cgi_handlers, 4);
}
#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "../sensor/am2320.h"
//#include "../sensor/am2320.c"



// SSI tags - tag length limited to 8 bytes by default
const char * ssi_tags[] = {"hum", "temp", "led", "dew"};

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) 
{
  size_t printed;
  float temp, hum, dew;
  switch (iIndex) 
  {
    case 0: // hum
      sensor(&temp, &hum, 0);
      printed = snprintf(pcInsert, iInsertLen, "%f", hum);
    break;

    case 1: // temp
      sensor(&temp, &hum, 0);
      printed = snprintf(pcInsert, iInsertLen, "%f", temp);
    break;

    case 2: // led
      {
        bool led_status = cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);

        if(led_status == true)
          printed = snprintf(pcInsert, iInsertLen, "ON");
        else
          printed = snprintf(pcInsert, iInsertLen, "OFF");
      }
    break;

    case 3: // dew
      sensor(&temp, &hum, 0);
      dew = dew_piont(temp, hum);
      printed = snprintf(pcInsert, iInsertLen, "%f", dew);
    break;

    default:
      printed = 0;
    break;
  }

  return (u16_t)printed;
}

// Initialise the SSI handler
void ssi_init() 
{
  http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
}

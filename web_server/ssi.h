#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "../sensor/am2320.h"
#include "../eeprom/eeprom.h"
//#include "../sensor/am2320.c"




// SSI tags - tag length limited to 8 bytes by default
const char * ssi_tags[] = {"hum", "temp", "led", "dew", "maxt", "mint", "avgt", "maxh", "minh", "avgh", "etemp", "ehum"};



u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) 
{
  size_t printed;
  float temp, hum, dew;
  struct data data1;
  char disp_unit;
  data1.temp_sum = 0;
  data1.hum_sum = 0;
  data1.counter = 1;
  get_data(&data1, temp, hum);
  switch (iIndex) 
  {
    case 0: // hum
      sensor(&temp, &hum, 0);
      printed = snprintf(pcInsert, iInsertLen, "%0.1f %%", hum);
    break;

    case 1: // temp
      sensor(&temp, &hum, 0);
      printed = snprintf(pcInsert, iInsertLen, "%0.1f", units(temp, unit, &disp_unit));
      if(disp_unit != 'K')
        printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%c", 248);
      printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%c", disp_unit);
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
      dew = dew_point(temp, hum);
      printed = snprintf(pcInsert, iInsertLen, "%0.1f", units(dew, unit, &disp_unit));
      if(disp_unit != 'K')
        printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%c", 248);
      printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%c", disp_unit);
    break;

    // case 4: //maxt
    //   printed = snprintf(pcInsert, iInsertLen, "%f", data1.max_temp);
    // break;

    // case 5: //mint
    //   printed = snprintf(pcInsert, iInsertLen, "%f", data1.min_temp);
    // break;

    // case 6: //avgt
    //   printed = snprintf(pcInsert, iInsertLen, "%f", data1.avg_temp);
    // break;

    // case 7: //maxh
    //   printed = snprintf(pcInsert, iInsertLen, "%f", data1.max_hum);
    // break;

    // case 8: //minh
    //   printed = snprintf(pcInsert, iInsertLen, "%f", data1.min_hum);
    // break;

    // case 9: //avgh
    //   printed = snprintf(pcInsert, iInsertLen, "%f", data1.avg_hum);
    // break;

    case 10: //etemp
      printed = snprintf(pcInsert, iInsertLen, "%0.1f", units(tr, unit, &disp_unit));
      if(disp_unit != 'K')
        printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%c", 248);
      printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%c", disp_unit);
    break;

    case 11: //ehum
      printed = snprintf(pcInsert, iInsertLen, "%f %%", hr);
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

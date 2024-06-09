#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "../sensor/am2320.h"
#include "../eeprom/eeprom.h"
//#include "../sensor/am2320.c"


// SSI tags - tag length limited to 8 bytes by default
const char * ssi_tags[] = {"hum", "temp", "led", "dew", "maxt", "mint", "avgt", "maxh", "minh", "avgh", "etemp", "ehum"};

// struct data
// {
//   float max_temp;
//   float min_temp;
//   float max_hum;
//   float min_hum;
//   float avg_temp;
//   float avg_hum;
//   long int counter;
//   float temp_sum;
//   float hum_sum;
// };

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen) 
{
  size_t printed;
  static float temp, hum, dew;
  static struct data data1;
  float max_temp, min_temp, avg_temp, max_hum, min_hum, avg_hum;
  char disp_unit;
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
        printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%s", "&#176;");
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
        printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%s", "&#176;");
      printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%c", disp_unit);
    break;

    case 4: //maxt
      max_temp = get_data(&data1, temp, hum, 1);
      //printed = snprintf(pcInsert, iInsertLen, "%f", max_temp);
      printed = snprintf(pcInsert, iInsertLen, "%0.1f", units(max_temp, unit, &disp_unit));
      if(disp_unit != 'K')
        printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%s", "&#176;");
      printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%c", disp_unit);
    break;

    case 5: //mint
      min_temp = get_data(&data1, temp, hum, 2);
      printed = snprintf(pcInsert, iInsertLen, "%0.1f", units(min_temp, unit, &disp_unit));
      if(disp_unit != 'K')
        printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%s", "&#176;");
      printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%c", disp_unit);
    break;

    case 6: //avgt
      avg_temp = get_data(&data1, temp, hum, 3);
      printed = snprintf(pcInsert, iInsertLen, "%0.1f", units(avg_temp, unit, &disp_unit));
      if(disp_unit != 'K')
        printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%s", "&#176;");
      printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%c", disp_unit);
    break;

    case 7: //maxh
      max_hum = get_data(&data1, temp, hum, 4);
      printed = snprintf(pcInsert, iInsertLen, "%0.1f %%", max_hum);
    break;

    case 8: //minh
      min_hum = get_data(&data1, temp, hum, 5);
      printed = snprintf(pcInsert, iInsertLen, "%0.1f %%", min_hum);
    break;

    case 9: //avgh
      avg_hum = get_data(&data1, temp, hum, 6);
      printed = snprintf(pcInsert, iInsertLen, "%0.1f %%", avg_hum);
    break;

    case 10: //etemp
      printed = snprintf(pcInsert, iInsertLen, "%0.1f", units(tr, unit, &disp_unit));
      if(disp_unit != 'K')
        printed = printed + snprintf(pcInsert + strlen(pcInsert), iInsertLen, "%c", "&#176;");
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

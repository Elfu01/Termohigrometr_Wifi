#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

#include "am2320.h"



unsigned int CRC16(uint8_t *ptr, uint8_t length)
{

      unsigned int crc = 0xFFFF;
      uint8_t s = 0x00;

      while(length--) {
        crc ^= *ptr++;
        for(s = 0; s < 8; s++) {
          if((crc & 0x01) != 0) {
            crc >>= 1;
            crc ^= 0xA001;

					} else crc >>= 1;
				}
      }
      return crc;
}

void init()
{
    i2c_init(i2c0, 100 * 1000);
    gpio_set_function(16, GPIO_FUNC_I2C);
    gpio_set_function(17, GPIO_FUNC_I2C);
        // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
}

void sensor(float *temp, float *hum, bool read)
{
  static float temperature, humiditi;
  
  if(!read)
  {
    *temp = temperature;
    *hum = humiditi;
    return;
  }

  uint8_t data_t[3];
	uint8_t buf[8];

  

	data_t[0]=0x03;
	data_t[1]=0x00;
	data_t[2]=0x04;
    
  i2c_write_blocking(i2c0, 0x5C, NULL, 1, false);
  busy_wait_us(800);
  i2c_write_blocking(i2c0, 0x5C, data_t, 3, false); //b8
  busy_wait_us(1500);
  i2c_read_blocking(i2c0, 0x5C, buf, 8, false);//b9


  if ((buf[7] << 8) + buf[6] == CRC16(buf, 6)) 
  {
	  *hum = (buf[2] << 8) + buf[3];
		*hum = *hum / 10.0;

    *temp = ((buf[4] & 0x7F) << 8) + buf[5];
    if((buf[4] & 0x80) == 0x80) *temp = *temp / (-10.0);
    else *temp = *temp / 10.0;

    temperature = *temp;
    humiditi = *hum;
	}
}

float dew_piont(float temp, float hum)
{
  return pow(hum / 100.0, 1.0 / 8.0) * (112.0 + (0.9 * temp)) + (0.1 * temp) - 112.0;
}



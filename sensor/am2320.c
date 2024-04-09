#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

#include "am2320.h"


struct data
{
  float max_temp;
  float min_temp;
  float max_hum;
  float min_hum;
  float avg_temp;
  float avg_hum;
  long int counter;
  float temp_sum;
  float hum_sum;
};


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

  struct data data1;
  
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

float avg_temp(float temp)
{
  static float sum = 0;
  static int count = 0;
  sum = sum + temp;
  count++;
  return sum/count;

}

float avg_hum(float hum)
{
  static float sum = 0;
  static int count = 0;
  sum = sum + hum;
  count++;
  return sum/count;

}

void get_data(struct data *data1, float temp, float hum)
{
  struct data data_t;

  data_t = *data1;

  data_t.temp_sum += temp;  
  data_t.hum_sum += hum;  

  data_t.avg_temp = data_t.temp_sum / data_t.counter;
  data_t.avg_hum = data_t.hum_sum / data_t.counter;

  data_t.counter = data_t.counter + 1;

  if(data_t.counter == 2)
  {
    data_t.max_temp = temp;
    data_t.min_temp = temp;
    data_t.max_hum = hum;
    data_t.min_hum = hum;

    *data1 = data_t;

    return;
  }

  if(temp > data_t.max_temp)
    data_t.max_temp = temp;

  if(temp < data_t.min_temp)
    data_t.min_temp = temp;

  if(hum > data_t.max_hum)
    data_t.max_hum = hum;

  if(hum < data_t.min_hum)
    data_t.min_hum = hum;

  *data1 = data_t;
}

float units(float temp, uint8_t unit)
{ 
  switch(unit)
  {
    case 0: //C
      return temp;
    case 1: //F
      return temp * 9 / 5 + 32;
    case 2: //K
      return temp + 273,15;
    default:
      return temp;
  }







  if(unit) //fahrenheit
    return temp * 9 / 5 + 32;
  
  else //kelwin
    return temp + 273,15;
}
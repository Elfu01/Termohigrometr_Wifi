void init();
unsigned int CRC16(uint8_t *ptr, uint8_t length);
void sensor(float *temp, float *hum);
float dew_piont(float hum, float temp);
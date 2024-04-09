void init();
unsigned int CRC16(uint8_t *ptr, uint8_t length);
void sensor(float *temp, float *hum, bool read);
float dew_piont(float temp, float hum);
float units(float temp, uint8_t unit);
//void get_data(struct data *data1, float temp, float hum);


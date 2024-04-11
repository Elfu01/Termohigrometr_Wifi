#define UNITS_PIN 20
#define MODE_PIN 26
#define SCL 17
#define SDA 16
#define AM2320_ADDR 0X5C

struct data;
void init();
unsigned int CRC16(uint8_t *ptr, uint8_t length);
void sensor(float *temp, float *hum, bool read);
float dew_point(float temp, float hum);
void units(struct data *data1, float *temp, float *dewPoint, uint8_t unit);
void get_data(struct data *data1, float temp, float hum);
uint8_t check_button();



#define HOLD_PIN 18
#define UNITS_PIN 20
#define MODE_PIN 26
#define SCL 1
#define SDA 0
#define AM2320_ADDR 0X5C

struct data;
void init();
unsigned int CRC16(uint8_t *ptr, uint8_t length);
void sensor(float *temp, float *hum, bool read);
float dew_point(float temp, float hum);
float units(float temp, uint8_t unit, char *disp);
void get_data(struct data *data1, float temp, float hum);
uint8_t check_button();



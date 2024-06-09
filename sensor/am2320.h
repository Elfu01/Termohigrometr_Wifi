#define HOLD_PIN 21
#define UNITS_PIN 18
#define MODE_PIN 16
#define SCL 27
#define SDA 26
#define AM2320_ADDR 0X5C

uint8_t unit;
int refresh_time;

struct data;


void init();
unsigned int CRC16(uint8_t *ptr, uint8_t length);
void sensor(float *temp, float *hum, bool read);
float dew_point(float temp, float hum);
float units(float temp, uint8_t unit, char *disp);
float get_data(struct data *data1, float temp, float hum, uint8_t num);
uint8_t check_button();



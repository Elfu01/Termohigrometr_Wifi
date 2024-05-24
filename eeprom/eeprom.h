#define EEPROM_ADDR 0xA0

// Define the Page Size and number of pages
#define PAGE_SIZE 8     // in Bytes
#define PAGE_NUM  16    // number of pages
#define PAGE_ADDRESS_START 3 //
float tr, hr;
uint16_t bytes_to_write(uint16_t size, uint16_t offset);
void eeprom_write_byte(uint8_t page, uint8_t offset, uint8_t *data, uint8_t size);
uint8_t eeprom_read_byte(uint8_t page, uint8_t offset, uint8_t *data, uint8_t size);
void float2Bytes(uint8_t *bytes_arr, float float_variable);
float bytesToFloat(const unsigned char* byte_array);
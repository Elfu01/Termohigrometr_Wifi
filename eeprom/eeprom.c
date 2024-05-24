#include <string.h>

uint16_t bytes_to_write(uint16_t size, uint16_t offset)
{
    if ((size + offset) < 8)
        return size;

    else
        return 8 - offset;
}

void eeprom_write_byte(uint8_t page, uint8_t offset, uint8_t *data, uint8_t size)
{
    uint8_t start_page = page;
    uint8_t end_page = page + ((size + offset) / 8);
    uint8_t num_of_pages = end_page - start_page + 1;
    uint8_t pos = 0;
    uint8_t data_send[size + 1];
    
    for(int j = 0; j <  size; j++)
        data_send[j + 1] = data[j];
    
    //size = size + 1;
    for (int i = 0; i < num_of_pages; i++)
    {
        uint8_t mem_address = start_page << 3 | offset;
        data_send[pos] = mem_address;
        uint8_t bytes_remaining = bytes_to_write(size, offset);
        i2c_write_blocking(i2c1, 0x50, &data_send[pos], bytes_remaining + 1, false);
        start_page = start_page + 1;
        offset = 0;
        size = size - bytes_remaining;
        pos = pos + bytes_remaining - 1;
        sleep_ms(5);
        
    }
    
}

uint8_t eeprom_read_byte(uint8_t page, uint8_t offset, uint8_t *data, uint8_t size)
{
    uint8_t start_page = page;
    uint8_t end_page = page + ((size + offset) / 8);
    uint8_t num_of_pages = end_page - start_page + 1;
    uint8_t pos = 0;
    for (int i = 0; i < num_of_pages; i++)
    {
        uint8_t mem_address = start_page << 3 | offset;
        uint8_t bytes_remaining = bytes_to_write(size, offset);
        i2c_write_blocking(i2c1, 0x50, &mem_address, 1, true);
        i2c_read_blocking(i2c1, 0x50, &data[pos], bytes_remaining, false);
        start_page = start_page + 1;
        offset = 0;
        size = size - bytes_remaining;
        pos = pos + bytes_remaining;
        sleep_ms(100);
    }
    return 0;
}

void float2Bytes(uint8_t *bytes_arr, float float_variable) 
{
    union 
    {
        float a;
        uint8_t bytes[4];
    } thing;

    thing.a = float_variable;
    memcpy(bytes_arr, thing.bytes, 4);
}

float bytesToFloat(const unsigned char* byte_array) 
{
    union FloatBytes 
    {
        float f;
        uint8_t bytes[4];
    } fb;

    for (size_t i = 0; i < 4; i++) 
    {
        fb.bytes[i] = byte_array[i];
    }
    return fb.f;
}
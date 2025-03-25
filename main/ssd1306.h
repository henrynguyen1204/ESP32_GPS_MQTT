#ifndef SSD1306_H
#define SSD1306_H

#include <stdio.h>
#include <string.h>
#include "driver/i2c.h"
#include "font_5x7.h"

#define I2C_MASTER_SCL_IO 7        // GPIO cho SCL
#define I2C_MASTER_SDA_IO 6        // GPIO cho SDA
#define I2C_MASTER_FREQ_HZ 400000   // Tần số I2C 400kHz
#define I2C_MASTER_PORT_NUM I2C_NUM_0 // Sử dụng I2C port 0

#define SSD1306_I2C_ADDRESS 0x3C
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_BUFFER_SIZE (SSD1306_WIDTH * (SSD1306_HEIGHT / 8))

void i2c_master_init();

void ssd1306_init();
void ssd1306_clear_screen();
void ssd1306_draw_char(uint8_t x, uint8_t y, char c);
void ssd1306_draw_string(uint8_t x, uint8_t y, const char *str);
void ssd1306_update_screen();

#endif // SSD1306_H

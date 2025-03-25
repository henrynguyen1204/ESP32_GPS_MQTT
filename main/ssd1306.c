#include "ssd1306.h"

static uint8_t ssd1306_buffer[SSD1306_BUFFER_SIZE];

void i2c_master_init() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_PORT_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_PORT_NUM, conf.mode, 0, 0, 0));
}

void ssd1306_send_command(uint8_t command) {
    uint8_t buffer[2] = {0x00, command};
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SSD1306_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write(cmd, buffer, sizeof(buffer), true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

void ssd1306_init() {
    ssd1306_send_command(0xAE);
    ssd1306_send_command(0xD5);
    ssd1306_send_command(0x80);
    ssd1306_send_command(0xA8);
    ssd1306_send_command(0x3F);
    ssd1306_send_command(0xD3);
    ssd1306_send_command(0x00);
    ssd1306_send_command(0x40);
    ssd1306_send_command(0x8D);
    ssd1306_send_command(0x14);
    ssd1306_send_command(0x20);
    ssd1306_send_command(0x00);
    ssd1306_send_command(0xA1);
    ssd1306_send_command(0xC8);
    ssd1306_send_command(0xDA);
    ssd1306_send_command(0x12);
    ssd1306_send_command(0x81);
    ssd1306_send_command(0xCF);
    ssd1306_send_command(0xD9);
    ssd1306_send_command(0xF1);
    ssd1306_send_command(0xDB);
    ssd1306_send_command(0x40);
    ssd1306_send_command(0xA4);
    ssd1306_send_command(0xA6);
    ssd1306_send_command(0xAF);
}

void ssd1306_clear_screen() {
    memset(ssd1306_buffer, 0x00, sizeof(ssd1306_buffer));
    ssd1306_update_screen();
}

void ssd1306_draw_char(uint8_t x, uint8_t y, char c) {
    if (c < 32 || c > 126) return;
    const uint8_t *char_data = font_5x7[c - 32];

    for (int i = 0; i < 5; i++) {
        ssd1306_buffer[x + (y / 8) * SSD1306_WIDTH + i] = char_data[i];
    }
}

void ssd1306_draw_string(uint8_t x, uint8_t y, const char *str) {
    while (*str) {
        ssd1306_draw_char(x, y, *str);
        x += 6;
        str++;
    }
}

void ssd1306_update_screen() {
    for (uint8_t page = 0; page < 8; page++) {
        ssd1306_send_command(0xB0 + page);
        ssd1306_send_command(0x00);
        ssd1306_send_command(0x10);
        uint8_t buffer[SSD1306_WIDTH + 1];
        buffer[0] = 0x40;
        memcpy(&buffer[1], &ssd1306_buffer[SSD1306_WIDTH * page], SSD1306_WIDTH);
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (SSD1306_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write(cmd, buffer, sizeof(buffer), true);
        i2c_master_stop(cmd);
        i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
    }
}

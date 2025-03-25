#include <stdio.h>
#include "ssd1306.h"
#include "gps.h"
#include "sim_uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Task để hiển thị thông tin GPS lên màn hình OLED
void gps_display_task(void *arg) {
    gps_data_t gps;
    char buffer[32];

    while (1) {
        if (read_gps_data(&gps)) {
            ssd1306_clear_screen();

            ssd1306_draw_string(0, 5, "Time:");
            ssd1306_draw_string(0, 20, "Lat:");
            ssd1306_draw_string(0, 35, "Lon:");
            ssd1306_draw_string(0, 50, "Speed:");

            snprintf(buffer, sizeof(buffer), "%s", gps.time);
            ssd1306_draw_string(40, 5, buffer);

            snprintf(buffer, sizeof(buffer), "%.6f %c", gps.latitude, gps.lat_dir);
            ssd1306_draw_string(40, 20, buffer);

            snprintf(buffer, sizeof(buffer), "%.6f %c", gps.longitude, gps.lon_dir);
            ssd1306_draw_string(40, 35, buffer);

            snprintf(buffer, sizeof(buffer), "%.2f kn", gps.speed);
            ssd1306_draw_string(40, 50, buffer);

            ssd1306_update_screen();
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Đợi 1 giây
    }
}

// Task để gửi thông tin GPS qua MQTT
void gps_mqtt_task(void *arg) {
    gps_data_t gps;
    char message[128]; // Đủ lớn để chứa toàn bộ thông tin GPS

    while (1) {
        if (read_gps_data(&gps)) {
            // Định dạng thông tin GPS thành JSON
            snprintf(message, sizeof(message),
                     "{\"time\": \"%s\", \"lat\": %.6f, \"lat_dir\": \"%c\", "
                     "\"lon\": %.6f, \"lon_dir\": \"%c\", \"speed\": %.2f}",
                     gps.time, gps.latitude, gps.lat_dir,
                     gps.longitude, gps.lon_dir, gps.speed);

            // Gửi message qua MQTT
            publish_message(PUB_TOPIC, message);
        }
    }
}

void app_main() {
    i2c_master_init();
    ssd1306_init();
    ssd1306_clear_screen();

    init_uart_gps();

    uart_init();
    sim_init();

    connect_mqtt(CLIENT_ID, BROKER_ADDRESS);
    subscribe_topic(SUB_TOPIC);
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    
    xTaskCreate(gps_display_task, "gps_display_task", 4096, NULL, 10, NULL);
    xTaskCreate(gps_mqtt_task, "gps_mqtt_task", 4096, NULL, 10, NULL);
}
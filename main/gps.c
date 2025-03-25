#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "driver/uart.h"
#include "esp_log.h"
#include "gps.h"

static double convert_nmea_to_decimal(const char *nmea, char direction) {
    double deg = atof(nmea) / 100.0;
    int int_deg = (int)deg;
    double decimal_deg = int_deg + (deg - int_deg) * 100 / 60;
    if (direction == 'W' || direction == 'S') decimal_deg *= -1;
    return decimal_deg;
}

static void parse_gpgga(const char *nmea, gps_data_t *gps) {
    char buffer[128];
    strcpy(buffer, nmea);
    char *token = strtok(buffer, ",");

    for (int field = 0; token != NULL; field++, token = strtok(NULL, ",")) {
        switch (field) {
            case 1: strcpy(gps->time, token); break;
            case 2: gps->latitude = convert_nmea_to_decimal(token, gps->lat_dir); break;
            case 3: gps->lat_dir = token[0]; break;
            case 4: gps->longitude = convert_nmea_to_decimal(token, gps->lon_dir); break;
            case 5: gps->lon_dir = token[0]; break;
            case 7: gps->satellites = atoi(token); break;
            case 9: gps->altitude = atof(token); break;
        }
    }
}

static void parse_gprmc(const char *nmea, gps_data_t *gps) {
    char buffer[128];
    strcpy(buffer, nmea);
    char *token = strtok(buffer, ",");

    for (int field = 0; token != NULL; field++, token = strtok(NULL, ",")) {
        switch (field) {
            case 1:
                if (strlen(token) >= 6 ) {
                    int hour, minute, second;
                    sscanf(token, "%2d%2d%2d", &hour, &minute, &second);
                    snprintf(gps->time, sizeof(gps->time), "%02d:%02d:%02d", hour, minute, second);
                }
                break;
            case 3: gps->latitude = convert_nmea_to_decimal(token, gps->lat_dir); break;
            case 4: gps->lat_dir = token[0]; break;
            case 5: gps->longitude = convert_nmea_to_decimal(token, gps->lon_dir); break;
            case 6: gps->lon_dir = token[0]; break;
            case 7: gps->speed = atof(token); break;
            case 8:
                if (strlen(token) == 6) {
                    int day, month, year;
                    sscanf(token, "%2d%2d%2d", &day, &month, &year);
                    snprintf(gps->date, sizeof(gps->date), "%02d/%02d/20%02d", day, month, year);
                }
                break;
        }
    }
}

static void parse_nmea_sentence(const char *nmea, gps_data_t *gps) {
    if (strncmp(nmea, "$GPGGA", 6) == 0) {
        parse_gpgga(nmea, gps);
    } else if (strncmp(nmea, "$GPRMC", 6) == 0) {
        parse_gprmc(nmea, gps);
    }
}

void init_uart_gps() {
    uart_config_t uart_config = {
        .baud_rate = 9600, 
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };
    
    uart_driver_install(UART_NUM, BUF_SIZE, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

}

bool read_gps_data(gps_data_t *gps) {
    uint8_t data[BUF_SIZE];
    int len = uart_read_bytes(UART_NUM, data, sizeof(data) - 1, 100 / portTICK_PERIOD_MS);
    
    if (len > 0) {
        data[len] = '\0';
        char *line = strtok((char *)data, "\r\n");
        while (line) {
            if (strncmp(line, "$GPGGA", 6) == 0 || strncmp(line, "$GPRMC", 6) == 0) {
                parse_nmea_sentence(line, gps);
                return true;
            }
            line = strtok(NULL, "\r\n");
        }
    }
    return false;
}

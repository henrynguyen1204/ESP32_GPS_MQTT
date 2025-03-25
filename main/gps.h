#ifndef GPS_H
#define GPS_H

#include "driver/uart.h"

#define UART_NUM        UART_NUM_1  
#define TX_PIN          UART_PIN_NO_CHANGE  
#define RX_PIN          5  
#define BUF_SIZE        1024

typedef struct {
    char time[10];      
    char date[11];      
    double latitude;    
    char lat_dir;       
    double longitude;   
    char lon_dir;       
    double speed;       
    double altitude;    
    int satellites;     
} gps_data_t;

void init_uart_gps();
bool read_gps_data(gps_data_t *gps);

#endif

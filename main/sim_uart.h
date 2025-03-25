#ifndef SIM_UART_H
#define SIM_UART_H

#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define UART_PORT UART_NUM_2
#define BUF_SIZE 1024
#define TX_2_PIN 15
#define RX_2_PIN 16

#define CLIENT_ID "843d6e7290a34c5b"
#define BROKER_ADDRESS "tcp://broker.emqx.io:1883"
#define SUB_TOPIC "project1/test"
#define PUB_TOPIC "project1/test1"

void uart_init();
void send_at_command(const char *command);
void receive_response();
void sim_init();
void connect_mqtt(const char *client_id, const char *broker_address);
void subscribe_topic(const char *sub_topic);
void publish_message();

#endif // SIM_UART_H

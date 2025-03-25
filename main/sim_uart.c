#include "sim_uart.h"

static const char *TAG = "SIM_UART";

void uart_init() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    ESP_ERROR_CHECK(uart_param_config(UART_PORT, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT, TX_2_PIN, RX_2_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_driver_install(UART_PORT, BUF_SIZE * 2, 0, 0, NULL, 0));
}

void send_at_command(const char *command) {
    ESP_LOGI(TAG, "Sending: %s", command);
    uart_write_bytes(UART_PORT, command, strlen(command));
    uart_write_bytes(UART_PORT, "\r\n", 2);
}

void receive_response() {
    uint8_t data[BUF_SIZE];
    int length = uart_read_bytes(UART_PORT, data, BUF_SIZE - 1, 1000 / portTICK_PERIOD_MS);
    if (length > 0) {
        data[length] = '\0';
        ESP_LOGI(TAG, "Received: %s", data);
    } else {
        ESP_LOGE(TAG, "No response received");
    }
}

void sim_init() {
    send_at_command("AT+CGEREP=0,0");
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    send_at_command("ATE0");
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    send_at_command("AT+CPIN?");
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    send_at_command("AT+CPSI?");
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void connect_mqtt(const char *client_id, const char *broker_address) {
    char command[128];

    send_at_command(" AT+CMQTTDISC=0,120");
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    send_at_command("AT+CMQTTSTOP");
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    send_at_command("AT+CMQTTSTART");
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    snprintf(command, sizeof(command), "AT+CMQTTACCQ=0,\"%s\",0", client_id);
    send_at_command(command);
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    snprintf(command, sizeof(command), "AT+CMQTTCONNECT=0,\"%s\",60,1", broker_address);
    send_at_command(command);
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void subscribe_topic(const char *sub_topic) {
    char command[128];

    snprintf(command, sizeof(command), "AT+CMQTTSUBTOPIC=0,%d,1", strlen(sub_topic));
    send_at_command(command);
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    send_at_command(sub_topic);
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    send_at_command("AT+CMQTTSUB=0");
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

void publish_message(const char *pub_topic, const char *message)
{
    char command[128];
    snprintf(command, sizeof(command),"AT+CMQTTTOPIC=0,%d", strlen(pub_topic));
    send_at_command(command);
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    send_at_command(pub_topic);
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    sniprintf(command, sizeof(command),"AT+CMQTTPAYLOAD=0,%d", strlen(message));
    send_at_command(command);
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    send_at_command(message);
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    send_at_command("AT+CMQTTPUB=0,1,60,0");
    receive_response();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "lm75.h"
#include "mcp3008.h"
#include "ads1015.h"
#include "utils.h"
#include "definitions.h"

#include "topics.h"
#include "mqtt.h"
#include "process_mqtt.h"



#define I2C_MASTER_NUM      I2C_NUM_0
#define SPI_CONTROLLER      SPI2_HOST
#define I2C_MASTER_FREQ_HZ  100000
#define SLEEP_SEGUNDO       1000                

#ifndef PIO_UNIT_TESTING

SemaphoreHandle_t spi_mutex = NULL;
SemaphoreHandle_t i2c_mutex = NULL;
void init_hardware()
{
    if(i2c_init(I2C_MASTER_NUM, I2C_MASTER_FREQ_HZ) != ESP_OK) 
    {
        ESP_LOGE("I2C:", "No se pudo inicializar el controlador I2C");
    }

    if(spi_bus_init(SPI_CONTROLLER) != ESP_OK)
    {
        ESP_LOGE("SPI:", "No se pudo inicializar el controlador SPI");
    }
}

void vTaskTemperaturas(void *pvParameters)
{
    lm75_t sensor_log, sensor_pot;
    lm75_Init(&sensor_log, LM75_LOGIC_ADDR);
    lm75_Init(&sensor_pot, LM75_POWER_ADDR);

    float temp1, temp2;
    sensor_cmd_t comando_recibido;
    mqtt_msg_t respuesta_mqtt;

    while(1)
    {
        if (xQueueReceive(temp_cmd_queue, &comando_recibido, portMAX_DELAY))
        {
            if (comando_recibido.cmd == SENSOR_CMD_GET_TEMP_ALL)
            {
                lm75_read_celsius_temp(&sensor_log, &temp1);
                lm75_read_celsius_temp(&sensor_pot, &temp2);
                printf("[TEMP_LOG] %.2f°C\n", temp1);
                printf("[TEMP_POT] %.2f°C\n", temp2);

                memset(&respuesta_mqtt, 0, sizeof(mqtt_msg_t));
                strcpy(respuesta_mqtt.topic, MQTT_TOPIC_DATA_TEMP);
                snprintf(respuesta_mqtt.data, sizeof(respuesta_mqtt.data), "%.2f", temp1);
                xQueueSend(mqtt_tx_queue, &respuesta_mqtt, 0);

                memset(&respuesta_mqtt, 0, sizeof(mqtt_msg_t));
                strcpy(respuesta_mqtt.topic, MQTT_TOPIC_DATA_TEMP);
                snprintf(respuesta_mqtt.data, sizeof(respuesta_mqtt.data), "%.2f", temp2);
                xQueueSend(mqtt_tx_queue, &respuesta_mqtt, 0);
            }
            else if (comando_recibido.cmd == SENSOR_CMD_GET_TEMP_LOGIC)
            {
                lm75_read_celsius_temp(&sensor_log, &temp1);
                printf("[TEMP_LOG] %.2f°C\n", temp1);

                memset(&respuesta_mqtt, 0, sizeof(mqtt_msg_t));
                strcpy(respuesta_mqtt.topic, MQTT_TOPIC_DATA_TEMP);
                snprintf(respuesta_mqtt.data, sizeof(respuesta_mqtt.data), "%.2f", temp1);
                xQueueSend(mqtt_tx_queue, &respuesta_mqtt, 0);
            }
            else if (comando_recibido.cmd == SENSOR_CMD_GET_TEMP_POWER)
            {
                lm75_read_celsius_temp(&sensor_pot, &temp2);
                printf("[TEMP_POT] %.2f°C\n", temp2);

                memset(&respuesta_mqtt, 0, sizeof(mqtt_msg_t));
                strcpy(respuesta_mqtt.topic, MQTT_TOPIC_DATA_TEMP);
                snprintf(respuesta_mqtt.data, sizeof(respuesta_mqtt.data), "%.2f", temp2);
                xQueueSend(mqtt_tx_queue, &respuesta_mqtt, 0);
            }
        }
    }
}

void vTaskIntensidades(void *pvParameters)
{
    mcp3008_t mcp3008;
    mcp3008_init(&mcp3008);

    float amp;
    sensor_cmd_t comando_recibido;
    mqtt_msg_t respuesta_mqtt;

    uint8_t canales[] = {MCP_SGL_CH0, MCP_SGL_CH1, MCP_SGL_CH2, MCP_SGL_CH3, MCP_SGL_CH4, MCP_SGL_CH5, MCP_SGL_CH6, MCP_SGL_CH7};

    while (1)
    {
        if (xQueueReceive(current_cmd_queue, &comando_recibido, portMAX_DELAY))
        {
            if (comando_recibido.cmd == SENSOR_CMD_GET_CURRENT_ALL)
            {
                char payload_completo[MQTT_DATA_MAX] = {0};
                char token_canal[32] = {0};

                for(int i = 0; i<8; i++)
                {
                    mcp3008_amperios(&mcp3008, canales[i], &amp);
                    printf("[Canal %d]: %.2f A\n", i, (double)amp);

                    snprintf(token_canal, sizeof(token_canal), "CH%d:%.2fA ", i, (double)amp);
                    if (strlen(payload_completo) + strlen(token_canal) < MQTT_DATA_MAX)
                    {
                        strcat(payload_completo, token_canal);
                    }
                }

                memset(&respuesta_mqtt, 0, sizeof(mqtt_msg_t));
                strcpy(respuesta_mqtt.topic, MQTT_TOPIC_DATA_CURRENT);
                strcpy(respuesta_mqtt.data, payload_completo);
                xQueueSend(mqtt_tx_queue, &respuesta_mqtt, 0);
            }
        }
    }
}

void vTaskVoltages (void *pvParameters)
{
    ads1015_t ads1015;
    ads1015_Init(&ads1015, ADS1015_ADDR);

    float v1;   // Voltage of channel 0
    float v2;   // Voltage of channel 1
    float v3;   // Voltage of channel 2

    sensor_cmd_t comando_recibido;
    mqtt_msg_t respuesta_mqtt;

    while (1)
    {
        if(xQueueReceive(voltage_cmd_queue, &comando_recibido, portMAX_DELAY))
        {
            if (comando_recibido.cmd == SENSOR_CMD_GET_VOLT_ALL)
            {

            }
            else if(comando_recibido.cmd == SENSOR_CMD_GET_VOLT_CH0)
            {
                ads1015_SetChannel(&ads1015, ADS1015_CANAL_0);
                ads1015_convertRead(&ads1015, &v1);

                printf("[VOLT_LOG] Channel0: %.2f°C\n", v1);

                memset(&respuesta_mqtt, 0, sizeof(mqtt_msg_t));
                strcpy(respuesta_mqtt.topic, MQTT_TOPIC_DATA_VOLTAGE);
                snprintf(respuesta_mqtt.data, sizeof(respuesta_mqtt.data), "%.2f", v1);
                xQueueSend(mqtt_tx_queue, &respuesta_mqtt, 0);
            }
            else if(comando_recibido.cmd == SENSOR_CMD_GET_VOLT_CH1)
            {
                ads1015_SetChannel(&ads1015, ADS1015_CANAL_1);
                ads1015_convertRead(&ads1015, &v2);

                printf("[VOLT_LOG] Channel1: %.2f°C\n", v2);

                memset(&respuesta_mqtt, 0, sizeof(mqtt_msg_t));
                strcpy(respuesta_mqtt.topic, MQTT_TOPIC_DATA_VOLTAGE);
                snprintf(respuesta_mqtt.data, sizeof(respuesta_mqtt.data), "%.2f", v2);
                xQueueSend(mqtt_tx_queue, &respuesta_mqtt, 0);
            }
            else if(comando_recibido.cmd == SENSOR_CMD_GET_VOLT_CH2)
            {
                ads1015_SetChannel(&ads1015, ADS1015_CANAL_2);
                ads1015_convertRead(&ads1015, &v3);

                printf("[VOLT_LOG] Channel2: %.2f°C\n", v3);

                memset(&respuesta_mqtt, 0, sizeof(mqtt_msg_t));
                strcpy(respuesta_mqtt.topic, MQTT_TOPIC_DATA_VOLTAGE);
                snprintf(respuesta_mqtt.data, sizeof(respuesta_mqtt.data), "%.2f", v2);
                xQueueSend(mqtt_tx_queue, &respuesta_mqtt, 0);
            }
        }
    }
    
}

/* TODO CAMBIARLO para el LIS3DH

void vTaskInclinacion(void *pvParameters)
{
    as5047d_t as5047d;
    as5047d_init(&as5047d);

    uint16_t ang;
    sensor_cmd_t comando_recibido;
    mqtt_msg_t respuesta_mqtt;

    while (1)
    {
        if (xQueueReceive(angle_cmd_queue, &comando_recibido, portMAX_DELAY))
        {
            if (comando_recibido.cmd == SENSOR_CMD_GET_ANGLE)
            {
                as5047d_read_bits(&as5047d, AS5047D_ANGLEUNC, 0, &ang);
                printf("[AS5047D] %u\n", ang);

                memset(&respuesta_mqtt, 0, sizeof(mqtt_msg_t));
                strcpy(respuesta_mqtt.topic, MQTT_TOPIC_DATA_ANGLE);
                snprintf(respuesta_mqtt.data, sizeof(respuesta_mqtt.data), "%u", ang);
                xQueueSend(mqtt_tx_queue, &respuesta_mqtt, 0);
            }
        }
    }
}
*/

void vTaskMotores(void *pvParameters)
{
    gpio_set_direction(EJE1_DIR1, GPIO_MODE_OUTPUT);
    gpio_set_level(EJE1_DIR1, 0);

    gpio_set_direction(EJE1_DIR2, GPIO_MODE_OUTPUT);
    gpio_set_level(EJE1_DIR2, 0);

    gpio_set_direction(EJE2_DIR3, GPIO_MODE_OUTPUT);
    gpio_set_level(EJE2_DIR3, 0);

    gpio_set_direction(EJE2_DIR4, GPIO_MODE_OUTPUT);
    gpio_set_level(EJE2_DIR4, 0);

    gpio_set_direction(EJE3_DIR5, GPIO_MODE_OUTPUT);
    gpio_set_level(EJE3_DIR5, 0);

    gpio_set_direction(EJE3_DIR6, GPIO_MODE_OUTPUT);
    gpio_set_level(EJE3_DIR6, 0);

    
    gpio_set_direction(EJE1_PWM1, GPIO_MODE_OUTPUT);
    gpio_set_level(EJE1_PWM1, 0);

    gpio_set_direction(EJE1_PWM2, GPIO_MODE_OUTPUT);
    gpio_set_level(EJE1_PWM2, 0);

    gpio_set_direction(EJE2_PWM3, GPIO_MODE_OUTPUT);
    gpio_set_level(EJE2_PWM3, 0);

    gpio_set_direction(EJE2_PWM4, GPIO_MODE_OUTPUT);
    gpio_set_level(EJE2_PWM4, 0);

    gpio_set_direction(EJE3_PWM5, GPIO_MODE_OUTPUT);
    gpio_set_level(EJE3_PWM5, 0);

    gpio_set_direction(EJE3_PWM6, GPIO_MODE_OUTPUT);
    gpio_set_level(EJE3_PWM6, 0);

    motor_cmd_t comando_recibido;

    while (1)
    {
        if (xQueueReceive(motor_cmd_queue, &comando_recibido, portMAX_DELAY))
        {
            // Aquí se procesarán los comandos del motor cuando se asigne el hardware pertinente
        }
    }
}

void recibir_MQTT(void *pvParameters)
{
    mqtt_msg_t msg;

    while (1)
    {
        if (xQueueReceive(mqtt_rx_queue, &msg, portMAX_DELAY))
        {
            printf("CMD recibido:\n");
            printf("TOPIC: %s\n", msg.topic);
            printf("DATA: %s\n", msg.data);

            mqtt_process_message(&msg);
        }
    }
}

void mandar_MQTT(void *pvParameters)
{
    mqtt_msg_t msg;

    while (1)
    {
        if (xQueueReceive(mqtt_tx_queue, &msg, portMAX_DELAY))
        {
            ESP_LOGI("MQTT_TX", "Enviando -> Topic: %s | Data: %s", msg.topic, msg.data);

            mqtt_publish(msg.topic, msg.data);
        }
    }
}


void init_mqtt_tasks()
{
    system_init();

    mqtt_router_init();

    // Recibir mensajes MQTT en núcleo 0
    xTaskCreatePinnedToCore(recibir_MQTT,"RecibirMQTT", 4096, NULL, 2, NULL, 0);

    // Mandar mensajes MQTT en núcleo 0
    xTaskCreatePinnedToCore(mandar_MQTT, "MandarMQTT", 4096, NULL, 2, NULL, 0);
}

void init_sensor_tasks()
{
    xTaskCreatePinnedToCore(vTaskTemperaturas, "Temperaturas", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(vTaskIntensidades, "Intensidades", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(vTaskVoltages, "Voltages", 4096, NULL, 2, NULL, 1);
    //xTaskCreatePinnedToCore(vTaskInclinacion, "Inclinacion", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(vTaskMotores, "Motores", 4096, NULL, 3, NULL, 1);
}

void app_main(void) 
{
    init_hardware();

    // Inicializar tareas

    init_mqtt_tasks();     // Núcleo 0

    init_sensor_tasks();   // Núcleo 1
}

#endif
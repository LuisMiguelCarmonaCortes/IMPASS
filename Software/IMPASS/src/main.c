#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "lm75.h"
#include "mcp3008.h"
#include "as5047d.h"
#include "mqtt.h"
#include "definitions.h"
#include "utils.h"


#define I2C_MASTER_NUM      I2C_NUM_0
#define SPI_CONTROLLER      SPI2_HOST
#define I2C_MASTER_FREQ_HZ  100000
#define SLEEP_SEGUNDO       1000                

#ifndef PIO_UNIT_TESTING

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

    /*TODO: Un topic MQTT hará que se lea una temperatura o las dos*/
    while(1)
    {
        lm75_read_celsius_temp(&sensor_log, &temp1);
        lm75_read_celsius_temp(&sensor_pot, &temp2);
        printf("[TEMP_LOG] %.2f°C\n", temp1);
        printf("[TEMP_POT] %.2f°C\n", temp2);

        vTaskDelay(pdMS_TO_TICKS(2 * SLEEP_SEGUNDO));
    }
}

void vTaskIntensidades(void *pvParameters)
{
    mcp3008_t mcp3008;
    mcp3008_init(&mcp3008);

    float amp;

    /*TODO: un topic de MQTT hara que se lea el canal o canales*/
    uint8_t canales[] = {MCP_SGL_CH0, MCP_SGL_CH1, MCP_SGL_CH2, MCP_SGL_CH3, MCP_SGL_CH4, MCP_SGL_CH5, MCP_SGL_CH6, MCP_SGL_CH7};

    while (1)
    {
        for(int i = 0; i<8; i++)
        {
            mcp3008_amperios(&mcp3008, canales[i], &amp);
            printf("[Canal %d]: %.2f A\n", i);
        }
        
        vTaskDelay(pdMS_TO_TICKS(2 * SLEEP_SEGUNDO));
    }
}

void vTaskInclinacion(void *pvParameters)
{
    as5047d_t as5047d;
    as5047d_init(&as5047d);

    uint16_t ang;

    while (1)
    {
        as5047d_read_bits(&as5047d, AS5047D_ANGLEUNC, NULL, &ang);
        printf("[AS5047D] %u\n", ang);
        vTaskDelay(pdMS_TO_TICKS(3 * SLEEP_SEGUNDO));
    }
}

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

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(4 * SLEEP_SEGUNDO));
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

            // Aquí decides qué hacer
            // Ejemplo:
            if (strcmp(msg.topic, "impass/motor") == 0)
            {
                // actuar sobre motores
            }
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
    // Recibir mensajes MQTT en núcleo 0
    xTaskCreatePinnedToCore(recibir_MQTT,"RecibirMQTT", 4096, NULL, 2, NULL, 0);

    // Mandar mensajes MQTT en núcleo 0
    xTaskCreatePinnedToCore(mandar_MQTT, "MandarMQTT", 4096, NULL, 2, NULL, 0);
}

void init_sensor_tasks()
{
    xTaskCreatePinnedToCore(vTaskTemperaturas, "Temperaturas", 2048, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(vTaskIntensidades, "Intensidades", 2048, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(vTaskInclinacion, "Inclinacion", 2048, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(vTaskMotores, "Motores", 2048, NULL, 3, NULL, 1);
}

void app_main(void) 
{
    init_hardware();

    // Inicializar tareas
    init_sensor_tasks();   // Núcleo 1
    init_mqtt_tasks();     // Núcleo 0
}

#endif
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "lm75.h"
#include "mcp3008.h"
#include "definitions.h"
#include "utils.h"
#include "driver/gpio.h"

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

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2 * SLEEP_SEGUNDO));
    }
}

void vTaskInclinacion(void *pvParameters)
{
    

    while (1)
    {
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
    
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(SLEEP_SEGUNDO));
    }
}

void mandar_MQTT(void *pvParameters)
{
    
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2 * SLEEP_SEGUNDO));
    }
}


void init_mqtt_tasks()
{
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
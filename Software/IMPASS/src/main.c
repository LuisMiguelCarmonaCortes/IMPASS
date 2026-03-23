#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "lm75.h"
#include "mcp3008.h"
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
    lm75_t sensor;
    lm75_Init(&sensor, LM75_LOGIC_ADDR);

    float temp;

    while(1)
    {
        lm75_read_celsius_temp(&sensor, &temp);
        printf("[TEMP] %.2f°C\n", temp);

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
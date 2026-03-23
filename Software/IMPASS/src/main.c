#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "lm75.h"
#include "driver/i2c.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "definitions.h"
#include "utils.h"

#define I2C_MASTER_NUM       I2C_NUM_0      // Usamos I2C0
#define I2C_MASTER_FREQ_HZ   100000         // 100 kHz
#define WIFI_SSID            "GPON3817_2.4G."
#define WIFI_PASS            "SeyySfdJ"

#ifndef PIO_UNIT_TESTING

SemaphoreHandle_t i2c_mutex;
static EventGroupHandle_t s_wifi_event_group;
#define WIFI_CONNECTED_BIT BIT0
static const char *TAG = "wifi_event";

// Handler de eventos WiFi
static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "Desconectado, reconectando...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Conectado con IP: %s", ip4addr_ntoa(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// Inicializa WiFi en modo STA
void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT,
                                        ESP_EVENT_ANY_ID,
                                        &wifi_event_handler,
                                        NULL,
                                        &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT,
                                        IP_EVENT_STA_GOT_IP,
                                        &wifi_event_handler,
                                        NULL,
                                        &instance_got_ip);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "Inicializando WiFi STA...");
}

void app_main(void) 
{
    // Inicializa NVS (necesario para WiFi)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Inicializa WiFi y espera conexión
    wifi_init_sta();
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT,
                        false, true, portMAX_DELAY);

    i2c_mutex = xSemaphoreCreateMutex();
    if (i2c_mutex == NULL) {
        ESP_LOGE("I2C:", "No se pudo crear el mutex de I2C");
        return;
    }

    if(i2c_init(I2C_MASTER_NUM, I2C_MASTER_FREQ_HZ) != ESP_OK) {
        ESP_LOGE("I2C:", "No se pudo inicializar el controlador I2C");
    }

    lm75_t sensor;
    lm75_Init(&sensor, LM75_LOGIC_ADDR);
    
    float temp;
    uint16_t raw;
    while(1) {
        lm75_read_celsius_temp(&sensor, &temp);
        printf("Temperatura: %.2f°C\n", temp);
        lm75_read_raw_temp(&sensor, &raw);
        printf("Temperatura_ raw: %u\n", raw);

        // Opcional: muestra RSSI
        wifi_ap_record_t ap_info;
        if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
            printf("Señal WiFi (RSSI): %d dBm\n", ap_info.rssi);
        }

        vTaskDelay(pdMS_TO_TICKS(2000)); // espera 2 segundos
    }
}

#endif
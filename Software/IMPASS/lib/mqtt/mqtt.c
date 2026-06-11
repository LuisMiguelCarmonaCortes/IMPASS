#include "mqtt.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "MQTT_UTILS";

QueueHandle_t mqtt_rx_queue = NULL;
QueueHandle_t mqtt_tx_queue = NULL;

esp_mqtt_client_handle_t mqtt_client = NULL;

void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if(event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        ESP_LOGW(TAG, "Wifi desconectado, reconectando ...");
        esp_wifi_connect();
    }
    else if(event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ESP_LOGI(TAG, "Wifi conectado y con IP");
    }
}

void wifi_init(void)
{
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();

    ESP_LOGI(TAG, "Inicializando Wifi ...");
}


void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t) event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT conectado ");
        esp_mqtt_client_subscribe(mqtt_client, "impass/cmd/#", 0);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "MQTT desconectado");
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "Mensaje recibido");

        mqtt_msg_t msg = {0};

        int topic_len = event->topic_len < MQTT_TOPIC_MAX ? event->topic_len : MQTT_TOPIC_MAX - 1;
        int data_len  = event->data_len  < MQTT_DATA_MAX  ? event->data_len  : MQTT_DATA_MAX - 1;

        memcpy(msg.topic, event->topic, topic_len);
        msg.topic[topic_len] = '\0';

        memcpy(msg.data, event->data, data_len);
        msg.data[data_len] = '\0';

        xQueueSend(mqtt_rx_queue, &msg, 0);
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "Error en MQTT");
        break;

    default:
        break;
    }
}

void mqtt_init()
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_URI,
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

esp_mqtt_client_start(mqtt_client);

ESP_LOGI(TAG, "MQTT Iniciado");
}

void mqtt_publish(const char *topic, const char *msg)
{
    if(mqtt_client != NULL)
    {
        esp_mqtt_client_publish(mqtt_client, topic, msg, 0, 1, 0);
    }
}

void system_init(void)
{
    esp_err_t ret = nvs_flash_init();

    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        nvs_flash_init();
    }

    // Se crean las colas
    mqtt_rx_queue = xQueueCreate(10, sizeof(mqtt_msg_t));
    mqtt_tx_queue = xQueueCreate(10, sizeof(mqtt_msg_t));

    wifi_init();
    vTaskDelay(pdMS_TO_TICKS(5000));

    mqtt_init();
}
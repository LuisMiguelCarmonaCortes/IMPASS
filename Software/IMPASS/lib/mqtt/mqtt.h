/****************************************************************************** 
 * @file    mqtt.h
 * @brief   Librería para gestión de comunicación MQTT con colas RX/TX
 *          en el sistema IMPASS (ESP32-S3)
 *
 * @project IMPASS - Trabajo Fin de Máster
 * @company Universidad de Malaga, Máster en Sistemas Electrónicos para Entornos Inteligentes
 * @author  Luis Miguel Carmona Cortes
 * @date    2026
 * @version 1.0
 *
 * @details
 * Este archivo define la interfaz para la comunicación MQTT en el sistema.
 * Implementa un modelo desacoplado mediante colas de FreeRTOS:
 *
 *   - Cola RX: mensajes recibidos desde el broker MQTT
 *   - Cola TX: mensajes a enviar al broker MQTT
 *
 * Esto permite separar la lógica de comunicación de las tareas de aplicación,
 * mejorando la escalabilidad, mantenibilidad y robustez del sistema.
 *
 * Incluye:
 *   - Inicialización de WiFi y cliente MQTT
 *   - Publicación de mensajes
 *   - Recepción de mensajes mediante eventos
 *   - Estructura de mensajes MQTT
 *   - Definición de colas FreeRTOS
 *
 * @note
 * Antes de usar MQTT, debe ejecutarse system_init()
 * para inicializar NVS, WiFi y el cliente MQTT.
 ******************************************************************************/

#ifndef MQTT_H
#define MQTT_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "esp_event.h"
#include "mqtt_client.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define MQTT_TOPIC_MAX 64
#define MQTT_DATA_MAX  128

typedef struct {
    char topic[MQTT_TOPIC_MAX];
    char data[MQTT_DATA_MAX];
} mqtt_msg_t;

extern QueueHandle_t mqtt_rx_queue;
extern QueueHandle_t mqtt_tx_queue;

extern esp_mqtt_client_handle_t mqtt_client;

/* ===== Configuarcion WiFi =====*/
#define WIFI_SSID "GPON3817_2.4G."
#define WIFI_PASS "SeyySfdJ"

/* ===== Configuarcion MQTT =====*/
#define MQTT_BROKER_IP      "test.mosquitto.org"
#define MQTT_BROKER_PORT    1883

#define MQTT_URI            "mqtt://" MQTT_BROKER_IP ":1883"

// Topics


// WiFi
void wifi_init();
void wifi_event_handler(void *arg, esp_event_base_t even_base, int32_t event_id, void *event_data);

// MQTT
void mqtt_init();
void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);

void mqtt_publish(const char *topic, const char *msg);

void system_init(void);

#endif 
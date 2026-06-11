/******************************************************************************
 * @file    process_mqtt.h
 * @brief   Router de comandos MQTT para el sistema IMPASS
 *
 * @project IMPASS - Trabajo Fin de Máster
 * @company Universidad de Malaga, Máster en Sistemas Electrónicos para Entornos Inteligentes
 * @author  Luis Miguel Carmona Cortes
 * @date    2026
 * @version 1.0
 *
 * @details
 * Este módulo desacopla la recepción MQTT de la lógica de aplicación.
 *
 * Los mensajes recibidos desde el broker son procesados por el router,
 * que analiza el topic y genera comandos internos para las distintas
 * tareas del sistema mediante colas FreeRTOS.
 *
 * Actualmente se gestionan:
 *
 *   - Lectura de temperaturas
 *   - Lectura de intensidades
 *   - Lectura de inclinación
 *   - Control de motores
 *   - Comandos de sistema
 *
 * El objetivo es mantener la capa MQTT independiente de la lógica
 * específica de sensores y actuadores.
 ******************************************************************************/

#ifndef MQTT_ROUTER_H
#define MQTT_ROUTER_H

#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "mqtt.h"

/* ===== Comandos de sensores ===== */

typedef enum
{
    SENSOR_CMD_NONE = 0,

    SENSOR_CMD_GET_TEMP_ALL,
    SENSOR_CMD_GET_TEMP_LOGIC,
    SENSOR_CMD_GET_TEMP_POWER,

    SENSOR_CMD_GET_CURRENT_ALL,

    SENSOR_CMD_GET_ANGLE

} sensor_cmd_type_t;

typedef struct
{
    sensor_cmd_type_t cmd;

} sensor_cmd_t;

/* ===== Comandos de motores ===== */

typedef struct
{
    uint8_t axis;
    uint8_t dir;
    uint8_t pwm;

} motor_cmd_t;

/* ===== Comandos de sistema ===== */

typedef enum
{
    SYSTEM_CMD_NONE = 0,

    SYSTEM_CMD_REBOOT,
    SYSTEM_CMD_WIFI_RESTART,
    SYSTEM_CMD_STATUS

} system_cmd_type_t;

typedef struct
{
    system_cmd_type_t cmd;

} system_cmd_t;

/* ===== Colas de comandos ===== */

extern QueueHandle_t temp_cmd_queue;
extern QueueHandle_t current_cmd_queue;
extern QueueHandle_t angle_cmd_queue;

extern QueueHandle_t motor_cmd_queue;
extern QueueHandle_t system_cmd_queue;

void mqtt_router_init(void);
void mqtt_process_message(mqtt_msg_t *msg);

#endif
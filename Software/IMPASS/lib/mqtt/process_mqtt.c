#include "process_mqtt.h"

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "topics.h"

static const char *TAG = "MQTT_ROUTER";

/* ===== Colas de comandos ===== */

QueueHandle_t temp_cmd_queue    = NULL;
QueueHandle_t current_cmd_queue = NULL;
QueueHandle_t angle_cmd_queue   = NULL;

QueueHandle_t motor_cmd_queue  = NULL;
QueueHandle_t system_cmd_queue = NULL;

/* ===== Funciones privadas ===== */

static void handle_temp(const char *payload);
static void handle_current(const char *payload);
static void handle_angle(const char *payload);
static void handle_motor(const char *payload);
static void handle_system(const char *payload);

void mqtt_router_init(void)
{
    temp_cmd_queue    = xQueueCreate(10, sizeof(sensor_cmd_t));
    current_cmd_queue = xQueueCreate(10, sizeof(sensor_cmd_t));
    angle_cmd_queue   = xQueueCreate(10, sizeof(sensor_cmd_t));

    motor_cmd_queue  = xQueueCreate(10, sizeof(motor_cmd_t));
    system_cmd_queue = xQueueCreate(10, sizeof(system_cmd_t));

    if(temp_cmd_queue == NULL ||
       current_cmd_queue == NULL ||
       angle_cmd_queue == NULL ||
       motor_cmd_queue  == NULL ||
       system_cmd_queue == NULL)
    {
        ESP_LOGE(TAG, "Error creando colas del router MQTT");
    }
}

void mqtt_process_message(mqtt_msg_t *msg)
{
    if(strcmp(msg->topic, MQTT_TOPIC_CMD_TEMP) == 0)
    {
        handle_temp(msg->data);
    }
    else if(strcmp(msg->topic, MQTT_TOPIC_CMD_CURRENT) == 0)
    {
        handle_current(msg->data);
    }
    else if(strcmp(msg->topic, MQTT_TOPIC_CMD_ANGLE) == 0)
    {
        handle_angle(msg->data);
    }
    else if(strcmp(msg->topic, MQTT_TOPIC_CMD_MOTOR) == 0)
    {
        handle_motor(msg->data);
    }
    else if(strcmp(msg->topic, MQTT_TOPIC_CMD_SYSTEM) == 0)
    {
        handle_system(msg->data);
    }
    else
    {
        ESP_LOGW(TAG, "Topic no reconocido: %s", msg->topic);
    }
}

static void handle_temp(const char *payload)
{
    sensor_cmd_t cmd;

    if(strcmp(payload, "ALL") == 0)
    {
        cmd.cmd = SENSOR_CMD_GET_TEMP_ALL;
    }
    else if(strcmp(payload, "LOGIC") == 0)
    {
        cmd.cmd = SENSOR_CMD_GET_TEMP_LOGIC;
    }
    else if(strcmp(payload, "POWER") == 0)
    {
        cmd.cmd = SENSOR_CMD_GET_TEMP_POWER;
    }
    else
    {
        ESP_LOGW(TAG, "Comando temperatura desconocido");
        return;
    }

    xQueueSend(temp_cmd_queue, &cmd, 0);
}


static void handle_current(const char *payload)
{
    sensor_cmd_t cmd;

    if(strcmp(payload, "ALL") == 0)
    {
        cmd.cmd = SENSOR_CMD_GET_CURRENT_ALL;

        xQueueSend(current_cmd_queue, &cmd, 0);
    }
}

static void handle_angle(const char *payload)
{
    sensor_cmd_t cmd;

    if(strcmp(payload, "GET") == 0)
    {
        cmd.cmd = SENSOR_CMD_GET_ANGLE;

        xQueueSend(angle_cmd_queue, &cmd, 0);
    }
}

static void handle_motor(const char *payload)
{
    motor_cmd_t cmd;

    int axis;
    int pwm;
    int dir;

    if(sscanf(payload,
              "AXIS=%d,PWM=%d,DIR=%d",
              &axis,
              &pwm,
              &dir) != 3)
    {
        ESP_LOGW(TAG, "Payload motor inválido");
        return;
    }

    cmd.axis = axis;
    cmd.pwm  = pwm;
    cmd.dir  = dir;

    xQueueSend(motor_cmd_queue, &cmd, 0);

    ESP_LOGI(TAG,
             "Motor -> Axis=%u PWM=%u DIR=%u",
             cmd.axis,
             cmd.pwm,
             cmd.dir);
}


static void handle_system(const char *payload)
{
    system_cmd_t cmd;

    if(strcmp(payload, "REBOOT") == 0)
    {
        cmd.cmd = SYSTEM_CMD_REBOOT;
    }
    else if(strcmp(payload, "WIFI_RESTART") == 0)
    {
        cmd.cmd = SYSTEM_CMD_WIFI_RESTART;
    }
    else if(strcmp(payload, "STATUS") == 0)
    {
        cmd.cmd = SYSTEM_CMD_STATUS;
    }
    else
    {
        ESP_LOGW(TAG, "Comando de sistema desconocido");
        return;
    }

    xQueueSend(system_cmd_queue, &cmd, 0);
}
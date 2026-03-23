/******************************************************************************
 * @file    motores.h
 * @brief   Librería para la gestión de motores/bobinas en el sistema IMPASS (ESP32-S3)
 *
 * @project IMPASS - Trabajo Fin de Máster
 * @company Universidad de Malaga, Máster en Sistemas Electrónicos para Entornos Inteligentes
 * @author  Luis Miguel Carmona Cortes
 * @date    2026
 * @version 1.0
 *
 * @details
 * Este archivo contiene la interfaz de control para 6 actuadores (motores/bobinas)
 * mediante el periférico MCPWM del ESP32-S3, permitiendo control de dirección 
 * y modulación de ancho de pulso (PWM).
 *
 * @note    El control de velocidad se realiza mediante Duty Cycle de 8 bits (0-255).
 ******************************************************************************/

#ifndef MOTORES_H
#define MOTORES_H

#include "definitions.h"
#include "driver/mcpwm_prelude.h"
#include <stdint.h>

/* ===== Estructura de los motores ====== */

typedef struct {
    int pin_dir;
    int pin_pwm;
    mcpwm_cmpr_handle_t comparator; // Manejador para cambiar el Duty Cycle
} motor_mcpwm_obj_t;

/* ===== Codigo de errores =====*/

typedef enum {
    MOTORES_OK = 0,
    MOTORES_ERROR,
    MOTORES_INVALID_PARAM
} motores_Status_t;

/* ===== Funciones =====*/
motores_Status_t motores_init(uint32_t resolution, uint32_t period);
motores_Status_t motores_avanza(int eje, uint8_t duty_cycle);
motores_Status_t motores_retrocede(int eje, uint8_t duty_cycle);
motores_Status_t motores_para(int eje);

motores_Status_t solenoide_bloquea(int solenoide);
motores_Status_t solenoide_desbloquea(int solenoide);

#endif
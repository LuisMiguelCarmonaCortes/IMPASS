/******************************************************************************
 * @file    lm75.h
 * @brief   Librería para la configuración y lectura del sensor de temperatura LM75H en el sistema IMPASS (ESP32-S3)
 *
 * @project IMPASS - Trabajo Fin de Máster
 * @company Universidad de Malaga, Máster en Sistemas Electrónicos para Entornos Inteligentes
 * @author  Luis Miguel Carmona Cortes
 * @date    2026
 * @version 1.0
 *
 * @details
 * Este archivo proporciona las definiciones, funciones y estructuras necesarias
 * para inicializar, configurar y leer datos del sensor de temperatura LM75H
 * a través del bus I2C en la PCB basada en ESP32-S3.
 *
 * Incluye:
 *   - Direcciones I2C del sensor
 *   - Configuración de registros internos
 *   - Funciones para obtener la temperatura en grados Celsius
 *
 * @note
 * Las conexiones I2C coincidan con las definidas en `definitions.h`.
 ******************************************************************************/

#ifndef LM75_H
#define LM75_H

#include <stdint.h>

/* ===== Definiciones de registros =====*/

#define LM75_TEMP_REG 0x00   // Contiene dos bytes de datos de 8 bits. Para almacenar los datos de temperatura medidos
#define LM75_CONF_REG 0x01   // Contiene un único byte de datos de 8 bits. Para configurar el estado de funcionamiento del dispositivo
#define LM75_THYST_REG 0x02  // Contiene dos bytes de datos de 8 bits. Para almacenar el límite de histéresis Thyst
#define LM75_TOS_REG 0x03    // Contiene dos bytes de datos de 8 bits. Para almacenar el límite Tos de apagado por sobrecalentamiento

/* ====== Definicion de mascaras =====*/
#define LM75_FAULT_QUEUE (0x03 << 3)    // Bit 3 y Bit 4
#define LM75_FAULT_QUEUE_BIT4 (1 << 4)  // Bit 4
#define LM75_FAULT_QUEUE_BIT3 (1 << 3)  // Bit 3
#define LM75_POLARITY (1 << 2)          // Bit 2
#define LM75_MODE (1 << 1)              // Bit 1
#define LM75_SHUTDOWN (1 << 0)          // Bit 0

/* ===== Estructura del sensor =====*/

typedef struct {
    uint8_t i2c_addr;       // Direccion I2C
    uint8_t config_reg;     // Registro de configuracion
} lm75_t;

/* ===== Codigo de errores =====*/

typedef enum {
    LM75_OK = 0,
    LM75_ERROR,
    I2C_ERROR,
    LM75_INVALID_PARAM
} lm75_Status_t;

/* ===== Funciones =====*/

// Configurar
lm75_Status_t lm75_Init(lm75_t *dev, uint8_t dir);              // Inicia por defecto
lm75_Status_t lm75_SetFaultQueue(lm75_t *dev, uint8_t value);   // 
lm75_Status_t lm75_SetPolarity(lm75_t *dev, uint8_t polarity);  // 
lm75_Status_t lm75_SetMode(lm75_t *dev, uint8_t mode);          // 
lm75_Status_t lm75_SetShutdown(lm75_t *dev, uint8_t shutdown);  // 

// Leer configuracion
lm75_Status_t lm75_GetConfig(lm75_t *dev);                      // Lee la configuracion actual

// Leer temperatura
lm75_Status_t lm75_read_raw_temp(lm75_t *dev, uint16_t *temp);  // La temperatura se lee en 11 bits MSB
lm75_Status_t lm75_read_celsius_temp(lm75_t *dev, float *temp); // Devuelve la temperatura en ºC 

// Escribir 
lm75_Status_t lm75_SetThyst(lm75_t *dev, float temp);
lm75_Status_t lm75_SetTos(lm75_t *dev, float temp);


#endif
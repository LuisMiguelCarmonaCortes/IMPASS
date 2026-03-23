/******************************************************************************
 * @file    mcp3008.h
 * @brief   Librería para la configuración y lectura del ADC MCP3008 en el sistema IMPASS (ESP32-S3)
 *
 * @project IMPASS - Trabajo Fin de Máster
 * @company Universidad de Malaga, Máster en Sistemas Electrónicos para Entornos Inteligentes
 * @author  Luis Miguel Carmona Cortes
 * @date    2026
 * @version 1.0
 *
 * @details
 * Este archivo proporciona las definiciones, funciones y estructuras necesarias
 * para inicializar y realizar lecturas de canales analógicos a través del ADC 
 * MCP3008 utilizando el protocolo SPI en la PCB basada en ESP32-S3.
 *
 * Incluye:
 * - Configuración de canales (Single-ended)
 * - Conversión de valores crudos a voltaje (referencia 3.3V)
 * - Funciones de acceso al bus SPI para el ADC
 *
 * @note
 * El MCP3008 requiere una frecuencia de reloj SPI compatible con el datasheet 
 * para garantizar la precisión de los 10 bits de resolución.
 ******************************************************************************/

#ifndef MCP3008_H
#define MCP3008_H

#include "definitions.h"
#include <stdint.h>
#include "driver/spi_master.h"

/* ===== Bytes de envio =====*/
#define START_BYTE  0x01
#define DUMMY_BYTE  0x00

/* ===== Valor de Vref =====*/
#define VREF 5000

/* ===== Valores del acs712 =====*/
#define ACS712_SENSITIVITY 66.0f
#define ACS712_OFFSET_MV   2500

/* ===== Definicion de canales del MCP3008 =====*/
// Single ended
#define MCP_SGL_CH0  0x80
#define MCP_SGL_CH1  0x90
#define MCP_SGL_CH2  0xA0
#define MCP_SGL_CH3  0xB0
#define MCP_SGL_CH4  0xC0
#define MCP_SGL_CH5  0xD0
#define MCP_SGL_CH6  0xE0
#define MCP_SGL_CH7  0xF0

// Diferential
#define MCP_DIFF_CH0_CH1  0x00
#define MCP_DIFF_CH1_CH0  0x10
#define MCP_DIFF_CH2_CH3  0x20
#define MCP_DIFF_CH3_CH2  0x30
#define MCP_DIFF_CH4_CH5  0x40
#define MCP_DIFF_CH5_CH4  0x50
#define MCP_DIFF_CH6_CH7  0x60
#define MCP_DIFF_CH7_CH6  0x70

/* ===== Estructura del dispositivo MCP3008 ====== */
typedef struct {
    spi_device_handle_t spi_handle;
    uint8_t channel;
} mcp3008_t;

/* ===== Codigo de errores =====*/

typedef enum {
    MCP3008_OK = 0,
    MCP3008_ERROR,
    SPI_ERROR,
    MCP3008_INVALID_PARAM
} mcp3008_Status_t;

/* ===== Funciones =====*/

mcp3008_Status_t mcp3008_init(mcp3008_t *dev);

mcp3008_Status_t mcp3008_read_channel(mcp3008_t *dev, uint8_t channel, uint16_t *result);

mcp3008_Status_t mcp3008_raw_to_mv(mcp3008_t *dev, uint8_t channel, uint16_t *vref_mv);

mcp3008_Status_t mcp3008_amperios(mcp3008_t *dev, uint8_t channel, float *amp);

#endif
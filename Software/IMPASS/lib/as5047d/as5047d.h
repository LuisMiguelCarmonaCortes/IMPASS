/******************************************************************************
 * @file    as5047d.h
 * @brief   Librería para la configuración y lectura del sensor magnético
 *          AS5047D en el sistema IMPASS (ESP32-S3)
 *
 * @project IMPASS - Trabajo Fin de Máster
 * @company Universidad de Malaga, Máster en Sistemas Electrónicos para Entornos Inteligentes
 * @author  Luis Miguel Carmona Cortes
 * @date    2026
 * @version 1.0
 *
 * @details
 * Este archivo proporciona las definiciones, funciones y estructuras necesarias
 * para inicializar, configurar y leer datos del sensor magnético AS5047D
 * mediante comunicación SPI en la PCB basada en ESP32-S3.
 *
 * El AS5047D es un encoder magnético absoluto de alta resolución (14 bits)
 * que permite medir la posición angular de un eje en tiempo real.
 *
 * Incluye:
 *   - Definición de registros volátiles y no volátiles
 *   - Máscaras de bits para configuración y diagnóstico
 *   - Funciones de acceso a registros mediante SPI
 *   - Funciones para lectura de ángulo y estado del sensor
 *
 * @note
 * La configuración del bus SPI y los pines utilizados deben coincidir
 * con los definidos en `definitions.h`.
 ******************************************************************************/

#ifndef AS5047D_H
#define AS5047D_H

#include "definitions.h"
#include <stdint.h>
#include "driver/spi_master.h"

/* ===== Definiciones de registros volatiles =====*/

#define AS5047D_NOP         0x00    // No operacion
#define AS5047D_ERRFL       0x01    // Registro de errores
#define AS5047D_PROG        0x03    // Registro de programacion
#define AS5047D_DIAAGC      0x3FFC  // Registro de diagnostico y AGC
#define AS5047D_MAG         0x3FFD  // Registro de magnitu CORDIC
#define AS5047D_ANGLEUNC    0x3FFE  // Angulo medido sin compensacion dinamica
#define AS5047D_ANGLECOM    0x3FFF  // Angulo medido con compensacion dinamica

/* ====== Definicion de mascaras =====*/

// ERRFL
#define AS5047D_PARERR  (1 << 2)        // Error de paridad
#define AS5047D_INVCOMM (1 << 1)        // Error comando invalido, si es 1
#define AS5047D_FRERR   (1 << 0)        // Error en la trama SPI, si es 1

// PROG
#define AS5047D_PROGVER (1 << 6)        // Verificacion de programacion, tiene que ser 1 para verificar
#define AS5047D_PROGOTP (1 << 3)        // Empieza un ciclo de programacion
#define AS5047D_OTPREF  (1 << 2)        // Refresca los registros no volatiles con el contenido de la programacion
#define AS5047D_PROGEN  (1 << 0)        // Habilita programar toa la memoria

// DIAAGC
#define AS5047D_MAGL    (1 << 11)       // Campo magnetico muy bajo; AGC = FF
#define AS5047D_MAGH    (1 << 10)       // Campo magnetico muy alto; AGC = 00
#define AS5047D_COF     (1 << 9)        // Overflow de CORDIC
#define AS5047D_LF      (1 << 8)        // Offset; 0 --> no esta listo 1 --> terminado 
#define AS5047D_AGC      0x00ff         // Valor de ganancia


/* ===== Definiciones de registros no volatiles*/

#define AS5047D_ZPOSM       0x0016  // Posicion cero MSB
#define AS5047D_ZPOSL       0x0017  // Posicion cero LSB y MAG diagnosis
#define AS5047D_SETTINGS1   0x0018  // Registro de opciones 1
#define AS5047D_SETTINGS2   0x0019  // Registro de opciones 2 

/* ====== Definicion de mascaras =====*/

// ZPOSL
#define AS5047D_COMP_L_EN   (1 << 6)    // Habilita la contribucion de MAGH
#define AS5047D_COMP_H_EN   (1 << 7)    // Habilita la contribucion de MAGL

// SETTINGS1
#define AS5047D_FACTORY_RESET   (1 << 0)    // Preprogramado a 1
#define AS5047D_DIR             (1 << 2)    // Direccion de rotacion
#define AS5047D_UVW_ABI         (1 << 3)    // Define la salida PWM
#define AS5047D_DAECDIS         (1 << 4)    // Deshabilita la compensancion de angulo
#define AS5047D_ABIBIN          (1 << 5)    // ABI decimal o binario
#define AS5047D_DATASELECT      (1 << 6)    // Que dato puede ser leido por el registro 0x3fff 
#define AS5047D_PWM_ON          (1 << 7)    // Habilita el PWM

// SETTINGS2

#define AS5047D_UVWPP           0x0003   // UVW numero de polos
#define AS5047D_HYS             0x0018   // Opciones de hysteresis
#define AS5047D_AVIRES          0x00D0   // Resolucion de ABI


/* ===== Estructura del sensor =====*/

typedef struct {
    spi_device_handle_t spi_handle;
    uint8_t cs_pin;

    uint16_t settings1;
    uint16_t settings2;

} as5047d_t;


/* ===== Codigo de errores =====*/

typedef enum {
    AS5047D_OK = 0,
    AS5047D_ERROR,
    AS5047D_ERR_SPI,
    AS5047D_INVALID_PARAM
} as5047d_Status_t;

/* ===== Funciones =====*/

as5047d_Status_t as5047d_program(as5047d_t *dev);
as5047d_Status_t as5047d_init(as5047d_t *dev);

as5047d_Status_t as5047d_read_bits(as5047d_t *dev, uint16_t registro, uint16_t mascara, uint16_t *dato);
as5047d_Status_t as5047d_read_register(as5047d_t *dev, uint16_t registro, uint16_t *dato);

as5047d_Status_t as5047d_write_bits(as5047d_t *dev, uint16_t mascara, uint16_t dato);
as5047d_Status_t as5047d_write_register(as5047d_t *dev, uint16_t registro, uint16_t dato);


#endif
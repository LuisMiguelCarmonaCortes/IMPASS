/******************************************************************************
 * @file    definitions.h
 * @brief   Definiciones de hardware del sistema IMPASS (ESP32-S3)
 *
 * @project IMPASS - Trabajo Fin de Máster
 * @company Universidad de Malaga, master Sistemas Electronicos para Entornos Inteligentes
 * @author  Luis Miguel Carmona Cortes
 * @date    2026
 * @version 1.0
 *
 * @details
 * Este archivo contiene la asignación de pines, direcciones I2C y
 * configuraciones hardware específicas de la PCB basada en ESP32-S3.
 *
 * @note    Cualquier modificación en el hardware debe reflejarse aquí.
 ******************************************************************************/

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

/* ===== Definicion de pines ESP32-S3 ===== */

// Pines para los Puentes H
#define EJE1_DIR1 45     // GPIO 45
#define EJE1_PWM1 48     // GPIO 48
#define EJE1_DIR2 39     // GPIO 39
#define EJE1_PWM2 38     // GPIO 38

#define EJE2_DIR3 41     // GPIO 41
#define EJE2_PWM3 40     // GPIO 40
#define EJE2_DIR4 4      // GPIO 4
#define EJE2_PWM4 5      // GPIO 5

#define EJE3_DIR5 6      // GPIO 6
#define EJE3_PWM5 7      // GPIO 7
#define EJE3_DIR6 17     // GPIO 17
#define EJE3_PWM6 18     // GPIO 18

// Pines Solenoides
#define SOL1 3      // GPIO 3
#define SOL2 2      // GPIO 2
#define SOL3 1      // GPIO 1

// Pines I2C
#define I2C_SDA_GPIO 15      // GPIO 15
#define I2C_SCL_GPIO 16      // GPIO 16

// Pines SPI
#define CS_ADC 10   // GPIO 10
#define CS_INC 9    // GPIO 9
#define MOSI 11     // GPIO 11
#define SCK 12      // GPIO 12
#define MISO 13     // GPIO 13

/* ===== Definicion de direcciones I2C =====*/

// LM75B Placa Logica
#define LM75_LOGIC_ADDR  0x4F

// LM75B Placa Potencia
#define LM75_POWER_ADDR  0x48

// ADS1015
#define ADS1015_ADDR 0x49

// LIS3DH
#define LIS3DH_ADDR  0x19


#endif
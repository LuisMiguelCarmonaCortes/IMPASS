/******************************************************************************
 * @file    ads1015.h
 * @brief   Librería para la configuración y lectura del convertidor analógico
 *          digital ADS1015 en el sistema IMPASS (ESP32-S3)
 *
 * @project IMPASS - Trabajo Fin de Máster
 * @company Universidad de Málaga, Máster en Sistemas Electrónicos para Entornos Inteligentes
 * @author  Luis Miguel Carmona Cortés
 * @date    2026
 * @version 1.0
 *
 * @details
 * Este archivo proporciona las definiciones, funciones y estructuras necesarias
 * para inicializar, configurar y leer datos del convertidor analógico-digital
 * ADS1015 mediante comunicación I2C en la PCB basada en ESP32-S3.
 *
 * El ADS1015 es un convertidor analógico-digital (ADC) de 12 bits con
 * cuatro canales de entrada multiplexados, capaz de realizar conversiones
 * en modo simple o diferencial, con ganancia programable (PGA) y un
 * comparador integrado para aplicaciones de adquisición de datos.
 *
 * Incluye:
 *   - Definición de registros de configuración y conversión
 *   - Máscaras de bits para configuración del dispositivo
 *   - Funciones de inicialización y configuración mediante I2C
 *   - Funciones para la lectura de uno o varios canales analógicos
 *   - Funciones de conversión de los datos digitales a tensión
 *
 * @note
 * La configuración del bus I2C y las direcciones del dispositivo deben
 * coincidir con las definidas en `definitions.h`.
 ******************************************************************************/

#ifndef ADS1015_H
#define ADS1015_H

#include <stdint.h>

/* ===== Definiciones de registros =====*/
#define ADS1015_CONV_REG    0x00
#define ADS1015_CONF_REG    0x01
#define ADS1015_LO_TRG_REG  0x10
#define ADS1015_HI_TRG_REG  0x11

/* ====== Definicion de mascaras =====*/
// Escritura
#define ADS1015_OS_START_CONVERSION   (1 << 15)

// Lectura
#define ADS1015_OS_CONVERSION_BUSY    (0 << 15)
#define ADS1015_OS_CONVERSION_READY   (1 << 15)

// Canal
#define ADS1015_CANAL_0 (0x4 << 12)
#define ADS1015_CANAL_1 (0x5 << 12)
#define ADS1015_CANAL_2 (0x6 << 12)
#define ADS1015_CANAL_3 (0x7 << 12)

// Ganancia
#define ADS1015_PGA_6_144V   (0x0 << 9)
#define ADS1015_PGA_4_096V   (0x1 << 9)
#define ADS1015_PGA_2_048V   (0x2 << 9)
#define ADS1015_PGA_1_024V   (0x3 << 9)
#define ADS1015_PGA_0_512V   (0x4 << 9)
#define ADS1015_PGA_0_256V   (0x5 << 9)

// Modo de monitoreo
#define ADS1015_CONTINUO    (0 << 8)
#define ADS1015_DISPARO     (1 << 8)

// Velocidad
#define ADS1015_128_SPS     (0x0 << 5)
#define ADS1015_250_SPS     (0x1 << 5)
#define ADS1015_490_SPS     (0x2 << 5)
#define ADS1015_920_SPS     (0x3 << 5)
#define ADS1015_1600_SPS    (0x4 << 5)
#define ADS1015_2400_SPS    (0x5 << 5)
#define ADS1015_3300_SPS    (0x6 << 5)

// Modo de comparacion
#define ADS1015_TRADICIONAL (0x0 << 4)
#define ADS1015_VENTANA     (0x1 << 4)

// Polaridad resistencia de alerta
#define ADS1015_NIVEL_BAJO  (0x0 << 3)
#define ADS1015_NIVEL_ALTO  (0x1 << 3)

// Alerta fija
#define ADS1015_NO_FIJA (0x0 << 2)
#define ADS1015_FIJA    (0x1 << 2)

// Habilitar el comparador
#define ADS1015_COMP_QUE_DISABLE   0x03
#define ADS1015_COMP_QUE_4         0x02
#define ADS1015_COMP_QUE_2         0x01
#define ADS1015_COMP_QUE_1         0x00

// Bloques
#define ADS1015_MUX_MASK      (0x7 << 12)
#define ADS1015_PGA_MASK      (0x7 << 9)
#define ADS1015_MODE_MASK     (1 << 8)
#define ADS1015_DR_MASK       (0x7 << 5)
#define ADS1015_COMP_MODE_MASK (1 << 4)
#define ADS1015_COMP_POL_MASK  (1 << 3)
#define ADS1015_COMP_LAT_MASK  (1 << 2)
#define ADS1015_COMP_QUE_MASK  (0x3)

/* ===== Estructura del sensor =====*/

typedef struct {
    uint8_t i2c_addr;        // Direccion I2C
    uint16_t config_reg;     // Registro de configuracion
    uint16_t lo_trg_reg;     // Limite inferior
    uint16_t hi_trg_reg;     // Limite superior
    uint16_t conver_reg;     // Valor del ADC 
} ads1015_t;

/* ===== Codigo de errores =====*/

typedef enum {
    ADS1015_OK = 0,
    ADS1015_ERROR,
    ADS1015_I2C_ERROR,
    ADS1015_INVALID_PARAM
} ads1015_Status_t;

/* ===== Funciones =====*/

// Configurar
ads1015_Status_t ads1015_Init(ads1015_t *dev, uint8_t dir);         // Inicia por defecto
ads1015_Status_t ads1015_SetChannel(ads1015_t *dev, uint16_t ch);     // Selecciona el canal 
ads1015_Status_t ads1015_SetPGA(ads1015_t *dev, uint16_t pga);       // Selecciona la ganancia
ads1015_Status_t ads1015_SetMode(ads1015_t *dev, uint16_t mode);     // Selecciona el modo de muestreo
ads1015_Status_t ads1015_SetSpeed(ads1015_t *dev, uint16_t speed);   // Selecciona la velocidad de conversion
ads1015_Status_t ads1015_SetCompMode(ads1015_t *dev, uint16_t mode); // Selecciona de comparacion
ads1015_Status_t ads1015_SetCompPol(ads1015_t *dev, uint16_t pol);   // Selecciona la polarizacion de la alerta
ads1015_Status_t ads1015_SetComLatch(ads1015_t *dev, uint16_t lat);  // Selecciona el modo de la alerta
ads1015_Status_t ads1015_SetCompQue(ads1015_t *dev, uint16_t que);   // Selecciona el modo del comparador

// Leer configuracion
ads1015_Status_t ads1015_GetConfig(ads1015_t *dev);     // Lee la configuracion actual

// Leer ADC
ads1015_Status_t ads1015_readADC(ads1015_t *dev, int16_t *adc);   // Se lee el canal del ADC
ads1015_Status_t ads1015_convertRead(ads1015_t *dev, float *volt); // Devuelve el voltaje en V

// Escribir limites
ads1015_Status_t ads1015_SetLo(ads1015_t *dev, float volt);     // Limite inferior en voltios
ads1015_Status_t ads1015_SetHi(ads1015_t *dev, float volt);     // Limite superior en voltios




#endif